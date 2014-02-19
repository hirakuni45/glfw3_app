//=====================================================================//
/*!	@file
	@brief	Arithmetic クラス@n
			※数式処理をサポートする簡易演算解析クラス
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include "utils/arith.hpp"
#include <boost/foreach.hpp>

namespace utils {

	void arith::value::sum(char c)
	{
		if(c == '_') ;	// 数字列にある '_'（アンダースコアー）は無視
		else if(hex_value_ == false && integer_ == 0 && point_ == false && (c =='x' || c == 'X' || c == '$')) {
			hex_value_ = true;
		} else if(c == '.') {
			if(point_ == false) {
				point_ = true;
			} else {	// 少数点が２つ以上ある場合
				error_.set(error::fatal);
			}
		} else if(c >= '0' && c <= '9') {
			c -= '0';
			if(point_ == false) {
				if(hex_value_) {
					integer_ <<= 4;
					integer_ |= c;
				} else {
					integer_ *= 10;
					integer_ += c;
					float_ *= 10.0f;
					float_ += static_cast<float>(c);
					double_ *= 10.0;
					double_ += static_cast<double>(c);
				}
			} else {
				f_scale_ *= 0.1f;
				float_ += static_cast<float>(c) * f_scale_;
				d_scale_ *= 0.1;
				double_ += static_cast<double>(c) * d_scale_;
			}
		} else if(hex_value_ == true && (c >= 'a' && c <= 'f')) {
			c -= 'a';
			c += 10;
			integer_ <<= 4;
			integer_ |= c;
		} else if(hex_value_ == true && (c >= 'A' && c <= 'F')) {
			c -= 'A';
			c += 10;
			integer_ <<= 4;
			integer_ |= c;
		} else {
			error_.set(error::number_fatal);
		}
	}


	void arith::value::neg()
	{
		integer_ = -integer_;
		float_   = -float_;
		double_  = -double_;
	}


	void arith::value::inv()
	{
		integer_ = integer_ ^ -1;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	シンボルに値を登録
		@param[in]	name	シンボル名
		@param[in]	val		整数値
	*/
	//-----------------------------------------------------------------//
	void arith::set_symbol(const std::string& name, long val)
	{
		symbol_map_it it = symbol_.find(name);
		if(it == symbol_.end()) {
			value v;
			symbol_[name] = v;
			symbol_map_it it = symbol_.find(name);
		}
		value& vp = (*it).second;
		vp.integer_ = val;
		vp.float_ = static_cast<float>(val);
		vp.double_ = static_cast<double>(val);
		vp.point_ = false;
		vp.dbe_ = false;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	シンボルに値を登録
		@param[in]	name	シンボル名
		@param[in]	val		浮動小数点
	*/
	//-----------------------------------------------------------------//
	void arith::set_symbol(const std::string& name, float val)
	{
		symbol_map_it it = symbol_.find(name);
		if(it == symbol_.end()) {
			value v;
			symbol_[name] = v;
			symbol_map_it it = symbol_.find(name);
		}
		value& vp = (*it).second;
		vp.integer_ = static_cast<long>(val);
		vp.float_ = val;
		vp.double_ = static_cast<double>(val);
		vp.point_ = true;
		vp.dbe_ = false;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	シンボルに値を登録
		@param[in]	name	シンボル名
		@param[in]	val		倍精度浮動小数点
	*/
	//-----------------------------------------------------------------//
	void arith::set_symbol(const std::string& name, double val)
	{
		symbol_map_it it = symbol_.find(name);
		if(it == symbol_.end()) {
			value v;
			symbol_[name] = v;
			symbol_map_it it = symbol_.find(name);
		}
		value& vp = (*it).second;
		vp.integer_ = static_cast<long>(val);
		vp.float_ = static_cast<float>(val);
		vp.double_ = val;
		vp.point_ = true;
		vp.dbe_ = true;
	}


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	数値入力
		@return 数値を返す
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	arith::value arith::number_()
	{
		bool inv = false;
		bool neg = false;

		// 符号、反転の判定
		if(ch_ == '-') {
			inv = true;
			ch_ = *tx_++;
		} else if(ch_ == '+') {
			ch_ = *tx_++;
		} else if(ch_ == '!' || ch_ == '~') {
			neg = true;
			ch_ = *tx_++;
		}

		value v;
		if(ch_ == '(') {
			v = factor_();
		} else {
//			long val_bin = 0;
//			long val_oct = 0;
//			long val_hex = 0;

//			bool bin_ok = false;
//			bool oct_ok = false;
//			bool hex_ok = false;

			bool symbol = false;
			std::string sym;

			if(ch_ >= 'A' && ch_ <= 'Z') symbol = true;
			else if(ch_ >= 'a' && ch_ <= 'z') symbol = true;
			else if(ch_ == '_' || ch_ == '?') symbol = true;

			while(ch_ != 0) {
				if(ch_ == '+') break;
				else if(ch_ == '-') break;
				else if(ch_ == '*') break;
				else if(ch_ == '/') break;
				else if(ch_ == '&') break;
				else if(ch_ == '^') break;
				else if(ch_ == '|') break;
				else if(ch_ == '%') break;
				else if(ch_ == ')') break;
				else if(ch_ == '<') break;
				else if(ch_ == '>') break;
				else if(ch_ == '!') break;
				else if(ch_ == '~') break;

				if(symbol) {
					sym += ch_;
				} else {
#if 0
					if(m_ch == 'b' || m_ch == 'B') {
						if(v.m_error[binary_fatal] == false) {
							bin_ok = true;
						}
					}
					if(m_ch == 'o' || m_ch == 'O') {
						if(v.m_error[octal_fatal] == false) {
							oct_ok = true;
						}
					}
					if(m_ch == 'h' || m_ch == 'H') {
						if(v.m_error[hexdecimal_fatal] == false) {
							hex_ok = true;
						}
					}
#endif
					v.sum(ch_);

					if(v.error_[error::binary_fatal] == true || v.error_[error::octal_fatal] == true) ;
					else if(v.error_.any()) break;
				}
				ch_ = *tx_++;
			}

			if(symbol) {
				symbol_map_cit cit = symbol_.find(sym);
				if(cit != symbol_.end()) {
					v = (*cit).second;
				} else {
					v.error_.set(error::symbol_fatal);
				}
			}
		}
//						v.m_error.reset(integer_fatal);
//						v.m_error.reset(float_fatal);
//						v.m_error.reset(double_fatal);
		if(inv) v.inv();
		if(neg) v.neg();
		v.error_.reset(error::binary_fatal);
		v.error_.reset(error::octal_fatal);

		return v;
	}


	arith::value arith::factor_()
	{
		value v;
		if(ch_ != '(') {
			v = number_();
		} else {
			ch_ = *tx_++;
			v = expression_();
			if(ch_ != ')') {
				v.error_.set(error::fatal);
			} else {
				ch_ = *tx_++;
			}
		}
		return v;
	}


	arith::value arith::term_()
	{
		value v = factor_();
		while(v.error_.any() != true) {
			bool fin = false;
			switch(ch_) {
			case '*':
				ch_ = *tx_++;
				v *= factor_();
				break;
			case '/':
				ch_ = *tx_++;
				if(ch_ == '/') {
					ch_ = *tx_++;
					v %= factor_();
				} else {
					v /= factor_();
				}
				break;
			case '<':
				ch_ = *tx_++;
				if(ch_ == '<') {
					ch_ = *tx_++;
					v <<= factor_();
				} else {
					v.error_.set(error::fatal);
				}
				break;
			case '>':
				ch_ = *tx_++;
				if(ch_ == '>') {
					ch_ = *tx_++;
					v <<= factor_();
				} else {
					v.error_.set(error::fatal);
				}
				break;
			default:
				fin = true;
				break;
			}
			if(fin) break;
		}
		return v;
	}


	arith::value arith::expression_()
	{
		value v = term_();
		while(v.error_.any() != true) {
			bool fin = false;
			switch(ch_) {
			case '+':
				ch_ = *tx_++;
				v += term_();
				break;
			case '-':
				ch_ = *tx_++;
				v -= term_();
				break;
			case '&':
				ch_ = *tx_++;
				v &= term_();
				break;
			case '^':
				ch_ = *tx_++;
				v ^= term_();
				break;
			case '|':
				ch_ = *tx_++;
				v |= term_();
				break;
			default:
				fin = true;
				break;
			}
			if(fin) break;
		}
		return v;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	解析を開始
		@param[in]	text	解析テキスト
		@return	文法にエラーがあった場合、「false」
	*/
	//-----------------------------------------------------------------//
	bool arith::analize(const std::string& text)
	{
		std::string s;

		// スペースと TAB を除外
		BOOST_FOREACH(char c, text) {
			if(c == ' ' || c == '\t') ;
			else if(c < ' ') {
				value_.error_.set(error::fatal);
				return false;
			} else {
				s += c;
			}
		}

		tx_ = s.c_str();
		ch_ = *tx_++;
		if(ch_ != 0) {
			value_ = expression_();
		} else {
			value_.error_.set(error::fatal);
		}

		if(value_.error_.any() == true) {
			return false;
		} else if(ch_ != 0) {
			value_.error_.set(error::fatal);
			return false;
		}
		return true;
	}

}
