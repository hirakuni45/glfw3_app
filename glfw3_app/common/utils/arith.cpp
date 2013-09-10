//=====================================================================//
/*!	@file
	@brief	Arithmetic クラス@n
			※数式処理をサポートする簡易演算解析クラス
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include "arith.hpp"

namespace fio {

	void arith::value::sum(char c)
	{
		if(c == '_') ;	// 数字列にある '_'（アンダースコアー）は無視
		else if(m_hex_value == false && m_integer == 0 && m_point == false && (c =='x' || c == 'X' || c == '$')) {
			m_hex_value = true;
		} else if(c == '.') {
			if(m_point == false) {
				m_point = true;
			} else {	// 少数点が２つ以上ある場合
				m_error.set(fatal);
			}
		} else if(c >= '0' && c <= '9') {
			c -= '0';
			if(m_point == false) {
				if(m_hex_value) {
					m_integer <<= 4;
					m_integer |= c;
				} else {
					m_integer *= 10;
					m_integer += c;
					m_float *= 10.0f;
					m_float += static_cast<float>(c);
					m_double *= 10.0;
					m_double += static_cast<double>(c);
				}
			} else {
				m_f_scale *= 0.1f;
				m_float += static_cast<float>(c) * m_f_scale;
				m_d_scale *= 0.1;
				m_double += static_cast<double>(c) * m_d_scale;
			}
		} else if(m_hex_value == true && (c >= 'a' && c <= 'f')) {
			c -= 'a';
			c += 10;
			m_integer <<= 4;
			m_integer |= c;
		} else if(m_hex_value == true && (c >= 'A' && c <= 'F')) {
			c -= 'A';
			c += 10;
			m_integer <<= 4;
			m_integer |= c;
		} else {
			m_error.set(number_fatal);
		}
	}


	void arith::value::neg()
	{
		m_integer = -m_integer;
		m_float   = -m_float;
		m_double  = -m_double;
	}


	void arith::value::inv()
	{
		m_integer = m_integer ^ -1;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	シンボルに値を登録
		@param[in]	name	シンボル名
		@param[in]	val		整数値
	*/
	//-----------------------------------------------------------------//
	void arith::set_symbol_i(const char* name, long val)
	{
		symbol_map_it it = m_symbol.find(name);
		if(it == m_symbol.end()) {
			value v;
			m_symbol[name] = v;
			symbol_map_it it = m_symbol.find(name);
		}
		value& vp = (*it).second;
		vp.m_integer = val;
		vp.m_float = static_cast<float>(val);
		vp.m_double = static_cast<double>(val);
		vp.m_point = false;
		vp.m_dbe = false;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	シンボルに値を登録
		@param[in]	name	シンボル名
		@param[in]	val		浮動小数点
	*/
	//-----------------------------------------------------------------//
	void arith::set_symbol_f(const char* name, float val)
	{
		symbol_map_it it = m_symbol.find(name);
		if(it == m_symbol.end()) {
			value v;
			m_symbol[name] = v;
			symbol_map_it it = m_symbol.find(name);
		}
		value& vp = (*it).second;
		vp.m_integer = static_cast<long>(val);
		vp.m_float = val;
		vp.m_double = static_cast<double>(val);
		vp.m_point = true;
		vp.m_dbe = false;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	シンボルに値を登録
		@param[in]	name	シンボル名
		@param[in]	val		倍精度浮動小数点
	*/
	//-----------------------------------------------------------------//
	void arith::set_symbol_d(const char* name, double val)
	{
		symbol_map_it it = m_symbol.find(name);
		if(it == m_symbol.end()) {
			value v;
			m_symbol[name] = v;
			symbol_map_it it = m_symbol.find(name);
		}
		value& vp = (*it).second;
		vp.m_integer = static_cast<long>(val);
		vp.m_float = static_cast<float>(val);
		vp.m_double = val;
		vp.m_point = true;
		vp.m_dbe = true;
	}


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	数値入力
		@return 数値を返す
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	arith::value arith::number()
	{
		bool	inv = false;
		bool	neg = false;

		// 符号、反転の判定
		if(m_ch == '-') {
			inv = true;
			m_ch = *m_tx++;
		} else if(m_ch == '+') {
			m_ch = *m_tx++;
		} else if(m_ch == '!' || m_ch == '~') {
			neg = true;
			m_ch = *m_tx++;
		}

		value v;
		if(m_ch == '(') {
			v = factor();
		} else {
//			long val_bin = 0;
//			long val_oct = 0;
//			long val_hex = 0;

//			bool bin_ok = false;
//			bool oct_ok = false;
//			bool hex_ok = false;

			bool symbol = false;
			std::string sym;

			if(m_ch >= 'A' && m_ch <= 'Z') symbol = true;
			else if(m_ch >= 'a' && m_ch <= 'z') symbol = true;
			else if(m_ch == '_' || m_ch == '?') symbol = true;

			while(m_ch != 0) {
				if(m_ch == '+') break;
				else if(m_ch == '-') break;
				else if(m_ch == '*') break;
				else if(m_ch == '/') break;
				else if(m_ch == '&') break;
				else if(m_ch == '^') break;
				else if(m_ch == '|') break;
				else if(m_ch == '%') break;
				else if(m_ch == ')') break;
				else if(m_ch == '<') break;
				else if(m_ch == '>') break;
				else if(m_ch == '!') break;
				else if(m_ch == '~') break;

				if(symbol) {
					sym += m_ch;
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
					v.sum(m_ch);

					if(v.m_error[binary_fatal] == true || v.m_error[octal_fatal] == true) ;
					else if(v.m_error.any()) break;
				}
				m_ch = *m_tx++;
			}

			if(symbol) {
				symbol_map_cit cit = m_symbol.find(sym);
				if(cit != m_symbol.end()) {
					v = (*cit).second;
				} else {
					v.m_error.set(symbol_fatal);
				}
			}
		}
//						v.m_error.reset(integer_fatal);
//						v.m_error.reset(float_fatal);
//						v.m_error.reset(double_fatal);
		if(inv) v.inv();
		if(neg) v.neg();
		v.m_error.reset(binary_fatal);
		v.m_error.reset(octal_fatal);

		return v;
	}


	arith::value arith::factor()
	{
		value v;
		if(m_ch != '(') {
			v = number();
		} else {
			m_ch = *m_tx++;
			v = expression();
			if(m_ch != ')') {
				v.m_error.set(fatal);
			} else {
				m_ch = *m_tx++;
			}
		}
		return v;
	}


	arith::value arith::term()
	{
		value v = factor();
		while(v.m_error.any() != true) {
			bool fin = false;
			switch(m_ch) {
			case '*':
				m_ch = *m_tx++;
				v *= factor();
				break;
			case '/':
				m_ch = *m_tx++;
				if(m_ch == '/') {
					m_ch = *m_tx++;
					v %= factor();
				} else {
					v /= factor();
				}
				break;
			case '<':
				m_ch = *m_tx++;
				if(m_ch == '<') {
					m_ch = *m_tx++;
					v <<= factor();
				} else {
					v.m_error.set(fatal);
				}
				break;
			case '>':
				m_ch = *m_tx++;
				if(m_ch == '>') {
					m_ch = *m_tx++;
					v <<= factor();
				} else {
					v.m_error.set(fatal);
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


	arith::value arith::expression()
	{
		value v = term();
		while(v.m_error.any() != true) {
			bool fin = false;
			switch(m_ch) {
			case '+':
				m_ch = *m_tx++;
				v += term();
				break;
			case '-':
				m_ch = *m_tx++;
				v -= term();
				break;
			case '&':
				m_ch = *m_tx++;
				v &= term();
				break;
			case '^':
				m_ch = *m_tx++;
				v ^= term();
				break;
			case '|':
				m_ch = *m_tx++;
				v |= term();
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
	bool arith::analize(const char *text)
	{
		std::string s;

		// スペースと TAB を除外
		char c;
		while((c = *text++) != 0) {
			if(c == ' ' || c == '\t') ;
			else if(c < ' ') {
				m_value.m_error.set(fatal);
				return false;
			} else {
				s += c;
			}
		}

		m_tx = s.c_str();
		m_ch = *m_tx++;
		if(m_ch != 0) {
			m_value = expression();
		} else {
			m_value.m_error.set(fatal);
		}

		if(m_value.m_error.any() == true) {
			return false;
		} else if(m_ch != 0) {
			m_value.m_error.set(fatal);
			return false;
		}
		return true;
	}

}
