#pragma once
//=====================================================================//
/*!	@file
	@brief	OpenGL フォント・クラス（ヘッダー）
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include <vector>
#include <map>
#include <stack>
#include <string>
#include <boost/unordered_map.hpp>
#include "core/ftimg.hpp"
#include "gl_fw/gl_info.hpp"
#include "img_io/i_img.hpp"
#include "utils/vtx.hpp"
#include "utils/string_utils.hpp"

namespace gl {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	サイズ、コード
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	struct size_code_t {
		uint32_t	size_;
		uint32_t	code_;

		size_code_t(uint32_t size = 0, uint32_t code = 0) : size_(size), code_(code) { }

		inline size_t hash() const {
			size_t h = 0;
			boost::hash_combine(h, size_);
			boost::hash_combine(h, code_);
			return h;
		}
		bool operator == (const size_code_t& t) const {
			return (t.size_ == size_ && t.code_ == code_);
		}
	};
	inline size_t hash_value(const size_code_t& t) { return t.hash(); }


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	fonts クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	class fonts {

		static const int texture_page_width  = 256;	///< テクスチャーページの幅
		static const int texture_page_height = 256;	///< テクスチャーページの高さ

		struct tex_map {
			GLuint	id;		///< テクスチャー ID
			int		lcx;	///< ロケーションX
			int		lcy;	///< ロケーションY
			int		w;		///< フォントの幅
			int		h;		///< フォントの高さ
			img::ftimg::metrics	met;	///< フォントのメトリックス
		};

		typedef boost::unordered_map<size_code_t, tex_map>	fcode_map;

		// フォント基本環境
		struct finfo_t {
			short	size;			///< フォント基本サイズ
			short	spaceing;		///< スペーシング
			bool	antialias;		///< アンチエリアス・シェイプ
			bool	proportional;	///< プロポーショナルフォント
			bool	center;			///< センター描画
			finfo_t() : size(24), spaceing(2), antialias(true), proportional(true), center(true) { }
		};

		// コード・マップ構造体
		struct face_t {
			fcode_map	fcode_map_;
			/// 各サイズ毎の、半角文字の最大固定サイズ
			typedef std::map<int, int>	fix_width_map;
			fix_width_map	fix_width_map_;
			finfo_t			info_;
		};

		struct font_face {
			std::string	type_;
			int			size_;
		};
		std::stack<font_face>	stack_face_;

		// フォント・フェース・マップ
		typedef std::map<std::string, face_t>	face_map;
		face_map		face_map_;
		face_t*			face_;

		fcode_map::const_iterator find_font_code(uint32_t code) const {
			return face_->fcode_map_.find(size_code_t(face_->info_.size, code));
		}

		fcode_map::iterator install_font_code(uint32_t code, const tex_map& tmap) {
			std::pair<fcode_map::iterator, bool> ret;
			ret = face_->fcode_map_.insert(fcode_map::value_type(size_code_t(face_->info_.size, code), tmap));
			return ret.first;
		}

		struct tex_page {
			GLuint	id;
			int	size;
			int	x;
			int	y;
		};

		// サイズ別のマップ（効率が落ちるので、一つのページに複数のサイズを登録しない）
		typedef std::map<int, tex_page> tex_page_map;
		tex_page_map		tex_page_map_;

		std::vector<GLuint>	pages_;

		struct tex_uv {
			short	u, v;
		};
		struct vtx_xy {
			short	x, y;
		};
		tex_uv		coord_[4];
		vtx_xy		vertex_[4];

		img::rgba8	fore_color_;
		img::rgba8	back_color_;
		bool	setup_;
		bool	render_back_;
		bool	h_flip_;
		bool	v_flip_;
		bool	ccw_;
		bool	swap_color_;

		vtx::srect	clip_;

		bool allocate_font_texture_(int width, int height, tex_map& tmap);


		int font_width_(uint32_t code, int fw, int fh);

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief	コンストラクター
		*/
		//-----------------------------------------------------------------//
		fonts() : face_(0),
			fore_color_(255, 255, 255, 255), back_color_(0, 0, 0, 255),
			setup_(false),
			render_back_(false), h_flip_(false), v_flip_(false), ccw_(false),
			swap_color_(false), clip_(0, 0, 1024, 1024)
			{ }


		//-----------------------------------------------------------------//
		/*!
			@brief	デストラクター
		*/
		//-----------------------------------------------------------------//
		~fonts() { destroy(); }


		//-----------------------------------------------------------------//
		/*!
			@brief	初期化
			@param[in]	fpath	フォントパス
			@param[in]	alias	フォント別名
		*/
		//-----------------------------------------------------------------//
		void initialize(const std::string& fpath, const std::string& alias);


		//-----------------------------------------------------------------//
		/*!
			@brief	フォントをインストール
			@param[in]	ttfname	TrueType フォントファイルのパス
			@param[in]	alias	フォントの別名
			@return 正常なら「true」を返す
		*/
		//-----------------------------------------------------------------//
		bool install_font_type(const std::string& ttfname, const std::string& alias = "");


		//-----------------------------------------------------------------//
		/*!
			@brief	フォント・タイプを変更
			@return 正常なら「true」を返す
		*/
		//-----------------------------------------------------------------//
		bool set_font_type(const std::string& alias);


		//-----------------------------------------------------------------//
		/*!
			@brief	フォント・タイプを取得
			@return フォントタイプの別名
		*/
		//-----------------------------------------------------------------//
		const std::string& get_font_type() const;


		//-----------------------------------------------------------------//
		/*!
			@brief	フォント・サイズ設定
			@param[in]	s	インストールするフォントの高さ
		*/
		//-----------------------------------------------------------------//
		void set_font_size(int s);


		//-----------------------------------------------------------------//
		/*!
			@brief	フォント・サイズを得る
			@return		フォントの高さ
		*/
		//-----------------------------------------------------------------//
		int get_font_size() const { return face_->info_.size; }


		//-----------------------------------------------------------------//
		/*!
			@brief	フォント・スペーシングを設定
			@param[in]	spc	スペーシング
		*/
		//-----------------------------------------------------------------//
		void set_spaceing(int spc) { face_->info_.spaceing = spc; }


		//-----------------------------------------------------------------//
		/*!
			@brief	フォント・スペーシングを得る
			@return		スペーシング
		*/
		//-----------------------------------------------------------------//
		int get_spaceing() const { return face_->info_.spaceing; }


		//-----------------------------------------------------------------//
		/*!
			@brief	プロポーショナル・フォントを設定
			@param[in]	f	「false」の場合無効
		 */
		//-----------------------------------------------------------------//
		void enable_proportional(bool f = true) { face_->info_.proportional = f; }


		//-----------------------------------------------------------------//
		/*!
			@brief	アンチエリアスの設定
			@param[in]	f	「false」の場合無効
		 */
		//-----------------------------------------------------------------//
		void enable_antialias(bool f = true) {
			face_->info_.antialias = f;
			img::ftimg::get_instance().set_antialias(f);
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	センターに描画する場合
			@param[in]	flag	「false」の場合無効
		 */
   		//-----------------------------------------------------------------//
		void enable_center(bool f = true) { face_->info_.center = f; }


		//-----------------------------------------------------------------//
		/*!
			@brief	フォント環境を退避
		*/
		//-----------------------------------------------------------------//
		void push_font_face() {
			font_face t;
			t.type_ = get_font_type();
			t.size_ = get_font_size();
			stack_face_.push(t);
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	フォント環境を復帰
		*/
		//-----------------------------------------------------------------//
		void pop_font_face() {
			if(!stack_face_.empty()) {
				const font_face& t = stack_face_.top();
				set_font_type(t.type_);
				set_font_size(t.size_);
				stack_face_.pop();
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	標準的な描画前設定
		*/
		//-----------------------------------------------------------------//
		void setup_matrix();


		//-----------------------------------------------------------------//
		/*!
			@brief	描画前設定
			@param[in]	scx	スクリーンの開始位置 X
			@param[in]	scy	スクリーンの開始位置 Y
			@param[in]	scw	スクリーンの横幅（ピクセル単位）
			@param[in]	sch	スクリーンの高さ（ピクセル単位）
		*/
		//-----------------------------------------------------------------//
		void setup_matrix(int scx, int scy, int scw, int sch);


		//-----------------------------------------------------------------//
		/*!
			@brief	描画前設定
			@param[in]	scw	スクリーンの横幅（ピクセル単位）
			@param[in]	sch	スクリーンの高さ（ピクセル単位）
		*/
		//-----------------------------------------------------------------//
		void setup_matrix(int scw, int sch) { setup_matrix(0, 0, scw, sch); }


		//-----------------------------------------------------------------//
		/*!
			@brief	描画前設定、ビューポートとマトリックスの設定
			@param[in]	rect	開始位置、大きさ
		*/
		//-----------------------------------------------------------------//
		void setup_matrix_with_clip(const vtx::srect& rect);


		//-----------------------------------------------------------------//
		/*!
			@brief	描画後設定
		*/
		//-----------------------------------------------------------------//
		void restore_matrix();


		//-----------------------------------------------------------------//
		/*!
			@brief	水平方向を左右反転（フリップ）する
			@param[in]	flip	「true」を指定すると反転
		*/
		//-----------------------------------------------------------------//
		void set_holizontal_flip(bool flip = true) { h_flip_ = flip; }


		//-----------------------------------------------------------------//
		/*!
			@brief	垂直方向を上下反転（フリップ）する
			@param[in]	flip	「true」を指定すると反転
		*/
		//-----------------------------------------------------------------//
		void set_vertical_flip(bool flip = true) { v_flip_ = flip; }


		//-----------------------------------------------------------------//
		/*!
			@brief	フォントのフォア・カラーを設定
			@param[in]	c	カラー
		*/
		//-----------------------------------------------------------------//
		void set_fore_color(const img::rgba8& c) { fore_color_ = c; }


		//-----------------------------------------------------------------//
		/*!
			@brief	フォントのバック・カラーを設定
			@param[in]	c	カラー
		*/
		//-----------------------------------------------------------------//
		void set_back_color(const img::rgba8& c) { back_color_ = c; }


		//-----------------------------------------------------------------//
		/*!
			@brief	フォア・カラーとバック・カラーを入れ替える
			@param[in]	f	「false」元に戻す。
		*/
		//-----------------------------------------------------------------//
		void swap_color(bool f = true) { swap_color_ = f; }


		//-----------------------------------------------------------------//
		/*!
			@brief	バック・カラーの描画を有効にする。
			@param[in]	value	無効にする場合「false」
		*/
		//-----------------------------------------------------------------//
		void enable_back_color(bool value = true) { render_back_ = value; }


		//-----------------------------------------------------------------//
		/*!
			@brief	クリップ設定
			@param[in]	clip	クリップ領域
		*/
		//-----------------------------------------------------------------//
		void set_clip(vtx::srect& clip) { clip_ = clip; }


		//-----------------------------------------------------------------//
		/*!
			@brief	クリップ基点設定
			@param[in]	org	開始位置
		*/
		//-----------------------------------------------------------------//
		void set_clip_org(const vtx::spos& org) { clip_.org = org; }


		//-----------------------------------------------------------------//
		/*!
			@brief	クリップ・サイズ設定
			@param[in]	size	サイズ
		*/
		//-----------------------------------------------------------------//
		void set_clip_size(const vtx::spos& size) { clip_.size = size; }


		//-----------------------------------------------------------------//
		/*!
			@brief	フォントビットマップの登録
			@param[in]	code	フォントのコード
			@return 登録できたら、「fcode_map」のイテレーターを返す
		*/
		//-----------------------------------------------------------------//
		fcode_map::iterator install_image(uint32_t code);


		//-----------------------------------------------------------------//
		/*!
			@brief	フォントの登録
			@param[in]	code	フォントのコード
			@return 登録できたら、「true」を返す。
		*/
		//-----------------------------------------------------------------//
		bool install_font(uint32_t code) {
			img::ftimg::get_instance().create_bitmap(face_->info_.size, code);
			fcode_map::iterator it = install_image(code);
			if(it == face_->fcode_map_.end()) return false;
			else return true;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	フォントの登録（文字列）
			@param[in]	list	フォントのコード列
		*/
		//-----------------------------------------------------------------//
		void install_font(const uint32_t* list) {
			uint32_t lc;
			while((lc = *list++) != 0) {
				install_font(lc);
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	フォントの登録（文字列）
			@param[in]	list	フォントのコード列
		*/
		//-----------------------------------------------------------------//
		void install_font(const uint16_t* list) {
			uint16_t wc;
			while((wc = *list++) != 0) {
				install_font(wc);
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	フォントの登録
			@param[in]	list	フォントのコード列
		*/
		//-----------------------------------------------------------------//
		void install_font(const char* list) {
			uint8_t c;
			while((c = static_cast<uint8_t>(*list++)) != 0) {
				install_font(c);
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	フォント・テクスチャー・ページを描画する
			@param[in]	page	描画するテクスチャーページ
		*/
		//-----------------------------------------------------------------//
		void draw_page(int page);


		//-----------------------------------------------------------------//
		/*!
			@brief	フォントを描画する
			@param[in]	pos	描画位置
			@param[in]	code	描画するコード
			@return	フォントの幅を返す。
		 */
		//-----------------------------------------------------------------//
		int draw(const vtx::spos& pos, uint32_t code);


		//-----------------------------------------------------------------//
		/*!
			@brief	フォントを描画する
			@param[in]	pos	描画位置
			@param[in]	text	描画文字列
			@param[in]	limit	改行のリミット幅
			@return	描画幅を返す（複数行の場合、最大値）
		 */
		//-----------------------------------------------------------------//
		int draw(const vtx::spos& pos, const std::string& text, short limit = 0) {
			utils::lstring ls;
			utils::utf8_to_utf32(text, ls);
			return draw(pos, ls, limit);
		}

		//-----------------------------------------------------------------//
		/*!
			@brief	フォントを描画する
			@param[in]	pos	描画位置
			@param[in]	text	描画するワイド文字列
			@param[in]	limit	改行のリミット幅
			@return	描画幅を返す（複数行の場合、最大値）
		 */
		//-----------------------------------------------------------------//
		int draw(const vtx::spos& pos, const utils::wstring& text, short limit = 0) {
			utils::lstring ls;
			utils::utf16_to_utf32(text, ls);
			return draw(pos, ls, limit);
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	フォントを描画する
			@param[in]	pos	描画位置
			@param[in]	text	描画ロング文字列
			@param[in]	limit	改行のリミット幅
			@return	描画幅を返す（複数行の場合、最大値）
		 */
		//-----------------------------------------------------------------//
		int draw(const vtx::spos& pos, const utils::lstring& text, short limit = 0);


		//-----------------------------------------------------------------//
		/*!
			@brief	フォントの幅を計算する
			@param[in]	code 計算するフォントのコード
			@return	フォントの幅を返す
		 */
		//-----------------------------------------------------------------//
		int get_width(uint32_t code);


		//-----------------------------------------------------------------//
		/*!
			@brief	フォントの幅を計算する
			@param[in]	text フォントのコード列
			@return	フォントの幅を返す
		 */
		//-----------------------------------------------------------------//
		int get_width(const std::string& text) {
			utils::lstring ls;
			utils::utf8_to_utf32(text, ls);
			return get_width(ls);
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	フォントの幅を計算する
			@param[in]	text フォントのコード列
			@return	フォントの幅を返す
		 */
		//-----------------------------------------------------------------//
		int get_width(const utils::wstring& text) {
			utils::lstring ls;
			utils::utf16_to_utf32(text, ls);
			return get_width(ls);
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	フォントの幅を計算する
			@param[in]	text フォントのロングコード列
			@return	フォントの幅を返す
		 */
		//-----------------------------------------------------------------//
		int get_width(const utils::lstring& text);


		//-----------------------------------------------------------------//
		/*!
			@brief	フォントの高さを取得する
			@return	フォントの高さを返す
		 */
		//-----------------------------------------------------------------//
		int get_height() const { return face_->info_.size; }


		//-----------------------------------------------------------------//
		/*!
			@brief	フォントの高さを取得する
			@param[in]	text フォントのコード列
			@return	フォントの高さを返す
		 */
		//-----------------------------------------------------------------//
		int get_height(const std::string& text) const {
			int h = face_->info_.size;
			const char*p = text.c_str();
			char c;
			while((c = *p++) != 0) {
				if(c == '\n' && *p != 0) {
					h += face_->info_.size;
				}
			}
			return h;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	文字列全体の大きさを取得
			@param[in]	s	文字列
			@return	大きさを返す
		 */
		//-----------------------------------------------------------------//
		vtx::spos get_size(const std::string& s) {
			utils::lstring ls;
			utils::utf8_to_utf32(s, ls);
			return get_size(ls);
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	文字列全体の大きさを取得
			@param[in]	s	文字列
			@return	大きさを返す
		 */
		//-----------------------------------------------------------------//
		vtx::spos get_size(const utils::wstring& s) {
			utils::lstring ls;
			utils::utf16_to_utf32(s, ls);
			return get_size(ls);
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	文字列全体の大きさを取得
			@param[in]	s	文字列
			@return	大きさを返す
		 */
		//-----------------------------------------------------------------//
		vtx::spos get_size(const utils::lstring& s);


		//-----------------------------------------------------------------//
		/*!
			@brief	複数文字列全体の大きさを取得
			@param[in]	wss	複数文字列
			@return	大きさを返す
		 */
		//-----------------------------------------------------------------//
		vtx::spos get_size(const utils::wstrings& wss) {
			vtx::spos size(0, 0);
			BOOST_FOREACH(const utils::wstring& ws, wss) {
				vtx::spos s = get_size(ws);
				size += s;
			}
			return size;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	フォントのバックの描画
			@param[in]	rect	描画位置と大きさ
		 */
		//-----------------------------------------------------------------//
		void draw_back(const vtx::srect& rect);


		//-----------------------------------------------------------------//
		/*!
			@brief	リソースを廃棄する。
		*/
		//-----------------------------------------------------------------//
		void destroy();

	};
}

