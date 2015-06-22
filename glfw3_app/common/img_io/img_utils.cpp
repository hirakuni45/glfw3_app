//=====================================================================//
/*!	@file
	@brief	イメージ・ユーティリティー
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include "img_io/img_utils.hpp"

namespace img {

	//-----------------------------------------------------------------//
	/*!
		@brief	ソース・イメージのコピーを作成
		@param[in]	src		ソースのイメージインターフェース
		@param[in]	opt		カラーパレットの最適化（１６色以下の場合）
		@return 作成したイメージインターフェースクラス
	*/
	//-----------------------------------------------------------------//
	i_img* copy_image(const i_img* src, bool opt)
	{
		if(src == 0) return 0;
		i_img* dst = 0;
		switch(src->get_type()) {
		case IMG::INDEXED8:
			{
				img_idx8* tmp = new img_idx8;
				tmp->create(src->get_size(), src->test_alpha());
				for(int i = 0; i < src->get_clut_max(); ++i) {
					rgba8 c;
					src->get_clut(i, c);
					tmp->put_clut(i, c);
				}
				vtx::spos p;
				for(p.y = 0; p.y < src->get_size().y; ++p.y) {
					for(p.x = 0; p.x < src->get_size().x; ++p.x) {
						idx8 c;
						src->get_pixel(p, c);
						tmp->put_pixel(p, c);
					}
				}
				if(opt) {
					tmp->index_optimize();
				}
				dst = dynamic_cast<i_img*>(tmp);
			}
			break;
		case IMG::GRAY8:
			{
				dst = dynamic_cast<i_img*>(new img_gray8);
				dst->create(src->get_size(), false);
				vtx::spos p;
				for(p.y = 0; p.y < src->get_size().y; ++p.y) {
					for(p.x = 0; p.x < src->get_size().x; ++p.x) {
						gray8 c;
						src->get_pixel(p, c);
						dst->put_pixel(p, c);
					}
				}
			}
			break;
		case IMG::FULL8:
			{
				dst = dynamic_cast<i_img*>(new img_rgba8);
				dst->create(src->get_size(), src->test_alpha());
				vtx::spos p;
				for(p.y = 0; p.y < src->get_size().y; ++p.y) {
					for(p.x = 0; p.x < src->get_size().x; ++p.x) {
						rgba8 c;
						src->get_pixel(p, c);
						dst->put_pixel(p, c);
					}
				}
			}
			break;
		default:
			break;
		}
		return dst;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	RGBA8 イメージへコピーする
		@param[in]	isrc	ソースのイメージインターフェース
		@param[in]	rect	ソース画像領域
		@param[in]	idst	コピー先 RGBA8 イメージ（リファレンス）
		@param[in]	pos	コピー先位置
	*/
	//-----------------------------------------------------------------//
	void copy_to_rgba8(const i_img* isrc, const vtx::srect& rect, img_rgba8& idst, const vtx::spos& pos)
	{
		if(isrc == 0) return;
		vtx::spos p;
		for(p.y = 0; p.y < rect.size.y; ++p.y) {
			for(p.x = 0; p.x < rect.size.x; ++p.x) {
				rgba8 c;
				isrc->get_pixel(rect.org + p, c);
				idst.put_pixel(pos + p, c);
			}
		}
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	IDX8 のカラー・ルック・アップ・テーブルをコピー
		@param[in]	src		ソースのイメージ
		@param[in]	dst		コピー先 RGBA8 イメージ（リファレンス）
	*/
	//-----------------------------------------------------------------//
	bool copy_to_idx8_clut(const i_img* src, img_idx8& dst)
	{
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
	bool match_idx8_clut(const i_img* src, img_idx8& dst)
	{
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
	bool copy_to_idx8(const i_img* isrc, const vtx::srect& rect, img_idx8& idst, const vtx::spos& pos)
	{
		if(isrc->get_type() != IMG::INDEXED8) return false;
		vtx::spos p;
		for(p.y = 0; p.y < rect.size.y; ++p.y) {
			for(p.x = 0; p.x < rect.size.x; ++p.x) {
				idx8 ix;
				isrc->get_pixel(rect.org + p, ix);
				idst.put_pixel(pos + p, ix);
			}
		}
		return true;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	50% にリサイズされた画像イメージを生成する
		@param[in]	src	ソースのイメージ
		@param[out]	dst	リサイズイメージ
	*/
	//-----------------------------------------------------------------//
	void scale_50percent(const i_img* src, img_rgba8& dst)
	{
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


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	lanczos-3 アルゴリズム、重みの計算
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	static const float pi_ = 3.14159265358979f;

	static float sinc_(float l)
	{
		return std::sin(pi_ * l) / (pi_ * l);
	}

	static float lanczos_(float d, float n)
	{
		if(d == 0.0f) {
			return 1.0f;
		} else if(std::abs(d) >= n) {
			return 0.0f;
		} else {
			return sinc_(d) * sinc_(d / n);
		}
	}

	static float lanczos_tbl_[(12 + 1) * (12 + 1)];
	static float lanczos_n_ = 0.0f;
	// -3.0, -2.5, -2.0, -1.5, -1.0, -0.5, 0.0, 0.5, 1.0, 1.5, 2.0, 2.5, 3.0
	static void init_lanczos_(float n)
	{
		if(lanczos_n_ == n) return;

		float y = -3.0f;
		for(int i = 0; i < (12 + 1); ++i) {
			float yl = lanczos_(y, n);
			float x = -3.0f;
			for(int j = 0; j < (12 + 1); ++j) {
				float xl = lanczos_(x, n);
				lanczos_tbl_[i * (12 + 1) + j] = xl * yl;
				x += 0.5f;
			}
			y += 0.5f;
		}
		lanczos_n_ = n;
	}


	static float lanczos_t_(float x, float y, float n)
	{
		int i = static_cast<int>(y * 2.0f);
		i += 6;
		int j = static_cast<int>(x * 2.0f);
		j += 6;
		if(i >= 0 && i < (12 + 1) && j >= 0 && j < (12 + 1)) {
			return lanczos_tbl_[i * (12 + 1) + j];
		} else {
			return lanczos_(x, n) * lanczos_(y, n);
		}
	}


	static void color_div_(const rgbaf& col, float total, rgba8& c)
	{
		rgbaf cc;
		if(total != 0.0f) {
			float sf = 1.0f / total;
			cc.r = col.r * sf;
			cc.g = col.g * sf;
			cc.b = col.b * sf;
			cc.a = col.a * sf;
		} else {
			cc = col;
		}

		if(cc.r < 0.0f) {
			c.r = 0;
		} else if(cc.r > 255.0f) {
			c.r = 255;
		} else {
			c.r = static_cast<unsigned char>(cc.r);
		}

		if(cc.g < 0.0f) {
			c.g = 0;
		} else if(cc.g > 255.0f) {
			c.g = 255;
		} else {
			c.g = static_cast<unsigned char>(cc.g);
		}

		if(cc.b < 0.0f) {
			c.b = 0;
		} else if(cc.b > 255.0f) {
			c.b = 255;
		} else {
			c.b = static_cast<unsigned char>(cc.b);
		}

		if(cc.a < 0.0f) {
			c.a = 0;
		} else if(cc.a > 255.0f) {
			c.a = 255;
		} else {
			c.a = static_cast<unsigned char>(cc.a);
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
	void resize_image(const i_img* src, img_rgba8& dst, float scale)
	{
		if(scale <= 0.0f) return;

		int sw = src->get_size().x;
		int sh = src->get_size().y;
		int dw = static_cast<int>(static_cast<float>(sw) * scale);
		int dh = static_cast<int>(static_cast<float>(sh) * scale);
		dst.create(vtx::spos(dw, dh), src->test_alpha());

		float n = 3.0f;
		init_lanczos_(n);

		float scn = 1.0f / scale;
		if(scale > 1.0f) {
			vtx::spos out;
			for(out.y = 0; out.y < dh; ++out.y) {
				float yy = (static_cast<float>(out.y) + 0.5f) * scn;
				int ys = static_cast<int>(yy - n);
				if(ys < 0) ys = 0;
				int ye = static_cast<int>(yy + n);
				if(ye > (sh - 1)) ye = sh - 1;
				for(out.x = 0; out.x < dw; ++out.x) {
					float xx = (static_cast<float>(out.x) + 0.5f) * scn;
					int xs = static_cast<int>(xx - n);
					if(xs < 0) xs = 0;
					int xe = static_cast<int>(xx + n);
					if(xe > (sw - 1)) xe = sw - 1;

					rgbaf col(0.0f);
					float weight_total = 0.0f;
					vtx::spos pos;
					for(pos.y = ys; pos.y <= ye; ++pos.y) {
						float yl = fabs((pos.y + 0.5f) - yy);
						for(pos.x = xs; pos.x <= xe; ++pos.x) {
							float xl = std::abs((static_cast<float>(pos.x) + 0.5f) - xx);
							float weight = lanczos_t_(xl, yl, n);
							rgba8 c;
							src->get_pixel(pos, c);
							col.r += c.r * weight;
							col.g += c.g * weight;
							col.b += c.b * weight;
							col.a += c.a * weight;
							weight_total += weight;
						}
					}
					rgba8 c;
					color_div_(col, weight_total, c);
					dst.put_pixel(out, c);
				}
			}
		} else {
			vtx::spos out;
			for(out.y = 0; out.y < dh; ++out.y) {
				float yy = static_cast<float>(out.y) + 0.5f;
				int ys = static_cast<int>((yy - n) * scn);
				if(ys < 0) ys = 0;
				int ye = static_cast<int>((yy + n) * scn);
				if(ye > (sh - 1)) ye = sh - 1;
				for(out.x = 0; out.x < dw; ++out.x) {
					float xx = static_cast<float>(out.x) + 0.5f;
					int xs = static_cast<int>((xx - n) * scn);
					if(xs < 0) xs = 0;
					int xe = static_cast<int>((xx + n) * scn);
					if(xe > (sw - 1)) xe = sw - 1;

					rgbaf col(0.0f);
					float weight_total = 0.0f;
					vtx::spos pos;
					for(pos.y = ys; pos.y <= ye; ++pos.y) {
						float yl = std::abs(((static_cast<float>(pos.y) + 0.5f) * scale) - yy);
						for(pos.x = xs; pos.x <= xe; ++pos.x) {
							float xl = std::abs(((static_cast<float>(pos.x) + 0.5f) * scale) - xx);
							float weight = lanczos_t_(xl, yl, n);
							rgba8 c;
							src->get_pixel(pos, c);
							col.r += static_cast<float>(c.r) * weight;
							col.g += static_cast<float>(c.g) * weight;
							col.b += static_cast<float>(c.b) * weight;
							col.a += static_cast<float>(c.a) * weight;
							weight_total += weight;
						}
					}
					rgba8 c;
					color_div_(col, weight_total, c);
					dst.put_pixel(out, c);
				}
			}
		}
	}
}
