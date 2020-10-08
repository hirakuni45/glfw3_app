#pragma once
//=====================================================================//
/*!	@file
	@brief	フォントイメージを freetype2 で扱うクラス
    @author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2017, 2019 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/glfw3_app/blob/master/LICENSE
*/
//=====================================================================//
#include <string>
#include <map>
#include <boost/unordered_map.hpp>
#include <ft2build.h>
#include FT_FREETYPE_H
#include "utils/singleton_policy.hpp"
#include "utils/file_io.hpp"
#include "img_io/img_gray8.hpp"

#include <iostream>

namespace img {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	フォントイメージ(Bitmap)クラス@n
				※FreeType2 ライブラリーを使う。
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	class ftimg : public utils::singleton_policy<ftimg> {

		friend struct utils::singleton_policy<ftimg>;

	public:
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		/*!
			  @brief	フォントの測定基準構造体@n
		                ・水平基準は、フォントのベースライン@n
						・垂直基準は、フォントの中心
		*/
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		struct metrics {
			float	bitmap_w;	///< フォント・ビットマップの横幅
			float	bitmap_h;	///< フォント・ビットマップの高さ
			float	width;		///< フォントの幅
			float	height;		///< フォントの高さ
			float	hori_x;		///< 水平基準 X 軸オフセット
			float	hori_y;		///< 水平基準 Y 軸オフセット
			float	vert_x;		///< 垂直基準 X 軸オフセット
			float	vert_y;		///< 垂直基準 Y 軸オフセット
		};

	private:
		std::string	root_path_;
		std::string	home_path_;

		FT_Library	library_;

		struct atr_t {
			short	offset_;
			short	height_;
			atr_t() : offset_(0), height_(0) { }
		};
		typedef std::map<int, atr_t>	atr_map;

		struct face_t {
			FT_Face		face_;
			atr_map		atr_map_;
			face_t(FT_Face face) : face_(face), atr_map_() { }
		};
		typedef std::pair<std::string, face_t>	face_pair;
		typedef boost::unordered_map<std::string, face_t>	face_map;
		typedef face_map::iterator			face_map_it;
		typedef face_map::const_iterator	face_map_cit;
		face_map	face_map_;

		face_map_it	current_face_;

		FT_Matrix	matrix_;

		metrics		metrics_;
		img_gray8	gray_;

		bool		antialias_;

		face_map_it find_face_(const std::string& name) { return face_map_.find(name); }
		face_map_it install_face_(const std::string& name, const face_t& face) {
			std::pair<face_map_it, bool> ret;
			ret = face_map_.insert(face_pair(name, face));
			return ret.first;
		}
		void erase_face_() { face_map_.clear(); }


#if 0
void get_metrics(uint32_t code)
{
	FT_Error error;

	error = FT_Set_Pixel_Sizes(face_, w, h);

	FT_Vector	pen;
	pen.x = pen.y = 0;
	FT_Set_Transform(face_, &matrix_, &pen);

	if(antialias) {
		error = FT_Load_Char(face_, code, FT_LOAD_RENDER);
	} else {
		error = FT_Load_Char(face_, code, FT_LOAD_MONOCHROME);
	}

	FT_GlyphSlot slot = face_->glyph;
	FT_Bitmap* bitmap = &slot->bitmap;

	metrics_.bitmap_w = (float)bitmap->width;
	metrics_.bitmap_h = (float)bitmap->rows;
	metrics_.width    = (float)slot->metrics.width  / 64.0f;
	metrics_.height   = (float)slot->metrics.height / 64.0f;
	metrics_.hori_x   = (float)slot->metrics.horiBearingX / 64.0f;
	metrics_.hori_y   = (float)slot->metrics.horiBearingY / 64.0f;
	metrics_.vert_x   = (float)slot->metrics.vertBearingX / 64.0f;
	metrics_.vert_y   = (float)slot->metrics.vertBearingY / 64.0f;

}
#endif

		//-----------------------------------------------------------------//
		/*!
			@brief	コンストラクター
		 */
		//-----------------------------------------------------------------//
		ftimg() : library_(),
				  face_map_(), current_face_(face_map_.end()),
				  matrix_(), metrics_(), gray_(), antialias_(false) { }

		ftimg(const ftimg& fti);
		ftimg& operator = (const ftimg& fti);

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief	デストラクター
		 */
		//-----------------------------------------------------------------//
		~ftimg() {
			for(face_map_it it = face_map_.begin(); it != face_map_.end(); ++it) {
				FT_Done_Face(it->second.face_);
			}
			FT_Done_FreeType(library_);
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	初期化
			@param[in]	root	ルートパスの設定
			@return エラーなら「false」
		 */
		//-----------------------------------------------------------------//
		bool initialize(const std::string& root) {
			root_path_ = root;
			if(!root_path_.empty()) {
				if(root_path_.back() != '/') root_path_ += '/';
				if(!utils::is_directory(root)) {
					std::cerr << "Warrning: fontfile path is not directory (" <<
						root << ")" << std::endl;
					return false;
				}
			} else {
				std::cerr << "Warrning: fontfile root path to empty." << std::endl;
				return false;
			}

			// make home path
			auto p = getenv("HOME");
			if(p != nullptr) {
				home_path_ = p;
				home_path_ += '/';
			}

			FT_Error error = FT_Init_FreeType(&library_);
			if(error) {
				std::cerr << "FT Library init error: " << static_cast<int>(error) << std::endl;
				return false;
			}

			double	angle = (0.0 / 360) * 2.0 * 3.14159265398979;
			matrix_.xx = static_cast<FT_Fixed>( cos( angle ) * 0x10000L );
			matrix_.xy = static_cast<FT_Fixed>(-sin( angle ) * 0x10000L );
			matrix_.yx = static_cast<FT_Fixed>( sin( angle ) * 0x10000L );
			matrix_.yy = static_cast<FT_Fixed>( cos( angle ) * 0x10000L );
			return true;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	フォントをインストール
			@param[in]	fontfile	フォント・ファイル名
			@param[in]	alias		フォントの別名@n
						省略した場合、フォント・ファイル名と同一となる。
			@return 成功した場合は「true」
		 */
		//-----------------------------------------------------------------//
		bool install_font_type(const std::string& fontfile, const std::string& alias = "")
		{
			if(fontfile.empty()) return false;

			std::string name;
			if(alias.empty()) {
				name = utils::get_file_base(utils::get_file_name(fontfile));
			} else {
				name = alias;
			}

			face_map_it it = find_face_(name);
			if(it != face_map_.end()) {
				current_face_ = it;
				return true;
			}

			std::string path;
			if(utils::probe_full_path(fontfile)) {
				path = fontfile;
			} else if(!fontfile.empty() && fontfile[0] == '~') {
				auto tmp = fontfile;
				tmp[0] = '/';
				path = home_path_ + tmp;
			} else {
				path = root_path_ + fontfile;
			}

#ifdef WIN32
			if(!utils::probe_file(path)) {
				// インストールしたフォントファイルが、ユーザーディレクトリにある場合に対応
				std::string userpath = "c:/Users/";
				userpath += std::getenv("USERNAME");
				userpath += "/AppData/Local/Microsoft/Windows/Fonts/";
				userpath += fontfile;
				path = userpath;
			}
#endif

			if(!utils::probe_file(path)) {
				std::cerr << "Can't find: '" << path << "'" << std::endl;
				return false;
			}

			FT_Face face;
			FT_Error error = FT_New_Face(library_, utils::system_path(path).c_str(), 0, &face);
			if(error) {
				std::cerr << "ftimg error(FT_New_Face): '" << path << "'" << std::endl;
				current_face_ = face_map_.end();
				return false;
			}
///			cout << "ftimg install: " << path << ", " << static_cast<int>(face_) << endl;

			face_t t(face);

			FT_Vector pen;
			pen.x = pen.y = 0;
			FT_Set_Transform(face, &matrix_, &pen);
			current_face_ = install_face_(name, t);

			return current_face_ != face_map_.end();
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	フォントを指定
			@param[in]	alias	フォント名
			@return 正常なら「true」
		 */
		//-----------------------------------------------------------------//
		bool set_font(const std::string& alias) {
			face_map_it it = find_face_(alias);
			if(it != face_map_.end()) {
				current_face_ = it;
				FT_Vector pen;
				pen.x = pen.y = 0;
				FT_Set_Transform(it->second.face_, &matrix_, &pen);
				return true;
			} else {
				return false;
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	フォントを取得
			@return フォントの別名を返す
		 */
		//-----------------------------------------------------------------//
		const std::string& get_font() const {
			static std::string tmp;
			if(current_face_ == face_map_.end()) return tmp;
			return current_face_->first;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	フォントの有無を検査
			@param[in]	alias	フォント名
			@return フォントがインストール済みの場合は「true」
		 */
		//-----------------------------------------------------------------//
		bool find_font(const std::string& alias) const {
			face_map_cit cit = face_map_.find(alias);
			if(cit != face_map_.end()) {
				return true;
			} else {
				return false;
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	フォント（face）を破棄する
			@param[in]	alias	フォント名
			@return 正常なら「true」
		 */
		//-----------------------------------------------------------------//
		bool delete_font(const std::string& alias) {
			face_map_it it = find_face_(alias);
			if(it != face_map_.end()) {
				FT_Done_Face(it->second.face_);
				face_map_.erase(it);
				return true;
			} else {
				return false;
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	生成するビットマップのアンチエリアス設定をする
			@param[in]	value	「true」（無し） の場合は、アンチエリアス有効、@n
								「false」 を指定すると、アンチエリアスし無効
		 */
		//-----------------------------------------------------------------//
		void set_antialias(bool value = true) { antialias_ = value; }


		//-----------------------------------------------------------------//
		/*!
			@brief	unicode に対応するビットマップを生成する。
			@param[in]	size	生成するビットマップのサイズ
			@param[in]	unicode	生成するビットマップの UNICODE
		 */
		//-----------------------------------------------------------------//
		void create_bitmap(int size, uint32_t unicode)
		{
			if(current_face_ == face_map_.end()) return;

			// 基準点へのオフセットが無い場合
			face_t& t = current_face_->second;
			if(t.atr_map_.find(size) == t.atr_map_.end()) {
				struct met {
					int	ofs;
					int rows;
				};
				std::vector<met> mets;
				int offset = 0;
				for(int ch = 0x21; ch <= 0x7f; ++ch) {
					if(ch == 0x3f) continue;
					FT_Set_Pixel_Sizes(t.face_, size, size);
					FT_Load_Char(t.face_, ch, FT_LOAD_RENDER);
					FT_GlyphSlot slot = t.face_->glyph;
					met m;
					m.ofs = slot->metrics.horiBearingY / 64;
					FT_Bitmap* bitmap = &slot->bitmap;
					m.rows = bitmap->rows;
					mets.push_back(m);
					if(offset < m.ofs) offset = m.ofs;
				}
				int height = 0;
				BOOST_FOREACH(const met& m, mets) {
					int l = offset - m.ofs + m.rows + 1;
					if(height < l) height = l;
				}
				atr_t at;
				at.offset_ = offset;
				if(height < size) height = size;
				at.height_ = height;
				std::pair<int, atr_t> v(size, at);
/// std::cout << current_face_->first << ", Size: " << size << ", Height: " << at.height_ << std::endl;
				t.atr_map_.insert(v);
			}

			const atr_t& at = t.atr_map_[size];

			vtx::spos fs(size, at.height_);
			gray_.create(fs);
			gray_.fill(gray8(0));

			FT_Error error = FT_Set_Pixel_Sizes(t.face_, size, size);
			if(antialias_) {
				error = FT_Load_Char(t.face_, unicode, FT_LOAD_RENDER);
			} else {
				error = FT_Load_Char(t.face_, unicode, FT_LOAD_MONOCHROME);
			}
			FT_GlyphSlot slot = t.face_->glyph;
#if 0
			if(slot->format == FT_GLYPH_FORMAT_OUTLINE) {
				int strength = 2 << 6;
				FT_Outline_Embolden(&slot->outline, strength);
				FT_Render_Glyph(slot, FT_RENDER_MODE_NORMAL);
			}
#endif
			FT_Bitmap* bitmap = &slot->bitmap;
			metrics_.bitmap_w = static_cast<float>(bitmap->width);
			metrics_.bitmap_h = static_cast<float>(bitmap->rows);
			metrics_.width    = static_cast<float>(slot->metrics.width)  / 64.0f;
			metrics_.height   = static_cast<float>(slot->metrics.height) / 64.0f;
			metrics_.hori_x   = static_cast<float>(slot->metrics.horiBearingX) / 64.0f;
			metrics_.hori_y   = static_cast<float>(slot->metrics.horiBearingY) / 64.0f;
			metrics_.vert_x   = static_cast<float>(slot->metrics.vertBearingX) / 64.0f;
			metrics_.vert_y   = static_cast<float>(slot->metrics.vertBearingY) / 64.0f;

			vtx::spos ofs(static_cast<short>(metrics_.hori_x), at.offset_ - static_cast<short>(metrics_.hori_y) + 1);

		// グレイスケールでレンダリング出来なかった場合は、モノカラーとなる。
			if(bitmap->pixel_mode != FT_PIXEL_MODE_MONO) {		// gray-scale 0 to 255
				vtx::spos p;
				for(p.y = 0; p.y < bitmap->rows; p.y++) {
					for(p.x = 0; p.x < bitmap->width; p.x++) {
						img::gray8 c;
						c.g = bitmap->buffer[p.y * bitmap->width + p.x];
						gray_.put_pixel(p + ofs, c);
					}
				}
			} else {	// monochrome
				int bitpos = 0;
				vtx::spos p;
				for(p.y = 0; p.y < bitmap->rows; p.y++) {
					for(p.x = 0; p.x < bitmap->width; p.x++) {
						img::gray8 c;
						if(bitmap->buffer[bitpos >> 3] & (1 << (~bitpos & 7))) c.g = 255; else c.g = 0;
						bitpos++;
						gray_.put_pixel(p + ofs, c);
					}
					if(bitpos & 7) {
						bitpos |= 7;
						bitpos++;
					}
				}
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	フォントのビットマップイメージを得る。
			@return	ビットマップイメージの参照
		 */
		//-----------------------------------------------------------------//
		const img_gray8& get_img() const { return gray_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	フォントの測定基準（メトリックス）を得る。
			@return	metrics 構造体
		 */
		//-----------------------------------------------------------------//
		const metrics& get_metrics() const { return metrics_; }

	};

}	// namespace img
