#pragma once
//=====================================================================//
/*!	@file
	@brief	RX グループ MTU3 制御
    @author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2018, 2019 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/RX/blob/master/LICENSE
*/
//=====================================================================//
#include "renesas.hpp"
#include "common/intr_utils.hpp"
#include "common/vect.h"

namespace device {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  MTU 基本クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	class mtu_base {
	public:
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		/*!
			@brief  アウトプット・タイプ
		*/
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		enum class OUTPUT_TYPE : uint8_t {
			NONE,				///< 出力しない
			LOW_TO_HIGH,		///< 初期０で、変化で１
			HIGH_TO_LOW,		///< 初期１で、変化で０
			TOGGLE,				///< トグル出力
		};


		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		/*!
			@brief  キャプチャー・タイプ
		*/
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		enum class CAPTURE_TYPE : uint8_t {
			POSITIVE,	///< 立ち上がり
			NEGATIVE,	///< 立下り
			DUAL,		///< 両エッジ
		};


		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		/*!
			@brief  キャプチャー構造体
		*/
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		struct capture_t {
			volatile uint32_t	all_count_;
			volatile uint16_t	ovfw_limit_;	///< オーバーフローの最大値
			volatile uint16_t	ovfw_count_;

			capture_t() : all_count_(0),
				ovfw_limit_(get_mtu_master_clock() / 65536),   // タイムアウト１秒
				ovfw_count_(0)
			{ }

			void clear() {
				all_count_ = 0;
				ovfw_count_ = 0;
			}
		};
	};


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  MTU 制御クラス
		@param[in]	MTU	MTU ユニット
		@param[in]	MTASK	メイン割り込みタスク
		@param[in]	OTASK	オーバーフロー割り込みタスク
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	template <class MTUX, class MTASK = utils::null_task, class OTASK = utils::null_task>
	class mtu_io : public mtu_base {
	public:

		typedef MTUX mtu_type;

	private:

		uint32_t	clk_base_;

		struct task_t {
			volatile uint32_t	tgr_adr_;		// TGR の実アドレス
			volatile uint32_t	main_tick_;
			volatile uint32_t	ovfw_tick_;

			capture_t	cap_;

			task_t() : tgr_adr_(MTUX::TGRA.address()),
				main_tick_(0), ovfw_tick_(0), cap_() { }
		};

		static task_t	tt_;

		static MTASK	mtask_;
		static OTASK	otask_;

		typename MTUX::channel	channel_;

		ICU::VECTOR		intr_vec_;


		static INTERRUPT_FUNC void cap_task_()
		{
			++tt_.main_tick_;

			tt_.cap_.all_count_ = (tt_.ovfw_tick_ << 16) | rd16_(tt_.tgr_adr_);
			mtask_();
			tt_.ovfw_tick_ = 0;
		}

		static INTERRUPT_FUNC void ovf_task_()
		{
			++tt_.ovfw_tick_;
			if(tt_.ovfw_tick_ >= tt_.cap_.ovfw_limit_) {
				tt_.ovfw_tick_ = 0;
				MTUX::TCNT = 0;
				++tt_.cap_.ovfw_count_;
			}
			otask_();
		}

		static INTERRUPT_FUNC void out_task_()
		{
			++tt_.main_tick_;
			mtask_();
		}

		// ch: A, B, C, D (0 to 3)
		// dv: 0 to 10
		static void set_TCR_(typename MTUX::channel ch, uint8_t dv)
		{
			static const uint8_t ckt[] = {
				0b000000,  // (0)  1/1
				0b001000,  // (1)  1/2
				0b000001,  // (2)  1/4
				0b010000,  // (3)  1/8
				0b000010,  // (4)  1/16
				0b011000,  // (5)  1/32
				0b000011,  // (6)  1/64
				0b100000,  // (7)  1/128(NG: 1/256)
				0b100000,  // (8)  1/256
				0b101000,  // (9)  1/512(NG: 1/1024)
				0b101000,  // (10) 1/1024
			};
			static const uint8_t cclr[4] = { 0b001, 0b010, 0b101, 0b110 };
			MTUX::TCR  = MTUX::TCR.TPSC.b(ckt[dv] & 7)
					   | MTUX::TCR.CKEG.b(0b00)
					   | MTUX::TCR.CCLR.b(cclr[static_cast<uint8_t>(ch)]);
			MTUX::TCR2 = MTUX::TCR2.TPSC2.b((ckt[dv] >> 3) & 7);
		}


