#pragma once
//=====================================================================//
/*!	@file
	@brief	OpenGL モーション・オブジェクト・クラス（ヘッダー）
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include <vector>
#include "gl_fw/gl_info.hpp"
#include "img_io/i_img.hpp"
#include "img_io/img_rgba8.hpp"
#include "utils/vtx.hpp"

namespace gl {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	テクスチャーページ内の割り当てを行うクラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	class texture_mem {

		GLuint	id_;

		int		block_w_;
		int		block_h_;
		int		block_num_;

		std::vector<unsigned int>	tex_map_;

		bool scan(int x, int w, int y, int h) {
			for(int i = 0; i < h; ++i) {
				for(int j = 0; j < w; ++j) {
					int a = (i + y) * block_w_ + (j + x);
					if(tex_map_[a >> 5] & (1 << (a & 31))) return false;
				}
			}
			return true;
		}

		void fill(int x, int w, int y, int h) {
			for(int i = 0; i < h; ++i) {
				for(int j = 0; j < w; ++j) {
					int a = (i + y) * block_w_ + (j + x);
					tex_map_[a >> 5] |= 1 << (a & 31);
				}
			}
		}

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief	コンストラクター
		 */
		//-----------------------------------------------------------------//
		texture_mem() : id_(0), block_w_(0), block_h_(0), block_num_(0) { }


		//-----------------------------------------------------------------//
		/*!
			@brief	デストラクター
		 */
		//-----------------------------------------------------------------//
		~texture_mem() { }


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
		void initialize(int pgw, int pgh, GLint internalFormat, bool mp, const img::img_rgba8& im);


		//-----------------------------------------------------------------//
		/*!
			@brief	いっぱいか調べる
			@return いっぱいなら「true」
		 */
		//-----------------------------------------------------------------//
		bool is_full() const {
			if(block_num_ > 0) return false;
			else return true;
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
		bool allocate(short& x, short& y, short w, short h);


		//-----------------------------------------------------------------//
		/*!
			@brief	テクスチャーのIDを返す。
			@return	テクスチャーID
		 */
		//-----------------------------------------------------------------//
		GLuint get_id() const { return id_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	廃棄
		 */
		//-----------------------------------------------------------------//
		void destroy();


		//-----------------------------------------------------------------//
		/*!
			@brief	テクスチャーページの利用状態をダンプ（デバッグ確認用）
			@param[in]	ost	ファイル・ストリーム
		 */
		//-----------------------------------------------------------------//
		void dump(std::ostream& ost);
	};


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	モーション・オブジェクト（一般的にはスプライト）@n
				の機能を提供するクラス。
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	struct mobj {

		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		/*!
			@brief	アトリビュート（描画オプション）
		*/
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		enum attribute {
			normal,		///< 通常描画
			h_flip,		///< 水平方向反転
			v_flip,		///< 垂直方向反転
			hv_flip,	///< 水平、垂直方向反転
		};


		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		/*!
			@brief	モーション・オブジェクト構造体
		*/
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		struct obj {
			GLuint	id;		///< texture ID

			short	tx;		///< texture coordinate U location
			short	ty;		///< texture coordinate V location
			short	tw;		///< texture width (cordinate U)
			short	th;		///< texture height (cordinate V)

			short	oxp;	///< draw offset X positive
			short	oyp;	///< draw offset Y positive
			short	oxn;	///< draw offset X negative (flip)
			short	oyn;	///< draw offset Y negative (flip)
			short	dw;		///< draw width
			short	dh;		///< draw height

			vtx::spos	size;		///< master object size

			char	nx;		///< 分割数Ｘ
			char	ny;		///< 分割数Ｙ

			bool	mp;		///< mipmap
			bool	ex;		///< EX format

			obj*	link;	///< リンクオブジェクト
			obj() : link(0) { }
		};

		typedef unsigned int			handle;			///< 管理ハンドル
		typedef std::vector<handle>		handles;   		///< 管理ハンドル群
		typedef handles::iterator		handles_it;		///< 管理ハンドル群イテレーター
		typedef handles::const_iterator	handles_cit;	///< 管理ハンドル群 RO イテレーター

	private:
		static const int mipmap_level_max_ = 5;		///< ミップマップの最大レベル

		int			tex_page_w_;
		int			tex_page_h_;

		typedef std::vector<obj*>::iterator			objs_it;
		typedef std::vector<obj*>::const_iterator	objs_cit;
		typedef std::vector<obj*>					objs;

		objs		objs_;

		typedef std::vector<texture_mem>		texture_mems;
		typedef texture_mems::iterator			tex_mem_it;
		typedef texture_mems::const_iterator	tex_mem_cit;
		texture_mems							texture_mems_;

		int			space_;
		GLint		internal_format_;
		
		typedef std::vector<GLuint>				texture_pages;
		texture_pages							texture_pages_;

		bool allocate_texture(texture_mems& mems, obj* o);
		void add_texture_page(texture_mems& mems, obj* o, const img::img_rgba8& im);
		void destroy_texture_page(texture_mems& mems);

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief	コンストラクター
		 */
		//-----------------------------------------------------------------//
		mobj() : tex_page_w_(256), tex_page_h_(256),
			space_(0), internal_format_(GL_RGBA) { }


		//-----------------------------------------------------------------//
		/*!
			@brief	デストラクター
		 */
		//-----------------------------------------------------------------//
		~mobj() {
			destroy();
			destroy_texture_page(texture_mems_);
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	モーションオブジェクト初期化
			@param[in]	pgw	テクスチャーページの最大横サイズ
			@param[in]	pgh	テクスチャーページの最大高さサイズ
		 */
		//-----------------------------------------------------------------//
		void initialize(int pgw = 256, int pgh = 256);


		//-----------------------------------------------------------------//
		/*!
			@brief	オブジェクトを得る。
			@param[in]	h	ハンドル
			@return モーションオブジェクト構造体のポインター
		 */
		//-----------------------------------------------------------------//
		const obj* get_obj(handle h) const {
			if(h > 0 && h < objs_.size()) return objs_[h]; else return 0;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	スペーシングを設定する
			@param[in]	spc		スペース幅
		 */
		//-----------------------------------------------------------------//
		void set_space(int spc) { space_ = spc; }


		//-----------------------------------------------------------------//
		/*!
			@brief	モーションオブジェクトの大きさを得る
			@param[in]	h	ハンドル
			@return サイズ
		 */
		//-----------------------------------------------------------------//
		const vtx::spos& get_size(handle h) const;


		//-----------------------------------------------------------------//
		/*!
			@brief	モーションオブジェクトとして登録する。
			@param[in]	imf		登録画像
			@param[in]	mipmap	ミっプマップの場合は「true」
			@return	ハンドルを返す
		 */
		//-----------------------------------------------------------------//
		handle install(const img::i_img* imf, bool mipmap = false);


		//-----------------------------------------------------------------//
		/*!
			@brief	モーションオブジェクトとして拡張登録する。
			@param[in]	imf		登録最小画像
			@param[in]	size	実際の拡張サイズ
			@param[in]	grid	グリッド・サイズ（指定しないと１６ピクセル）
			@return	ハンドルを返す
		 */
		//-----------------------------------------------------------------//
		handle install(const img::i_img* imf, const vtx::spos& size, const vtx::spos& grid = vtx::spos(16, 16));


		//-----------------------------------------------------------------//
		/*!
			@brief	テクスチャーを登録する。
			@param[in]	imf		イメージインターフェース
			@return	テクスチャー・ハンドルを返す
		 */
		//-----------------------------------------------------------------//
		GLuint install_texture(const img::i_img* imf);


		//-----------------------------------------------------------------//
		/*!
			@brief	テクスチャーを登録する。
			@param[in]	ptr		圧縮テクスチャーのポインター
			@param[in]	size	圧縮テクスチャーのサイズ
			@param[in]	type	圧縮タイプ
			@param[in]	w		横幅
			@param[in]	h		高さ
			@return	テクスチャー・ハンドルを返す
		 */
		//-----------------------------------------------------------------//
		GLuint install_compressed_texture(const void* ptr, size_t size, int type, int w, int h);


		//-----------------------------------------------------------------//
		/*!
			@brief	画像をモーションオブジェクトとして登録する。
			@param[in]	root	モーションオブジェクト設定情報
			@return	ハンドルを返す
		 */
		//-----------------------------------------------------------------//
		handle install_direct(const obj& root);


		//-----------------------------------------------------------------//
		/*!
			@brief	内部テクスチャー保持のフォーマットを設定する
			@param[in]	id	内部フォーマットの OpenGL 番号
		 */
		//-----------------------------------------------------------------//
		void set_internal_format(GLint id) { internal_format_ = id; }


		//-----------------------------------------------------------------//
		/*!
			@brief	モーション・オブジェクト描画前設定（テクスチャースケールのみ）
		*/
		//-----------------------------------------------------------------//
		void setup_matrix();


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
		void setup_matrix(int x, int y, int w, int h, float zn = -1.0f, float zf = 1.0f);


		//-----------------------------------------------------------------//
		/*!
			@brief	モーション・オブジェクト描画用マトリックスの設定
			@param[in]	w	横幅の指定
			@param[in]	h	高さの指定
			@param[in]	zn	Z (手前)
			@param[in]	zf	Z (奥)
		 */
		//-----------------------------------------------------------------//
		void setup_matrix(int w, int h, float zn = -1.0f, float zf = 1.0f) {
			setup_matrix(0, 0, w, h, zn, zf);
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
		void copy_image(handle h, const img::i_img* src, int dst_x, int dst_y);


		//-----------------------------------------------------------------//
		/*!
			@brief	モーション・オブジェクトの描画
			@param[in]	h	ハンドル
			@param[in]	atr	アトリビュート
			@param[in]	xx	描画位置 X
			@param[in]	yy	描画位置 Y
			@param[in]	linear	「true」ならリニアフィルター
		 */
		//-----------------------------------------------------------------//
		void draw(handle h, attribute atr, short xx, short yy, bool linear = true);


		//-----------------------------------------------------------------//
		/*!
			@brief	モーション・オブジェクトの描画（原点を中心）
			@param[in]	h	ハンドル
			@param[in]	atr	アトリビュート
			@param[in]	linear	「true」ならリニア
		 */
		//-----------------------------------------------------------------//
		void draw_center(handle h, attribute atr, bool linear = true) {
			const vtx::spos& size = get_size(h);
			draw(h, atr, size.x / -2, size.y / -2, linear);
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
		void draw_sub(handle h, attribute atr, short xx, short yy, short ox, short oy, short ww, short hh, bool liner = true);


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
		void draws(const handle* hs, attribute atr, short xx, short yy, bool linear = true);


		//-----------------------------------------------------------------//
		/*!
			@brief	拡張登録されたモーションオブジェクトのリサイズ
			@param[in]	h		ハンドル
			@param[in]	size	新しいサイズ
			@return エラーなら「false」
		 */
		//-----------------------------------------------------------------//
		bool resize(handle h, const vtx::spos& size);


		//-----------------------------------------------------------------//
		/*!
			@brief	シザー方形を定義
			@param[in]	x	左下隅 X 軸
			@param[in]	y	左下隅 Y 軸
			@param[in]	w	横幅
			@param[in]	h	高さ
		 */
		//-----------------------------------------------------------------//
		static void set_scissor(int x, int y, int w, int h) {
			::glEnable(GL_SCISSOR_TEST);
			::glScissor(x, y, w, h);
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	シザー方形を廃棄
		 */
		//-----------------------------------------------------------------//
		static void reset_scissor() { ::glDisable(GL_SCISSOR_TEST); }


		//-----------------------------------------------------------------//
		/*!
			@brief	テクスチャー・マトリックスを元に戻す
		 */
		//-----------------------------------------------------------------//
		void restore_matrix() {
			::glMatrixMode(GL_TEXTURE);
			::glLoadIdentity();
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	廃棄
		 */
		//-----------------------------------------------------------------//
		void destroy();

	};	// mobj

};	// gui

