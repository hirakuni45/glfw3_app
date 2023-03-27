#pragma once
//=====================================================================//
/*!	@file
	@brief	PDF ファイル（入力のみ）を扱うクラス（ヘッダー）
    @author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2017, 2023 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/glfw3_app/blob/master/LICENSE
*/
//=====================================================================//
#include <string>
#include <mupdf/fitz.h>
#include "img_io/i_img_io.hpp"
#include "img_io/img_rgba8.hpp"

namespace img {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	PDF ファイル・クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	class pdf_in {
		fz_context*		context_;
		fz_document*	document_;
		fz_outline *	outline_;

		float			rotation_;
		vtx::spos		size_;

		std::string		doctitle_;

		int				page_count_;
		int				page_no_;
		int				page_current_;

		shared_img		img_;

		std::string convert_win32_path_(const std::string& path)
		{
			std::string p;
			for(auto ch : path) {
				if(ch == '/') {
					p += '\\';
				} else {
					p += ch;
				}
			}
			return p;
		}

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief	コンストラクター
		*/
		//-----------------------------------------------------------------//
		pdf_in() noexcept : context_(nullptr), document_(nullptr), outline_(nullptr),
				   rotation_(0.0f), size_(0),
				   page_count_(0), page_no_(0), page_current_(0) { }


		//-----------------------------------------------------------------//
		/*!
			@brief	デストラクター
		*/
		//-----------------------------------------------------------------//
		~pdf_in() { destroy(); }