		static bool make_clock_(uint32_t frq, uint8_t& dv, uint32_t& match)
		{
			dv = 0;
			match = get_mtu_master_clock() / frq;
			while(match > 65535) {
				++dv;
				bool mod = match & 1;
				match /= 2;
				if(mod) ++match;
			}
			if(dv > 10) {
				// overflow clock divide...
				return false;
			}
			return true;
		}


		void set_output_type_(typename MTUX::channel ch, OUTPUT_TYPE ot, uint32_t& match)
		{
			uint8_t ctd = 0;
			switch(ot) {
			case OUTPUT_TYPE::LOW_TO_HIGH: ctd = 0b0010; break;
			case OUTPUT_TYPE::HIGH_TO_LOW: ctd = 0b0101; break;
			case OUTPUT_TYPE::TOGGLE:
				{
					ctd = 0b0111;
					bool mod = match & 1;
					match /= 2;
					if(mod) ++match;
				}
				break;
			default: break;
			}
			MTUX::TIOR.set(ch, ctd);
		}

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief  コンストラクター
		*/
		//-----------------------------------------------------------------//
		mtu_io() noexcept : clk_base_(get_mtu_master_clock()), intr_vec_(ICU::VECTOR::NONE) { }


		//-----------------------------------------------------------------//
		/*!
			@brief  ノーマル・モード（コンペア・マッチ・タイマー）
			@param[in]	ch		出力チャネル
			@param[in]	ot		出力タイプ
			@param[in]	frq		出力周波数
			@param[in]	lvl		割り込みレベル
			@return 成功なら「true」
		*/
		//-----------------------------------------------------------------//
		bool start_normal(typename MTUX::channel ch, OUTPUT_TYPE ot, uint32_t frq,
			uint8_t lvl = 0) noexcept
		{
			if(MTUX::PERIPHERAL == peripheral::MTU5) {  // MTU5 は通常出力として利用不可
				return false;
			}

			power_mgr::turn(MTUX::PERIPHERAL);

			channel_ = ch;

			bool pena = (ot != OUTPUT_TYPE::NONE);
			port_map::turn(MTUX::PERIPHERAL, static_cast<port_map::channel>(ch), pena);

			uint8_t dv;
			uint32_t match;
			if(!make_clock_(frq, dv, match)) {
				return false;
			}

			set_output_type_(ch, ot, match);

			set_TCR_(ch, dv);
			MTUX::TMDR1 = 0x00;  // 通常動作

			if(lvl > 0) {
				auto cvec = MTUX::get_vec(static_cast<typename MTUX::interrupt>(ch));
				intr_vec_ = icu_mgr::set_interrupt(cvec, out_task_, lvl);
				MTUX::TIER = (1 << static_cast<uint8_t>(ch)) | MTUX::TIER.TCIEV.b();
			}

			// 各チャネルに相当するジャネラルレジスタ
			tt_.tgr_adr_ = MTUX::TGRA.address() + static_cast<uint32_t>(ch) * 2;
			wr16_(tt_.tgr_adr_, match - 1);

			MTUX::TCNT = 0;
			MTUX::enable();

			return true;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  周期再設定（コンペア・マッチ・タイマー周期）
			@param[in]	ch		出力チャネル
			@param[in]	frq		出力周波数
			@return 成功なら「true」
		*/
		//-----------------------------------------------------------------//
		static bool set_freq(typename MTUX::channel ch, uint32_t frq) noexcept
		{
			uint8_t dv;
			uint32_t match;
			if(!make_clock_(frq, dv, match)) {
				return false;
			}

			set_TCR_(ch, dv);

			auto tior = MTUX::TIOR.get(ch);
			if(tior == 0b0111) {  // toggle なら１／２にする
				bool mod = match & 1;
				match /= 2;
				if(mod) ++match;
			}

			if((match - 1) < MTUX::TCNT()) {
				MTUX::TCNT = 0;
			}
			auto adr = MTUX::TGRA.address() + static_cast<uint32_t>(ch) * 2;
			wr16_(adr, match - 1);

			return true;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  周期設定（コンペア・マッチ・タイマー周期）@n
					※レガシー
			@param[in]	frq		出力周波数
			@return 成功なら「true」
		*/
		//-----------------------------------------------------------------//
		bool set_frq(uint32_t frq) noexcept
		{
			uint8_t dv;
			uint32_t match;
			if(!make_clock_(frq, dv, match)) {
				return false;
			}

			set_TCR_(channel_, dv);

			auto tior = MTUX::TIOR.get(channel_);
			if(tior == 0b0111) {  // toggle なら１／２にする
				bool mod = match & 1;
				match /= 2;
				if(mod) ++match;
			}

			if((match - 1) < MTUX::TCNT()) {
				MTUX::TCNT = 0;
			}
			auto adr = MTUX::TGRA.address() + static_cast<uint32_t>(channel_) * 2;
			wr16_(adr, match - 1);

			return true;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  ＰＷＭ出力開始（モード２）
			@param[in]	ch		出力チャネル
			@param[in]	ot		出力タイプ
			@param[in]	frq		周期
			@param[in]	level	割り込みレベル
			@return 成功なら「true」
		*/
		//-----------------------------------------------------------------//
		bool start_pwm2(typename MTUX::channel ch, OUTPUT_TYPE ot, uint32_t frq,
			uint8_t level = 0) noexcept
		{
			if(peripheral::MTU3 <= MTUX::PERIPHERAL && MTUX::PERIPHERAL <= peripheral::MTU7) {
				return false;
			}

			power_mgr::turn(MTUX::PERIPHERAL);

			bool pena = (ot != OUTPUT_TYPE::NONE);
			port_map::turn(MTUX::PERIPHERAL, static_cast<port_map::channel>(ch), pena);

			uint8_t dv;
			uint32_t match;
			if(!make_clock_(frq, dv, match)) {
				return false;
			}

			set_output_type_(ch, ot, match);

			set_TCR_(ch, dv);
			MTUX::TMDR1.MD = 0b0011;  // PWM mode 2

			// 各チャネルに相当するジャネラルレジスタ
			tt_.tgr_adr_ = MTUX::TGRA.address() + static_cast<uint32_t>(ch) * 2;
			wr16_(tt_.tgr_adr_, match - 1);

			MTUX::TCNT = 0;
			MTUX::enable();

			return true;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  ベース・クロック（インプット・キャプチャ）の設定 @n
					※MTU3 モジュールへの供給クロックのＮ分の１を設定
			@param[in]	clk	ベース・クロック
		*/
		//-----------------------------------------------------------------//
		void set_base_clock(uint32_t clk) noexcept {
			uint32_t a = get_mtu_master_clock();
			while(a < clk) {
				a <<= 1;
			}
			clk_base_ = a;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  ベース・クロックの取得
			@return ベース・クロック
		*/
		//-----------------------------------------------------------------//
		uint32_t get_base_clock() const noexcept { return clk_base_; }


		//-----------------------------------------------------------------//
		/*!
			@brief  PWM デューティー設定
			@param[in]	ch		出力チャネル
			@param[in]	duty	デューティー
			@return 成功なら「true」
		*/
		//-----------------------------------------------------------------//
		bool set_duty(typename MTUX::channel ch, uint16_t duty) noexcept
		{
			// 工事中
			return true;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  インプットキャプチャ開始 @n
					※カウントクロックは「set_limit_clock」により @n
					変更が可能。
			@param[in]	ch		入力チャネル
			@param[in]	ct		キャプチャ・タイプ
			@param[in]	lvl		割り込みレベル（割り込みを使わない場合エラー）
			@return 成功なら「true」
		*/
		//-----------------------------------------------------------------//
		bool start_capture(typename MTUX::channel ch, CAPTURE_TYPE ct, uint8_t lvl) noexcept
		{
			if(lvl == 0) return false;

			// MTU5 はインプットキャプチャとして利用不可
			if(MTUX::PERIPHERAL == peripheral::MTU5) {
				return false;
			}

			power_mgr::turn(MTUX::PERIPHERAL);

			port_map::turn(MTUX::PERIPHERAL, static_cast<port_map::channel>(ch));

			uint8_t ctd = 0;
			switch(ct) {
			case CAPTURE_TYPE::POSITIVE: ctd = 0b1000; break;
			case CAPTURE_TYPE::NEGATIVE: ctd = 0b1001; break;
			case CAPTURE_TYPE::DUAL:     ctd = 0b1011; break;
			default: break;
			}
			MTUX::TIOR.set(ch, ctd);

			uint32_t dv = get_mtu_master_clock() / clk_base_;
			--dv;
			if(dv >= 10) dv = 10;

			set_TCR_(ch, dv);
			MTUX::TMDR1 = 0x00;  // 通常動作

			if(lvl > 0) {
				auto ovec = MTUX::get_vec(MTUX::interrupt::OVF);
				icu_mgr::set_interrupt(ovec, ovf_task_, lvl);
				auto cvec = MTUX::get_vec(static_cast<typename MTUX::interrupt>(ch));
				icu_mgr::set_interrupt(cvec, cap_task_, lvl);
				MTUX::TIER = (1 << static_cast<uint8_t>(ch)) | MTUX::TIER.TCIEV.b();
			}

			tt_.cap_.clear();

			// 各チャネルに相当するジャネラルレジスタ
			tt_.tgr_adr_ = MTUX::TGRA.address() + static_cast<uint32_t>(ch) * 2;
			wr16_(tt_.tgr_adr_, 0x0000);

			MTUX::TCNT = 0;
			MTUX::enable();

			return true;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  位相係数モード開始（モード１）@n
					・MTCLKA, MTCLKB に入力された位相の異なる信号でカウンター更新
			@param[in]	ch		出力チャネル
			@param[in]	level	割り込みレベル
			@return 成功なら「true」
		*/
		//-----------------------------------------------------------------//
		bool start_count_phase(typename MTUX::channel ch, uint8_t level = 0) noexcept
		{
			// 工事中
			if(peripheral::MTU1 == MTUX::PERIPHERAL || peripheral::MTU2 == MTUX::PERIPHERAL) {
			} else {
				return false;
			}

			power_mgr::turn(MTUX::PERIPHERAL);
#if 0
			bool pena = (ot != OUTPUT_TYPE::NONE);
			port_map::turn(MTUX::get_peripheral(), static_cast<port_map::channel>(ch), pena);

			set_output_type_(ch, ot, match);

			set_TCR_(ch, dv);
			MTUX::TMDR1.MD = 0b0100;  // 位相係数 mode 1

			// 各チャネルに相当するジャネラルレジスタ
			tt_.tgr_adr_ = MTUX::TGRA.address() + static_cast<uint32_t>(ch) * 2;
			wr16_(tt_.tgr_adr_, match - 1);
#endif
			MTUX::TCNT = 0;
			MTUX::enable();

			return true;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  停止
		*/
		//-----------------------------------------------------------------//
		void stop() noexcept
		{
//			MTUX::TIOR.set(ch, 0x00);
			MTUX::TIER = 0x00;
			MTUX::enable(false);
//			icu_mgr::set_level(MTUX::get_vec(MTUX::interrupt::OVF), 0);
//			port_map::turn(MTUX::get_peripheral(), static_cast<port_map::channel>(ch), false);
			if(MTU::TSTRA() == 0 && MTU::TSTRB() == 0 && MTU::TSTR() == 0) {			
				power_mgr::turn(MTUX::get_peripheral(), false);
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  割り込みベクターの取得
			@return 割り込みベクター
		*/
		//-----------------------------------------------------------------//
		ICU::VECTOR get_intr_vec() const noexcept { return intr_vec_; }


		//-----------------------------------------------------------------//
		/*!
			@brief  キャプチャ構造体の参照（RO）
			@return	キャプチャ構造体
		*/
		//-----------------------------------------------------------------//
		static const capture_t& get_capture() noexcept { return tt_.cap_; }


		//-----------------------------------------------------------------//
		/*!
			@brief  キャプチャ構造体の参照
			@return	キャプチャ構造体
		*/
		//-----------------------------------------------------------------//
		static capture_t& at_capture() noexcept { return tt_.cap_; }


		//-----------------------------------------------------------------//
		/*!
			@brief  MAIN tick の取得
			@return	MAIN tick
		*/
		//-----------------------------------------------------------------//
		static uint32_t get_main_tick() noexcept { return tt_.main_tick_; }


		//-----------------------------------------------------------------//
		/*!
			@brief  MTASK クラスの参照
			@return MTASK クラス
		*/
		//-----------------------------------------------------------------//
		static MTASK& at_main_task() noexcept { return mtask_; }


		//-----------------------------------------------------------------//
		/*!
			@brief  OTASK クラスの参照
			@return OTASK クラス
		*/
		//-----------------------------------------------------------------//
		static OTASK& at_ovfl_task() noexcept { return otask_; }
	};

	template <class MTUX, class MTASK, class OTASK>
		typename mtu_io<MTUX, MTASK, OTASK>::task_t mtu_io<MTUX, MTASK, OTASK>::tt_;
	template <class MTUX, class MTASK, class OTASK> MTASK mtu_io<MTUX, MTASK, OTASK>::mtask_;
	template <class MTUX, class MTASK, class OTASK> OTASK mtu_io<MTUX, MTASK, OTASK>::otask_;
}
