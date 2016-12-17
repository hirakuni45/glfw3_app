#pragma once
//=====================================================================//
/*!	@file
	@brief	整数演算　クラス
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include <cstdint>

namespace utils {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	intmath クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	struct intmath {

		// 円周率(3.141592654 * 2^30)
		static const uint32_t pai_ = 0xC90FDAA2;
		static const uint32_t pai_shift_ = 30;

		uint32_t rand_base_a_;
		uint32_t rand_base_b_;
		uint32_t rand_seed_;

		intmath() : rand_base_a_(0), rand_base_b_(0), rand_seed_(0) { }


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


		//-----------------------------------------------------------------//
		/*!
			@brief	M 系列乱数を発生させる多項式ジェネレーター（32ビット）@n
					※全て「１」を与えるとロックする
			@param[in]	in	入力値（-1 は構造上ロックするので設定禁止）
			@param[in]	ta	タップのビットを立てる
			@param[in]	tb	タップのビットを立てる
			@return	答え
		 */
		//-----------------------------------------------------------------//
		template<typename T>
		T mrand(T in, T ta, T tb)
		{
			T sg = 0;
			if(in & ta) sg = 1;
			if(in & tb) return in + in + sg;
			else return in + in + (sg ^ 1);
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	乱数に初期値を与える
			@param[in]	a	a パラメーター(-1）の場合は無設定
			@param[in]	b	b パラメーター(-1）の場合は無設定
			@param[in]	seed	ベース乱数発生機のシード
		 */
		//-----------------------------------------------------------------//
		void rand_seed(uint32_t a, uint32_t b, uint32_t seed)
		{
			if(a != 0xffffffff) rand_base_a_ = a;
			if(b != 0xffffffff) rand_base_b_ = b;
			rand_seed_ = seed;
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

		//-----------------------------------------------------------------//
		/*!
			@brief	乱数を得る
			@return	32ビットの乱数
		 */
		//-----------------------------------------------------------------//
		uint32_t rand32()
		{
			rand_base_a_ = mrand<uint32_t>(rand_base_a_, 0x0004, 0x4000);
			rand_base_a_ += (rand_base_a_ << 2) + 1;
			rand_base_b_ = mrand<uint32_t>(rand_base_b_, 0x0008, 0x4000);
			rand_base_b_ += (rand_base_b_ << 2) + 1;
			rand_seed_ ^= rand_base_a_ ^ (rand_base_b_ << 16);
			return rand_seed_;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	1/4 周期の sin テーブルの生成 @n
					X' = X * COS(s) - Y * SIN(s) @n
					Y' = X * SIN(s) + Y * COS(s) @n
					※角度「s」が十分小さい場合、以下のように近似できる。@n
					COS(s) ≒ 1 @n
					SIN(s) ≒ s @n
					X' = X   - Y・s @n
					Y' = X'・s + Y  @n
					上記の原理を使って、三角関数テーブルを作成する
			@param[out]	tbl	テーブルを受け取る配列
			@param[in]	shi	テーブル長に対するシフト数（２＾ｎ）
			@param[in]	len	腕の長さ（COS(0)に対する値）
		 */
		//-----------------------------------------------------------------//
		static void build_sin_tables(int16_t* tbl, uint16_t shi, int16_t len = 0x4000)
		{
			int16_t gain = 16;  // 精度を確保する為の下駄
			int64_t x = len << gain;  // cos(0) の値
			int64_t y = 0;    // sin(0) の値
			for(uint16_t i = 0; i < (1 << shi); ++i) {
				*tbl++ = y >> gain;   // pai_ のビット位置補正
				// 全周は、２・πなので＋１
				x -= (static_cast<int64_t>(pai_) * y) >> (pai_shift_ + shi + 1);
				y += (static_cast<int64_t>(pai_) * x) >> (pai_shift_ + shi + 1);
			}
		}

	};
}
