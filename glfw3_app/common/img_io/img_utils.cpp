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
				for(int y = 0; y < src->get_size().y; ++y) {
					for(int x = 0; x < src->get_size().x; ++x) {
						idx8 c;
						src->get_pixel(x, y, c);
						tmp->put_pixel(x, y, c);
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
				for(int y = 0; y < src->get_size().y; ++y) {
					for(int x = 0; x < src->get_size().x; ++x) {
						gray8 c;
						src->get_pixel(x, y, c);
						dst->put_pixel(x, y, c);
					}
				}
			}
			break;
		case IMG::FULL8:
			{
				dst = dynamic_cast<i_img*>(new img_rgba8);
				dst->create(src->get_size(), src->test_alpha());
				for(int y = 0; y < src->get_size().y; ++y) {
					for(int x = 0; x < src->get_size().x; ++x) {
						rgba8 c;
						src->get_pixel(x, y, c);
						dst->put_pixel(x, y, c);
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
		@param[in]	src		ソースのイメージインターフェース
		@param[in]	src_x	ソース画像の開始点 X
		@param[in]	src_y	ソース画像の開始点 Y
		@param[in]	src_w	ソース画像の横幅
		@param[in]	src_h	ソース画像の高さ
		@param[in]	dst		コピー先 RGBA8 イメージ（リファレンス）
		@param[in]	dst_x	コピー先位置 X
		@param[in]	dst_y	コピー先位置 Y
	*/
	//-----------------------------------------------------------------//
	void copy_to_rgba8(const i_img* src, int src_x, int src_y, int src_w, int src_h, img_rgba8& dst, int dst_x, int dst_y)
	{
		if(src == 0) return;
		for(int y = 0; y < src_h; ++y) {
			for(int x = 0; x < src_w; ++x) {
				rgba8 c;
				src->get_pixel(src_x + x, src_y + y, c);
				dst.put_pixel(dst_x + x, dst_y + y, c);
			}
		}
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	IDX8 イメージへコピーする
		@param[in]	src		ソースのイメージ
		@param[in]	src_x	ソース画像の開始点 X
		@param[in]	src_y	ソース画像の開始点 Y
		@param[in]	src_w	ソース画像の横幅
		@param[in]	src_h	ソース画像の高さ
		@param[in]	dst		コピー先 RGBA8 イメージ（リファレンス）
		@param[in]	dst_x	コピー先位置 X
		@param[in]	dst_y	コピー先位置 Y
		@return コピーに成功したら「true」を返す。
	*/
	//-----------------------------------------------------------------//
	bool copy_to_idx8(const i_img* src, int src_x, int src_y, int src_w, int src_h, img_idx8& dst, int dst_x, int dst_y)
	{
		if(src->get_type() != IMG::INDEXED8) return false;
		for(int y = 0; y < src_h; ++y) {
			for(int x = 0; x < src_w; ++x) {
				idx8 ix;
				src->get_pixel(src_x + x, src_y + y, ix);
				dst.put_pixel(dst_x + x, dst_y + y, ix);
			}
		}
		return true;
	}


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	lanczos-3 アルゴリズム、重みの計算
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	static const float g_pi = 3.14159265358979f;

	static float sinc(float l)
	{
		return sinf(g_pi * l) / (g_pi * l);
	}

	static float lanczos(float d, float n)
	{
		if(d == 0.0f) {
			return 1.0f;
		} else if(fabs(d) >= n) {
			return 0.0f;
		} else {
			return sinc(d) * sinc(d / n);
		}
	}

	static float g_lanczos_tbl[(12 + 1) * (12 + 1)];
	static float g_lanczos_n = 0.0f;
	// -3.0, -2.5, -2.0, -1.5, -1.0, -0.5, 0.0, 0.5, 1.0, 1.5, 2.0, 2.5, 3.0
	static void init_lanczos(float n)
	{
		if(g_lanczos_n == n) return;

		float y = -3.0f;
		for(int i = 0; i < (12 + 1); ++i) {
			float yl = lanczos(y, n);
			float x = -3.0f;
			for(int j = 0; j < (12 + 1); ++j) {
				float xl = lanczos(x, n);
				g_lanczos_tbl[i * (12 + 1) + j] = xl * yl;
				x += 0.5f;
			}
			y += 0.5f;
		}
		g_lanczos_n = n;
	}


	static float lanczos_t(float x, float y, float n)
	{
		int i = static_cast<int>(y * 2.0f);
		i += 6;
		int j = static_cast<int>(x * 2.0f);
		j += 6;
		if(i >= 0 && i < (12 + 1) && j >= 0 && j < (12 + 1)) {
			return g_lanczos_tbl[i * (12 + 1) + j];
		} else {
			return lanczos(x, n) * lanczos(y, n);
		}
	}


	static void color_div(const rgbaf& col, float total, rgba8& c)
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
		init_lanczos(n);

		float scn = 1.0f / scale;
		if(scale > 1.0f) {
			for(int h = 0; h < dh; ++h) {
				float yy = (static_cast<float>(h) + 0.5f) * scn;
				int ys = static_cast<int>(yy - n);
				if(ys < 0) ys = 0;
				int ye = static_cast<int>(yy + n);
				if(ye > (sh - 1)) ye = sh - 1;
				for(int w = 0; w < dw; ++w) {
					float xx = (static_cast<float>(w) + 0.5f) * scn;
					int xs = static_cast<int>(xx - n);
					if(xs < 0) xs = 0;
					int xe = static_cast<int>(xx + n);
					if(xe > (sw - 1)) xe = sw - 1;

					rgbaf col(0.0f);
					float weight_total = 0.0f;
					for(int y = ys; y <= ye; ++y) {
						float yl = fabs((y + 0.5f) - yy);
						for(int x = xs; x <= xe; ++x) {
							float xl = fabs((x + 0.5f) - xx);
							float weight = lanczos_t(xl, yl, n);
							rgba8 c;
							src->get_pixel(x, y, c);
							col.r += c.r * weight;
							col.g += c.g * weight;
							col.b += c.b * weight;
							col.a += c.a * weight;
							weight_total += weight;
						}
					}
					rgba8 c;
					color_div(col, weight_total, c);
					dst.put_pixel(w, h, c);
				}
			}
		} else {
			for(int h = 0; h < dh; ++h) {
				float yy = static_cast<float>(h) + 0.5f;
				int ys = static_cast<int>((yy - n) * scn);
				if(ys < 0) ys = 0;
				int ye = static_cast<int>((yy + n) * scn);
				if(ye > (sh - 1)) ye = sh - 1;
				for(int w = 0; w < dw; ++w) {
					float xx = static_cast<float>(w) + 0.5f;
					int xs = static_cast<int>((xx - n) * scn);
					if(xs < 0) xs = 0;
					int xe = static_cast<int>((xx + n) * scn);
					if(xe > (sw - 1)) xe = sw - 1;

					rgbaf col(0.0f);
					float weight_total = 0.0f;
					for(int y = ys; y <= ye; ++y) {
						float yl = fabs((((float)y + 0.5f) * scale) - yy);
						for(int x = xs; x <= xe; ++x) {
							float xl = fabs((((float)x + 0.5f) * scale) - xx);
							float weight = lanczos_t(xl, yl, n);
							rgba8 c;
							src->get_pixel(x, y, c);
							col.r += (float)c.r * weight;
							col.g += (float)c.g * weight;
							col.b += (float)c.b * weight;
							col.a += (float)c.a * weight;
							weight_total += weight;
						}
					}
					rgba8 c;
					color_div(col, weight_total, c);
					dst.put_pixel(w, h, c);
				}
			}
		}
	}


}
