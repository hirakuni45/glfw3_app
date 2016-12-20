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

			fio.put("##START##\r\n");

			fio.put("#HeaderInformation#\r\n");
			fio.put("1,1\r\n");

			fio.put("#DeviceInformation#\r\n");
			fio.put(utils::utf8_to_sjis("test,0,1,0,1,φ1,test device\r\n"));

			fio.put("#PinInformation#\r\n");
			uint32_t cnt = 0;
			for(uint32_t ch = 0; ch < 24; ++ch) {
				if(logic_.count1(ch)) ++cnt;
			}
			fio.put(std::to_string(cnt) + "\r\n");
			for(uint32_t ch = 0; ch < 24; ++ch) {
				if(logic_.count1(ch)) {
					auto s = (boost::format("DATA%02d,%d,,,,,\r\n") % ch % (ch + 1)).str();
					fio.put(s);
				}
			}

			fio.put("#CommandInformation#\r\n");
			fio.put("1\r\n");
			fio.put("DIRC,Direct data\r\n");

			fio.put("#CommandInstruction#\r\n");
			fio.put(std::to_string(logic_.size()) + "\r\n");

			//HW出力,1,C,0,0,1,DEC,1,1,0,0,1,0xAA55AA,0,
			for(uint32_t i = 0; i < logic_.size(); ++i) {
				auto bits = logic_.get(i);
				auto s = (boost::format("%06X") % bits).str();
				auto sjis = utils::utf8_to_sjis("HW出力,1,C,0,0,1,DEC,1,1,0,0,1,0x" + s + ",0,\r\n");
				fio.put(sjis);
			}

			fio.close();

			return true;
		}
	};
}
