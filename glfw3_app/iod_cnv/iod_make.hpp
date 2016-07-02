#pragma once
//=====================================================================//
/*!	@file
	@brief	I/O Device Make @n
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include <boost/format.hpp>
#include "utils/string_utils.hpp"
#include "utils/text_edit.hpp"

namespace utils {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	I/O Device Make クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	struct iod_make {

		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		/*!
			@brief	レジスター定義
		*/
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		struct reg_t { 
	  		std::string	title;		///< レジスタータイトル
			std::string	base;		///< レジスターベースクラス
			std::string	address;	///< レジスターアドレス
			std::string	local;		///< レジスターローカル名
			std::string	name;		///< レジスター名
		};

	private:

		utils::text_edit	edit_;

		bool	start_ = false;

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief	コンストラクター
		*/
		//-----------------------------------------------------------------//
		iod_make() { }


		//-----------------------------------------------------------------//
		/*!
			@brief	タイトル構築
			@param[in]	タイトル
		*/
		//-----------------------------------------------------------------//
		void start(const std::string& title)
		{
			if(start_) return;
			start_ = true;

			edit_.insert("#pragma once");

			edit_ += "//=====================================================================//";
			edit_ += "/*!	@file";
			edit_ += (boost::format("	@brief	%1%@n") % title).str();
			edit_ += "	Copyright 2016 Kunihito Hiramatsu";
			edit_ += "	@author	平松邦仁 (hira@rvf-rc45.net)";
			edit_ += "*/";
			edit_ += "//=====================================================================//";

			edit_ += "#include \"common/io_utils.hpp\"";
			edit_ += "namespace device {";
			edit_ += "\n";
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	構築
			@param[in]	def	定義
		*/
		//-----------------------------------------------------------------//
		void add(const reg_t& reg)
		{
			edit_ += "/// @brief レジスタ定義";
			edit_ += (boost::format("	typedef %1%<%2%> %3%_io;") % reg.base % reg.address % reg.local).str();

			edit_ += "	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//";
			edit_ += "	/*!";
			edit_ += (boost::format("		@brief	%1% %2%") % reg.title % reg.name).str();
			edit_ += "	*/";
			edit_ += "	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//";
			edit_ += (boost::format("	struct %1%_t : public %2%_io {")
						% reg.local % reg.local).str();
			edit_ += "		using pinsr_io::operator =;";
			edit_ += "		using pinsr_io::operator ();";
			edit_ += "		using pinsr_io::operator |=;";
			edit_ += "		using pinsr_io::operator &=;";
			edit_ += "\n";


			edit_ += "	};";
			edit_ += (boost::format("	static %1%_t %2%;") % reg.local % reg.name).str();
#if 0
		bit_t<pinsr_io, 6> TRJIOSEL;  /// TRJIO 入力信号選択（0: 外部 TRJIO端子から、1: VCOUT1 から内部入力） 
		bit_t<pinsr_io, 7> IOINSEL;   /// 端子レベル強制読み出し（0: 禁止 PDi レジスタ制御、1: 許可）
#endif
			edit_ += "}";
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	全体をセーブ
			@param[in]	filename	ファイル名
			@return 成功なら「true」
		*/
		//-----------------------------------------------------------------//
		bool save(const std::string& filename)
		{
			return edit_.save(filename);
		}


	};
}

