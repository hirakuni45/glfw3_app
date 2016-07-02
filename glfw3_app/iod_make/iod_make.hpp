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


		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		/*!
			@brief	ビット定義
		*/
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		struct bit_t {
			std::string	base;		///< ベースクラス
			std::string	pos;		///< 位置
			std::string	name;		///< ビット名
			std::string	comment;	///< コメント
			bit_t(const std::string& b, const std::string& p, const std::string& n, const std::string& c = "") :
				base(b), pos(p), name(n), comment(c) { }
		};
		typedef std::vector<bit_t>	bits_type;

	private:

		utils::text_edit	edit_;

		bool	start_ = false;
		bool	end_ = false;

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
		void start(const std::string& title, const std::string& title_2nd, const std::string& title_3rd, const std::string& author)
		{
			if(start_) return;
			start_ = true;

			edit_.insert("#pragma once");

			edit_ += "//=====================================================================//";
			edit_ += "/*!	@file";
			edit_ += (boost::format("	@brief	%1%@n") % title).str();
			if(!title_2nd.empty()) {
				edit_ += (boost::format("			%1% @n") % title_2nd).str();
			}
			if(!title_3rd.empty()) {
				edit_ += (boost::format("			%1% @n") % title_3rd).str();
			}
			if(!author.empty()) {
				edit_ += (boost::format("	@author	%1%") % author).str();
			}		
			edit_ += "*/";
			edit_ += "//=====================================================================//";

			edit_ += "#include \"common/io_utils.hpp\"";
			edit_ += "";
			edit_ += "namespace device {";
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	レジスター定義追加
			@param[in]	reg		レジスター定義
			@param[in]	bits	ビット定義郡
		*/
		//-----------------------------------------------------------------//
		void add(const reg_t& reg, const bits_type& bits)
		{
			edit_ += "";
			edit_ += (boost::format("	/// @brief %1% レジスタ定義") % reg.name).str();
			edit_ += (boost::format("	typedef %1%<%2%> %3%_io;") % reg.base % reg.address % reg.local).str();

			edit_ += "	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//";
			edit_ += "	/*!";
			edit_ += (boost::format("		@brief	%1% %2%") % reg.title % reg.name).str();
			edit_ += "	*/";
			edit_ += "	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//";
			edit_ += (boost::format("	struct %1%_t : public %2%_io {")
						% reg.local % reg.local).str();
			edit_ += (boost::format("		using %1%_io::operator =;") % reg.local).str();
			edit_ += (boost::format("		using %1%_io::operator ();") % reg.local).str();
			edit_ += (boost::format("		using %1%_io::operator |=;") % reg.local).str();
			edit_ += (boost::format("		using %1%_io::operator &=;") % reg.local).str();

			for(const auto& t : bits) {
   
				auto s = (boost::format("		%1%<%2%_io, %3%> %4%;")
					% t.base % reg.local % t.pos % t.name).str();
				if(!t.comment.empty()) {
					s += "	///< " + t.comment;
				}
				edit_ += s;
			}

			edit_ += "	};";
			edit_ += (boost::format("	static %1%_t %2%;") % reg.local % reg.name).str();
			edit_ += "";
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	終了
		*/
		//-----------------------------------------------------------------//
		void end()
		{
			if(end_) return;
			end_ = true;
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
			end();
			return edit_.save(filename);
		}
	};
}

