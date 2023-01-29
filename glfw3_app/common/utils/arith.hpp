#pragma once
//=====================================================================//
/*!	@file
	@brief	Arithmetic クラス @n
			※テキストの数式を展開して、実計算結果を得る。 @n
			簡易演算式解析 @n
			Copyright 2017, 2023 Kunihito Hiramatsu
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include <string>
#include <bitset>
#include <boost/unordered_map.hpp>

namespace utils {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	Arithmetic クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	struct arith {

		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		/*!
			@brief	エラー・タイプ
		*/
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		struct error {
			enum type {
				none,				///< エラー無

				fatal,				///< エラー
				number_fatal,		///< 数字の変換に関するエラー
				zero_divide,		///< ０除算エラー
				binary_fatal,		///< ２進データの変換に関するエラー
				octal_fatal,		///< ８進データの変換に関するエラー
				hexdecimal_fatal,	///< １６進データの変換に関するエラー
				integer_fatal,		///< 整数データの変換に関するエラー
				float_fatal,		///< 浮動小数点データの変換に関するエラー
				double_fatal,		///< 倍精度浮動小数点データの変換に関するエラー
				symbol_fatal,		///< シンボルデータの変換に関するエラー

				limit_
			};
		};

	private:
		const char*		tx_;
		char			ch_;

		class value {
		public:
			std::bitset<error::limit_>	error_;

			bool	hex_value_;
			long	integer_;

			bool	point_;

			float	float_;
			float	f_scale_;

			bool	dbe_;
			double	double_;
			double	d_scale_;

			value() : hex_value_(false), integer_(0), point_(false),
					  float_(0.0f), f_scale_(1.0f),
					  dbe_(false), double_(0.0), d_scale_(1.0) { }

			void sum(char c)
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

			void neg()
			{
				integer_ = -integer_;
				float_   = -float_;
				double_  = -double_;
			}

			void inv()
			{
				integer_ = integer_ ^ -1;
			}

			value& operator += (const value& v) {
				integer_ += v.integer_;
				float_ += v.float_;
				if(dbe_) double_ += v.double_;
				if(v.point_) point_ = v.point_;
				error_ |= v.error_;
				return *this;
			}

			value& operator -= (const value& v) {
				integer_ -= v.integer_;
				float_ -= v.float_;
				if(dbe_) double_ -= v.double_;
				if(v.point_) point_ = v.point_;
				error_ |= v.error_;
				return *this;
			}

			value& operator &= (const value& v) {
				integer_ &= v.integer_;
				error_.set(error::float_fatal);
				if(dbe_) error_.set(error::double_fatal);
				if(v.point_) point_ = v.point_;
				error_ |= v.error_;
				return *this;
			}

			value& operator ^= (const value& v) {
				integer_ ^= v.integer_;
				error_.set(error::float_fatal);
				if(dbe_) error_.set(error::double_fatal);
				if(v.point_) point_ = v.point_;
				error_ |= v.error_;
				return *this;
			}

			value& operator |= (const value& v) {
				integer_ |= v.integer_;
				error_.set(error::float_fatal);
				if(dbe_) error_.set(error::double_fatal);
				if(v.point_) point_ = v.point_;
				error_ |= v.error_;
				return *this;
			}

			value& operator *= (const value& v) {
				integer_ *= v.integer_;
				float_ *= v.float_;
				if(dbe_) double_ *= v.double_;
				if(v.point_) point_ = v.point_;
				error_ |= v.error_;
				return *this;
			}

			value& operator /= (const value& v) {
				if(v.integer_ == 0 || v.float_ == 0.0f || v.double_ == 0.0) {
					error_.set(error::zero_divide);
				} else {
					integer_ /= v.integer_;
					float_ /= v.float_;
					if(dbe_) double_ /= v.double_;
				}
				if(v.point_) point_ = v.point_;
				error_ |= v.error_;
				return *this;
			}

			value& operator %= (const value& v) {
				if(v.integer_ == 0 || v.float_ == 0.0f || v.double_ == 0.0) {
					error_.set(error::zero_divide);
				} else {
					integer_ %= v.integer_;
					error_.set(error::float_fatal);
					if(dbe_) error_.set(error::double_fatal);
				}
				if(v.point_) point_ = v.point_;
				error_ |= v.error_;
				return *this;
			}

			value& operator <<= (const value& v) {
				integer_ <<= v.integer_;
				error_.set(error::float_fatal);
				if(dbe_) error_.set(error::double_fatal);
				if(v.point_) point_ = v.point_;
				error_ |= v.error_;
				return *this;
			}

			value& operator >>= (const value& v) {
				integer_ >>= v.integer_;
				error_.set(error::float_fatal);
				if(dbe_) error_.set(error::double_fatal);
				if(v.point_) point_ = v.point_;
				error_ |= v.error_;
				return *this;
			}
		};

		value		value_;

		typedef boost::unordered_map<std::string, value>					symbol_map;
		typedef boost::unordered_map<std::string, value>::iterator			symbol_map_it;
		typedef boost::unordered_map<std::string, value>::const_iterator	symbol_map_cit;
		symbol_map	symbol_;

		value number_()
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
//				long val_bin = 0;
//				long val_oct = 0;
//				long val_hex = 0;