		//-----------------------------------------------------------------//
		/*!
			@brief	初期化
		*/
		//-----------------------------------------------------------------//
		void initialize() noexcept
		{
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	PDF ファイルを開く
			@param[in]	filename	ファイル名
			@return 正常にオープンできれば「true」
		*/
		//-----------------------------------------------------------------//
		bool open(const std::string& filename) noexcept
		{
			if(context_ == nullptr) {
				context_ = fz_new_context(NULL, NULL, FZ_STORE_UNLIMITED);
			}

			fz_try(context_)
				fz_register_document_handlers(context_);
			fz_catch(context_)

			close();

			document_ = fz_open_document(context_, filename.c_str());
			if(document_ == nullptr) {
				return false;
			}

			// パスワードが必要な場合
			char* password = nullptr;
			if (fz_needs_password(context_, document_)) {
				if (!fz_authenticate_password(context_, document_, password)) {
					fz_drop_document(context_, document_);
					return false;
				}
			}

			// ドキュメントのページ数を取得
			page_count_ = fz_count_pages(context_, document_);
			page_no_ = 0;
			page_current_ = -1;

			outline_ = fz_load_outline(context_, document_);
			if(outline_ != nullptr) {
				doctitle_ = outline_->title;
			}

			return true;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	PDF ファイルを開く
			@param[in]	filename	ファイル名
			@return 正常にオープンできれば「true」
		*/
		//-----------------------------------------------------------------//
		bool open(const utils::wstring& filename) noexcept
		{
			std::string s;
			utils::utf16_to_utf8(filename, s);
			return open(s);
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	ドキュメント・タイトルを取得
			@return タイトル
		*/
		//-----------------------------------------------------------------//
		const std::string& get_document_title() const noexcept { return doctitle_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	ページを設定
			@param[in]	page	設定ページ
		*/
		//-----------------------------------------------------------------//
		void set_page(int page) noexcept { if(page >= 0 && page < page_count_) page_no_ = page; }


		//-----------------------------------------------------------------//
		/*!
			@brief	現在のページを取得
			@return 現在のページ
		*/
		//-----------------------------------------------------------------//
		int get_page() const noexcept { return page_no_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	前ページ
		*/
		//-----------------------------------------------------------------//
		void prev_page() noexcept { --page_no_; if(page_no_ < 0) page_no_ = 0; }


		//-----------------------------------------------------------------//
		/*!
			@brief	次ページ
		*/
		//-----------------------------------------------------------------//
		void next_page() noexcept { ++page_no_; if(page_no_ >= page_count_) page_no_ = page_count_ - 1; }


		//-----------------------------------------------------------------//
		/*!
			@brief	ページをレンダリングする
			@param[in]	size	レンダリングサイズ
			@return 正常終了なら「true」
		*/
		//-----------------------------------------------------------------//
		bool render(const vtx::spos& size) noexcept
		{
			if(document_ == nullptr || page_count_ <= 0) {
				return false;
			}

			// カレントページ移動、サイズ変更の場合レンダリング
			if(size.x > 0 && size.y > 0 && size_ != size) ;
			else if(page_no_ >= 0 && page_no_ < page_count_ && page_no_ != page_current_) ;
			else {
				return false;
			}

			size_ = size;
			page_current_ = page_no_;

			fz_page* page;
			page = fz_load_page(context_, document_, page_no_);

			fz_matrix rot = fz_rotate(rotation_);
			fz_matrix mat = fz_pre_scale(rot, 1.0f, 1.0f);

			fz_rect tmp = fz_bound_page(context_, page);
			fz_rect bounds = fz_transform_rect(tmp, mat);
			fz_irect bbox = fz_round_rect(bounds);
			float xx = static_cast<float>(bbox.x1 - bbox.x0);
			float yy = static_cast<float>(bbox.y1 - bbox.y0);

			// サイズのマッチング
			float zoom = 1.0f;
			float zoomx = static_cast<float>(size.x) / xx;
			float zoomy = static_cast<float>(size.y) / yy;
			if((zoomx * xx) > static_cast<float>(size.x)) {
				zoom = zoomx;
			} else {
				zoom = zoomy;
			}

			{
				fz_matrix mat = fz_pre_scale(rot, zoom, zoom);
				fz_rect tmp = fz_bound_page(context_, page);
				fz_rect bounds = fz_transform_rect(tmp, mat);
				fz_irect bbox = fz_round_rect(bounds);

				int w = bbox.x1 - bbox.x0;
				int h = bbox.y1 - bbox.y0;
// std::cout << boost::format("size: %d, %d\n") % w % h;
				// fz_device_rgb ---> RGBA
				auto im = new img_rgba8;
				im->create(vtx::spos(w, h), true);

				int alpha = 1;
				int stride = w * 4;
				auto rgba = reinterpret_cast<unsigned char*>(im->at_image());
				fz_pixmap* pix = fz_new_pixmap_with_data(context_, fz_device_rgb(context_), w, h, NULL, alpha, stride, rgba);
				fz_clear_pixmap_with_value(context_, pix, 0xff);

				fz_device *dev = fz_new_draw_device(context_, mat, pix);
				fz_run_page(context_, page, dev, mat, NULL);

				img_ = shared_img(im);
				fz_drop_device(context_, dev);
				fz_drop_pixmap(context_, pix);
			}
			fz_drop_page(context_, page);

			return true;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	PDF をクローズする
		*/
		//-----------------------------------------------------------------//
		void close() noexcept
		{
			if(outline_ != nullptr) {
				fz_drop_outline(context_, outline_);
				outline_ = nullptr;
			}

			if(document_ != nullptr) {
				fz_drop_document(context_, document_);
				document_ = nullptr;
			}
			page_count_ = 0;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	PDF のページ数を取得
			@return 最大ページ数
		*/
		//-----------------------------------------------------------------//
		int get_page_limit() const noexcept { return page_count_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	画像の参照
			@return 画像のコンテキスト
		*/
		//-----------------------------------------------------------------//
//		img::img_rgba8& at_img_rgba8() noexcept { return img_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	レンダリングされたビットマップを取得
			@return 画像のインターフェース・クラス
		*/
		//-----------------------------------------------------------------//
		const shared_img get_image() const noexcept { return img_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	廃棄
		*/
		//-----------------------------------------------------------------//
		void destroy() noexcept
		{
			close();

			if(context_ != nullptr) {
				fz_drop_context(context_);
			}
			context_ = nullptr;
		}
	};
}
