//=====================================================================//
/*!	@file
	@brief	デバイスレコーダー
    @author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2017 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/glfw3_app/blob/master/LICENSE
*/
//=====================================================================//
#include "gl_fw/devrec.hpp"

namespace gl {

	//-----------------------------------------------------------------//
	/*!
		@brief	初期化
	*/
	//-----------------------------------------------------------------//
	void devrec::initialize()
	{
		m_get_pos = 0;
		m_record.clear();
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	レコードのロード
		@param[in]	fin コンテキスト
		@return 成功なら「true」
	*/
	//-----------------------------------------------------------------//
	bool devrec::load(utils::file_io& fin)
	{
		long fs = fin.get_file_size();
		m_record.resize(fs / rec_pad_size);
		for(unsigned int i = 0; i < m_record.size(); ++i) {
			m_record[i].read(fin);
		}
#ifdef __PPU__
		utils::file_io::reorder_memory(&m_record[0], fs, "ssbb");
#endif
		m_get_pos = 0;
		return true;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	レコードのセーブ
		@param[in]	fout コンテキスト
		@return 成功なら「true」
	*/
	//-----------------------------------------------------------------//
	bool devrec::save(utils::file_io& fout)
	{
#ifdef __PPU__
		utils::file_io::reorder_memory(&m_record[0], m_record.size() * sizeof(rec_pad), "ssbb");
#endif
		for(unsigned int i = 0; i < m_record.size(); ++i) {
			m_record[i].write(fout);
		}
		m_record.clear();
		return true;
	}


}
