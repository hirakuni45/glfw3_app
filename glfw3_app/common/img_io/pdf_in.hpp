#pragma once
//=========================================================================//
/*!	@file
	@brief	PDF ファイル（入力のみ）を扱うクラス（ヘッダー）
    @author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2017, 2023 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/glfw3_app/blob/master/LICENSE
*/
//=========================================================================//
#include <string>
#include <mupdf/fitz.h>
#include "img_io/i_img_io.hpp"
#include "img_io/img_rgba8.hpp"
#include "utils/format.hpp"

namespace img {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	PDF ファイル・クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	class pdf_in {
	public:

		enum class area_type {
			NONE,
			FIT,
			ZOOM,
		};

		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		/*!
			@brief	エリア構造体
		*/
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		struct area_t {
			area_type	atype;
			vtx::spos	size;
			float		zoom;
			float		rotation;

			area_t() noexcept :
				atype(area_type::NONE), size(0), zoom(0.0f), rotation(0.0f) { }
			explicit area_t(float zm, float r = 0.0f) noexcept :
				atype(area_type::ZOOM), size(0), zoom(zm), rotation(r) { }
			area_t(const vtx::spos& sz, float r = 0.0f) noexcept :
				atype(area_type::FIT), size(sz), zoom(0.0f), rotation(r) { }

			bool operator == (const area_t& src) const noexcept {
				return (src.atype == atype && src.size == size && src.zoom == zoom && src.rotation == rotation);
			}
			bool operator != (const area_t& src) const noexcept {
				return (src.atype != atype || src.size != size || src.zoom != zoom || src.rotation != rotation);
			}
		};

		typedef std::vector<fz_outline*> OUTLINES;

	private:
		fz_context*		context_;
		fz_document*	document_;
		fz_outline*		outline_;

		area_t			area_;

		OUTLINES		outlines_;

		int				page_count_;
		int				page_no_;
		int				page_current_;

		shared_img		img_;


		void reset_doc_() noexcept
		{
			page_count_ = 0;
			page_no_ = 0;
			page_current_ = -1;
		}

