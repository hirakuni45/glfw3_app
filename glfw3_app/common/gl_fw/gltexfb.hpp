#pragma once
//=====================================================================//
/*!	@file
	@brief	OpenGL テクスチャー・フレーム・バッファ・クラス（ヘッダー）@n
			テクスチャーを２枚初期化して、それをダブルバッファとして@n
			使い、ビットマップの動画表示などを行う。@n
			24(RGB)、32(RGBA) ビットの表示モードに対応。
    @author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2017 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/glfw3_app/blob/master/LICENSE
*/
//=====================================================================//
#include <vector>
#include "gl_fw/gl_info.hpp"
#include "utils/vtx.hpp"

namespace gl {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	texfb クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	struct texfb {

		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		/*!
			@brief	レンダリングのソースフォーマット
		*/
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		enum class IMAGE {
			INDEXED,	///< 8 ビットのインデックスドカラー
			GRAY,		///< 8 ビットのグレースケール
			RGBA4444,	///< RGBA4444 16 ビットカラー画像
			RGB,		///< RGB 24 ビットカラー画像
			RGBA,		///< RGBA 32 ビットカラー画像
			BGR,		///< BGR 24 ビットカラー画像（BGR オーダー）
		};

	private:
		uint32_t	frame_count_;

		int			disp_page_;
		GLuint		tex_type_;
		int			tex_depth_;

		vtx::ipos	disp_start_;
		vtx::ipos	disp_size_;
		vtx::ipos	tex_size_;

		struct tex_page {
			union {
				GLuint	ids_[2];
				struct {
					GLuint	fore_;
					GLuint	back_;
				};
			};
			tex_page(GLuint fore, GLuint back) : fore_(fore), back_(back) { }
			tex_page() : fore_(0), back_(0) { }
		};
		tex_page	tex_id_;

		bool	h_flip_;
		bool	v_flip_;

		void draw_quad_(GLuint tex_id)
		{
			glEnable(GL_TEXTURE_2D);

			glBindTexture(GL_TEXTURE_2D, tex_id);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

			vtx::fpos uv_top;
			uv_top.x = static_cast<float>(disp_start_.x) / static_cast<float>(tex_size_.x - 1);
			uv_top.y = static_cast<float>(disp_start_.y) / static_cast<float>(tex_size_.y - 1);
			vtx::fpos uv_end;		
			uv_end.x = uv_top.x +
				static_cast<float>(disp_size_.x) / static_cast<float>(tex_size_.x);
			uv_end.y = uv_top.y +
				static_cast<float>(disp_size_.y) / static_cast<float>(tex_size_.y);

			GLfloat tu, bu;
			if(h_flip_) { tu = uv_end.x; bu = uv_top.x; }
			else { tu = uv_top.x; bu = uv_end.x; }
			GLfloat tv, bv;
			if(v_flip_) { tv = uv_end.y; bv = uv_top.y; }
			else { tv = uv_top.y; bv = uv_end.y; }

			vtx::fpos tex[4];
			vtx::fpos vec[4];
			tex[0].set(tu, bv);
			vec[0].set(0.0f, static_cast<float>(disp_size_.y));
			tex[1].set(tu, tv);
			vec[1].set(0.0f, 0.0f);
			tex[2].set(bu, bv);
			vec[2].set(static_cast<float>(disp_size_.x), static_cast<float>(disp_size_.y));
			tex[3].set(bu, tv);
			vec[3].set(static_cast<float>(disp_size_.x), 0.0f);
			glEnableClientState(GL_TEXTURE_COORD_ARRAY);
			glEnableClientState(GL_VERTEX_ARRAY);
			glTexCoordPointer(2, GL_FLOAT, 0, &tex[0]);
			glVertexPointer(2, GL_FLOAT, 0, &vec[0]);
			glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
			glDisableClientState(GL_TEXTURE_COORD_ARRAY);
			glDisableClientState(GL_VERTEX_ARRAY);

			glDisable(GL_TEXTURE_2D);
		}


		void destroy_()
		{
			glDeleteTextures(2, tex_id_.ids_);
		}

