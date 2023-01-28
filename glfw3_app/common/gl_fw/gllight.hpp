#pragma once
//=====================================================================//
/*!	@file
	@brief	OpenGL ライト環境管理（ヘッダー）
    @author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2017, 2023 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/glfw3_app/blob/master/LICENSE
*/
//=====================================================================//
#include <vector>
#include <string>
#include <iostream>
#include <boost/format.hpp>
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

		struct light_material_ {
			const char*		name_;
			GLfloat			ambient_[4];
			GLfloat			diffuse_[4];
			GLfloat			specular_[4];
			GLfloat			shininess_;
		};

		static constexpr light_material_ light_material_tables_[] = {
			// (エメラルド)
			{
				"emerald",
				{ 0.0215f,  0.1745f,   0.0215f,  1.0f },
				{ 0.07568f, 0.61424f,  0.07568f, 1.0f },
				{ 0.633f,   0.727811f, 0.633f,   1.0f },
				76.8f
			},

			// (翡翠)
			{
				"jade",
				{ 0.135f,     0.2225f,   0.1575f,   1.0f },
				{ 0.54f,      0.89f,     0.63f,     1.0f },
				{ 0.316228f,  0.316228f, 0.316228f, 1.0f },
				12.8f
			},

			// (黒曜石)
			{
				"obsidian",
				{ 0.05375f,   0.05f,     0.06625f,  1.0f },
				{ 0.18275f,   0.17f,     0.22525f,  1.0f },
				{ 0.332741f,  0.328634f, 0.346435f, 1.0f },
				38.4f
			},

			// (真珠)
			{
				"pearl",
				{ 0.25f,      0.20725f,  0.20725f,  1.0f },
				{ 1.0f,       0.829f,    0.829f,    1.0f },
				{ 0.296648f,  0.296648f, 0.296648f, 1.0f },
				10.24f
			},

			//(ルビー)
			{
				"ruby",
				{ 0.1745f,    0.01175f,  0.01175f,  1.0f },
				{ 0.61424f,   0.04136f,  0.04136f,  1.0f },
				{ 0.727811f,  0.626959f, 0.626959f, 1.0f },
				76.8f
			},

			// (トルコ石)
			{
				"turquoise",
				{ 0.1f,       0.18725f,  0.1745f,   1.0f },
				{ 0.396f,     0.74151f,  0.69102f,  1.0f },
				{ 0.297254f,  0.30829f,  0.306678f, 1.0f },
				12.8f
			},

			// (真鍮)
			{
				"brass",
				{ 0.329412f,  0.223529f, 0.027451f, 1.0f },
				{ 0.780392f,  0.568627f, 0.113725f, 1.0f },
				{ 0.992157f,  0.941176f, 0.807843f, 1.0f },
				27.89743616f
			},

			// (青銅)
			{
				"bronze",
				{ 0.2125f,    0.1275f,   0.054f,    1.0f },
				{ 0.714f,     0.4284f,   0.18144f,  1.0f },
				{ 0.393548f,  0.271906f, 0.166721f, 1.0f },
				25.6f
			},

			// (クローム) 
			{
				"chrome",
				{ 0.25f,      0.25f,     0.25f,     1.0f },
				{ 0.4f,       0.4f,      0.4f,      1.0f },
				{ 0.774597f,  0.774597f, 0.774597f, 1.0f },
				76.8f
			},

			// (銅)
			{
				"copper",
				{ 0.19125f,   0.0735f,   0.0225f,   1.0f },
				{ 0.7038f,    0.27048f,  0.0828f,   1.0f },
				{ 0.256777f,  0.137622f, 0.086014f, 1.0f },
				12.8f
			},

			// (金)
			{
				"gold",
				{ 0.24725f,   0.1995f,   0.0745f,    1.0f },
				{ 0.75164f,   0.60648f,  0.22648f,   1.0f },
				{ 0.628281f,  0.555802f, 0.366065f,  1.0f },
				51.2f
			},

			// (銀)
			{
				"silver",
				{ 0.19225f,   0.19225f,  0.19225f,   1.0f },
				{ 0.50754f,   0.50754f,  0.50754f,   1.0f },
				{ 0.508273f,  0.508273f, 0.508273f,  1.0f },
				51.2f
			},

			// プラスチック(黒)
			{
				"black_plastic",
				{ 0.0f,        0.0f,      0.0f,       1.0f },
				{ 0.01f,       0.01f,     0.01f,      1.0f },
				{ 0.50f,       0.50f,     0.50f,      1.0f },
				32.0f
			},

			// プラスチック(シアン)
			{
				"cyan_plastic",
				{ 0.0f,        0.1f,        0.06f,       1.0f },
				{ 0.0f,        0.50980392f, 0.50980392f, 1.0f },
				{ 0.50196078f, 0.50196078f, 0.50196078f, 1.0f },\
				32.0f
			},

			// プラスチック(緑)
			{
				"green_plastic",
				{ 0.0f,        0.0f,        0.0f,        1.0f },
				{ 0.1f,        0.35f,       0.1f,        1.0f },
				{ 0.45f,       0.55f,       0.45f,       1.0f },
				32.0f
			},

			// プラスチック(赤)
			{
				"red_plastic",
				{ 0.0f,        0.0f,        0.0f,        1.0f },
				{ 0.5f,        0.0f,        0.0f,        1.0f },
				{ 0.7f,        0.6f,        0.6f,        1.0f },
				32.0f
			},

			// プラスチック(白)
			{
				"white_plastic",
				{ 0.0f,        0.0f,         0.0f,       1.0f },
				{ 0.55f,       0.55f,        0.55f,      1.0f },
				{ 0.70f,       0.70f,        0.70f,      1.0f },
				32.0f
			},

			// プラスチック(黄)
			{
				"yellow_plastic",
				{ 0.0f,        0.0f,         0.0f,       1.0f },
				{ 0.5f,        0.5f,         0.0f,       1.0f },
				{ 0.60f,       0.60f,        0.50f,      1.0f },
				32.0f
			},

			// ゴム(黒)
			{
				"black_rubber",
				{ 0.02f,       0.02f,        0.02f,      1.0f },
				{ 0.01f,       0.01f,        0.01f,      1.0f },
				{ 0.4f,        0.4f,         0.4f,       1.0f },
				10.0f
			},

			// ゴム(シアン)
			{
				"cyan_rubber",
				{ 0.0f,        0.05f,        0.05f,      1.0f },
				{ 0.4f,        0.5f,         0.5f,       1.0f },
				{ 0.04f,       0.7f,         0.7f,       1.0f },
				10.0f
			},

			// ゴム(緑)
			{
				"green_rubber",
				{ 0.0f,        0.05f,        0.0f,       1.0f },
				{ 0.4f,        0.5f,         0.4f,       1.0f },
				{ 0.04f,       0.7f,         0.04f,      1.0f },
				10.0f
			},

			// ゴム(赤)
			{
				"red_rubber",
				{ 0.05f,       0.0f,         0.0f,       1.0f },
				{ 0.5f,        0.4f,         0.4f,       1.0f },
				{ 0.7f,        0.04f,        0.04f,      1.0f },
				10.0f
			},

			// ゴム(白)
			{
				"white_rubber",
				{ 0.05f,       0.05f,        0.05f,      1.0f },
				{ 0.5f,        0.5f,         0.5f,       1.0f },
				{ 0.7f,        0.7f,         0.7f,       1.0f },
				10.0f
			},

			// ゴム(黄)
			{
				"yellow_rubber",
				{ 0.05f,       0.05f,        0.0f,       1.0f },
				{ 0.5f,        0.5f,         0.4f,       1.0f },
				{ 0.7f,        0.7f,         0.04f,      1.0f },
				10.0f
			},
		};


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
		void set_material(material::type id) const
		{
			const light_material_& mte = light_material_tables_[id];
			glMaterialfv(GL_FRONT, GL_AMBIENT,   mte.ambient_);
			glMaterialfv(GL_FRONT, GL_DIFFUSE,   mte.diffuse_);
			glMaterialfv(GL_FRONT, GL_SPECULAR,  mte.specular_);
			glMaterialfv(GL_FRONT, GL_SHININESS, &mte.shininess_);

#ifdef __PPU__
			glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT,   mte.ambient_);
			glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE,   mte.diffuse_);
			glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR,  mte.specular_);
			glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, &mte.shininess_);
