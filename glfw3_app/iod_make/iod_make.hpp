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
#include "def_st.hpp"

namespace utils {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	I/O Device Make クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	struct iod_make {

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
			@brief	ベース構築
			@param[in]	base	ベース設定
		*/
		//-----------------------------------------------------------------//
		void start(const base_t& base)
		{
			if(start_) return;
			start_ = true;

			edit_.insert("#pragma once");

			edit_ += "//=====================================================================//";
			edit_ += "/*!	@file";
			{
				auto ss = base.get(base_t::type::title);
				if(!ss.empty()) {
					edit_ += (boost::format("	@brief	%1% @n") % ss[0]).str();
					for(uint32_t i = 1; i < ss.size(); ++i) {
						edit_ += (boost::format("			%1%") % ss[i]).str();
						if(i != (ss.size() - 1)) edit_ += " @";
					}
				}
			}
			{
				auto ss = base.get(base_t::type::author);
				if(!ss.empty()) {
					edit_ += (boost::format("	@author	%1%") % ss[0]).str();
				}
			}		
			edit_ += "*/";
			edit_ += "//=====================================================================//";

			{
				auto ss = base.get(base_t::type::file);
				if(!ss.empty()) {
					std::string path = ss[0];
					edit_ += "#include \"" + path + '"';
				}
			}

			edit_ += "";
			{
				auto ss = base.get(base_t::type::space);
				std::string dev;
				if(ss.empty()) dev = "device";
				else dev = ss[0];
				edit_ += "namespace " + dev + " {";
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	レジスター定義追加
			@param[in]	t	クラス定義
			@return エラーなら「false」
		*/
		//-----------------------------------------------------------------//
		bool add(const class_t& t)
		{
	  		const auto& title = t.reg_.get(reg_t::type::title);
			if(title.empty()) return false;
			const auto& base = t.reg_.get(reg_t::type::base);
			if(base.empty()) return false;
			const auto& address = t.reg_.get(reg_t::type::address);
			if(address.empty()) return false;
			const auto& name = t.reg_.get(reg_t::type::name);
			if(name.empty()) return false;

			std::string	local = to_lower_text(name[0]);
			std::string bit_base;
			bool read = false;
			bool write = false;
			if(base[0].find("rw") != std::string::npos) {
				read = true; write = true;
				local += "_rw";
				bit_base = "bit_rw_t";
			}
			else if(base[0].find("ro") != std::string::npos) {
				read = true;
				local += "_ro";
				bit_base = "bit_ro_t";
			}
			else if(base[0].find("wo") != std::string::npos) {
				write = true;
				local += "_wo";
				bit_base = "bit_wo_t";
			}

			edit_ += "";
			edit_ += (boost::format("	/// @brief %1% レジスタ定義") % name[0]).str();
			edit_ += (boost::format("	typedef %1%<%2%> %3%;") % base[0] % address[0] % local).str();

			edit_ += "	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//";
			edit_ += "	/*!";
			{
				edit_ += (boost::format("		@brief	%1% @n") % title[0]).str();
				for(uint32_t i = 1; i < title.size(); ++i) {
					edit_ += (boost::format("				%1%") % title[i]).str();
					if(i != (title.size() - 1)) edit_ += " @";
				}
			}
			edit_ += "	*/";
			edit_ += "	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//";
			edit_ += (boost::format("	struct %1%_t : public %2% {") % local % local).str();

			if(write) {
				edit_ += (boost::format("		using %1%::operator =;") % local).str();
			}
			if(read) {
				edit_ += (boost::format("		using %1%::operator ();") % local).str();
			}
			if(read && write) {
				edit_ += (boost::format("		using %1%::operator |=;") % local).str();
				edit_ += (boost::format("		using %1%::operator &=;") % local).str();
			}

			if(!t.bits_.empty()) {
				edit_ += "";
			}
			// ビット位置定義
			for(const auto& b : t.bits_) {

				const auto& title = b.get(bit_t::type::title);
				const auto& def = b.get(bit_t::type::def);
				if(def.empty()) return false;
				const auto& name = b.get(bit_t::type::name);
				if(name.empty()) return false;

				std::string pos = def[0];
				if(def.size() > 1 && def[1] != "1") {
					pos += ", ";
					pos += def[1];
				}

				auto s = (boost::format("		%1%<%2%, %3%> %4%;")
					% bit_base % local % pos % name[0]).str();
				if(!title.empty()) {
					s += "	///< " + title[0];
				}
				edit_ += s;
			}

			edit_ += "	};";
			edit_ += (boost::format("	static %1%_t %2%;") % local % name[0]).str();
			edit_ += "";

			return true;
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

