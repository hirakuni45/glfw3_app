//=====================================================================//
/*!	@file
	@brief	PMX ファイルを扱うクラス
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include "pmx_io.hpp"

namespace mdf {

	static bool probe_(utils::file_io& fio)
	{
		std::string s;
		if(fio.get(s, 4) != 4) {
			return false;
		}
		if(s != "PMX ") {
			return false;
		}
		return true;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	ファイルが有効か検査
		@return 有効なら「true」
	*/
	//-----------------------------------------------------------------//
	bool pmx_io::probe(utils::file_io& fio)
	{
		size_t pos = fio.tell();
		bool f = probe_(fio);
		fio.seek(pos, utils::file_io::seek::set);
		return f;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	ロード
		@param[in]	fio	ファイル入出力クラス
		@return 成功なら「true」
	*/
	//-----------------------------------------------------------------//
	bool pmx_io::load(utils::file_io& fio)
	{

		if(!probe_(fio)) {
			return false;
		}

		if(!fio.get(version_)) {
			return false;
		}

		if(!reading_info_.get(fio)) {
			return false;
		}
//		std::cout << "Appendix UV: " << static_cast<int>(reading_info_.appendix_uv) << std::endl;
//		std::cout << "Text Encodng: " << static_cast<int>(reading_info_.text_encode_type) << std::endl;

		bool utf16 = (reading_info_.text_encode_type == 0 ? true : false);
		if(!model_info_.get(fio, utf16)) {
			return false;
		}
//		std::cout << model_info_.name << std::endl;
//		std::cout << model_info_.comment << std::endl;

		// 頂点データの読み込み
		{
			uint32_t num;
			if(!fio.get(num)) return false;
			std::cout << num << std::endl;

			pmx_vertexes_.reserve(num);
			pmx_vertexes_.clear();
			for(uint32_t i = 0; i < num; ++i) {
				pmx_vertex v;
				if(!v.get(fio, reading_info_)) return false;
				pmx_vertexes_.push_back(v);
			}



		}



		return true;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	セーブ
		@param[in]	fio	ファイル入出力クラス
		@return 成功なら「true」
	*/
	//-----------------------------------------------------------------//
	bool pmx_io::save(utils::file_io fio)
	{
		return false;
	}
}

