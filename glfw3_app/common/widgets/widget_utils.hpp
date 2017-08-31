#pragma once
//=====================================================================//
/*!	@file
	@brief	GUI widget ユーティリティー（ヘッダー）
    @author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2017 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/glfw_app/blob/master/LICENSE
*/
//=====================================================================//
#include <string>
#include "widgets/widget.hpp"
#include "widgets/widget_director.hpp"
#include "utils/vmath.hpp"
#include "utils/string_utils.hpp"
#include "gl_fw/glutils.hpp"

namespace gui {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	画像を作成
		@param[in]	image	画像
		@param[in]	size	サイズ
		@param[out]	pa		ペイントクラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	void create_image_base(const img::i_img* image, const vtx::ipos& size, img::paint& pa);


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	ラウンド・フレームの作成
		@param[out]	pa		ペイント・フレーム
		@param[in]	pp		プレート・パラメーター
		@param[in]	cp		カラー・パラメーター
		@param[in]	size	サイズ
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	void create_round_frame(img::paint& pa, const widget::plate_param& pp,
		const widget::color_param& cp, const vtx::ipos& size);


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	画像を作成
		@param[in]	wd		Widget ディレクター
		@param[in]	file	画像ファイル名
		@param[in]	rect	位置と大きさ（サイズ０指定で画像サイズで作成）
		@param[in]	parents	ペアレント
		@return 画像ボタン
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	template <class WIDGET>
	WIDGET* create_image(widget_director& wd, const std::string& file,
		const vtx::irect& rect = vtx::irect(0), widget* parents = 0) {
		using namespace gui;

		WIDGET* w;
		widget::param wp(rect, parents);
		if(wd.at_img_files().load(file)) {
			typename WIDGET::param wp_;
			wp_.image_ = wd.at_img_files().get_image().get();
			w = wd.add_widget<WIDGET>(wp, wp_);
		} else {
			wd.add_error_report("Can't load: " + file);
			typename WIDGET::param wp_("X");
			// ロード出来ない場合の仮の大きさ
			wp.rect_.size.set(32, 32);
			w = wd.add_widget<WIDGET>(wp, wp_);
		}
		return w;
	}


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	テキストを設定
		@param[in]	w	ウィジェット
		@param[in]	text	テキスト
		@return 失敗なら「false」
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	template <class W>
	void set_widget_text(W* w, const std::string& text) {
		w->at_local_param().text_param_.set_text(text);
	}


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	親子関係を継承した位置を合算する
		@param[in]	root	起点になるウィジェット
		@param[out]	pos		位置を受け取る参照
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	void final_position(const widget* root, vtx::ipos& pos);


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	親子関係を継承したクリップ領域を得る
		@param[in]	root	起点になるウィジェット
		@param[out]	org		基点を受け取る参照
		@param[out]	rect	領域を受け取る参照
		@return クリップ内なら「true」
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	bool final_clip(widget* root, vtx::ipos& org, vtx::irect& rect);


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	オブジェクトの描画
		@param[in]	wd		Widget ディレクター
		@param]in]	h		オブジェクト・ハンドル
		@param[in]	clip	描画エリア
		@param[in]	ofs		描画オフセット
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	void draw_mobj(widget_director& wd, gl::mobj::handle h, const vtx::irect& clip, const vtx::ipos& ofs = vtx::ipos(0));


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	フォントの描画
		@param[in]	tp	テキスト・パラメーター
		@param[in]	rect   	描画位置
		@param[in]	clip	描画エリア
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	void draw_text(const widget::text_param& tp, const vtx::irect& rect, const vtx::irect& clip);


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	テキスト・レンダリング
		@param[in]	wd	widget_director
		@param[in]	oh	object ハンドル
		@param[in]	wp	widget パラメーター
		@param[in]	tp	テキスト・パラメーター
		@param[in]	pp	プレート・パラメーター
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	void render_text(widget_director& wd, gl::mobj::handle oh, const widget::param& wp, const widget::text_param& tp, const widget::plate_param& pp);


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	フレームの初期化
		@param[in]	wd	widget_director
		@param[in]	wp	widget::param
		@param[in]	pp	widget::plate_param
		@param[in]	cp	widget::color_param
		@return モーション・オブジェクトのハンドルを返す
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	gl::mobj::handle frame_init(widget_director& wd, widget::param& wp, const widget::plate_param& pp, const widget::color_param& cp);


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	クリッピングされたモーションオブジェクトのレンダリング
		@param[in]	mo	モーションオブジェクト
		@param[in]	moh	モーションオブジェクトハンドル
		@param[in]	clip	clip パラメーター
		@param[in]	ofs	描画オフセット
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	void render_clipped_mobj(gl::mobj& mo, gl::mobj::handle moh, const vtx::irect& clip,
		const vtx::ipos& ofs);


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	シフト・テキスト更新
		@param[in]	bp	widget ベースパラメーター
		@param[in]	tp	text パラメーター
		@param[in]	sp	shift パラメーター
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	void shift_text_update(const widget::param& bp, widget::text_param& tp, widget::shift_param& sp);


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	シフト・テキスト・レンダー
		@param[in]	bp	widget ベースパラメーター
		@param[in]	tp	text パラメーター
		@param[in]	pp	plate パラメーター
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	void shift_text_render(const widget::param& bp, const widget::text_param& tp, const widget::plate_param& pp);

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	ボーダーの描画
		@param[in]	rect   	描画位置とサイズ
		@param[in]	color	描画カラー
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	void draw_border(const vtx::srect& rect, const widget::color_param& color = widget_director::default_border_color_);
}
