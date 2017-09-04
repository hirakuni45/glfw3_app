//=====================================================================//
/*!	@file
	@brief	MMD ファイルを扱うクラス
    @author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2017 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/glfw3_app/blob/master/LICENSE
*/
//=====================================================================//
#include "mdf/mmd_io.hpp"

namespace mdf {

	//-----------------------------------------------------------------//
	/*!
		@brief	ロード
		@param[in]	fio	ファイル入出力クラス
		@return 成功なら「true」
	*/
	//-----------------------------------------------------------------//
	bool mmd_io::load(utils::file_io& fio)
	{
		if(pmd_.load(fio)) {
			type_ = type::pmd;
			return true;
		} else if(pmx_.load(fio)) {
			type_ = type::pmx;
			return true;
		}
		type_ = type::none;
		return false;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	セーブ
		@param[in]	fio	ファイル入出力クラス
		@return 成功なら「true」
	*/
	//-----------------------------------------------------------------//
	bool mmd_io::save(utils::file_io& fio)
	{


		return false;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	レンダリング・セットアップ
	*/
	//-----------------------------------------------------------------//
	void mmd_io::render_setup()
	{
		if(type_ == type::pmd) pmd_.render_setup();
		else if(type_ == type::pmx) pmx_.render_setup(); 
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	レンダリング
	*/
	//-----------------------------------------------------------------//
	void mmd_io::render_surface()
	{
		if(type_ == type::pmd) pmd_.render_surface();
		else if(type_ == type::pmx) pmx_.render_surface(); 
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	ボーンのフルパスを生成
		@param[in]	index	ボーンのインデックス
		@param[out]	path	フル・パスを受け取る参照	
	*/
	//-----------------------------------------------------------------//
	bool mmd_io::create_bone_path(uint32_t index, std::string& path)
	{
		if(type_ == type::pmd) pmd_.create_bone_path(index, path);
		else if(type_ == type::pmx) pmx_.create_bone_path(index, path);

		return false;
	}


}
