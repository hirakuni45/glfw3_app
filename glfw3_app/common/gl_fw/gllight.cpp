//=====================================================================//
/*!	@file
	@brief	OpenGL ライト環境管理
    @author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2017 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/glfw3_app/blob/master/LICENSE
*/
//=====================================================================//
#include <iostream>
#include <boost/format.hpp>
#include "gl_fw/gllight.hpp"

namespace gl {

	struct light_material_ {
		const char*		name_;
		GLfloat			ambient_[4];
		GLfloat			diffuse_[4];
		GLfloat			specular_[4];
		GLfloat			shininess_;
	};

	static const light_material_ light_material_tables_[] = {
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


	//-----------------------------------------------------------------//
	/*!
		@brief	マテリアルの設定
		@param[in]	id	マテルアル ID
	*/
	//-----------------------------------------------------------------//
	void light::set_material(material::type id) const
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
	light::material::type light::lookup_material(const std::string& name) const
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
		@brief	サービス
	*/
	//-----------------------------------------------------------------//
	void light::service()
	{
		static const GLenum light_idx[] = {
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
	void light::list_material() const
	{
		for(size_t i = 0; i < (sizeof(light_material_tables_) / sizeof(light_material_)); ++i) {
			const light_material_& mte = light_material_tables_[i];
			std::cout << boost::format("(%d) '%s'") % i % mte.name_ << std::endl;
		}
	}

}
