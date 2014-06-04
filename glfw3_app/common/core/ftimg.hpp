#pragma once
//=====================================================================//
/*!	@file
	@brief	フォントイメージを freetype2 で扱うクラス（ヘッダー）
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include <string>
#include <map>
#include <boost/unordered_map.hpp>
#ifndef DEPEND_ESCAPE
#include <ft2build.h>
#include FT_FREETYPE_H
#endif
#include "utils/singleton_policy.hpp"
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
		friend class utils::singleton_policy<ftimg>;

		std::string	root_path_;

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

		//-----------------------------------------------------------------//
		/*!
			@brief	コンストラクター
		 */
		//-----------------------------------------------------------------//
		ftimg() : root_path_(), library_(),
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
		 */
		//-----------------------------------------------------------------//
		void initialize(const std::string& root) {
			root_path_ = root;

			FT_Error error = FT_Init_FreeType(&library_);
			if(error) {
				std::cerr << "FT Library init error: " << static_cast<int>(error) << std::endl;
			}

			double	angle = (0.0 / 360) * 2.0 * 3.14159265398979;
			matrix_.xx = static_cast<FT_Fixed>( cos( angle ) * 0x10000L );
			matrix_.xy = static_cast<FT_Fixed>(-sin( angle ) * 0x10000L );
			matrix_.yx = static_cast<FT_Fixed>( sin( angle ) * 0x10000L );
			matrix_.yy = static_cast<FT_Fixed>( cos( angle ) * 0x10000L );
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
		bool install_font_type(const std::string& fontfile, const std::string& alias = "");


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
		void create_bitmap(int size, uint32_t unicode) ;


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
