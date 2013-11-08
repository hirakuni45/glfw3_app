#pragma once
//=====================================================================//
/*! @file
	@brief  BMC コア関係
	@author 平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include <bitset>
#include "utils/string_utils.hpp"

namespace app {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  BMC コア・クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	class bmc_core {
	public:

		struct option {
			enum type {
				preview,
				verbose,
				true_color,
				inverse,
				bdf_type,
				dither,
				header,
				clipx,
				clipy,
				append,
				opterr,

				limit_
			};
		};

	private:
		std::bitset<option::limit_>	option_;
		std::string	inp_fname_;
		std::string out_fname_;

		int		argc_;
		char**	argv_;

		float	version_;

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief  コンストラクター
		*/
		//-----------------------------------------------------------------//
		bmc_core(int argc, char** argv) : argc_(argc), argv_(argv), version_(0.1f) { }


		//-----------------------------------------------------------------//
		/*!
			@brief  help 表示
		*/
		//-----------------------------------------------------------------//
		void help() const;


		//-----------------------------------------------------------------//
		/*!
			@brief  コマンドライン解析
			@param[in]	ss	コマンド郡
		*/
		//-----------------------------------------------------------------//
		bool analize(const utils::strings& ss);

	};
}
