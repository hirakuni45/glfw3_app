//=====================================================================//
/*!	@file
	@brief	OpenGL モーション・オブジェクト・クラス
    @author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2017 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/glfw3_app/blob/master/LICENSE
*/
//=====================================================================//
#include "gl_fw/glmobj.hpp"
#include "gl_fw/glutils.hpp"
#include "img_io/img_utils.hpp"

#include <boost/format.hpp>
#include <iostream>

using namespace std;
using namespace vtx;

namespace gl {

	using namespace img;

	//-----------------------------------------------------------------//
	/*!
		@brief	初期化
		@param[in]	pgw	テクスチャーページの最大横サイズ
		@param[in]	pgh	テクスチャーページの最大高さサイズ
		@param[in]	internalFormat	OpenGL の内部画像形式
		@param[in]	mp	ミップマップの場合「true」
		@param[in]	im	初期化イメージ
	 */
	//-----------------------------------------------------------------//
	void texture_mem::initialize(int pgw, int pgh, GLint internalFormat, bool mp, const img_rgba8& im)
	{
		// 16 ピクセル毎のブロック管理
		int ww = pgw / 16;
		if(pgw & 15) ++ww;
		int hh = pgh / 16;
		if(pgh & 15) ++hh;
		block_w_ = ww;
		block_h_ = hh;
		block_num_ = ww * hh;
		tex_map_.resize(block_num_ >> 5);
		for(int i = 0; i < (block_num_ >> 5); ++i) tex_map_[i] = 0;

		glGenTextures(1, &id_);
		glBindTexture(GL_TEXTURE_2D, id_);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
		if(im.get_size().x != pgw || im.get_size().y != pgh) {
			img_rgba8 dst;
			dst.create(vtx::spos(pgw, pgh), true);
			dst.fill(rgba8(0, 0, 0, 0));
			dst.copy(vtx::spos(0), im, vtx::srect(vtx::spos(0), im.get_size()));
			glTexImage2D(GL_TEXTURE_2D, 0,
						   internalFormat, pgw, pgh, 0, GL_RGBA, GL_UNSIGNED_BYTE, dst());
		} else {
			glTexImage2D(GL_TEXTURE_2D, 0,
						 internalFormat, pgw, pgh, 0, GL_RGBA, GL_UNSIGNED_BYTE, im());
		}
#if 0
		if(mp) {
			for(int i = 1; i < g_mipmap_level_max; ++i) {
				pgw /= 2;
				pgh /= 2;
				glTexImage2D(GL_TEXTURE_2D, i, internalFormat, pgw, pgh, 0, GL_RGBA, GL_UNSIGNED_BYTE, im());
			}
		}
#endif
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	テクスチャー・エリアの割り当て
		@param[in]	x	アロケートの位置 X を受け取るリファレンス
		@param[in]	y	アロケートの位置 Y を受け取るリファレンス
		@param[in]	w	アロケートする幅
		@param[in]	h	アロケートする高さ
		return 失敗したら「false」が返る
	 */
	//-----------------------------------------------------------------//
	bool texture_mem::allocate(short& x, short& y, short w, short h)
	{
		if(block_num_ <= 0) return false;

		int w_num = w >> 4;
		if(w & 15) w_num++;
		int h_num = h >> 4;
		if(h & 15) h_num++;
		if(w_num > block_w_ || h_num > block_h_) return false;

		for(int i = 0; i < (block_h_ - h_num + 1); ++i) {
			for(int j = 0; j < (block_w_ - w_num + 1); ++j) {
				if(scan(j, w_num, i, h_num)) {
					x = j << 4;
					y = i << 4;
					block_num_ -= w_num * h_num;
					fill(j, w_num, i, h_num);
					return true;
				}
			}
		}

		return false;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	廃棄
	 */
	//-----------------------------------------------------------------//
	void texture_mem::destroy()
	{
		std::vector<unsigned int>().swap(tex_map_);
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	テクスチャーページの利用状態をダンプ（デバッグ確認用）
		@param[in]	ost	ファイル・ストリーム
	 */
	//-----------------------------------------------------------------//
	void texture_mem::dump(std::ostream& ost)
	{
		for(int j = 0; j < 16; ++j) {
			unsigned short bits = 1;
			for(int i = 0; i < 16; ++i) {
				char c;
				if(tex_map_[j] & bits) c = '*';
				else c = ' ';
				ost << boost::format("%c") % c;
				i <<= 1;
			}
			ost << std::endl;
		}
	}


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	テクスチャー小片のアロケート
		@param[in]	mems	テクスチャー管理（vector）
		@param[in]	mo		モーションオブジェクト情報
		@return	管理領域があれば「true」
	 */
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	bool mobj::allocate_texture(texture_mems& mems, obj* mo)
	{
		for(unsigned int i = 0; i < mems.size(); ++i) {
			if(mems[i].allocate(mo->tx, mo->ty, mo->tw, mo->th)) {
				mo->id = mems[i].get_id();
				return true;
			}
		}
		return false;
	}


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	テクスチャー小片を新規に追加
		@param[in]	mems	テクスチャー管理（vector）
		@param[in]	mo		モーションオブジェクト情報
		@param[in]	im		初期化画像
	 */
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	void mobj::add_texture_page(texture_mems& mems, obj* mo, const img_rgba8& im)
	{
		texture_mem mem;
		mem.initialize(tex_page_w_, tex_page_h_, internal_format_, mo->mp, im);
		mem.allocate(mo->tx, mo->ty, mo->tw, mo->th);
		mems.push_back(mem);
		mo->id = mem.get_id();
	}


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	テクスチャー小片を全廃棄
		@param[in]	mems	テクスチャー管理（vector）
	 */
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	void mobj::destroy_texture_page(texture_mems& mems)
	{
		BOOST_FOREACH(texture_mem& txm, mems) {
			GLuint id = txm.get_id();
			glDeleteTextures(1, &id);
		}
		texture_mems().swap(mems);
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	初期化
		@param[in]	pgw	テクスチャーページの最大横サイズ
		@param[in]	pgh	テクスチャーページの最大高さサイズ
	 */
	//-----------------------------------------------------------------//
	void mobj::initialize(int pgw, int pgh)
	{
		// 0 番ハンドルは使わないので無効とする。
		objs_.clear();
		objs_.push_back(0);
		texture_pages_.clear();
		tex_page_w_ = pgw;
		tex_page_h_ = pgh;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	モーションオブジェクトの大きさを得る
		@param[in]	h	ハンドル
		@return サイズ
	 */
	//-----------------------------------------------------------------//
	const vtx::spos& mobj::get_size(handle h) const
	{
		if(h > 0 && h < objs_.size()) {
			return objs_[h]->size;
		} else {
			static vtx::spos zero_(0);
			return zero_;
		}
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	モーションオブジェクトとして登録する。
		@param[in]	imf		登録画像
		@param[in]	mipmap	ミっプマップの場合は「true」
		@return	ハンドルを返す
	 */
	//-----------------------------------------------------------------//
	mobj::handle mobj::install(const img::i_img* imf, bool mipmap)
	{
		mipmap = false;

		obj* root = 0;
		obj* lk = 0;
		int sox = 0;
		int soy = 0;
		const vtx::spos& isz = imf->get_size();
		if(isz.x > tex_page_w_) sox = 2;
		if(isz.y > tex_page_h_) soy = 2;
		int hh = isz.y;
		short oy = 0;
		while(hh > 0) {
			short ox = 0;
			int ww = isz.x;
			while(ww > 0) {
				obj* mo = new obj;
				mo->nx = 0;
				mo->ny = 0;
				if(lk) lk->link = mo;
				mo->size = isz;	// 元の画像サイズ
				mo->mp = mipmap;
				mo->ex = false;
				int txw;
				if(ww > tex_page_w_) {
					mo->tw = tex_page_w_ - sox;
					txw = tex_page_w_;
				} else {
					mo->tw = ww;
					txw = ww;
				}
				mo->dw = mo->tw;
				int txh;
				if(hh > tex_page_h_) {
					mo->th = tex_page_h_ - soy;
					txh = tex_page_h_;
				} else {
					mo->th = hh;
					txh = hh;
				}
				mo->dh = mo->th;
				// positive offset
				mo->oxp = ox;
				mo->oyp = oy;
				// negative offset
				mo->oxn = isz.x - (ox + mo->dw);
				mo->oyn = isz.y - (oy + mo->dh);

				// テクスチャー領域のアロケート
				{
					img_rgba8 im;
					im.create(vtx::spos(txw, txh), true);
					copy_to_rgba8(imf, vtx::srect(ox, oy, txw, txh), im, vtx::spos(0, 0));
					bool f = false;
					if(sox == 0 && soy == 0) {
						f = allocate_texture(texture_mems_, mo);
					}
					if(!f) {
						add_texture_page(texture_mems_, mo, im);
					} else {
						glBindTexture(GL_TEXTURE_2D, mo->id);
						glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
						glTexSubImage2D(GL_TEXTURE_2D, 0,
							mo->tx, mo->ty, txw, txh, GL_RGBA, GL_UNSIGNED_BYTE,
							im());
					}
					if(mipmap) {
						int ofsx = mo->tx;
						int ofsy = mo->ty;
///						img_rgba8 tmp;
						for(int i = 1; i < mipmap_level_max_; ++i) {
							ofsx /= 2;
							ofsy /= 2;
							txw /= 2;
							txh /= 2;
///							scale_50percent(imf, tmp);
///							glTexSubImage2D(GL_TEXTURE_2D, i, ofsx, ofsy, txw, txh, GL_RGBA, GL_UNSIGNED_BYTE, im);
						}
					}
				}
				if(root == 0) root = mo;
				ww -= mo->tw;
				ox += mo->tw;
				lk = mo;
			}
			hh -= lk->th;
			oy += lk->th;
		}

		handle h = objs_.size();
		objs_.push_back(root);
		return h;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	モーションオブジェクトとして拡張登録する。
		@param[in]	imf		登録最小画像（グリッド・サイズの３倍）
		@param[in]	size	実際の拡張サイズ
		@param[in]	grid	グリッド・サイズ（指定しないと１６ピクセル）
		@return	ハンドルを返す
	 */
	//-----------------------------------------------------------------//
	mobj::handle mobj::install(const img::i_img* imf, const vtx::spos& size, const vtx::spos& grid)
	{
		const vtx::spos& isz = imf->get_size();
		// 基本テクスチャーサイズを超える場合はエラーとする。
		if(isz.x > tex_page_w_ || isz.y > tex_page_h_) return 0;

		int jn;
		if(size.y < (grid.y * 3)) {
			jn = 1;
		} else if(isz.y >= (grid.y * 3)) {
			jn = 3;
		} else {
			return 0;
		}
		int in;
		if(size.x < (grid.x * 3)) {
			in = 1;
		} else if(isz.x >= (grid.x * 3)) {
			in = 3;
		} else {
			return 0;
		}

		obj* root = 0;
		obj* lk = 0;
		short oy = 0;
		for(int j = 0; j < jn; ++j) {
			short ox = 0;
			for(int i = 0; i < in; ++i) {
				obj* mo = new obj;
				mo->nx = in;
				mo->ny = jn;
				if(lk) lk->link = mo;
				mo->size = size;	// 元の画像サイズ
				mo->mp = false;
				mo->ex = true;

				if(in == 1) {
					mo->dw = size.x;
				} else if(i != 1) {
					mo->dw = grid.x;
				} else {
					mo->dw = size.x - grid.x * 2;
				}
				if(jn == 1) {
					mo->dh = size.y;
				} else if(j != 1) {
					mo->dh = grid.y;
				} else {
					mo->dh = size.y - grid.y * 2;
				}

				// positive offset
				mo->oxp = ox;
				mo->oyp = oy;
				// negative offset
				mo->oxn = size.x - (ox + mo->dw);
				mo->oyn = size.y - (oy + mo->dh);

				// テクスチャー領域のアロケート
				{
					img_rgba8 im;
					im.create(vtx::spos(mo->dw + 2, mo->dh + 2), true);
					im.fill(rgba8(0));
					static const short sofs[3] = { 0, -1, -1 };
					static const short cpyw[3] = { 1,  2,  1 };
					static const short dofs[3] = { 1,  0,  0 };
					copy_to_rgba8(imf, vtx::srect(ox + sofs[i], oy + sofs[j],
						mo->dw + cpyw[i], mo->dh + cpyw[j]), im, vtx::spos(dofs[i], dofs[j]));
					mo->tw = im.get_size().x;
					mo->th = im.get_size().y;
					bool f = allocate_texture(texture_mems_, mo);
					if(!f) {
						add_texture_page(texture_mems_, mo, im);
					} else {
						glBindTexture(GL_TEXTURE_2D, mo->id);
						glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
						glTexSubImage2D(GL_TEXTURE_2D, 0,
										mo->tx, mo->ty, im.get_size().x, im.get_size().y,
										GL_RGBA, GL_UNSIGNED_BYTE, im());
					}
				}
				mo->tx += 1;
				mo->ty += 1;
				mo->tw -= 2;
				mo->th -= 2;
				if(root == 0) root = mo;
				ox += mo->dw;
				lk = mo;
			}
			oy += lk->dh;
		}
		handle h = objs_.size();
		objs_.push_back(root);
		return h;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	テクスチャーを登録する。
		@param[in]	imf		イメージインターフェース
		@return	テクスチャー・ハンドルを返す
	 */
	//-----------------------------------------------------------------//
	GLuint mobj::install_texture(const img::i_img* imf)
	{
		if(imf == 0) return 0;

		GLuint id = 0;
		int level = 0;
		const vtx::spos& size = imf->get_size();
		if(imf->get_type() == IMG::FULL8) {
			glGenTextures(1, &id);
			glBindTexture(GL_TEXTURE_2D, id);
			glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
			glTexImage2D(GL_TEXTURE_2D, level, internal_format_,
						 size.x, size.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, (*imf)());

		} else if(imf->get_type() == IMG::INDEXED8) {
			glGenTextures(1, &id);
			glBindTexture(GL_TEXTURE_2D, id);
			img_rgba8 im;
			im.create(size, true);
			copy_to_rgba8(imf, im, vtx::spos(0));
			glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
			glTexImage2D(GL_TEXTURE_2D, level, internal_format_,
						   size.x, size.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, im());
		} else {
			std::cout << boost::format("Install Texture No Type: %d") % static_cast<int>(imf->get_type()) << std::endl;
		}
		texture_pages_.push_back(id);
		return id;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	テクスチャーを登録する。
		@param[in]	ptr		圧縮テクスチャーのポインター
		@param[in]	size	圧縮テクスチャーのサイズ
		@param[in]	type	圧縮タイプ
		@param[in]	w		横幅
		@param[in]	h		高さ
		@return テクスチャー・ハンドルを返す
	 */
	//-----------------------------------------------------------------//
	GLuint mobj::install_compressed_texture(const void* ptr, size_t size, int type, int w, int h)
	{
		GLuint id;
		::glGenTextures(1, &id);
		::glBindTexture(GL_TEXTURE_2D, id);
		::glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
#ifdef IPHONE_IPAD
		::glCompressedTexImage2D(GL_TEXTURE_2D, level, type, w, h, 0, size, ptr);
#endif
		texture_pages_.push_back(id);
		return id;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	画像をモーションオブジェクトとして登録する。
		@param[in]	root	モーションオブジェクト設定情報
		@return ハンドルを返す
	 */
	//-----------------------------------------------------------------//
	mobj::handle mobj::install_direct(const obj& root)
	{
		obj* top = new obj;
		*top = root;
		obj* p = top;
		while(p->link) {
			obj* tmp = new obj;
			tmp = p->link;
			p->link = tmp;
			p = tmp;
		}
		handle h = objs_.size();
		objs_.push_back(top);
		return h;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	モーション・オブジェクト描画前設定（テクスチャースケールのみ）
	 */
	//-----------------------------------------------------------------//
	void mobj::setup_matrix()
	{
		glMatrixMode(GL_TEXTURE);
		glLoadIdentity();
		float u = static_cast<float>(tex_page_w_);
		float v = static_cast<float>(tex_page_h_);
		glScalef(1.0f / u, 1.0f / v, 1.0f);

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
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
	void mobj::setup_matrix(int x, int y, int w, int h, float zn, float zf)
	{
		setup_matrix();

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrthof(static_cast<float>(x), static_cast<float>(w),
				 static_cast<float>(h), static_cast<float>(y), zn, zf);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	モーションオブジェクトのイメージを更新
		@param[in]	h	ハンドル
		@param[in]	src	ソース、イメージインターフェース
		@param[in]	dst	ディスとネーションのオフセット
	 */
	//-----------------------------------------------------------------//
	void mobj::copy_image(handle h, const img::i_img* src, const vtx::spos& dst)
	{
		const i_img* imif = src;
		// RGBA8 以外の画像タイプなら、一旦 RGBA8 形式で画像を作成。
		img_rgba8 tmp;
		if(src->get_type() != IMG::FULL8) {
			tmp.create(src->get_size(), true);
			copy_to_rgba8(src, tmp);
			imif = &tmp;
		}

		const obj* m = objs_[h];
		while(m != 0) {
			glBindTexture(GL_TEXTURE_2D, m->id);
			int level = 0;
//			if(mop->link == 0 && mop->w >= imif->get_width() && mop->h >= imif->get_height()) {
			glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
			glTexSubImage2D(GL_TEXTURE_2D, level, m->tx + dst.x, m->ty + dst.y,
							imif->get_size().x, imif->get_size().y,
							GL_RGBA, GL_UNSIGNED_BYTE, (*imif)());
//			}
			m = m->link;
		}
		glFlush();
	}


	static void tex_para_(bool linear)
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		if(linear) {
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		} else {
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		}
	}

	//-----------------------------------------------------------------//
	/*!
		@brief	モーション・オブジェクトを描画する。
		@param[in]	h	ハンドル
		@param[in]	atr	アトリビュート
		@param[in]	pos	描画位置
		@param[in]	linear	「true」ならリニアフィルター
	 */
	//-----------------------------------------------------------------//
	void mobj::draw(handle h, attribute::type atr, const vtx::spos& pos, bool linear)
	{
		if(h == 0 || h >= objs_.size()) return;

		bool hf = false;
		bool vf = false;
		if(atr != attribute::normal) {
			if(atr == attribute::h_flip) hf = true;
			else if(atr == attribute::v_flip) vf = true;
			else if(atr == attribute::hv_flip) hf = vf = true;
		}

		const obj* m = objs_[h];
		do {
			short xp;
			if(hf) {
				xp = pos.x + m->oxn;
			} else {
				xp = pos.x + m->oxp;
			}
			short yp;
			if(vf) {
				yp = pos.y + m->oyn;
			} else {
				yp = pos.y + m->oyp;
			}

			spos vlist[4];
			spos tlist[4];
			switch(atr) {

			case attribute::h_flip:
				tlist[0].set(m->tx + m->tw, m->ty);
				vlist[0].set(   xp,            yp);
				tlist[1].set(m->tx + m->tw, m->ty + m->th);
				vlist[1].set(   xp,            yp + m->dh);
				tlist[3].set(m->tx,         m->ty + m->th);
				vlist[3].set(   xp + m->dw,    yp + m->dh);
				tlist[2].set(m->tx,         m->ty);
				vlist[2].set(   xp + m->dw,    yp);
				break;

			case attribute::v_flip:
				tlist[0].set(m->tx,         m->ty + m->th);
				vlist[0].set(   xp,            yp);
				tlist[1].set(m->tx,         m->ty);
				vlist[1].set(   xp,            yp + m->dh);
				tlist[3].set(m->tx + m->tw, m->ty);
				vlist[3].set(   xp + m->dw,    yp + m->dh);
				tlist[2].set(m->tx + m->tw, m->ty + m->th);
				vlist[2].set(   xp + m->dw,    yp);
				break;

			case attribute::hv_flip:
				tlist[0].set(m->tx + m->tw, m->ty + m->th);
				vlist[0].set(   xp,            yp);
				tlist[1].set(m->tx + m->tw, m->ty);
				vlist[1].set(   xp,            yp + m->dh);
				tlist[3].set(m->tx,         m->ty);
				vlist[3].set(   xp + m->dw,    yp + m->dh);
				tlist[2].set(m->tx,         m->ty + m->th);
				vlist[2].set(   xp + m->dw,    yp);
				break;

			case attribute::normal:
			default:
				tlist[0].set(m->tx,         m->ty);
				vlist[0].set(xp,               yp);
				tlist[1].set(m->tx,         m->ty + m->th);
				vlist[1].set(xp,               yp + m->dh);
				tlist[3].set(m->tx + m->tw, m->ty + m->th);
				vlist[3].set(   xp + m->dw,    yp + m->dh);
   				tlist[2].set(m->tx + m->tw, m->ty);
				vlist[2].set(   xp + m->dw,    yp);
				break;
			}

			glBindTexture(GL_TEXTURE_2D, m->id);
			tex_para_(linear);
			glEnableClientState(GL_TEXTURE_COORD_ARRAY);
			glEnableClientState(GL_VERTEX_ARRAY);
			glTexCoordPointer(2, GL_SHORT, 0, &tlist[0]);
			glVertexPointer(2, GL_SHORT, 0, &vlist[0]);
			glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
			glDisableClientState(GL_TEXTURE_COORD_ARRAY);
			glDisableClientState(GL_VERTEX_ARRAY);

			m = m->link;
		} while(m != 0) ;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	モーション・オブジェクトの描画
		@param[in]	h	ハンドル
		@param[in]	atr	アトリビュート
		@param[in]	pos	描画位置
		@param[in]	ofs	オフセット
		@param[in]	wh	描画幅、高さ
		@param[in]	linear	「true」ならリニアフィルター
	 */
	//-----------------------------------------------------------------//
	void mobj::draw_sub(handle h, attribute::type atr, const vtx::spos& pos, const vtx::spos& ofs, const vtx::spos& wh, bool linear)
	{
		if(h == 0 || h >= objs_.size()) return;

		const obj* m = objs_[h];
		do {
			short xp = m->oxp;
			short yp = m->oyp;
			short w  = m->dw;
			short h  = m->dh;

			short tx = m->tx;
			short ty = m->ty;

			short dx;
			if(ofs.x >= xp) dx = ofs.x - xp + wh.x;
			else dx = xp - ofs.x + wh.x;

			short dy;
			if(ofs.y >= yp) dy = ofs.y - yp + wh.y;
			else dy = yp - ofs.y + wh.y;

			if(dx < (w + wh.x) && dy < (h + wh.y)) {
				if(ofs.x >= xp) {
					w  -= ofs.x - xp;
					if(w > wh.x) w = wh.x;
					tx += ofs.x - xp;
					xp += ofs.x - xp;
				} else {
					w = ofs.x - xp + wh.x;
				}

				if(ofs.y >= yp) {
					h  -= ofs.y - yp;
					if(h > wh.y) h = wh.y;
					ty += ofs.y - yp;
					yp += ofs.y - yp;
				} else {
					h = ofs.y - yp + wh.y;
				}

				xp += pos.x;
				yp += pos.y;
				::glBindTexture(GL_TEXTURE_2D, m->id);

				tex_para_(linear);

				spos vlist[4];
				spos tlist[4];
				switch(atr) {
				case attribute::h_flip:
					tlist[0].set(tx + w, ty);
					vlist[0].set(xp, yp);
					tlist[1].set(tx + w, ty + h);
					vlist[1].set(xp, yp + h);
					tlist[3].set(tx, ty + h);
					vlist[3].set(xp + w, yp + h);
					tlist[2].set(tx, ty);
					vlist[2].set(xp + w, yp);
					break;

				case attribute::v_flip:
					tlist[0].set(tx, ty + h);
					vlist[0].set(xp, yp);
					tlist[1].set(tx, ty);
					vlist[1].set(xp, yp + h);
					tlist[3].set(tx + w, ty);
					vlist[3].set(xp + w, yp + h);
					tlist[2].set(tx + w, ty + h);
					vlist[2].set(xp + w, yp);
					break;

				case attribute::hv_flip:
					tlist[0].set(tx + w, ty + h);
					vlist[0].set(xp, yp);
					tlist[1].set(tx + w, ty);
					vlist[1].set(xp, yp + h);
					tlist[3].set(tx, ty);
					vlist[3].set(xp + w, yp + h);
					tlist[2].set(tx, ty + h);
					vlist[2].set(xp + w, yp);
					break;

				case attribute::normal:
				default:
					tlist[0].set(tx, ty);
					vlist[0].set(xp, yp);
					tlist[1].set(tx, ty + h);
					vlist[1].set(xp, yp + h);
					tlist[3].set(tx + w, ty + h);
					vlist[3].set(xp + w, yp + h);
					tlist[2].set(tx + w, ty);
					vlist[2].set(xp + w, yp);
					break;
				}
				glEnableClientState(GL_TEXTURE_COORD_ARRAY);
				glEnableClientState(GL_VERTEX_ARRAY);
				glTexCoordPointer(2, GL_SHORT, 0, &tlist[0]);
				glVertexPointer(2, GL_SHORT, 0, &vlist[0]);
				glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
				glDisableClientState(GL_TEXTURE_COORD_ARRAY);
				glDisableClientState(GL_VERTEX_ARRAY);
			}
			m = m->link;
		} while(m != 0) ;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	モーション・オブジェクト列の描画
		@param[in]	hs	ハンドルのポインター
		@param[in]	atr	アトリビュート
		@param[in]	pos	描画位置
		@param[in]	linear	「true」ならリニア
	 */
	//-----------------------------------------------------------------//
	void mobj::draws(const handle* hs, attribute::type atr, const vtx::spos& pos, bool linear)
	{
		handle h;
		vtx::spos p = pos;
		while((h = *hs++) != 0) {
			draw(h, atr, p, linear);
			const vtx::spos& size = get_size(h);
			p.x += size.x + space_;
		}
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	拡張登録されたモーションオブジェクトのリサイズ
		@param[in]	h		ハンドル
		@param[in]	size	新しいサイズ
		@return エラーなら「false」
	 */
	//-----------------------------------------------------------------//
	bool mobj::resize(handle h, const vtx::spos& size)
	{
		if(h == 0 && h >= objs_.size()) {
			return false;
		}

		obj* m = objs_[h];
		if(!m->ex) return false;
		if(m->nx == 0 || m->ny == 0) return false;
//		if(m->ww == size.x && m->hh == size.y) return true; 

		vtx::spos grid(m->tw, m->th);
		int nx = m->nx;
		int ny = m->ny;
		for(int j = 0; j < ny; ++j) {
			for(int i = 0; i < nx; ++i) {
				if(nx == 1) {
					m->oxp = 0;
					m->dw = size.x;
				} else if(i == 0) {
					m->oxp = 0;
					m->dw = grid.x;
				} else if(i == 1) {
					m->oxp = grid.x;
					m->dw = size.x - grid.x * 2;
				} else {
					m->oxp = size.x - grid.x;
					m->dw = grid.x;
				}
				m->oxn = size.x - (m->oxp + m->dw);
				m->size.x = size.x;

				if(ny == 1) {
					m->oyp = 0;
					m->dh = size.y;
				} else if(j == 0) {
					m->oyp = 0;
					m->dh = grid.y;
				} else if(j == 1) {
					m->oyp = grid.y;
					m->dh = size.y - grid.y * 2;
				} else {
					m->oyp = size.y - grid.y;
					m->dh = grid.y;
				}
				m->oyn = size.y - (m->oyp + m->dh);
				m->size.y = size.y;

				m = m->link;
			}
		}

		return true;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	廃棄
	 */
	//-----------------------------------------------------------------//
	void mobj::destroy()
	{
		for(objs_it it = objs_.begin(); it != objs_.end(); ++it) {
			obj* o = *it;
			while(o != 0) {
				obj* tmp = o;
				o = tmp->link;
				delete tmp;
			}
		}
		objs().swap(objs_);

		destroy_texture_page(texture_mems_);
		texture_mems().swap(texture_mems_);

		if(!texture_pages_.empty()) {
			::glDeleteTextures(texture_pages_.size(), &texture_pages_[0]);
		}
	}
}
/* ----- End Of File "glmobj.cpp" ----- */
