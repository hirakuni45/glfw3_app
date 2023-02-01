//=====================================================================//
/*!	@file
	@brief	PDF ファイル（入力のみ）を扱うクラス
    @author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2017 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/glfw3_app/blob/master/LICENSE
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
			int stride = w;
			fz_pixmap* pix = fz_new_pixmap_with_data(context_, fz_device_rgb(context_), w, h, seps, alpha, stride, (unsigned char*)img_.at_image());
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
	void pdf_in::close()
	{
		if(outline_) {
			fz_drop_outline(context_, outline_);
			outline_ = 0;
		}

		if(document_) {
			fz_drop_document(context_, document_);
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

		fz_drop_context(context_);
		context_ = 0;
	}
}