//				bool bin_ok = false;
//				bool oct_ok = false;
//				bool hex_ok = false;

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

		value factor_()
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

		value term_()
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

		value expression_()
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

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief	コンストラクター
		*/
		//-----------------------------------------------------------------//
		arith() : tx_(0), ch_(0) { }


		//-----------------------------------------------------------------//
		/*!
			@brief	デストラクター
		*/
		//-----------------------------------------------------------------//
		~arith() { }


		//-----------------------------------------------------------------//
		/*!
			@brief	シンボルに値を登録
			@param[in]	name	シンボル名
			@param[in]	val		整数値
		*/
		//-----------------------------------------------------------------//
		void set_symbol(const std::string& name, long val)
		{
			symbol_map_it it = symbol_.find(name);
			if(it == symbol_.end()) {
				value v;
				symbol_[name] = v;
///				symbol_map_it it = symbol_.find(name);
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
		void set_symbol(const std::string& name, float val)
		{
			symbol_map_it it = symbol_.find(name);
			if(it == symbol_.end()) {
				value v;
				symbol_[name] = v;
///				symbol_map_it it = symbol_.find(name);
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
		void set_symbol(const std::string& name, double val)
		{
			symbol_map_it it = symbol_.find(name);
			if(it == symbol_.end()) {
				value v;
				symbol_[name] = v;
///				symbol_map_it it = symbol_.find(name);
			}
			value& vp = (*it).second;
			vp.integer_ = static_cast<long>(val);
			vp.float_ = static_cast<float>(val);
			vp.double_ = val;
			vp.point_ = true;
			vp.dbe_ = true;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	解析を開始
			@param[in]	text	解析テキスト
			@return	文法にエラーがあった場合、「false」
		*/
		//-----------------------------------------------------------------//
		bool analize(const std::string& text)
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


		//-----------------------------------------------------------------//
		/*!
			@brief	エラーを受け取る
			@param[in]	type	エラーの種類
			@return エラーがある場合「true」
		*/
		//-----------------------------------------------------------------//
		bool get_error(error::type t) const { return value_.error_[t]; }


		//-----------------------------------------------------------------//
		/*!
			@brief	小数点表記か検査する
			@return 小数点表記なら「true」
		*/
		//-----------------------------------------------------------------//
		bool is_point() const { return value_.point_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	整数を取得
			@return 解析結果（整数）を受け取る
		*/
		//-----------------------------------------------------------------//
		int get_integer() const { return value_.integer_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	整数を取得
			@return 解析結果（32 ビット浮動小数点）を受け取る
		*/
		//-----------------------------------------------------------------//
		float get_float() const { return value_.float_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	整数を取得
			@return 解析結果（64 ビット浮動小数点）を受け取る
		*/
		//-----------------------------------------------------------------//
		double get_double() const { return value_.double_; }
	};
}
