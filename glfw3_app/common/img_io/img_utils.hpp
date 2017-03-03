#pragma once
//=====================================================================//
/*!	@file
	@brief	イメージ・ユーティリティー @n
			Copyright 2017 Kunihito Hiramatsu
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include "img_io/img.hpp"
#include "img_io/i_img.hpp"
#include "img_io/img_idx8.hpp"
#include "img_io/img_gray8.hpp"
#include "img_io/img_rgba8.hpp"
#include "img_io/perlin_noise.hpp"

namespace img {

	//-----------------------------------------------------------------//
	/*!
		@brief	ソース・イメージのコピーを作成
		@param[in]	src		ソースのイメージインターフェース
		@param[in]	opt		カラーパレットの最適化（１６色以下の場合）
		@return 作成したイメージインターフェースクラス
	*/
	//-----------------------------------------------------------------//
	i_img* copy_image(const i_img* src, bool opt = false);


	//-----------------------------------------------------------------//
	/*!
		@brief	RGBA8 イメージへコピーする
		@param[in]	isrc	ソースのイメージインターフェース
		@param[in]	rect	ソース画像領域
		@param[in]	idst	コピー先 RGBA8 イメージ（リファレンス）
		@param[in]	pos	コピー先位置
	*/
	//-----------------------------------------------------------------//
	void copy_to_rgba8(const i_img* isrc, const vtx::srect& rect, img_rgba8& idst, const vtx::spos& pos);


	//-----------------------------------------------------------------//
	/*!
		@brief	IDX8 のカラー・ルック・アップ・テーブルをコピー
		@param[in]	src		ソースのイメージ
		@param[in]	dst		コピー先 RGBA8 イメージ（リファレンス）
	*/
	//-----------------------------------------------------------------//
	bool copy_to_idx8_clut(const i_img* src, img_idx8& dst);


	//-----------------------------------------------------------------//
	/*!
		@brief	IDX8 のカラー・ルック・アップ・テーブルが同じか検査
		@param[in]	src		ソースのイメージ
		@param[in]	dst		比較の IDX8 イメージ（リファレンス）
		@return 同じなら「true」を返す
	*/
	//-----------------------------------------------------------------//
	bool match_idx8_clut(const i_img* src, img_idx8& dst);


	//-----------------------------------------------------------------//
	/*!
		@brief	IDX8 イメージへコピーする
		@param[in]	isrc   	ソースのイメージインターフェース
		@param[in]	rect   	ソース画像
		@param[in]	idst   	コピー先 RGBA8 イメージ（リファレンス）
		@param[in]	pos   	コピー先位置
		@return コピーに成功したら「true」を返す。
	*/
	//-----------------------------------------------------------------//
	bool copy_to_idx8(const i_img* isrc, const vtx::srect& rect, img_idx8& idst, const vtx::spos& pos);


	//-----------------------------------------------------------------//
	/*!
		@brief	RGBA8 イメージへコピーする
		@param[in]	isrc	ソースのイメージインターフェース
		@param[in]	dst		コピー先 RGBA8 イメージ（リファレンス）
		@param[in]	pos		コピー先位置
	*/
	//-----------------------------------------------------------------//
	inline void copy_to_rgba8(const i_img* isrc, img_rgba8& dst, const vtx::spos& pos = vtx::spos(0)) {
		if(isrc == 0) return;
		copy_to_rgba8(isrc, vtx::srect(vtx::spos(0), isrc->get_size()), dst, pos);
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	IDX8 イメージへコピーする
		@param[in]	isrc	ソースのイメージ
		@param[in]	dst		コピー先 IDX8 イメージ（リファレンス）
		@param[in]	pos		コピー先位置
		@return コピーに成功したら「true」を返す。
	*/
	//-----------------------------------------------------------------//
	inline bool copy_to_idx8(const i_img* isrc, img_idx8& dst, const vtx::spos& pos = vtx::spos(0)) {
		return copy_to_idx8(isrc, vtx::srect(vtx::spos(0), isrc->get_size()), dst, pos);
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	50% にリサイズされた画像イメージを生成する
		@param[in]	src	ソースのイメージ
		@param[out]	dst	リサイズイメージ
	*/
	//-----------------------------------------------------------------//
	void scale_50percent(const i_img* src, img_rgba8& dst);


	//-----------------------------------------------------------------//
	/*!
		@brief	画像をリサイズする（lanczos-3 アルゴリズム）
		@param[in]	src	ソースのイメージ
		@param[out]	dst	リサイズイメージ
		@param[in]	scale	スケール・ファクター
	*/
	//-----------------------------------------------------------------//
	void resize_image(const i_img* src, img_rgba8& dst, float scale);
}
