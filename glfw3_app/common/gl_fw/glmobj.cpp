//=====================================================================//
/*!	@file
	@brief	OpenGL モーション・オブジェクト・クラス
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include "glmobj.hpp"
#include "glutils.hpp"
#include "img_io/img_utils.hpp"
#include <boost/format.hpp>

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

		::glGenTextures(1, &id_);

		::glBindTexture(GL_TEXTURE_2D, id_);
		::glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		if(im.get_size().x != pgw || im.get_size().y != pgh) {
			img_rgba8 dst;
			dst.create(vtx::spos(pgw, pgh), true);
			dst.fill(rgba8(0));
			dst.copy(0, 0, im, 0, 0, im.get_size().x, im.get_size().y);
			::glTexImage2D(GL_TEXTURE_2D, 0,
						   internalFormat, pgw, pgh, 0, GL_RGBA, GL_UNSIGNED_BYTE, dst.get_image());
		} else {
			::glTexImage2D(GL_TEXTURE_2D, 0,
						   internalFormat, pgw, pgh, 0, GL_RGBA, GL_UNSIGNED_BYTE, im.get_image());
		}
#if 0
		if(mp) {
			for(int i = 1; i < g_mipmap_level_max; ++i) {
				pgw /= 2;
				pgh /= 2;
				::glTexImage2D(GL_TEXTURE_2D, i, internalFormat, pgw, pgh, 0, GL_RGBA, GL_UNSIGNED_BYTE, im.get_image());
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
	bool glmobj::allocate_texture(texture_mems& mems, mobj* mo)
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
	void glmobj::add_texture_page(texture_mems& mems, mobj* mo, const img_rgba8& im)
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
	void glmobj::destroy_texture_page(texture_mems& mems)
	{
		unsigned int idsize = mems.size();
		GLuint* idtbl = new GLuint[idsize];
		for(unsigned int i = 0; i < idsize; ++i) {
			texture_mem& mem = mems[i];
			idtbl[i] = mem.get_id();
		}
		::glDeleteTextures(idsize, idtbl);
		delete[] idtbl;
		texture_mems().swap(mems);
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	初期化
		@param[in]	pgw	テクスチャーページの最大横サイズ
		@param[in]	pgh	テクスチャーページの最大高さサイズ
	 */
	//-----------------------------------------------------------------//
	void glmobj::initialize(int pgw, int pgh)
	{
		// 0 番ハンドルは使わないので無効とする。
		mobjs_.clear();
		mobjs_.push_back(0);
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
	const vtx::spos& glmobj::get_size(handle h) const
	{
		if(h > 0 && h < mobjs_.size()) {
			return mobjs_[h]->size;
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
	glmobj::handle glmobj::install(const img::i_img* imf, bool mipmap)
	{
		mipmap = false;

		mobj* root = 0;
		mobj* lk = 0;
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
				mobj* mo = new mobj;
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
					copy_to_rgba8(imf, ox, oy, txw, txh, im, 0, 0);
					bool f = false;
					if(sox == 0 && soy == 0) {
						f = allocate_texture(texture_mems_, mo);
					}
					if(!f) {
						add_texture_page(texture_mems_, mo, im);
					} else {
						::glBindTexture(GL_TEXTURE_2D, mo->id);
						::glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
						::glTexSubImage2D(GL_TEXTURE_2D, 0,
							mo->tx, mo->ty, txw, txh, GL_RGBA, GL_UNSIGNED_BYTE,
							im.get_image());
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
///							::glTexSubImage2D(GL_TEXTURE_2D, i, ofsx, ofsy, txw, txh, GL_RGBA, GL_UNSIGNED_BYTE, im);
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

		handle h = mobjs_.size();
		mobjs_.push_back(root);
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
	glmobj::handle glmobj::install(const img::i_img* imf, const vtx::spos& size, const vtx::spos& grid)
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

		mobj* root = 0;
		mobj* lk = 0;
		short oy = 0;
		for(int j = 0; j < jn; ++j) {
			short ox = 0;
			for(int i = 0; i < in; ++i) {
				mobj* mo = new mobj;
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
					copy_to_rgba8(imf, ox + sofs[i], oy + sofs[j],
						mo->dw + cpyw[i], mo->dh + cpyw[j], im, dofs[i], dofs[j]);
					mo->tw = im.get_size().x;
					mo->th = im.get_size().y;
					bool f = allocate_texture(texture_mems_, mo);
					if(!f) {
						add_texture_page(texture_mems_, mo, im);
					} else {
						::glBindTexture(GL_TEXTURE_2D, mo->id);
						::glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
						::glTexSubImage2D(GL_TEXTURE_2D, 0,
										  mo->tx, mo->ty, im.get_size().x, im.get_size().y,
										  GL_RGBA, GL_UNSIGNED_BYTE, im.get_image());
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
		handle h = mobjs_.size();
		mobjs_.push_back(root);
		return h;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	テクスチャーを登録する。
		@param[in]	imf		イメージインターフェース
		@return	テクスチャー・ハンドルを返す
	 */
	//-----------------------------------------------------------------//
	GLuint glmobj::install_texture(const img::i_img* imf)
	{
		if(imf == 0) return 0;

		GLuint id = 0;
		int level = 0;
		const vtx::spos& size = imf->get_size();
		if(imf->get_type() == IMG::FULL8) {
			::glGenTextures(1, &id);
			::glBindTexture(GL_TEXTURE_2D, id);
			::glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
			::glTexImage2D(GL_TEXTURE_2D, level, internal_format_,
						   size.x, size.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, imf->get_image());

		} else if(imf->get_type() == IMG::INDEXED8) {
			::glGenTextures(1, &id);
			::glBindTexture(GL_TEXTURE_2D, id);
			img_rgba8 im;
			im.create(size, true);
			copy_to_rgba8(imf, im, 0, 0);
			::glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
			::glTexImage2D(GL_TEXTURE_2D, level, internal_format_,
						   size.x, size.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, im.get_image());
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
	GLuint glmobj::install_compressed_texture(const void* ptr, size_t size, int type, int w, int h)
	{
		int level = 0;
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
	glmobj::handle glmobj::install_direct(const mobj& root)
	{
		mobj* top = new mobj;
		*top = root;
		mobj* p = top;
		while(p->link) {
			mobj* tmp = new mobj;
			tmp = p->link;
			p->link = tmp;
			p = tmp;
		}
		handle h = mobjs_.size();
		mobjs_.push_back(top);
		return h;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	モーション・オブジェクト描画前設定（テクスチャースケールのみ）
	 */
	//-----------------------------------------------------------------//
	void glmobj::setup_matrix()
	{
		::glMatrixMode(GL_TEXTURE);
		::glLoadIdentity();
		float u = static_cast<float>(tex_page_w_);
		float v = static_cast<float>(tex_page_h_);
		::glScalef(1.0f / u, 1.0f / v, 1.0f);

		::glMatrixMode(GL_MODELVIEW);
		::glLoadIdentity();
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
	void glmobj::setup_matrix(int x, int y, int w, int h, float zn, float zf)
	{
		setup_matrix();

		::glMatrixMode(GL_PROJECTION);
		::glLoadIdentity();
		glOrthof(static_cast<float>(x), static_cast<float>(w),
				 static_cast<float>(h), static_cast<float>(y), zn, zf);
//		float ww = (float)w;
//		float hh = (float)h;
//		glTranslatef(ww * 0.5f, hh * 0.5f, 0.0f);
//		glRotatef(-90.0f, 0.0f, 0.0f, 1.0f);
//		glTranslatef(-ww * 0.5f, -hh * 0.5f, 0.0f);
		::glMatrixMode(GL_MODELVIEW);
		::glLoadIdentity();
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	モーションオブジェクトのイメージを更新
		@param[in]	h		ハンドル
		@param[in]	src		ソース、イメージインターフェース
		@param[in]	dst_x	ディスとネーションの X オフセット
		@param[in]	dst_y	ディスとネーションの Y オフセット
	 */
	//-----------------------------------------------------------------//
	void glmobj::copy_image(handle h, const img::i_img* src, int dst_x, int dst_y)
	{
		const i_img* imif = src;
		// RGBA8 以外の画像タイプなら、一旦 RGBA8 形式で画像を作成。
		img_rgba8 tmp;
		if(src->get_type() != IMG::FULL8) {
			tmp.create(src->get_size(), true);
			copy_to_rgba8(src, tmp, 0, 0);
			imif = &tmp;
		}

		const mobj* m = mobjs_[h];
		while(m != 0) {
			::glBindTexture(GL_TEXTURE_2D, m->id);
			int level = 0;
//			if(mop->link == 0 && mop->w >= imif->get_width() && mop->h >= imif->get_height()) {
				::glTexSubImage2D(GL_TEXTURE_2D, level,
								  m->tx + dst_x, m->ty + dst_y,
								  imif->get_size().x, imif->get_size().y,
								  GL_RGBA, GL_UNSIGNED_BYTE, imif->get_image());
//			}
			m = m->link;
		}
		::glFlush();
	}

	static void tex_para(bool linear)
	{
		::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		if(linear) {
			::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		} else {
			::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		}
	}

	//-----------------------------------------------------------------//
	/*!
		@brief	モーション・オブジェクトを描画する。
		@param[in]	h	ハンドル
		@param[in]	atr	アトリビュート
		@param[in]	xx	描画位置 X
		@param[in]	yy	描画位置 Y
		@param[in]	linear	「true」ならリニアフィルター
	 */
	//-----------------------------------------------------------------//
	void glmobj::draw(handle h, attribute atr, short xx, short yy, bool linear)
	{
		if(h == 0 || h >= mobjs_.size()) return;

		bool hf = false;
		bool vf = false;
		if(atr != normal) {
			if(atr == h_flip) hf = true;
			else if(atr == v_flip) vf = true;
			else if(atr == hv_flip) hf = vf = true;
		}

		const mobj* m = mobjs_[h];
		do {
			short xp;
			if(hf) {
				xp = xx + m->oxn;
			} else {
				xp = xx + m->oxp;
			}
			short yp;
			if(vf) {
				yp = yy + m->oyn;
			} else {
				yp = yy + m->oyp;
			}
			::glBindTexture(GL_TEXTURE_2D, m->id);

			tex_para(linear);

			spos vlist[4];
			spos tlist[4];
			switch(atr) {

			case h_flip:
				tlist[0].set(m->tx + m->tw, m->ty);
				vlist[0].set(   xp,            yp);
				tlist[1].set(m->tx + m->tw, m->ty + m->th);
				vlist[1].set(   xp,            yp + m->dh);
				tlist[3].set(m->tx,         m->ty + m->th);
				vlist[3].set(   xp + m->dw,    yp + m->dh);
				tlist[2].set(m->tx,         m->ty);
				vlist[2].set(   xp + m->dw,    yp);
				break;

			case v_flip:
				tlist[0].set(m->tx,         m->ty + m->th);
				vlist[0].set(   xp,            yp);
				tlist[1].set(m->tx,         m->ty);
				vlist[1].set(   xp,            yp + m->dh);
				tlist[3].set(m->tx + m->tw, m->ty);
				vlist[3].set(   xp + m->dw,    yp + m->dh);
				tlist[2].set(m->tx + m->tw, m->ty + m->th);
				vlist[2].set(   xp + m->dw,    yp);
				break;

			case hv_flip:
				tlist[0].set(m->tx + m->tw, m->ty + m->th);
				vlist[0].set(   xp,            yp);
				tlist[1].set(m->tx + m->tw, m->ty);
				vlist[1].set(   xp,            yp + m->dh);
				tlist[3].set(m->tx,         m->ty);
				vlist[3].set(   xp + m->dw,    yp + m->dh);
				tlist[2].set(m->tx,         m->ty + m->th);
				vlist[2].set(   xp + m->dw,    yp);
				break;

			case normal:
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
			::glEnableClientState(GL_TEXTURE_COORD_ARRAY);
			::glEnableClientState(GL_VERTEX_ARRAY);
			::glTexCoordPointer(2, GL_SHORT, 0, &tlist[0]);
			::glVertexPointer(2, GL_SHORT, 0, &vlist[0]);
			::glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
			::glDisableClientState(GL_TEXTURE_COORD_ARRAY);
			::glDisableClientState(GL_VERTEX_ARRAY);

			m = m->link;
		} while(m != 0) ;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	モーション・オブジェクトの描画
		@param[in]	h	ハンドル
		@param[in]	atr	アトリビュート
		@param[in]	xx	描画位置 X
		@param[in]	yy	描画位置 Y
		@param[in]	ox	オフセット X
		@param[in]	oy	オフセット Y
		@param[in]	ww	描画幅
		@param[in]	hh	描画高さ
		@param[in]	linear	「true」ならリニアフィルター
	 */
	//-----------------------------------------------------------------//
	void glmobj::draw_sub(handle h, attribute atr, short xx, short yy, short ox, short oy, short ww, short hh, bool linear)
	{
		if(h == 0 || h >= mobjs_.size()) return;

		const mobj* m = mobjs_[h];
		do {
			short xp = m->oxp;
			short yp = m->oyp;
			short w  = m->dw;
			short h  = m->dh;

			short tx = m->tx;
			short ty = m->ty;

			short dx;
			if(ox >= xp) dx = ox - xp + ww;
			else dx = xp - ox + w;

			short dy;
			if(oy >= yp) dy = oy - yp + hh;
			else dy = yp - oy + h;

			if(dx < (w + ww) && dy < (h + hh)) {
				if(ox >= xp) {
					w  -= ox - xp;
					if(w > ww) w = ww;
					tx += ox - xp;
					xp += ox - xp;
				} else {
					w = ox - xp + ww;
				}

				if(oy >= yp) {
					h  -= ox - xp;
					if(h > hh) h = hh;
					ty += oy - yp;
					yp += oy - yp;
				} else {
					h = oy - yp + hh;
				}

				xp += xx;
				yp += yy;
				::glBindTexture(GL_TEXTURE_2D, m->id);

				tex_para(linear);

				spos vlist[4];
				spos tlist[4];
				switch(atr) {
				case h_flip:
					tlist[0].set(tx + w, ty);
					vlist[0].set(xp, yp);
					tlist[1].set(tx + w, ty + h);
					vlist[1].set(xp, yp + h);
					tlist[3].set(tx, ty + h);
					vlist[3].set(xp + w, yp + h);
					tlist[2].set(tx, ty);
					vlist[2].set(xp + w, yp);
					break;

				case v_flip:
					tlist[0].set(tx, ty + h);
					vlist[0].set(xp, yp);
					tlist[1].set(tx, ty);
					vlist[1].set(xp, yp + h);
					tlist[3].set(tx + w, ty);
					vlist[3].set(xp + w, yp + h);
					tlist[2].set(tx + w, ty + h);
					vlist[2].set(xp + w, yp);
					break;

				case hv_flip:
					tlist[0].set(tx + w, ty + h);
					vlist[0].set(xp, yp);
					tlist[1].set(tx + w, ty);
					vlist[1].set(xp, yp + h);
					tlist[3].set(tx, ty);
					vlist[3].set(xp + w, yp + h);
					tlist[2].set(tx, ty + h);
					vlist[2].set(xp + w, yp);
					break;

				case normal:
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
				::glEnableClientState(GL_TEXTURE_COORD_ARRAY);
				::glEnableClientState(GL_VERTEX_ARRAY);
				::glTexCoordPointer(2, GL_SHORT, 0, &tlist[0]);
				::glVertexPointer(2, GL_SHORT, 0, &vlist[0]);
				::glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
				::glDisableClientState(GL_TEXTURE_COORD_ARRAY);
				::glDisableClientState(GL_VERTEX_ARRAY);
			}
			m = m->link;
		} while(m != 0) ;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	モーション・オブジェクト列の描画
		@param[in]	hs	ハンドルのポインター
		@param[in]	atr	アトリビュート
		@param[in]	xx	描画位置 X
		@param[in]	yy	描画位置 Y
		@param[in]	linear	「true」ならリニア
	 */
	//-----------------------------------------------------------------//
	void glmobj::draws(const handle* hs, attribute atr, short xx, short yy, bool linear)
	{
		handle h;
		while((h = *hs++) != 0) {
			draw(h, atr, xx, yy, linear);
			const vtx::spos& size = get_size(h);
			xx += size.x + space_;
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
	bool glmobj::resize(handle h, const vtx::spos& size)
	{
		if(h == 0 && h >= mobjs_.size()) {
			return false;
		}

		mobj* m = mobjs_[h];
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
	void glmobj::destroy()
	{
		for(mobjs_it it = mobjs_.begin(); it != mobjs_.end(); ++it) {
			mobj* mo = *it;
			while(mo != 0) {
				mobj* m = mo;
				mo = m->link;
				delete m;
			}
		}
		mobjs().swap(mobjs_);

		destroy_texture_page(texture_mems_);
		texture_mems().swap(texture_mems_);

		if(!texture_pages_.empty()) {
			::glDeleteTextures(texture_pages_.size(), &texture_pages_[0]);
		}
	}
}
/* ----- End Of File "glmobj.cpp" ----- */
