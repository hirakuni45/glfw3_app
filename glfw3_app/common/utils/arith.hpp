#ifndef ARITH_HPP
#define ARITH_HPP
//=====================================================================//
/*!	@file
	@brief	Arithmetic クラス（ヘッダー）@n
			※テキストの数式を展開して、実計算結果を得る。@n
			簡易演算解析ライブラリー
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include <string>
#include <bitset>
#include <boost/unordered_map.hpp>

namespace fio {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	Arithmetic クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	class arith {

	public:

		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		/*!
			@brief	エラー・タイプ
		*/
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		enum error {
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

			err_limit			///< ※管理用
		};

	private:
		const char*		m_tx;
		char			m_ch;

		class value {
		public:
			std::bitset<err_limit>	m_error;

			bool	m_hex_value;
			long	m_integer;

			bool	m_point;

			float	m_float;
			float	m_f_scale;

			bool	m_dbe;
			double	m_double;
			double	m_d_scale;

			value() : m_hex_value(false), m_integer(0), m_point(false),
					  m_float(0.0f), m_f_scale(1.0f),
					  m_dbe(false), m_double(0.0), m_d_scale(1.0) { }

			void sum(char c);
			void neg();
			void inv();

			value& operator += (const value& v) {
				m_integer += v.m_integer;
				m_float += v.m_float;
				if(m_dbe) m_double += v.m_double;
				if(v.m_point) m_point = v.m_point;
				m_error |= v.m_error;
				return *this;
			}

			value& operator -= (const value& v) {
				m_integer -= v.m_integer;
				m_float -= v.m_float;
				if(m_dbe) m_double -= v.m_double;
				if(v.m_point) m_point = v.m_point;
				m_error |= v.m_error;
				return *this;
			}

			value& operator &= (const value& v) {
				m_integer &= v.m_integer;
				m_error.set(float_fatal);
				if(m_dbe) m_error.set(double_fatal);
				if(v.m_point) m_point = v.m_point;
				m_error |= v.m_error;
				return *this;
			}

			value& operator ^= (const value& v) {
				m_integer ^= v.m_integer;
				m_error.set(float_fatal);
				if(m_dbe) m_error.set(double_fatal);
				if(v.m_point) m_point = v.m_point;
				m_error |= v.m_error;
				return *this;
			}

			value& operator |= (const value& v) {
				m_integer |= v.m_integer;
				m_error.set(float_fatal);
				if(m_dbe) m_error.set(double_fatal);
				if(v.m_point) m_point = v.m_point;
				m_error |= v.m_error;
				return *this;
			}

			value& operator *= (const value& v) {
				m_integer *= v.m_integer;
				m_float *= v.m_float;
				if(m_dbe) m_double *= v.m_double;
				if(v.m_point) m_point = v.m_point;
				m_error |= v.m_error;
				return *this;
			}

			value& operator /= (const value& v) {
				if(v.m_integer == 0 || v.m_float == 0.0f || v.m_double == 0.0) {
					m_error.set(zero_divide);
				} else {
					m_integer /= v.m_integer;
					m_float /= v.m_float;
					if(m_dbe) m_double /= v.m_double;
				}
				if(v.m_point) m_point = v.m_point;
				m_error |= v.m_error;
				return *this;
			}

			value& operator %= (const value& v) {
				if(v.m_integer == 0 || v.m_float == 0.0f || v.m_double == 0.0) {
					m_error.set(zero_divide);
				} else {
					m_integer %= v.m_integer;
					m_error.set(float_fatal);
					if(m_dbe) m_error.set(double_fatal);
				}
				if(v.m_point) m_point = v.m_point;
				m_error |= v.m_error;
				return *this;
			}

			value& operator <<= (const value& v) {
				m_integer <<= v.m_integer;
				m_error.set(float_fatal);
				if(m_dbe) m_error.set(double_fatal);
				if(v.m_point) m_point = v.m_point;
				m_error |= v.m_error;
				return *this;
			}

			value& operator >>= (const value& v) {
				m_integer >>= v.m_integer;
				m_error.set(float_fatal);
				if(m_dbe) m_error.set(double_fatal);
				if(v.m_point) m_point = v.m_point;
				m_error |= v.m_error;
				return *this;
			}
		};

		value		m_value;

		typedef boost::unordered_map<std::string, value>					symbol_map;
		typedef boost::unordered_map<std::string, value>::iterator			symbol_map_it;
		typedef boost::unordered_map<std::string, value>::const_iterator	symbol_map_cit;
		symbol_map	m_symbol;

		value number();
		value factor();
		value term();
		value expression();
	public:
		//-----------------------------------------------------------------//
		/*!
			@brief	コンストラクター
		*/
		//-----------------------------------------------------------------//
		arith() : m_tx(0), m_ch(0) { }


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
		void set_symbol_i(const char* name, long val);


		//-----------------------------------------------------------------//
		/*!
			@brief	シンボルに値を登録
			@param[in]	name	シンボル名
			@param[in]	val		浮動小数点
		*/
		//-----------------------------------------------------------------//
		void set_symbol_f(const char* name, float val);


		//-----------------------------------------------------------------//
		/*!
			@brief	シンボルに値を登録
			@param[in]	name	シンボル名
			@param[in]	val		倍精度浮動小数点
		*/
		//-----------------------------------------------------------------//
		void set_symbol_d(const char* name, double val);


		//-----------------------------------------------------------------//
		/*!
			@brief	解析を開始
			@param[in]	text	解析テキスト
			@return	文法にエラーがあった場合、「false」
		*/
		//-----------------------------------------------------------------//
		bool analize(const char* text);


		//-----------------------------------------------------------------//
		/*!
			@brief	エラーを受け取る
			@param[in]	type	エラーの種類
			@return エラーがある場合「true」
		*/
		//-----------------------------------------------------------------//
		bool get_error(error type) const { return m_value.m_error[type]; }


		//-----------------------------------------------------------------//
		/*!
			@brief	小数点表記か検査する
			@return 小数点表記なら「true」
		*/
		//-----------------------------------------------------------------//
		bool is_point() const { return m_value.m_point; }


		//-----------------------------------------------------------------//
		/*!
			@brief	整数を取得
			@return 解析結果（整数）を受け取る
		*/
		//-----------------------------------------------------------------//
		int get_integer() const { return m_value.m_integer; }


		//-----------------------------------------------------------------//
		/*!
			@brief	整数を取得
			@return 解析結果（32 ビット浮動小数点）を受け取る
		*/
		//-----------------------------------------------------------------//
		float get_float() const { return m_value.m_float; }


		//-----------------------------------------------------------------//
		/*!
			@brief	整数を取得
			@return 解析結果（64 ビット浮動小数点）を受け取る
		*/
		//-----------------------------------------------------------------//
		double get_double() const { return m_value.m_double; }

	};

}
#endif
