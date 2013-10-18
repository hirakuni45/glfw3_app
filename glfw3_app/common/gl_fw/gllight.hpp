#pragma once
//=====================================================================//
/*!	@file
	@brief	OpenGL ライト環境管理（ヘッダー）
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include <vector>
#include <string>
#include "gl_fw/gl_info.hpp"
#include "utils/vtx.hpp"
#include "img_io/img.hpp"

namespace gl {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	ライト・クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	struct light {
		typedef size_t	handle;

		struct material {
			enum type {
				emerald,	///< エメラルド
				jade,		///< 翡翠
				obsidian,	///< 黒曜石
				pearl,		///< 真珠
				ruby,		///< ルビー
				turquoise,	///< トルコ石
				brass,		///< 真鍮
				bronze,		///< 青銅
				chrome,		///< クローム
				copper,		///< 銅
				gold,		///< 金
				silver,		///< 銀
				black_plastic,	///< プラスチック(黒)
				cyan_plastic,	///< プラスチック(シアン)
				green_plastic,	///< プラスチック(緑)
				red_plastic,	///< プラスチック(赤)
				white_plastic,	///< プラスチック(白)
				yellow_plastic,	///< プラスチック(黄)
				black_rubber,	///< ゴム(黒)
				cyan_rubber,	///< ゴム(シアン)
				green_rubber,	///< ゴム(緑)
				red_rubber,		///< ゴム(赤)
				white_rubber,	///< ゴム(白)
				yellow_rubber,	///< ゴム(黄)
				none_,			///< 無効
			};
		};

	private:
		bool	enable_;

		struct light_env {
			bool		enable_;
			vtx::fvtx4	pos_;
			img::rgbaf	ambient_;
			img::rgbaf	diffuse_;
			img::rgbaf	specular_;
			light_env() : enable_(true), pos_(0.0f),
				ambient_(0.0f, 0.0f, 0.0f, 1.0f),
				diffuse_(1.0f, 1.0f, 1.0f, 1.0f),
				specular_(1.0f, 1.0f, 1.0f, 1.0f)
			{ }
		};

		typedef std::vector<light_env>						light_envs;
		typedef std::vector<light_env>::iterator			light_envs_it;
		typedef std::vector<light_env>::const_iterator		light_envs_cit;

		light_envs		light_envs_;

		uint32_t	light_num_;

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief	コンストラクター
		*/
		//-----------------------------------------------------------------//
		light() : enable_(false), light_num_(0) { }


		//-----------------------------------------------------------------//
		/*!
			@brief	デストラクター
		*/
		//-----------------------------------------------------------------//
		~light() { }


		//-----------------------------------------------------------------//
		/*!
			@brief	全体ライトの On/Off
			@param[in]	f	フラグ
		*/
		//-----------------------------------------------------------------//
		void enable(bool f = true) {
			enable_ = f;
			if(light_num_ != 0 && enable_) {
				glEnable(GL_LIGHTING);
			} else {
				glDisable(GL_LIGHTING);
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	ライトを作成する
			@return ライトのハンドル
		*/
		//-----------------------------------------------------------------//
		handle create() {
			handle h = light_envs_.size();
			light_env env;
			light_envs_.push_back(env);
			return h;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	ライトを On/Off する
			@param[in]	h	ハンドル
			@param[in]	f	フラグ
		*/
		//-----------------------------------------------------------------//
		void enable(handle h, bool f = true) { light_envs_[h].enable_ = f; }


		//-----------------------------------------------------------------//
		/*!
			@brief	ライト位置を設定する
			@param[in]	h	ハンドル
			@param[in]	pos	位置
		*/
		//-----------------------------------------------------------------//
		void set_position(handle h, const vtx::fvtx& pos) {
			light_envs_[h].pos_.set(pos.x, pos.y, pos.z);
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	アンビエント・カラーを設定する
			@param[in]	h	ハンドル
			@param[in]	col	ライト・カラー
		*/
		//-----------------------------------------------------------------//
		void set_ambient_color(handle h, const img::rgbaf& col) {
			light_envs_[h].ambient_ = col;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	デュフューズ・カラーを設定する
			@param[in]	h	ハンドル
			@param[in]	col	ライト・カラー
		*/
		//-----------------------------------------------------------------//
		void set_diffuse_color(handle h, const img::rgbaf& col) {
			light_envs_[h].diffuse_ = col;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	スペキュラー・カラーを設定する
			@param[in]	h	ハンドル
			@param[in]	col	ライト・カラー
		*/
		//-----------------------------------------------------------------//
		void set_specular_color(handle h, const img::rgbaf& col) {
			light_envs_[h].specular_ = col;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	マテリアルの設定
			@param[in]	id	マテルアル ID
		*/
		//-----------------------------------------------------------------//
		void set_material(material::type id) const;


		//-----------------------------------------------------------------//
		/*!
			@brief	マテリアルのスキャン
			@param[in]	name	マテルアル名
			@return	マテリアル ID
		*/
		//-----------------------------------------------------------------//
		material::type lookup_material(const std::string& name) const;


		//-----------------------------------------------------------------//
		/*!
			@brief	マテリアルの設定
			@param[in]	name	マテルアル名
		*/
		//-----------------------------------------------------------------//
		bool set_material(const std::string& name) const {
			material::type id = lookup_material(name);
			if(id != material::type::none_) {
				set_material(id);
				return true;
			} else {
				return false;
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	サービス
		*/
		//-----------------------------------------------------------------//
		void service();


		//-----------------------------------------------------------------//
		/*!
			@brief	マテリアルの全リストを表示
		*/
		//-----------------------------------------------------------------//
		void list_material() const;

	};

}	// namespace gl

