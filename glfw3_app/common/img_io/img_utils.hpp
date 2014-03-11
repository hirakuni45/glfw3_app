#ifndef IMG_UTILS_HPP
#define IMG_UTILS_HPP
//=====================================================================//
/*!	@file
	@brief	イメージ・ユーティリティー
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include "img.hpp"
#include "i_img.hpp"
#include "img_idx8.hpp"
#include "img_gray8.hpp"
#include "img_rgba8.hpp"

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
	inline bool copy_to_idx8_clut(const i_img* src, img_idx8& dst) {
		if(src->get_type() != IMG::INDEXED8) return false;
		for(int i = 0; i < src->get_clut_max(); ++i) {
			rgba8 c;
			src->get_clut(i, c);
			dst.put_clut(i, c);
		}
		return true;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	IDX8 のカラー・ルック・アップ・テーブルが同じか検査
		@param[in]	src		ソースのイメージ
		@param[in]	dst		比較の IDX8 イメージ（リファレンス）
		@return 同じなら「true」を返す
	*/
	//-----------------------------------------------------------------//
	inline bool match_idx8_clut(const i_img* src, img_idx8& dst) {
		if(src->get_type() != IMG::INDEXED8) return false;
		if(src->get_clut_max() < dst.get_clut_max()) return false;
		for(int i = 0; i < src->get_clut_max(); ++i) {
			rgba8 sc;
			src->get_clut(i, sc);
			rgba8 dc;
			dst.get_clut(i, dc);
			if(sc != dc) return false;
		}
		return true;
	}


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
	inline void scale_50percent(const i_img* src, img_rgba8& dst) {
		if(src == 0) return;

		dst.destroy();
		const vtx::spos& size = src->get_size();
		dst.create(size / 2, src->test_alpha());
		vtx::spos p;
		for(p.y = 0; p.y < size.y; p.y += 2) {
			for(p.x = 0; p.x < size.x; p.x += 2) {
				int	r, g, b, a;
			  	rgba8	c;
				src->get_pixel(p, c);
				r = c.r; g = c.g; b = c.b; a = c.a;
				src->get_pixel(vtx::spos(p.x + 1, p.y), c);
				r += c.r; g += c.g; b += c.b; a += c.a;
				src->get_pixel(vtx::spos(p.x, p.y + 1), c);
				r += c.r; g += c.g; b += c.b; a += c.a;
				src->get_pixel(vtx::spos(p.x + 1, p.y + 1), c);
				r += c.r; g += c.g; b += c.b; a += c.a;
				c.r = r >> 2; c.g = g >> 2; c.b = b >> 2; c.a = a >> 2;
				dst.put_pixel(vtx::spos(p.x / 2, p.y / 2), c);
			}
		}
	}


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
#endif	// IMG_UTILS_HPP
