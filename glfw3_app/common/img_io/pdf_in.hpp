#pragma once
//=====================================================================//
/*!	@file
	@brief	PDF ファイル（入力のみ）を扱うクラス（ヘッダー）
    @author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2017 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/glfw3_app/blob/master/LICENSE
*/
//=====================================================================//
#include <string>
extern "C" {
#include <mupdf/fitz.h>
};
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

		img_rgba8		img_;

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief	コンストラクター
		*/
		//-----------------------------------------------------------------//
		pdf_in() : context_(nullptr), document_(nullptr), outline_(nullptr),
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
		void initialize()
		{
			context_ = fz_new_context(NULL, NULL, FZ_STORE_UNLIMITED);
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	PDF ファイルを開く
			@param[in]	filename	ファイル名
			@return 正常にオープンできれば「true」
		*/
		//-----------------------------------------------------------------//
		bool open(const std::string& filename)
		{
			close();

			document_ = fz_open_document(context_, filename.c_str());
			if(document_ == 0) {
				return false;
			}

			// パスワードが必要な場合
			char* password = 0;
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
			if(outline_) {
				doctitle_ = outline_->title;
			}
// std::cout << boost::format("PDF Pages: %d\n") % page_count_;

			return true;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	PDF ファイルを開く
			@param[in]	filename	ファイル名
			@return 正常にオープンできれば「true」
		*/
		//-----------------------------------------------------------------//
		bool open(const utils::wstring& filename) {
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
		const std::string& get_document_title() const { return doctitle_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	ページを設定
			@param[in]	page	設定ページ
		*/
		//-----------------------------------------------------------------//
		void set_page(int page) { if(page >= 0 && page < page_count_) page_no_ = page; }


		//-----------------------------------------------------------------//
		/*!
			@brief	現在のページを取得
			@return 現在のページ
		*/
		//-----------------------------------------------------------------//
		int get_page() const { return page_no_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	前ページ
		*/
		//-----------------------------------------------------------------//
		void prev_page() { --page_no_; if(page_no_ < 0) page_no_ = 0; }


		//-----------------------------------------------------------------//
		/*!
			@brief	次ページ
		*/
		//-----------------------------------------------------------------//
		void next_page() { ++page_no_; if(page_no_ >= page_count_) page_no_ = page_count_ - 1; }


		//-----------------------------------------------------------------//
		/*!
			@brief	ページをレンダリングする
			@param[in]	size	レンダリングサイズ
			@return 正常終了なら「true」
		*/
		//-----------------------------------------------------------------//
		bool render(const vtx::spos& size)
		{
			if(document_ == 0 || page_count_ <= 0) {
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
// std::cout << boost::format("Scale: %f, %f\n") % zoomx % zoomy;
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
				img_.create(vtx::spos(w, h), true);

				fz_separations* seps = fz_new_separations(context_, 0);
				int alpha = 255;
				int stride = w * 4;
				fz_pixmap* pix = fz_new_pixmap_with_data(context_, fz_device_rgb(context_), w, h, seps, alpha, stride,
					reinterpret_cast<unsigned char*>(img_.at_image()));
				fz_clear_pixmap_with_value(context_, pix, 0xff);

				fz_device *dev = fz_new_draw_device(context_, mat, pix);
				fz_run_page(context_, page, dev, mat, NULL);
				fz_drop_device(context_, dev);

				fz_drop_pixmap(context_, pix);
				fz_drop_separations(context_, seps);
			}
			fz_drop_page(context_, page);

			return true;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	PDF をクローズする
		*/
		//-----------------------------------------------------------------//
		void close()
		{
			if(outline_) {
				fz_drop_outline(context_, outline_);
				outline_ = nullptr;
			}

			if(document_) {
				fz_drop_document(context_, document_);
				document_ = nullptr;
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	PDF のページ数を取得
			@return 最大ページ数
		*/
		//-----------------------------------------------------------------//
		int get_page_limit() const { return page_count_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	画像の参照
			@return 画像のコンテキスト
		*/
		//-----------------------------------------------------------------//
		img::img_rgba8& at_img_rgba8() { return img_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	レンダリングされたビットマップを取得
			@return 画像のインターフェース・クラス
		*/
		//-----------------------------------------------------------------//
		const i_img* get_image_if() const { return &img_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	廃棄
		*/
		//-----------------------------------------------------------------//
		void destroy()
		{
			close();

			fz_drop_context(context_);
			context_ = 0;
		}
	};
}