#endif
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	マテリアルのスキャン
			@param[in]	name	マテルアル名
			@return	マテリアル ID
		*/
		//-----------------------------------------------------------------//
		material::type lookup_material(const std::string& name) const
		{
			for(size_t i = 0; i < (sizeof(light_material_tables_) / sizeof(light_material_)); ++i) {
				const light_material_& mte = light_material_tables_[i];
				if(name == mte.name_) {
					return static_cast<material::type>(i);
				}
			}
			return material::none_;
		}


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
		void service()
		{
			static constexpr GLenum light_idx[] = {
				GL_LIGHT0, GL_LIGHT1, GL_LIGHT2, GL_LIGHT3,
				GL_LIGHT4, GL_LIGHT5, GL_LIGHT6, GL_LIGHT7
			};

			int i = 0;
			for(light_envs_cit cit = light_envs_.begin(); cit != light_envs_.end(); ++cit) {
				const light_env& env = *cit;
				if(env.enable_ == true && i < 8) {
					glLightfv(light_idx[i], GL_POSITION, env.pos_.getXYZW());
					glLightfv(light_idx[i], GL_AMBIENT,  env.ambient_.rgba);
					glLightfv(light_idx[i], GL_DIFFUSE,  env.diffuse_.rgba);
					glLightfv(light_idx[i], GL_SPECULAR, env.specular_.rgba);
					glEnable(light_idx[i]);
					i++;
				}
			}
			light_num_ = i;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	マテリアルの全リストを表示
		*/
		//-----------------------------------------------------------------//
		void list_material() const
		{
			for(size_t i = 0; i < (sizeof(light_material_tables_) / sizeof(light_material_)); ++i) {
				const light_material_& mte = light_material_tables_[i];
				std::cout << boost::format("(%d) '%s'") % i % mte.name_ << std::endl;
			}
		}
	};

}	// namespace gl
