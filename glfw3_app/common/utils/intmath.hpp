#pragma once
//=====================================================================//
/*!	@file
	@brief	整数演算　クラス
    @author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2017 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/glfw_app/blob/master/LICENSE
*/
//=====================================================================//
#include <cstdint>

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
/*!
	@brief	imath クラス
*/
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
struct imath {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	平方根表現構造体
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	template <typename T>
	struct sqrt_t {

		typedef T value_type;

		T	val;	///< 答え
		T	mod;	///< 余り

		sqrt_t(T v = 0, T m = 0) : val(v), mod(m) { }
	};


	//-----------------------------------------------------------------//
	/*!
		@brief	１６ビットレンジの平方根を求める
		@param[int]	in	整数値
		@return	答え
	 */
	//-----------------------------------------------------------------//
	static sqrt_t<uint16_t> sqrt16(uint16_t in)
	{
		uint32_t a = in;
		uint32_t b = 0x4000;
		for(int i = 0; i < 8; ++i) {
			if(a >= b) {
				a -= b;
				b = ((b + b) & 0xfffe0000) + 0x10000 + (b & 0xffff);
			} else {
				b = ((b + b) & 0xfffe0000) + (b & 0xffff);
			}
			a <<= 2;
		}
		sqrt_t<uint16_t> ans(b >> 16, a >> 16);
		return ans;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	３２ビットレンジの平方根を求める
		@param[int]	in	整数値
		@return	答え
	 */
	//-----------------------------------------------------------------//
	static sqrt_t<uint32_t> sqrt32(uint32_t al)
	{
		uint32_t ah, bh, bl;

		ah = bh = 0;
		bl = 0x40000000;
		for(int i = 0; i < 16; ++i) {
			if(al >= bl) {
				if(ah >= bh) {
					ah -= bh;
					al -= bl;
					bh += bh + 1;
				} else {
					bh += bh;
				}
			} else {
				if(ah >= (bh + 1)) {
					ah -= bh + 1;
					al -= bl;
					bh += bh + 1;
				} else {
					bh += bh;
				}
			}
			ah <<= 2;
			ah += al >> 30;
			al <<= 2;
		}

		sqrt_t<uint32_t> ans(bh, ah);
		return ans;
	}


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	Ｍ系列乱数クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	struct mrand {

		uint32_t base_a_;
		uint32_t base_b_;
		uint32_t seed_;

		mrand() : base_a_(0), base_b_(0), seed_(0) { }


		//-------------------------------------------------------------//
		/*!
			@brief	M 系列乱数を発生させる多項式ジェネレーター（32ビット）@n
					※全て「１」を与えるとロックする
			@param[in]	in	入力値（-1 は構造上ロックするので設定禁止）
			@param[in]	ta	タップのビットを立てる
			@param[in]	tb	タップのビットを立てる
			@return	答え
		 */
		//-------------------------------------------------------------//
		template<typename T>
		T rgen(T in, T ta, T tb)
		{
			T sg = 0;
			if(in & ta) sg = 1;
			if(in & tb) return in + in + sg;
			else return in + in + (sg ^ 1);
		}


		//-------------------------------------------------------------//
		/*!
			@brief	乱数に初期値を与える
			@param[in]	a	a パラメーター(-1）の場合は無設定
			@param[in]	b	b パラメーター(-1）の場合は無設定
			@param[in]	seed	ベース乱数発生機のシード
		 */
		//-------------------------------------------------------------//
		void seed(uint32_t a, uint32_t b, uint32_t seed)
		{
			if(a != 0xffffffff) base_a_ = a;
			if(b != 0xffffffff) base_b_ = b;
			seed_ = seed;
		}

#if 0
static short randTapTables[] = {
	0x0001,0x0040,	0x0004,0x0040,	/*  7 bits */
	0x0008,0x0100,					/*  9 bits */
	0x0004,0x0200,					/* 10 bits */
	0x0002,0x0400,					/* 11 bits */
	0x0001,0x4000,					/* 15 bits */
	0x0008,0x4000,	0x0004,0x4000	/* 15 bits */
};
#endif

		//-------------------------------------------------------------//
		/*!
			@brief	３２ビット乱数を得る
			@return	３２ビットの乱数
		 */
		//-------------------------------------------------------------//
		uint32_t rand32()
		{
			base_a_ = rgen<uint32_t>(base_a_, 0x0004, 0x4000);
			base_a_ += (base_a_ << 2) + 1;
			base_b_ = rgen<uint32_t>(base_b_, 0x0008, 0x4000);
			base_b_ += (base_b_ << 2) + 1;
			seed_ ^= base_a_ ^ (base_b_ << 16);
			return seed_;
		}
	};


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	SIN, COS テーブル生成 @n
				1/4 周期の sin テーブルの生成 @n
				X' = X * COS(s) - Y * SIN(s) @n
				Y' = X * SIN(s) + Y * COS(s) @n
				※角度「s」が十分小さい場合、以下のように近似できる。@n
				COS(s) ≒ 1 @n
				SIN(s) ≒ s @n
				X' = X   - Y・s @n
				Y' = X'・s + Y  @n
				上記の原理を使って、三角関数テーブルを作成する
		@param[in]	shi	テーブルサイズ（２のＮ乗）
		@param[in]	len	正規化された値の最大値（腕の長さ）
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	template <uint16_t shi, uint16_t len>
	class sin_cos {

		static const uint32_t pai_ = 0xC90FDAA2;	///< 円周率(3.141592654 * 2^30)
		static const uint32_t pai_shift_ = 30;		///< 円周率、小数点位置

		uint16_t tbl_[(1 << shi) + 1];

		void build_()
		{
			int16_t gain = 16;  // 精度を確保する為の下駄
			int64_t x = len << gain;  // cos(0) の値
			int64_t y = 0;    // sin(0) の値
			for(uint16_t i = 0; i < ((1 << shi) + 1); ++i) {
				tbl_[i] = y >> gain;   // pai_ のビット位置補正
				// 全周は、２・πなので＋１
				x -= (static_cast<int64_t>(pai_) * y) >> (pai_shift_ + shi + 1);
				y += (static_cast<int64_t>(pai_) * x) >> (pai_shift_ + shi + 1);
			}
		}

	public:
		sin_cos() { build_(); }


		//-----------------------------------------------------------------//
		/*!
			@brief	sin、cos の値を取得
			@param[in]	a	角度（0 to 65535)
			@param[out]	sin	sin 値
			@param[out]	cos	cos 値
		 */
		//-----------------------------------------------------------------//
		void get(uint16_t a, int16_t& sin, int16_t& cos) const
		{
			uint16_t sa, sb;
			if(a & 0x8000) sa = 0xffff; else sa = 0;
			if(a & 0x4000) sb = 0xffff; else sb = 0;
			a = ((a >> (16 - (shi + 2))) ^ sb) & ((1 << shi) - 1);
			sin = (static_cast<uint16_t>(tbl_[a]) ^ sa) + sa;
			a ^= (1 << shi) - 1;
			sa ^= sb;
			cos = (static_cast<uint16_t>(tbl_[a]) ^ sa) + sa;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	テーブルへのダイレクトアクセス
			@param[in]	idx	インデックス
			@return テーブルの値
		 */
		//-----------------------------------------------------------------//
		int16_t operator [] (uint16_t idx) const {
			if(sizeof(tbl_) <= idx) return 0;
			return tbl_[idx];
		}
	};
};