		void collect_outline_() noexcept
		{
			auto p = outline_;
			while(p != nullptr) {
				outlines_.push_back(p);
				p = p->next;
			}
		}

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief	コンストラクター
		*/
		//-----------------------------------------------------------------//
		pdf_in() noexcept :
			context_(nullptr), document_(nullptr), outline_(nullptr),
			area_(),
			outlines_(),
			page_count_(0), page_no_(0), page_current_(-1),
			img_()
		{ }


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
			@brief	PDF ファイルを開く @n
					ドキュメントのページ数、アウトラインを取得出来る。
			@param[in]	filename	ファイル名
			@param[in]	password	パスフレーズ（必要な場合）
			@return 正常にオープンできれば「true」
		*/
		//-----------------------------------------------------------------//
		bool open(const std::string& filename, const std::string& password = "") noexcept
		{
			if(context_ == nullptr) {
				context_ = fz_new_context(NULL, NULL, FZ_STORE_UNLIMITED);
				fz_try(context_)
					fz_register_document_handlers(context_);
				fz_catch(context_) {
					std::cerr << "cannot register document handlers: " << fz_caught_message(context_) << std::endl;
					fz_drop_context(context_);
					context_ = nullptr;
					return false;
				}
			}

			close();
			reset_doc_();

			document_ = fz_open_document(context_, filename.c_str());
			if(document_ == nullptr) {
//				std::cerr << "cannot open document: " << fz_caught_message(context_) << std::endl;
				reset_doc_();
				return false;
			}

			// パスワードが必要な場合
			if(fz_needs_password(context_, document_)) {
				if (!fz_authenticate_password(context_, document_, password.c_str())) {
					fz_drop_document(context_, document_);
					document_ = nullptr;
					return false;
				}
			}

			// ドキュメントのページ数を取得
			fz_try(context_)
				page_count_ = fz_count_pages(context_, document_);
			fz_catch(context_) {
				std::cerr << "cannot count number of pages: " << fz_caught_message(context_) << std::endl;
				fz_drop_context(context_);
				context_ = nullptr;
				fz_drop_document(context_, document_);
				document_ = nullptr;
				return false;
			}

			// ドキュメントのアウトラインを取得
			outline_ = fz_load_outline(context_, document_);
			if(outline_ != nullptr) {
				collect_outline_();
			}

			return true;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	ドキュメントが有効か検査
			@return 有効なら「true」
		*/
		//-----------------------------------------------------------------//
		bool is_document() const noexcept
		{
			return (context_ != nullptr) && (document_ != nullptr);
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	アウトラインを取得
			@return タイトル
		*/
		//-----------------------------------------------------------------//
		const OUTLINES& get_outlines() const noexcept { return outlines_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	ページを設定
			@param[in]	page	設定ページ
			@return 正常なら「true」
		*/
		//-----------------------------------------------------------------//
		bool set_page(int page) noexcept
		{
			if(context_ == nullptr || document_ == nullptr) {
				return false;
			}
			if(page >= 0 && page < page_count_) {
				page_no_ = page;
				return true;
			}
			return false;
		}


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
			@param[in]	area	エリア情報
			@return 正常終了なら「true」
		*/
		//-----------------------------------------------------------------//
		bool render(const area_t& area) noexcept
		{
			if(context_ == nullptr || document_ == nullptr || page_count_ <= 0) {
				return false;
			}

			// エリア変更、カレントページ移動の場合レンダリング
			if(area != area_) ;
			else if(page_count_ > 0 && page_no_ >= 0 && page_no_ <= page_count_ && page_no_ != page_current_) ;
			else {
				return false;
			}

			auto page = fz_load_page(context_, document_, page_no_);
			if(page == nullptr) {
				std::cerr << "load page fail: " << fz_caught_message(context_) << std::endl;
				return false;
			}
			area_ = area;
			page_current_ = page_no_;

			{
				fz_rect bound = fz_bound_page(context_, page);
				auto xx = bound.x1 - bound.x0;
				auto yy = bound.y1 - bound.y0;
				float zoom = 1.0f;
				if(area_.atype == area_type::FIT) {
					fz_matrix m0 = fz_rotate(area.rotation);
					fz_matrix m1 = fz_pre_translate(m0, -xx * 0.5f, -yy * 0.5f);
					fz_rect bbox = fz_transform_rect(bound, m1);
					auto dx = bbox.x1 - bbox.x0;
					auto dy = bbox.y1 - bbox.y0;
					auto zoomx = static_cast<float>(area_.size.x) / dx;
					auto zoomy = static_cast<float>(area_.size.y) / dy;
					zoom = std::min(zoomx, zoomy);
				} else if(area_.atype == area_type::ZOOM) {
					zoom = area_.zoom;
				}
				fz_matrix m0 = fz_scale(zoom, zoom);
				fz_matrix m1 = fz_pre_rotate(m0, area.rotation);
				fz_matrix page_mat = fz_pre_translate(m1, -xx * 0.5f, -yy * 0.5f);

				fz_irect tmp = fz_round_rect(fz_transform_rect(bound, page_mat));
				int w = tmp.x1 - tmp.x0;
				int h = tmp.y1 - tmp.y0;

				auto im = new img_rgba8;
				im->create(vtx::spos(w, h), true);

				int alpha = 1;
				int stride = w * 4;
				auto rgba = reinterpret_cast<unsigned char*>(im->at_image());
				fz_pixmap* pix = fz_new_pixmap_with_data(context_, fz_device_rgb(context_), w, h, NULL, alpha, stride, rgba);
				fz_clear_pixmap_with_value(context_, pix, 0xff);
				fz_matrix pix_mat = fz_translate(w * 0.5f, h * 0.5f);
				fz_device* dev = fz_new_draw_device(context_, pix_mat, pix);
				fz_run_page(context_, page, dev, page_mat, NULL);
				fz_drop_device(context_, dev);
				fz_drop_pixmap(context_, pix);
				img_ = shared_img(im);
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
				OUTLINES tmp;
				tmp.swap(outlines_);
			}

			if(document_ != nullptr) {
				fz_drop_document(context_, document_);
				document_ = nullptr;
			}
			page_count_ = 0;
			page_current_ = -1;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	PDF のページ数を取得
			@return 最大ページ数
		*/
		//-----------------------------------------------------------------//
		int get_page_limit() const noexcept {
			if(context_ == nullptr || document_ == nullptr) {
				return 0;
			}
			return page_count_;
		}


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
				context_ = nullptr;
			}
		}
	};
}