	public:
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		/*!
			@brief	設定エラーコード一覧
		*/
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		enum class error {
			NONE = 0,		///< エラー無し
			WIDTH_OVER = 1,	///< 設定できるテクスチャーの横幅を超えた
			HEIGHT_OVER,	///< 設定できるテクスチャーの高さを超えた
			DEPTH,			///< 設定できるテクスチャーの色深度が無い
		};


		//-----------------------------------------------------------------//
		/*!
			@brief	コンストラクター
		*/
		//-----------------------------------------------------------------//
		texfb() : frame_count_(0),
			disp_page_(0), tex_type_(0), tex_depth_(0),
			disp_start_(0, 0), disp_size_(0, 0), tex_size_(0, 0),
			tex_id_(0, 0),
			h_flip_(false), v_flip_(false)
		{ }


		//-----------------------------------------------------------------//
		/*!
			@brief	デストラクター
		*/
		//-----------------------------------------------------------------//
		~texfb() { destroy_(); }


		//-----------------------------------------------------------------//
		/*!
			@brief	テクスチャー・フレーム・バッファの設定
			@param[in]	width	フレーム・バッファの横幅（最大５１２）
			@param[in]	height	フレーム・バッファの高さ（最大５１２）
			@param[in]	depth	フレーム・バッファの色深度（１６、２４、３２）
			@return				成功すると、error::NONE が返る
							それ以外の場合はエラー
		*/
		//-----------------------------------------------------------------//
		error initialize(int width, int height, int depth)
		{
			glDeleteTextures(2, tex_id_.ids_);

			int tw = width;
			int th = height;

			// テクスチャーのサイズは、２のｎ乗サイズにする(OpenGL 1.1準拠)
			// ※最大５１２ピクセル
#if 0
			if(width <= 64) tw = 64;
			else if(width <= 128) tw = 128;
			else if(width <= 256) tw = 256;
			else if(width <= 512) tw = 512;
			else {
				return error::WIDTH_OVER;
			}

			if(height <= 64) th = 64;
			else if(height <= 128) th = 128;
			else if(height <= 256) th = 256;
			else if(height <= 512) th = 512;
			else {
				return error::HEIGHT_OVER;
			}
#endif	
			if(depth == 4) {
				tex_depth_ = 4;
			} else if(depth == 8) {
				tex_depth_ = 8;
			} else if(depth == 16) {
#ifdef OPENGL_ES
				tex_type_ = GL_RGBA4_OES;
#else
				tex_type_ = GL_RGBA4;
#endif
				tex_depth_ = 16;
			} else if(depth == 24) {
				tex_type_ = GL_RGB;
				tex_depth_ = 24;
			} else if(depth == 32) {
				tex_type_ = GL_RGBA;
				tex_depth_ = 32;
			} else {
				return error::DEPTH;
			}

			// フレーム・バッファの要求サイズ
			disp_size_.x = width;
			disp_size_.y = height;

			// テクスチャーのサイズ
			tex_size_.x = tw;
			tex_size_.y = th;

			glGenTextures(2, tex_id_.ids_);

			std::vector<uint8_t> img;
			size_t sz = 4 * tex_size_.x * tex_size_.y;
			img.resize(sz, 128);
			for(int i = 0; i < 2; ++i) {
				glBindTexture(GL_TEXTURE_2D, tex_id_.ids_[i]);
				int level = 0;
				int border = 0;
				glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
				glTexImage2D(GL_TEXTURE_2D, level, tex_type_, tw, th, border,
					GL_RGBA, GL_UNSIGNED_BYTE, &img.front());
			}
			return error::NONE;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	モーション・オブジェクト描画用マトリックスの設定
			@param[in]	x	開始位置 X
			@param[in]	y	開始位置 Y
			@param[in]	w	横幅の指定
			@param[in]	h	高さの指定
			@param[in]	zn	Z (手前)
			@param[in]	zf	Z (奥)
		 */
		//-----------------------------------------------------------------//
		void setup_matrix(int x, int y, int w, int h, float zn = -1.0f, float zf = 1.0f)
		{
			glMatrixMode(GL_TEXTURE);
			glLoadIdentity();
			glMatrixMode(GL_PROJECTION);
			glLoadIdentity();
			glOrthof(static_cast<float>(x), static_cast<float>(w),
					 static_cast<float>(h), static_cast<float>(y), zn, zf);
			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();
		}


		//-----------------------------------------------------------------//
		/*!
			@brief		フレームバッファのの表示開始位置指定
			@param[in]	size	表示開始位置
		*/
		//-----------------------------------------------------------------//
		void set_disp_start(const vtx::ipos& start) { disp_start_ = start; }


		//-----------------------------------------------------------------//
		/*!
			@brief		フレームバッファの表示サイズ指定
			@param[in]	size	表示サイズ
		*/
		//-----------------------------------------------------------------//
		void set_disp_size(const vtx::ipos& size) {
			int width = size.x;
			int height = size.y;
#if 0
			int tw, th;
			if(width <= 64) tw = 64;
			else if(width <= 128) tw = 128;
			else if(width <= 256) tw = 256;
			else tw = 512;
			if(height <= 64) th = 64;
			else if(height <= 128) th = 128;
			else if(height <= 256) th = 256;
			else th = 512;
#endif
			disp_size_.set(width, height);
//			tex_size_.set(tw, th);
		}



		//-----------------------------------------------------------------//
		/*!
			@brief		反転画像の設定
			@param[in]	hf	「true」なら水平反転
			@param[in]	vf	「true」なら垂直変転
		*/
		//-----------------------------------------------------------------//
		void set_flip(bool hf, bool vf) { h_flip_ = hf; v_flip_ = vf; }


		//-----------------------------------------------------------------//
		/*!
			@brief	テクスチャー ID を取得
			@return テクスチャー ID
		*/
		//-----------------------------------------------------------------//
		GLuint get_texture_id() const {
			return tex_id_.ids_[disp_page_];
		}


		//-----------------------------------------------------------------//
		/*!
			@brief		テクスチャー・フレーム・バッファ・サービス@n
						※OpenGL 描画ループの中で、毎フレーム呼ぶ事
		*/
		//-----------------------------------------------------------------//
		void draw()
		{
			draw_quad_(tex_id_.ids_[disp_page_]);
			++frame_count_;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief		テクスチャー・フレーム・バッファ・ページ・フリップ@n
						※このフレームの次のフレームで表示される。
			@param[in]	scale	描画するテクスチャーポリゴンの表示スケール
		*/
		//-----------------------------------------------------------------//
		void flip()
		{
			disp_page_ ^= 1;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief		テクスチャー・フレーム・バッファ・レンダリング @n
						※RGBA8 の場合には変換が起こらない為高速
			@param[in]	srct	ソース・イメージのタイプ（RGB、RGBA、BGR）
			@param[in]	img		ソース・イメージのポインター
			@param[in]	alpha	24 -> 32 ビットフォーマット変換時のアルファ値
		*/
		//-----------------------------------------------------------------//
		void rendering(IMAGE srct, const void* img, int alpha = 255)
		{
			if(img == 0) return;

			// GL_RGB 又は、GL_RGBA への変換（必要な場合）
			uint8_t* dst = 0;
			GLuint src_type = GL_RGBA;
			if(tex_depth_ == 4) {
			} else if(tex_depth_ == 8) {
			} else if(tex_depth_ == 16) {		// RGBA4
			} else if(tex_depth_ == 24) {		// RGB8
				src_type = GL_RGB;
				if(srct == IMAGE::GRAY) {
					std::vector<uint8_t> dst;
					dst.resize(disp_size_.x * disp_size_.y * 3);
					const uint8_t* im = static_cast<const uint8_t*>(img);
					for(int y = 0; y < disp_size_.y; ++y) {
						uint8_t* p = &dst[y * disp_size_.x * 3];
						for(int x = 0; x < disp_size_.x; ++x) {
							char g = *im++;
							*p++ = g;
							*p++ = g;
							*p++ = g;
						}
					}
				} else if(srct == IMAGE::RGB) {
					// 変換の必要無し
				} else if(srct == IMAGE::RGBA) {
					dst = new uint8_t[disp_size_.x * disp_size_.y * 3];
					const uint8_t* im = static_cast<const uint8_t*>(img);
					for(int y = 0; y < disp_size_.y; ++y) {
						uint8_t* p = &dst[y * disp_size_.x * 3];
						for(int x = 0; x < disp_size_.x; ++x) {
							*p++ = *im++;
							*p++ = *im++;
							*p++ = *im++;
							im++;
						}
					}
				} else if(srct == IMAGE::BGR) {
					std::vector<uint8_t> dst;
					dst.resize(disp_size_.x * disp_size_.y * 3);
					const uint8_t* im = static_cast<const uint8_t*>(img);
					for(int y = 0; y < disp_size_.y; ++y) {
						uint8_t* p = &dst[y * disp_size_.x * 3];
						for(int x = 0; x < disp_size_.x; ++x) {
							p[2] = *im++;
							p[1] = *im++;
							p[0] = *im++;
							p += 3;
						}
					}
				}
			} else if(tex_depth_ == 32) {		// RGBA8
				if(srct == IMAGE::GRAY) {
					std::vector<uint8_t> dst;
					dst.resize(disp_size_.x * disp_size_.y * 4);
					const uint8_t* im = static_cast<const uint8_t*>(img);
					for(int y = 0; y < disp_size_.y; ++y) {
						uint8_t* p = &dst[y * disp_size_.x * 4];
						for(int x = 0; x < disp_size_.x; ++x) {
							char g = *im++;
							*p++ = g;
							*p++ = g;
							*p++ = g;
							*p++ = alpha;
						}
					}
				} else if(srct == IMAGE::RGB) {
					std::vector<uint8_t> dst;
					dst.resize(disp_size_.x * disp_size_.y * 4);
					const uint8_t* im = static_cast<const uint8_t*>(img);
					for(int y = 0; y < disp_size_.y; ++y) {
						uint8_t* p = &dst[y * disp_size_.x * 4];
						for(int x = 0; x < disp_size_.x; ++x) {
							*p++ = *im++;
							*p++ = *im++;
							*p++ = *im++;
							*p++ = alpha;
						}
					}
				} else if(srct == IMAGE::RGBA) {
					// 変換不要
				} else if(srct == IMAGE::BGR) {
					std::vector<uint8_t> dst;
					dst.resize(disp_size_.x * disp_size_.y * 4);
					const uint8_t* im = static_cast<const uint8_t*>(img);
					for(int y = 0; y < disp_size_.y; ++y) {
						uint8_t* p = &dst[y * disp_size_.x * 4];
						for(int x = 0; x < disp_size_.x; ++x) {
							p[3] = alpha;
							p[2] = *im++;
							p[1] = *im++;
							p[0] = *im++;
							p += 4;
						}
					}
				}
			}

			const void* src;
			if(dst) {
				src = dst;
			} else {
				src = img;
			}

			if(src) {
				glBindTexture(GL_TEXTURE_2D, tex_id_.ids_[disp_page_ ^ 1]);

				glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
				glTexSubImage2D(GL_TEXTURE_2D, 0,
					disp_start_.x, disp_start_.y, disp_size_.x, disp_size_.y,
					src_type, GL_UNSIGNED_BYTE, src);
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief		フレーム・バッファの横幅を得る
			@return		フレーム・バッファの横幅
		*/
		//-----------------------------------------------------------------//
		const vtx::ipos& get_size() const { return tex_size_; };


		//-----------------------------------------------------------------//
		/*!
			@brief		フレーム・バッファの色深度を得る
			@return		フレーム・バッファの色深度
		*/
		//-----------------------------------------------------------------//
		int get_depth() const { return tex_depth_; };


		//-----------------------------------------------------------------//
		/*!
			@brief		フレーム・カウント数を得る
			@return		フレーム・カウント数
		*/
		//-----------------------------------------------------------------//
		uint32_t get_frame_count() const { return frame_count_; };

	};

}
