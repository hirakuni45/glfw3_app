//=====================================================================//
/*!	@file
	@brief	PDF ファイル（入力のみ）を扱うクラス
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include <iostream>
#include "pdf_in.hpp"

namespace img {

	using namespace utils;

	//-----------------------------------------------------------------//
	/*!
		@brief	初期化
	*/
	//-----------------------------------------------------------------//
	void pdf_in::initialize()
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
	bool pdf_in::open(const std::string& filename)
	{
		close();

		document_ = fz_open_document(context_, filename.c_str());
		if(document_ == 0) {
			return false;
		}

		// パスワードが必要な場合
		char* password = 0;
		if (fz_needs_password(document_)) {
			if (!fz_authenticate_password(document_, password)) {
				fz_close_document(document_);
				return false;
			}
		}

		// ドキュメントのページ数を取得
		page_count_ = fz_count_pages(document_);
		page_no_ = 0;
		page_current_ = -1;

		outline_ = fz_load_outline(document_);
		if(outline_) {
			doctitle_ = outline_->title;
		}
// std::cout << boost::format("PDF Pages: %d\n") % page_count_;

		return true;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	ページをレンダリングする
		@param[in]	size	レンダリングサイズ
		@return 正常終了なら「true」
	*/
	//-----------------------------------------------------------------//
	bool pdf_in::render(const vtx::spos& size)
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
		page = fz_load_page(document_, page_no_);

		fz_matrix transform;
		fz_rotate(&transform, rotation_);
		fz_pre_scale(&transform, 1.0f, 1.0f);

		fz_rect bounds;
		fz_bound_page(document_, page, &bounds);
		fz_transform_rect(&bounds, &transform);
		fz_irect bbox;
		fz_round_rect(&bbox, &bounds);
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

		fz_rotate(&transform, rotation_);
		fz_pre_scale(&transform, zoom, zoom);
		fz_bound_page(document_, page, &bounds);
		fz_transform_rect(&bounds, &transform);
		fz_round_rect(&bbox, &bounds);
		{
			int w = bbox.x1 - bbox.x0;
			int h = bbox.y1 - bbox.y0;
// std::cout << boost::format("size: %d, %d\n") % w % h;
			// fz_device_rgb ---> RGBA
			img_.create(vtx::spos(w, h), true);

			fz_pixmap* pix = fz_new_pixmap_with_data(context_, fz_device_rgb(context_),
				w, h, (unsigned char*)img_.at_image());
			fz_clear_pixmap_with_value(context_, pix, 0xff);

			fz_device *dev = fz_new_draw_device(context_, pix);
			fz_run_page(document_, page, dev, &transform, NULL);
			fz_free_device(dev);

			fz_drop_pixmap(context_, pix);
		}
		fz_free_page(document_, page);

		return true;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	PDF をクローズする
	*/
	//-----------------------------------------------------------------//
	void pdf_in::close()
	{
		if(outline_) {
			fz_free_outline(context_, outline_);
			outline_ = 0;
		}

		if(document_) {
			fz_close_document(document_);
			document_ = 0;
		}
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	廃棄
	*/
	//-----------------------------------------------------------------//
	void pdf_in::destroy()
	{
		close();

		fz_free_context(context_);
		context_ = 0;
	}
}
