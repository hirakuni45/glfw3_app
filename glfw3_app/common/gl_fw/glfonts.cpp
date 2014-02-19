//=====================================================================//
/*!	@file
	@brief	OpenGL フォント・クラス（ヘッダー）
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include "glfonts.hpp"
#include "img_io/img_utils.hpp"

using namespace std;

namespace gl {

	// 初期化時に設定ビットマップを作成しておくフォント列（unicode）
	static const wchar_t iniial_fonts[] = {
		
		0x0000
	};

	// MSGOTHIC では、16 ピクセル、アンチエリアスモードは正しく生成されないので注意！
	static const char* default_font_path_ = "c:/WINDOWS/Fonts/MSGOTHIC.TTC";
	static const char* default_font_face_ = "ms_gothic";

	//-----------------------------------------------------------------//
	/*!
		@brief	初期化
	*/
	//-----------------------------------------------------------------//
	void fonts::initialize()
	{
		if(kfm_ != 0) return;

		img::create_kfimg();		// 漢字フォントイメージクラス作成
		kfm_ = img::get_kfimg();	// 漢字フォントイメージクラス取得

		kfm_->initialize();

		install_font_type(default_font_path_, default_font_face_);

		fore_color_.set(255, 255, 255, 255);
		back_color_.set(0, 0, 0, 255);
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	フォントをインストール
		@param[in]	ttfname	TrueType フォントファイルのパス
		@param[in]	alias	フォントの別名
		@return 正常なら「true」を返す
	*/
	//-----------------------------------------------------------------//
	bool fonts::install_font_type(const std::string& ttfname, const std::string& alias)
	{
		if(kfm_ == 0) return false;

		bool f = kfm_->install_font_type(ttfname, alias);
		if(!f) {
			return false;
		}

		face_t ft;
		std::pair<face_map_it, bool> ret;
		ret = face_map_.insert(face_pair(alias, ft));
		face_map_it it = ret.first;
		face_ = &it->second;

		// 標準でアンチアリアスとする。
		enable_antialias();
		// 標準でプロポーショナル
		enable_proportional();
		// 標準で２４ピクセル
		set_font_size(24);

		return f;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	フォント・タイプを変更
		@return 正常なら「true」を返す
	*/
	//-----------------------------------------------------------------//
	bool fonts::set_font_type(const std::string& alias)
	{
		if(kfm_ == 0) return false;

		face_map_it it = face_map_.find(alias);
		if(it == face_map_.end()) return false;

		if(!kfm_->set_font(alias)) {
			return false;
		}

		face_ = &it->second;
		kfm_->set_antialias(face_->info_.antialias);

		return true;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	フォント・タイプを取得
		@return フォントタイプの別名
	*/
	//-----------------------------------------------------------------//
	const std::string& fonts::get_font_type() const
	{
		if(kfm_ == 0) {
			static std::string empty;
			return empty;
		}
		return kfm_->get_font();
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	フォント・サイズ設定
		@param[in]	s	インストールするフォントの高さ
	*/
	//-----------------------------------------------------------------//
	void fonts::set_font_size(int s)
	{
		face_->info_.size = s;

		face_t::fix_width_it it = face_->fix_width_.find(s);
		if(it == face_->fix_width_.end()) {
			// 半角文字はとりあえず全部インストールしとく
			bool tmp = face_->info_.proportional;	
			face_->info_.proportional = true;
			int fixw = 0;
			for(wchar_t i = 0x20; i < 0x7f; ++i) {
				install_font(i);
				int w = get_width(i);
				if(fixw < w) fixw = w;
/// std::cout << "Code: " << static_cast<int>(i) << ", W: " << w << std::endl;
			}
			face_->fix_width_.insert(face_t::fix_width_pair(s, fixw));
			face_->info_.proportional = tmp;
/// std::cout << "Size: " << s << ", Fixw: " << fixw << std::endl;
		}
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	標準的な描画前設定
	*/
	//-----------------------------------------------------------------//
	void fonts::setup_matrix()
	{
		if(setup_ == false) {
			::glMatrixMode(GL_TEXTURE);
			::glLoadIdentity();
			::glScalef(1.0f / 256.0f, 1.0f / 256.0f, 1.0f);

			::glMatrixMode(GL_MODELVIEW);
			::glLoadIdentity();
			setup_ = true;
		}
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	描画前設定
		@param[in]	scx	スクリーンの開始位置 X
		@param[in]	scy	スクリーンの開始位置 Y
		@param[in]	scw	スクリーンの横幅（ピクセル単位）
		@param[in]	sch	スクリーンの高さ（ピクセル単位）
	*/
	//-----------------------------------------------------------------//
	void fonts::setup_matrix(int scx, int scy, int scw, int sch)
	{
		if(setup_ == false) {
			::glMatrixMode(GL_TEXTURE);
			::glLoadIdentity();
			::glScalef(1.0f / 256.0f, 1.0f / 256.0f, 1.0f);

			::glMatrixMode(GL_PROJECTION);
			::glLoadIdentity();
			glOrthof((float)scx, (float)scw,
					(float)sch, (float)scy, -1.0f, 1.0f);

			::glMatrixMode(GL_MODELVIEW);
			::glLoadIdentity();
			setup_ = true;
		}
	}

#if 0
	//-----------------------------------------------------------------//
	/*!
		@brief	描画前設定、ビューポートとマトリックスの設定
		@param[in]	rect	開始位置、大きさ
	*/
	//-----------------------------------------------------------------//
	void fonts::setup_matrix_with_clip(const vtx::srect& rect)
	{
		int w = rect.size.x;
		int h = rect.size.y;
		int apph = ::glutGet(GLUT_WINDOW_HEIGHT);
		::glViewport(rect.org.x, apph - rect.org.y - h, w, h);
		setup_matrix(w, h);
	}
#endif

	//-----------------------------------------------------------------//
	/*!
		@brief	描画後設定
	*/
	//-----------------------------------------------------------------//
	void fonts::restore_matrix()
	{
		if(setup_) {
			::glMatrixMode(GL_TEXTURE);
			::glLoadIdentity();

			::glMatrixMode(GL_MODELVIEW);
			::glLoadIdentity();
			setup_ = false;
		}
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	開いているフォント・テクスチャーを確保する。
		@param[in]	width	フォントの幅
		@param[in]	height	フォントの高さ
		@param[in]	tmap	フォントテクスチャーマップ構造体
		@return	確保できれば「true」を返す。
	*/
	//-----------------------------------------------------------------//
	bool fonts::allocate_font_texture(int width, int height, tex_map& tmap)
	{
		int w;
		if(width > height) w = width; else w = height;
		if(w & 7) { w |= 7; ++w; }
		int h = w;

		tex_page_it it = tex_page_.find(h);
		if(it == tex_page_.end()) {
			tex_page tp;
			tp.id = 0;
			tp.size = h;
			tp.x = 0;
			tp.y = 0;
			tex_page_.insert(std::pair<int, tex_page>(h, tp));
			it = tex_page_.find(h);
		}

		tex_page& tp = it->second;
		if(tp.x == 0 && tp.y == 0) {
			// OpenGL テクスチャー ID を生成
			::glGenTextures(1, &tp.id);
			if(tp.id == 0) return false;
			pages_.push_back(tp.id);
		}

		tmap.id  = tp.id;
		tmap.lcx = tp.x;
		tmap.lcy = tp.y;
		tmap.w = width;
		tmap.h = height;

		tp.x += w;
		if((texture_page_width - tp.x) < w) {
			tp.x = 0;
			tp.y += h;
			if((texture_page_height - tp.y) < h) {
				tp.y = 0;
			}
		}

		return true;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	フォントビットマップの登録
		@param[in]	code	フォントのコード
		@param[in]	kfm	フォントビットマップ
		@return 登録できたら iterator を返す。
	*/
	//-----------------------------------------------------------------//
	fonts::fcode_map_it fonts::install_image(wchar_t code)
	{
		const img::i_img* image = kfm_->get_img();
		const vtx::spos& isz = image->get_size();

		tex_map tmap;
		tmap.met = kfm_->get_metrics();
		float font_width = tmap.met.width + tmap.met.hori_x + 0.5f;
		if(code == 0x20) {
			font_width = static_cast<float>(isz.y / 4);
		}
		if(allocate_font_texture((int)font_width, isz.y, tmap) != true) {
			return face_->fcode_map_.end();
		}

		::glBindTexture(GL_TEXTURE_2D, tmap.id);

		::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

		int level = 0;
		if(tmap.lcx == 0 && tmap.lcy == 0) {
			size_t	size = texture_page_width * texture_page_height * sizeof(img::rgba8);
			unsigned char* img = new unsigned char[size];	///< RGBA
			memset(img, 0x00, size);
#ifdef WIN32
			::glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
			::glTexImage2D(GL_TEXTURE_2D, level,
						   GL_RGBA, texture_page_width, texture_page_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, img);
#endif
#ifdef __PPU__
			::glTexImage2D(GL_TEXTURE_2D, level,
						   GL_RGBA, texture_page_width, texture_page_height, 0, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8, img);
#endif
			delete[] img;
		}

		{
			img::img_rgba8	rgba;
			rgba.create(isz, true);
			img::rgba8	c;
			img::gray8	g;
			for(int y = 0; y < isz.y; ++y) {
				for(int x = 0; x < isz.x; ++x) {
					image->get_pixel(x, y, g);
					unsigned char inten;
					if(g.g) inten = 255; else inten = 0;
					c.r = c.g = c.b = inten;
					c.a = g.g;
					rgba.put_pixel(x, y, c);
				}
			}
#ifdef WIN32
			::glTexSubImage2D(GL_TEXTURE_2D, level,
							  tmap.lcx, tmap.lcy, isz.x, isz.y, GL_RGBA, GL_UNSIGNED_BYTE, rgba.get_img());
#endif
#ifdef __PPU__
			::glTexSubImage2D(GL_TEXTURE_2D, level,
							  tmap.lcx, tmap.lcy, isz.x, isz.y, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8, rgba.get_img());
#endif
		}

//		if(h & 7) { h |= 7; ++h; }
		return install_font_code(code, tmap);
	}


	static bool char_to_ucs(const char* text, utils::wstring& dst)
	{
		unsigned char c;
		int cnt = 0;
		wchar_t	code = 0;
		while((c = *(unsigned char *)text++) != 0) {
			if(c < 0x80) { code = c; cnt = 0; }
			else if((c & 0xf0) == 0xe0) { code = (c & 0x0f); cnt = 2; }
			else if((c & 0xe0) == 0xc0) { code = (c & 0x1f); cnt = 1; }
			else if((c & 0xc0) == 0x80) {
				code <<= 6;
				code |= c & 0x3f;
				cnt--;
				if(cnt == 0 && code < 0x80) code = 0;	// 不正なコードとして無視
				else if(cnt < 0) code = 0;
			}
			if(cnt == 0 && code != 0) {
				if(code < 32) {
					if(code == 0x0d) {
					}
				} else {
					dst += code;
				}
			}
		}
		return true;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	フォントの登録（文字列）
		@param[in]	list	フォントのコード列
		@return 登録できたら、「true」を返す。
	*/
	//-----------------------------------------------------------------//
	bool fonts::install_font(const wchar_t* list)
	{
		bool f = false;
		wchar_t wc;
		while((wc = *list++) != 0) {
			if(install_font(wc)) f = true;
		}
		return f;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	フォントの登録（unicode 文字列）
		@param[in]	list	フォントのコード列
		@return 登録できたら、「true」を返す。
	*/
	//-----------------------------------------------------------------//
	bool fonts::install_font(const char* list)
	{
		wstring ws;
		char_to_ucs(list, ws);
		return install_font(ws.c_str());
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	リソースを廃棄する。
	*/
	//-----------------------------------------------------------------//
	void fonts::destroy()
	{
		if(!pages_.empty()) {
			::glDeleteTextures(pages_.size(), &pages_[0]);
			pages_.clear();
		}

		img::destroy_kfimg();
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	フォント・テクスチャー・ページを描画する
		@param[in]	page	描画するテクスチャーページ
	*/
	//-----------------------------------------------------------------//
	void fonts::draw_page(int page)
	{
		::glEnable(GL_TEXTURE_2D);

		::glBindTexture(GL_TEXTURE_2D, pages_[page]);

		::glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		::glEnableClientState(GL_VERTEX_ARRAY);

		coord_[0].u  =   0; coord_[0].v  =   0;
		vertex_[0].x =   0; vertex_[0].y = 256;
		coord_[1].u  =   0; coord_[1].v  = 256;
		vertex_[1].x =   0; vertex_[1].y =   0;
		coord_[3].u  = 256; coord_[3].v  = 256;
		vertex_[3].x = 256; vertex_[3].y =   0;
		coord_[2].u  = 256; coord_[2].v  =   0;
		vertex_[2].x = 256; vertex_[2].y = 256;
		::glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

		::glDisable(GL_TEXTURE_2D);
	}


	int fonts::font_width_(wchar_t code, int fw, int fh)
	{
		int fow = 0;
		// 等幅フォントで英数字の場合
		if(!face_->info_.proportional && code >= 0x20 && code < 0x7f) {
			face_t::fix_width_it it = face_->fix_width_.find(fh);
			if(it != face_->fix_width_.end()) {
				fow = it->second;
			} else {
				set_font_size(fh);
				face_t::fix_width_it it = face_->fix_width_.find(fh);
				fow = it->second;
			}
		} else {
			// プロポーショナル・フォントの場合にスペースコードは特殊処理
			if(code == 0x20) fow = fh / 4;
			else fow = fw;
			if(face_->info_.proportional) fow += face_->info_.spaceing;
		}
		return fow;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	フォントを１文字描画する
		@param[in]	pos	描画位置
		@param[in]	code	描画するコード
		@return	描画に成功したらフォントの幅を返す。
	 */
	//-----------------------------------------------------------------//
	int fonts::draw(const vtx::spos& pos, wchar_t code)
	{
		fcode_map_cit cit = find_font_code(code);
		if(cit == face_->fcode_map_.end()) {
			kfm_->create_bitmap(vtx::spos(face_->info_.size, face_->info_.size), code);
			cit = install_image(code);
		}
		const tex_map& tmap = cit->second;

		short x  = pos.x;
		short y  = pos.y;
		short fw = tmap.w;
		// 半角文字で、等幅表示の場合、中心に描画
		if(face_->info_.center && !face_->info_.proportional && code >= 0x20 && code < 0x7f) {
			if(fw < face_->info_.size) {
				x += (face_->info_.size - fw) / 2;
			}
		}

		short ox = 0;
		short oy = 0;

		const vtx::srect& clip = clip_;
//		if(clip.size.x < 0) { clip.org.x -= clip.org.x;                       clip.w = -clip.w; }
//		if(clip.size.y < 0) { clip.org.y -= clip.org.y - m_face->m_info.size; clip.h = -clip.h; }

		short	xt = x;
		short	xe = x + fw;
		short	clip_xe = clip.org.x + clip.size.x;
		if(xe < clip.org.x) return font_width_(code, fw, tmap.h);	// clip out!
		else if(clip_xe <= xt) return font_width_(code, fw, tmap.h);	// clip out!

		short	yt = y;
		short	ye = y + face_->info_.size;
		short	clip_ye = clip.org.y + clip.size.y;
		if(ye < clip.org.y) return font_width_(code, fw, tmap.h);	// clip out!
		else if(clip_ye <= yt) return font_width_(code, fw, tmap.h);	// clip out!

		short	ut = 0;
		short	ue = fw;
		if(xt < clip.org.x && clip.org.x <= xe) {
			ut = clip.org.x - xt;
			xt = clip.org.x;
		}
 		if(xt < clip_xe && clip_xe <= xe) {
			ue -= xe - clip_xe;
			xe = clip_xe;
		}

		short	vt = 0;
		short	ve = face_->info_.size;
		if(yt < clip.org.y && clip.org.y <= ye) {
			vt = clip.org.y - yt;
			yt = clip.org.y;
		}
		if(yt < clip_ye && clip_ye <= ye) {
			ve -= ye - clip_ye;
			ye = clip_ye;
		}

		ut += tmap.lcx;
		vt += tmap.lcy;
		ue += tmap.lcx;
		ve += tmap.lcy;

		if(h_flip_) {
			coord_[0].u = ue;
			coord_[1].u = ue;
			coord_[3].u = ut;
			coord_[2].u = ut;
		} else {
			coord_[0].u = ut;
			coord_[1].u = ut;
			coord_[3].u = ue;
			coord_[2].u = ue;
		}

		if(v_flip_) {
			coord_[0].v = vt;
			coord_[1].v = ve;
			coord_[3].v = ve;
			coord_[2].v = vt;
		} else {
			coord_[0].v = ve;
			coord_[1].v = vt;
			coord_[3].v = vt;
			coord_[2].v = ve;
		}

		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer(2, GL_SHORT, 0, vertex_);
		if(render_back_) {
			img::rgba8 bc;
			if(swap_color_) {
				bc = fore_color_;
			} else {
				bc = back_color_;
			}
			short i = face_->info_.spaceing;
			if(ccw_) {
				vertex_[0].x = ox + xt;     vertex_[0].y = oy + yt;
				vertex_[1].x = ox + xt;     vertex_[1].y = oy + ye;
				vertex_[3].x = ox + xe + i; vertex_[3].y = oy + ye;
				vertex_[2].x = ox + xe + i; vertex_[2].y = oy + yt;
			} else {
				vertex_[0].x = ox + xt;     vertex_[0].y = oy + ye;
				vertex_[1].x = ox + xt;     vertex_[1].y = oy + yt;
				vertex_[3].x = ox + xe + i; vertex_[3].y = oy + yt;
				vertex_[2].x = ox + xe + i; vertex_[2].y = oy + ye;
			}
			glDisableClientState(GL_TEXTURE_COORD_ARRAY);
			glDisable(GL_TEXTURE_2D);
			glColor4ub(bc.r, bc.g, bc.b, bc.a);
			glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		}

		if(ccw_) {
			vertex_[0].x = ox + xt; vertex_[0].y = oy + yt;
			vertex_[1].x = ox + xt; vertex_[1].y = oy + ye;
			vertex_[3].x = ox + xe; vertex_[3].y = oy + ye;
			vertex_[2].x = ox + xe; vertex_[2].y = oy + yt;
		} else {
			vertex_[0].x = ox + xt; vertex_[0].y = oy + ye;
			vertex_[1].x = ox + xt; vertex_[1].y = oy + yt;
			vertex_[3].x = ox + xe; vertex_[3].y = oy + yt;
			vertex_[2].x = ox + xe; vertex_[2].y = oy + ye;
		}

		img::rgba8 fc;
		if(swap_color_) {
			fc = back_color_;
		} else {
			fc = fore_color_;
		}

		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glTexCoordPointer(2, GL_SHORT, 0, coord_);
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, tmap.id);
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		glColor4ub(fc.r, fc.g, fc.b, fc.a);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		glDisableClientState(GL_VERTEX_ARRAY);

		return font_width_(code, fw, tmap.h);
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	フォントの幅を計算する
		@param[in]	code 計算するフォントのコード
		@return	フォントの幅を返す
	 */
	//-----------------------------------------------------------------//
	int fonts::get_width(wchar_t code)
	{
		fcode_map_cit cit = find_font_code(code);
		if(cit == face_->fcode_map_.end()) {
			kfm_->create_bitmap(vtx::spos(face_->info_.size, face_->info_.size), code);
			cit = install_image(code);
		}
		const tex_map& tmap = cit->second;

		return font_width_(code, tmap.w, tmap.h);
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	フォントを描画する
		@param[in]	pos	描画位置
		@param[in]	text	描画する文字列
		@param[in]	limit	改行のリミット幅
		@return	描画幅を返す（複数行の場合、最大値）
	 */
	//-----------------------------------------------------------------//
	int fonts::draw(const vtx::spos& pos, const utils::wstring& text, short limit)
	{
		short x = pos.x;
		short y = pos.y;
		short xt = x;
		short xx = x;
		const wchar_t* p = text.c_str();
		while(wchar_t code = *p++) {
			if(code < 32) {
				if(code == '\n') {
					x = xt;
					y += face_->info_.size;
				}
			} else {
				x += draw(vtx::spos(x, y), code);
				if(x > xx) xx = x;
			}
		}
		return xx - xt;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	フォントを描画する
		@param[in]	pos		描画位置
		@param[in]	text	描画する文字列 (UTF-8)
		@param[in]	limit	改行のリミット幅
		@return	描画幅を返す（複数行の場合、最大値）
	 */
	//-----------------------------------------------------------------//
	int fonts::draw(const vtx::spos& pos, const std::string& text, short limit)
	{
		short x = pos.x;
		short y = pos.y;
		short xx = x;
		unsigned char c;
		int cnt = 0;
		wchar_t	code = 0;
		const char* p = text.c_str();
		while((c = *(unsigned char *)p++) != 0) {
			if(c < 0x80) { code = c; cnt = 0; }
			else if((c & 0xf0) == 0xe0) { code = (c & 0x0f); cnt = 2; }
			else if((c & 0xe0) == 0xc0) { code = (c & 0x1f); cnt = 1; }
			else if((c & 0xc0) == 0x80) {
				code <<= 6;
				code |= c & 0x3f;
				cnt--;
				if(cnt == 0 && code < 0x80) code = 0;	// 不正なコードとして無視
				else if(cnt < 0) code = 0;
			}
			if(cnt == 0 && code != 0) {
				if(code < 32) {
					if(code == '\n') {
						x = pos.x;
						y += face_->info_.size;
					}
				} else {
					if(limit) {
						short w = get_width(code);
						if((x + w) >= limit) {
							x = pos.x;
							y += face_->info_.size;
						}	
					}
					x += draw(vtx::spos(x, y), code);
					if(x > xx) xx = x;
				}
			}
		}
		return xx - pos.x;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	フォントの幅を計算する
		@param[in]	text 計算するフォントのコード列
		@return	フォントの幅を返す
	 */
	//-----------------------------------------------------------------//
	int fonts::get_width(const std::string& text)
	{
		int xm = 0;
		int x = 0;
		int cnt = 0;
		wchar_t	code = 0;
		BOOST_FOREACH(char c, text) {
			unsigned char uc = static_cast<unsigned char>(c);
			if(uc < 0x80) { code = uc; cnt = 0; }
			else if((uc & 0xf0) == 0xe0) { code = (uc & 0x0f); cnt = 2; }
			else if((uc & 0xe0) == 0xc0) { code = (uc & 0x1f); cnt = 1; }
			else if((uc & 0xc0) == 0x80) {
				code <<= 6;
				code |= uc & 0x3f;
				cnt--;
				if(cnt == 0 && code < 0x80) code = 0;	// 不正なコードとして無視
				else if(cnt < 0) code = 0;
			}
			if(cnt == 0 && code != 0) {
				if(code < 32) {
					if(code == '\n') {
//						y -= size_;
						x = 0;
					}
				} else {
					x += get_width(code);
					if(xm < x) xm = x;
				}
			}
		}
		return xm;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	文字列全体の大きさを取得
		@param[in]	wt	文字列
		@return	大きさを返す
	 */
	//-----------------------------------------------------------------//
	vtx::spos fonts::get_size(const utils::wstring& wt)
	{
		vtx::spos size(0, 0);
		vtx::spos tmp(0, face_->info_.size);
		BOOST_FOREACH(wchar_t wch, wt) {
			if(wch >= 0x20) {
				tmp.x += get_width(wch);
			} else {
				if(wch == '\n') {
					tmp.y += face_->info_.size;
					if(size.x < tmp.x) size.x = tmp.x;
					tmp.x = 0;
				}
			}
		}
		if(size.x < tmp.x) size.x = tmp.x;
		if(tmp.x == 0) tmp.y -= face_->info_.size; 
		size.y = tmp.y;
		return size;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	フォントのバックの描画
		@param[in]	rect	描画位置と大きさ
	 */
	//-----------------------------------------------------------------//
	void fonts::draw_back(const vtx::srect& rect)
	{
		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer(2, GL_SHORT, 0, vertex_);
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		if(ccw_) {
			vertex_[0].x = rect.org.x;     vertex_[0].y = rect.org.y;
			vertex_[1].x = rect.org.x;     vertex_[1].y = rect.end_y();
			vertex_[3].x = rect.end_x();   vertex_[3].y = rect.end_y();
			vertex_[2].x = rect.end_x();   vertex_[2].y = rect.org.y;
		} else {
			vertex_[0].x = rect.org.x;     vertex_[0].y = rect.end_y();
			vertex_[1].x = rect.org.x;     vertex_[1].y = rect.org.y;
			vertex_[3].x = rect.end_x();   vertex_[3].y = rect.org.y;
			vertex_[2].x = rect.end_x();   vertex_[2].y = rect.end_y();
		}
		glDisable(GL_TEXTURE_2D);
		img::rgba8 fc;
		if(swap_color_) {
			fc = fore_color_;
		} else {
			fc = back_color_;
		}
		glColor4ub(fc.r, fc.g, fc.b, fc.a);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	}
};
