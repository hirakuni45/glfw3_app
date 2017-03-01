//=====================================================================//
/*!	@file
	@brief	OpenGL フォント・クラス @n
			Copyright 2017 Kunihito Hiramatsu
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include "gl_fw/glfonts.hpp"
#include "img_io/img_utils.hpp"
#include <iostream>

using namespace std;

namespace gl {

	bool fonts::allocate_font_texture_(int width, int height, tex_map& tmap)
	{
		int w;
		if(width > height) w = width; else w = height;
		if(w & 7) { w |= 7; ++w; }
		int h = w;

		tex_page_map::iterator it = tex_page_map_.find(h);
		if(it == tex_page_map_.end()) {
			tex_page tp;
			tp.id = 0;
			tp.size = h;
			tp.x = 0;
			tp.y = 0;
			tex_page_map_.emplace(h, tp);
			it = tex_page_map_.find(h);
		}

		tex_page& tp = it->second;
		if(tp.x == 0 && tp.y == 0) {
			// OpenGL テクスチャー ID を生成
			glGenTextures(1, &tp.id);
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
		@brief	初期化
		@param[in]	fpath	フォントパス
		@param[in]	alias	フォント別名
	*/
	//-----------------------------------------------------------------//
	void fonts::initialize(const std::string& fpath, const std::string& alias)
	{
		install_font_type(fpath, alias);

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
		bool f = img::ftimg::get_instance().install_font_type(ttfname, alias);
		if(!f) {
			std::cerr << "Install error: '" << ttfname << "'" << std::endl;
			return false;
		}

		face_t ft;
		std::pair<face_map::iterator, bool> ret;
		ret = face_map_.emplace(alias, ft);
		face_map::iterator it = ret.first;
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
		face_map::iterator it = face_map_.find(alias);
		if(it == face_map_.end()) return false;

		if(!img::ftimg::get_instance().set_font(alias)) {
			return false;
		}

		face_ = &it->second;
		img::ftimg::get_instance().set_antialias(face_->info_.antialias);

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
		return img::ftimg::get_instance().get_font();
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

		face_t::fix_width_map::iterator it = face_->fix_width_map_.find(s);
		if(it == face_->fix_width_map_.end()) {
			// 半角文字はとりあえず全部インストールしとく
			bool tmp = face_->info_.proportional;	
			face_->info_.proportional = true;
			int fixw = 0;
			for(uint8_t i = 0x20; i < 0x7f; ++i) {
				install_font(i);
				int w = get_width(i);
				if(fixw < w) fixw = w;
/// std::cout << "Code: " << static_cast<int>(i) << ", W: " << w << std::endl;
			}
			face_->fix_width_map_.emplace(s, fixw);
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
			glMatrixMode(GL_TEXTURE);
			glLoadIdentity();
			glScalef(1.0f / 256.0f, 1.0f / 256.0f, 1.0f);

			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();
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
			glMatrixMode(GL_TEXTURE);
			glLoadIdentity();
			glScalef(1.0f / 256.0f, 1.0f / 256.0f, 1.0f);

			glMatrixMode(GL_PROJECTION);
			glLoadIdentity();
			glOrthof(static_cast<float>(scx), static_cast<float>(scw),
					 static_cast<float>(sch), static_cast<float>(scy),
					 -1.0f, 1.0f);
			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();
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
		glViewport(rect.org.x, apph - rect.org.y - h, w, h);
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
			glMatrixMode(GL_TEXTURE);
			glLoadIdentity();

			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();
			setup_ = false;
		}
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	フォントビットマップの登録
		@param[in]	code	フォントのコード
		@return 登録できたら iterator を返す。
	*/
	//-----------------------------------------------------------------//
	fonts::fcode_map::iterator fonts::install_image(uint32_t code)
	{
		const img::img_gray8& gray = img::ftimg::get_instance().get_img();
		const vtx::spos& isz = gray.get_size();
// std::cout << static_cast<int>(code) << ": " << static_cast<int>(isz.x) << ", " << static_cast<int>(isz.y) << std::endl;
		tex_map tmap;
		tmap.met = img::ftimg::get_instance().get_metrics();
		float font_width = tmap.met.width + tmap.met.hori_x + 0.5f;
		if(code == 0x20) {
 			font_width = static_cast<float>(isz.y / 4);
		}
		if(!allocate_font_texture_(static_cast<int>(font_width), isz.y, tmap)) {
			return face_->fcode_map_.end();
		}

		glBindTexture(GL_TEXTURE_2D, tmap.id);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

		int level = 0;
		if(tmap.lcx == 0 && tmap.lcy == 0) {
			std::vector<uint8_t> clrimg;
			clrimg.resize(texture_page_width * texture_page_height);
			glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
			glTexImage2D(GL_TEXTURE_2D, level,
						 GL_ALPHA, texture_page_width, texture_page_height,
						 0, GL_ALPHA, GL_UNSIGNED_BYTE, &clrimg[0]);
		}

		{
			glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
			glTexSubImage2D(GL_TEXTURE_2D, level,
							tmap.lcx, tmap.lcy, isz.x, isz.y, GL_ALPHA, GL_UNSIGNED_BYTE, gray());
		}

//		if(h & 7) { h |= 7; ++h; }
		return install_font_code(code, tmap);
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	リソースを廃棄する。
	*/
	//-----------------------------------------------------------------//
	void fonts::destroy()
	{
		if(!pages_.empty()) {
			glDeleteTextures(pages_.size(), &pages_[0]);
			pages_.clear();
		}
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	フォント・テクスチャー・ページを描画する
		@param[in]	page	描画するテクスチャーページ
	*/
	//-----------------------------------------------------------------//
	void fonts::draw_page(int page)
	{
		glEnable(GL_TEXTURE_2D);

		glBindTexture(GL_TEXTURE_2D, pages_[page]);

		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glEnableClientState(GL_VERTEX_ARRAY);

		coord_[0].u  =   0; coord_[0].v  =   0;
		vertex_[0].x =   0; vertex_[0].y = 256;
		coord_[1].u  =   0; coord_[1].v  = 256;
		vertex_[1].x =   0; vertex_[1].y =   0;
		coord_[3].u  = 256; coord_[3].v  = 256;
		vertex_[3].x = 256; vertex_[3].y =   0;
		coord_[2].u  = 256; coord_[2].v  =   0;
		vertex_[2].x = 256; vertex_[2].y = 256;
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

		glDisable(GL_TEXTURE_2D);
	}


	int fonts::font_width_(uint32_t code, int fw, int fh)
	{
		int fow = 0;
		// 等幅フォントで英数字の場合
		if(!face_->info_.proportional && code >= 0x20 && code < 0x7f) {
			face_t::fix_width_map::iterator it = face_->fix_width_map_.find(fh);
			if(it != face_->fix_width_map_.end()) {
				fow = it->second;
			} else {
				set_font_size(fh);
				face_t::fix_width_map::iterator it = face_->fix_width_map_.find(fh);
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
		@param[in]	code 描画するコード
		@param[in]	inv	反転文字の場合「true」
		@return	描画に成功したらフォントの幅を返す。
	 */
	//-----------------------------------------------------------------//
	int fonts::draw(const vtx::ipos& pos, uint32_t code, bool inv)
	{
		fcode_map::const_iterator cit = find_font_code(code);
		if(cit == face_->fcode_map_.end()) {
			img::ftimg::get_instance().create_bitmap(face_->info_.size, code);
			cit = install_image(code);
		}
		const tex_map& tmap = cit->second;

		auto x = pos.x;
		auto y = pos.y;
		// 半角文字で、等幅表示の場合、中心に描画
		if(face_->info_.center && !face_->info_.proportional && code >= 0x20 && code < 0x7f) {
			if(tmap.w < face_->info_.size) {
				x += (face_->info_.size - tmap.w) / 2;
			}
		}

		int ox = 0;
		int oy = 0;

		const vtx::irect& clip = clip_;
//		if(clip.size.x < 0) { clip.org.x -= clip.org.x;                       clip.w = -clip.w; }
//		if(clip.size.y < 0) { clip.org.y -= clip.org.y - m_face->m_info.size; clip.h = -clip.h; }

		int xt = x;
		int xe = x + tmap.w;
		int clip_xe = clip.org.x + clip.size.x;
		if(xe < clip.org.x) return font_width_(code, tmap.w, tmap.h);	// clip out!
		else if(clip_xe <= xt) return font_width_(code, tmap.w, tmap.h);	// clip out!

		int yt = y;
		int ye = y + tmap.h;
		int clip_ye = clip.org.y + clip.size.y;
		if(ye < clip.org.y) return font_width_(code, tmap.w, tmap.h);	// clip out!
		else if(clip_ye <= yt) return font_width_(code, tmap.w, tmap.h);	// clip out!

		int ut = 0;
		int ue = tmap.w;
		if(xt < clip.org.x && clip.org.x <= xe) {
			ut = clip.org.x - xt;
			xt = clip.org.x;
		}
 		if(xt < clip_xe && clip_xe <= xe) {
			ue -= xe - clip_xe;
			xe = clip_xe;
		}

		int vt = 0;
		int ve = tmap.h;
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
		if(render_back_ || inv) {
			img::rgba8 bc;
			if(swap_color_ || inv) {
				bc = fore_color_;
			} else {
				bc = back_color_;
			}
			int i = face_->info_.spaceing;
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
		if(swap_color_ || inv) {
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

		return font_width_(code, tmap.w, tmap.h);
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	フォントを描画する
		@param[in]	pos	描画位置
		@param[in]	text	描画ロング文字列
		@param[in]	limit	改行のリミット幅
		@param[in]	cursor カーソル位置反転文字
		@return	描画幅を返す（複数行の場合、最大値）
	 */
	//-----------------------------------------------------------------//
	int fonts::draw(const vtx::ipos& pos, const utils::lstring& text, int limit, int cursor)
	{
		int x = pos.x;
		int y = pos.y;
		int xx = x;
		int n = 0;
		BOOST_FOREACH(uint32_t code, text) {
			if(code < 32) {
				if(code == '\n') {
					x = pos.x;
					y += face_->info_.size;
				}
			} else {
				if(limit) {
					int w = get_width(code);
					if((x + w) >= limit) {
						x = pos.x;
						y += face_->info_.size;
					}
				}
				x += draw(vtx::ipos(x, y), code, n == cursor);
				if(x > xx) xx = x;
			}
			++n;
		}
		return xx - pos.x;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	フォントの幅を計算する
		@param[in]	code 計算するフォントのコード
		@return	フォントの幅を返す
	 */
	//-----------------------------------------------------------------//
	int fonts::get_width(uint32_t code)
	{
		fcode_map::const_iterator cit = find_font_code(code);
		if(cit == face_->fcode_map_.end()) {
			img::ftimg::get_instance().create_bitmap(face_->info_.size, code);
			cit = install_image(code);
		}
		const tex_map& tmap = cit->second;

		return font_width_(code, tmap.w, tmap.h);
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	フォントの幅を計算する
		@param[in]	text フォントのロングコード列
		@return	フォントの幅を返す
	 */
	//-----------------------------------------------------------------//
	int fonts::get_width(const utils::lstring& text)
	{
		int len = 0;
		int lenmax = 0;
		BOOST_FOREACH(uint32_t lc, text) {
			if(lc >= 0x20) {
				len += get_width(lc);
			} else if(lc == '\n') {
				len = 0;
			}
			if(lenmax < len) lenmax = len;
		}
		return lenmax;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	文字列全体の大きさを取得
		@param[in]	s	文字列
		@return	大きさを返す
	 */
	//-----------------------------------------------------------------//
	vtx::ipos fonts::get_size(const utils::lstring& s)
	{
		vtx::ipos size(0, 0);
		vtx::ipos tmp(0, face_->info_.size);
		BOOST_FOREACH(uint32_t ch, s) {
			if(ch >= 0x20) {
				tmp.x += get_width(ch);
			} else {
				if(ch == '\n') {
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
	void fonts::draw_back(const vtx::irect& rect)
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
