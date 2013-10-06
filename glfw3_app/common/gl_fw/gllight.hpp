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
	class light {

	public:
		typedef size_t	handle;

	private:
		bool	enable_;

		struct light_env {
			bool		enable_;
			vtx::fvtx4	light_pos_;
			img::rgbaf	light_ambient_;
			img::rgbaf	light_diffuse_;
			img::rgbaf	light_specular_;
			light_env() : enable_(true) { }
		};

		typedef std::vector<light_env>						light_envs;
		typedef std::vector<light_env>::iterator			light_envs_it;
		typedef std::vector<light_env>::const_iterator		light_envs_cit;

		light_envs		light_envs_;

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief	コンストラクター
		*/
		//-----------------------------------------------------------------//
		light() : enable_(false) { }


		//-----------------------------------------------------------------//
		/*!
			@brief	デストラクター
		*/
		//-----------------------------------------------------------------//
		~light() { destroy(); }


		//-----------------------------------------------------------------//
		/*!
			@brief	初期化
		*/
		//-----------------------------------------------------------------//
		void initialize();


		//-----------------------------------------------------------------//
		/*!
			@brief	全体ライトの On/Off
			@param[in]	f	フラグ
		*/
		//-----------------------------------------------------------------//
		void enable(bool f = true) { enable_ = f; }


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
			light_envs_[h].light_pos_.set(pos.x, pos.y, pos.z);
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	アンビエント・カラーを設定する
			@param[in]	h	ハンドル
			@param[in]	col	ライト・カラー
		*/
		//-----------------------------------------------------------------//
		void set_ambient_color(handle h, const img::rgbaf& col) {
			light_envs_[h].light_ambient_ = col;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	デュフューズ・カラーを設定する
			@param[in]	h	ハンドル
			@param[in]	col	ライト・カラー
		*/
		//-----------------------------------------------------------------//
		void set_diffuse_color(handle h, const img::rgbaf& col) {
			light_envs_[h].light_diffuse_ = col;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	スペキュラー・カラーを設定する
			@param[in]	h	ハンドル
			@param[in]	col	ライト・カラー
		*/
		//-----------------------------------------------------------------//
		void set_specular_color(handle h, const img::rgbaf& col) {
			light_envs_[h].light_specular_ = col;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	マテリアルの設定
			@param[in]	id	マテルアル ID
		*/
		//-----------------------------------------------------------------//
		void set_material(int id) const;


		//-----------------------------------------------------------------//
		/*!
			@brief	マテリアルのスキャン
			@param[in]	name	マテルアル名
			@return	マテリアル ID
		*/
		//-----------------------------------------------------------------//
		int lookup_material(const std::string& name) const;


		//-----------------------------------------------------------------//
		/*!
			@brief	マテリアルの設定
			@param[in]	name	マテルアル名
		*/
		//-----------------------------------------------------------------//
		bool set_material(const std::string& name) const {
			int id = lookup_material(name);
			if(id >= 0) {
				set_material(id);
				return true;
			} else {
				return false;
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	サービス
			@return 有効なライトがあれば「true」
		*/
		//-----------------------------------------------------------------//
		bool service() const;


		//-----------------------------------------------------------------//
		/*!
			@brief	廃棄
		*/
		//-----------------------------------------------------------------//
		void destroy();


		//-----------------------------------------------------------------//
		/*!
			@brief	マテリアルの全リストを表示
		*/
		//-----------------------------------------------------------------//
		void list_material() const;

	};

}	// namespace gl

