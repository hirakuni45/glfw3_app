#pragma once
//=====================================================================//
/*!	@file
	@brief	Arithmetic クラス（ヘッダー）@n
			※テキストの数式を展開して、実計算結果を得る。@n
			簡易演算式解析
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

			void sum(char c);
			void neg();
			void inv();

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

		value number_();
		value factor_();
		value term_();
		value expression_();
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
		void set_symbol(const std::string& name, long val);


		//-----------------------------------------------------------------//
		/*!
			@brief	シンボルに値を登録
			@param[in]	name	シンボル名
			@param[in]	val		浮動小数点
		*/
		//-----------------------------------------------------------------//
		void set_symbol(const std::string& name, float val);


		//-----------------------------------------------------------------//
		/*!
			@brief	シンボルに値を登録
			@param[in]	name	シンボル名
			@param[in]	val		倍精度浮動小数点
		*/
		//-----------------------------------------------------------------//
		void set_symbol(const std::string& name, double val);


		//-----------------------------------------------------------------//
		/*!
			@brief	解析を開始
			@param[in]	text	解析テキスト
			@return	文法にエラーがあった場合、「false」
		*/
		//-----------------------------------------------------------------//
		bool analize(const std::string& text);


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
