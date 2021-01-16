#pragma once
//=============================================================================//
/*! @file
    @brief  mpfr ラッパークラス @n
			GNU gmp, mpfr の C++ ラッパー
    @author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2020 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/RX/blob/master/LICENSE
*/
//=============================================================================//
#include <cmath>
#include <mpfr.h>

namespace mpfr {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  mpfr オブジェクト
		@param[in]	num		有効桁数
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	template <uint32_t num>
	class value {

		mpfr_t		t_;
		mpfr_rnd_t	rnd_;

		static uint32_t ref_count_;

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief  コンストラクター
			@param[in]	rnd	丸めパラメータ
		*/
		//-----------------------------------------------------------------//
		value(mpfr_rnd_t rnd = MPFR_RNDN) noexcept : t_(), rnd_(rnd) {
			mpfr_init2(t_, num);
			++ref_count_;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  コンストラクター
			@param[in]	rnd	丸めパラメータ
		*/
		//-----------------------------------------------------------------//
		value(const value& t, mpfr_rnd_t rnd = MPFR_RNDN) noexcept : rnd_(rnd)
		{
			mpfr_init2(t_, num);
			mpfr_set(t_, t.t_, rnd_);
			++ref_count_;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  コンストラクター(int)
			@param[in]	iv	初期値
			@param[in]	rnd	丸めパラメータ
		*/
		//-----------------------------------------------------------------//
		explicit value(int iv, mpfr_rnd_t rnd = MPFR_RNDN) noexcept : rnd_(rnd) {
			mpfr_init2(t_, num);
			mpfr_set_si(t_, iv, rnd);
			++ref_count_;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  コンストラクター(float)
			@param[in]	iv	初期値
			@param[in]	rnd	丸めパラメータ
		*/
		//-----------------------------------------------------------------//
		explicit value(float iv, mpfr_rnd_t rnd = MPFR_RNDN) noexcept : rnd_(rnd) {
			mpfr_init2(t_, num);
			mpfr_set_flt(t_, iv, rnd);
			++ref_count_;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  コンストラクター(double)
			@param[in]	iv	初期値
			@param[in]	rnd	丸めパラメータ
		*/
		//-----------------------------------------------------------------//
		explicit value(double iv, mpfr_rnd_t rnd = MPFR_RNDN) noexcept : rnd_(rnd) {
			mpfr_init2(t_, num);
			mpfr_set_d(t_, iv, rnd);
			++ref_count_;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  コンストラクター(文字列)
			@param[in]	iv	初期値
			@param[in]	rnd	丸めパラメータ
		*/
		//-----------------------------------------------------------------//
		explicit value(const char* iv, mpfr_rnd_t rnd = MPFR_RNDN) noexcept : rnd_(rnd) {
			mpfr_init2(t_, num);
			mpfr_set_str(t_, iv, 10, rnd);
			++ref_count_;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  デストラクター
		*/
		//-----------------------------------------------------------------//
		~value() {
			mpfr_clear(t_);
			--ref_count_;
			if(ref_count_ == 0) {
				mpfr_free_cache();
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  mpfr_ptr を取得
			@return mpfr_ptr
		*/
		//-----------------------------------------------------------------//
		mpfr_ptr get() noexcept { return t_; }


		//-----------------------------------------------------------------//
		/*!
			@brief  mpfr_rnd_t を取得
			@return mpfr_rnd_t
		*/
		//-----------------------------------------------------------------//
		auto get_rnd() const noexcept { return rnd_; }


		//-----------------------------------------------------------------//
		/*!
			@brief  円周率を取得
			@return 円周率
		*/
		//-----------------------------------------------------------------//
		static const auto get_pi() {
			value tmp;
			mpfr_const_pi(tmp.t_, tmp.get_rnd());
			return tmp;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  ２の自然対数を取得
			@return ２の自然対数
		*/
		//-----------------------------------------------------------------//
		static const auto get_log2() {
			value tmp;
			mpfr_const_log2(tmp.t_, tmp.get_rnd());
			return tmp;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  Euler数を取得
			@return Euler数
		*/
		//-----------------------------------------------------------------//
		static const auto get_euler() {
			value tmp;
			mpfr_const_euler(tmp.t_, tmp.get_rnd());
			return tmp;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  整数部と小数部の分離
			@param[in]	src	ソース
			@param[out]	iop	整数部
			@param[out]	fop	小数部
		*/
		//-----------------------------------------------------------------//
		static void fmod(const value& src, value& iop, value& fop)
		{
			mpfr_modf(iop.t_, fop.t_, src.t_, fop.get_rnd());
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  整数除算(x/y)における余り
			@param[in]	x	分子
			@param[in]	y	分母
			@param[out]	m	余り
		*/
		//-----------------------------------------------------------------//
		static void mod(const value& x, const value& y, value& m)
		{
			mpfr_fmod(m.t_, x.t_, y.t_, m.get_rnd());
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	数値文字列の入力
			@param[in]	str		数値文字列
		*/
		//-----------------------------------------------------------------//
		void assign(const char* str) noexcept
		{
			mpfr_set_str(t_, str, 10, rnd_);
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	絶対値
			@param[in]	in	入力
			@return	in に対する絶対値
		*/
		//-----------------------------------------------------------------//
		static value abs(const value& in) noexcept
		{
			value out;
			mpfr_abs(out.t_, in.t_, in.rnd_);
			return out;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	べき乗 (this = this ^ n)
			@param[in]	n	n 乗
		*/
		//-----------------------------------------------------------------//
		void pow(const value& n) noexcept
		{
			mpfr_pow(t_, t_, n.t_, rnd_);
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  交換
			@param[in]	th	交換クラス
		*/
		//-----------------------------------------------------------------//
		void swap(value& th) noexcept
		{
			mpfr_swap(t_, th.t_);
		}


		bool operator == (int v) const noexcept { return mpfr_cmp_si(t_, v) == 0; }
		bool operator != (int v) const noexcept { return mpfr_cmp_si(t_, v) != 0; }
		bool operator == (long v) const noexcept { return mpfr_cmp_si(t_, v) == 0; }
		bool operator != (long v) const noexcept { return mpfr_cmp_si(t_, v) != 0; }
		bool operator == (double v) const noexcept { return mpfr_cmp_d(t_, v) == 0; }
		bool operator != (double v) const noexcept { return mpfr_cmp_d(t_, v) != 0; }


		value& operator = (const value& th) noexcept {
			mpfr_set(t_, th.t_, rnd_);
			return *this;
		}
		value& operator = (long v) noexcept {
			mpfr_set_si(t_, v, rnd_);
			return *this;
		}	
		value& operator = (double v) noexcept {
			mpfr_set_d(t_, v, rnd_);
			return *this;
		}


		const value operator - () noexcept
		{
			value tmp(*this);
			mpfr_neg(tmp.t_, tmp.t_, rnd_);
			return tmp;
		}


		value& operator += (const value& th) noexcept
		{
			mpfr_add(t_, t_, th.t_, rnd_);
			return *this;
		}

		value& operator -= (const value& th) noexcept
		{
			mpfr_sub(t_, t_, th.t_, rnd_);
			return *this;
		}

		value& operator *= (const value& th) noexcept
		{
			mpfr_mul(t_, t_, th.t_, rnd_);
			return *this;
		}

		value& operator /= (const value& th) noexcept
		{
			mpfr_div(t_, t_, th.t_, rnd_);
			return *this;
		}

		value operator + (const value& th) const noexcept { return value(*this) += th; }
		value operator - (const value& th) const noexcept { return value(*this) -= th; }
		value operator * (const value& th) const noexcept { return value(*this) *= th; }
		value operator / (const value& th) const noexcept { return value(*this) /= th; }


		//-----------------------------------------------------------------//
		/*!
			@brief  数値文字列を取得
			@param[in]	upn		小数点以下の文字数
			@param[out]	out		格納文字列ポインター
			@param[in]	len		格納文字列最大数
		*/
		//-----------------------------------------------------------------//
		void operator() (int upn, char* out, uint32_t len) noexcept
		{
			char form[16];
			utils::sformat("%%.%dRNf", form, sizeof(form)) % upn;
			mpfr_snprintf(out, len, form, t_);
//			mpfr_snprintf(out, len, "%.50RNf", t_);
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  三角関数 sin
			@param[in]	in	角度
		*/
		//-----------------------------------------------------------------//
		static value sin(const value& in)
		{
			value out;
			mpfr_sin(out.t_, in.t_, out.get_rnd());
			return out;
		}


		static value asin(const value& in)
		{
			value out;
			mpfr_asin(out.t_, in.t_, out.get_rnd());
			return out;
		}

		static value sinh(const value& in)
		{
			value out;
			mpfr_sinh(out.t_, in.t_, out.get_rnd());
			return out;
		}

		static value asinh(const value& in)
		{
			value out;
			mpfr_asinh(out.t_, in.t_, out.get_rnd());
			return out;
		}

		static value cos(const value& in)
		{
			value out;
			mpfr_cos(out.t_, in.t_, out.get_rnd());
			return out;
		}

		static value acos(const value& in)
		{
			value out;
			mpfr_acos(out.t_, in.t_, out.get_rnd());
			return out;
		}

		static value cosh(const value& in)
		{
			value out;
			mpfr_cosh(out.t_, in.t_, out.get_rnd());
			return out;
		}

		static value acosh(const value& in)
		{
			value out;
			mpfr_acosh(out.t_, in.t_, out.get_rnd());
			return out;
		}

		static value tan(const value& in)
		{
			value out;
			mpfr_tan(out.t_, in.t_, out.get_rnd());
			return out;
		}

		static value atan(const value& in)
		{
			value out;
			mpfr_atan(out.t_, in.t_, out.get_rnd());
			return out;
		}

		static value tanh(const value& in)
		{
			value out;
			mpfr_tanh(out.t_, in.t_, out.get_rnd());
			return out;
		}

		static value atanh(const value& in)
		{
			value out;
			mpfr_atanh(out.t_, in.t_, out.get_rnd());
			return out;
		}

		static value eint(const value& in)  // 指数積分
		{
			value out;
			mpfr_eint(out.t_, in.t_, out.get_rnd());
			return out;
		}

		static value log10(const value& in)
		{
			value out;
			mpfr_log10(out.t_, in.t_, out.get_rnd());
			return out;
		}


		static value log(const value& in)
		{
			value out;
			mpfr_log(out.t_, in.t_, out.get_rnd());
			return out;
		}


		static value sqrt(const value& in)
		{
			value out;
			mpfr_sqrt(out.t_, in.t_, out.get_rnd());
			return out;
		}


		static value exp10(const value& in)
		{
			value out;
			mpfr_exp10(out.t_, in.t_, out.get_rnd());
			return out;
		}
	};

	// テンプレート関数、実態の定義
	template<uint32_t num> uint32_t value<num>::ref_count_;
}
