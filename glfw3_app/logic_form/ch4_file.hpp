#pragma once
//=====================================================================//
/*! @file
	@brief  Checker4 File Class
	@author 平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include "logic.hpp"

namespace tools {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  チェッカー４・ファイル・クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	class ch4_file {

		logic&	logic_;

	public:
		//-------------------------------------------------------------//
		/*!
			@brief  コンストラクター
		*/
		//-------------------------------------------------------------//
		ch4_file(logic& log) : logic_(log) { }


		//-------------------------------------------------------------//
		/*!
			@brief  セーブ
			@param[in]	name	ファイル名
			@return エラー無ければ「true」
		*/
		//-------------------------------------------------------------//
		bool save(const std::string& name)
		{
			if(logic_.size() == 0) return false;

			utils::file_io fio;
			if(!fio.open(name, "wb")) {
				return false;
			}

			for(uint32_t i = 0; i < logic_.size(); ++i) {
				auto bits = logic_.get(i);
				auto s = (boost::format("%06X") % bits).str();
				fio.put("0x" + s + "\n");
			}

			fio.close();

			return true;
		}
	};
}
