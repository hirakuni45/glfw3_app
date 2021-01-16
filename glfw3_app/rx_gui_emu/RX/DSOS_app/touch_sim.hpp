#pragma once
//=========================================================================//
/*!	@file
	@brief	Touch Simulator @n
			FT5202 Capacitive Touch Panel Controller simulator for Mouse
    @author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2020 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/RX/blob/master/LICENSE
*/
//=========================================================================//
#include <cstdint>
#include "common/vtx.hpp"
#include "common/format.hpp"

namespace chip {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  タッチ・シュミレーション・クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	class touch_sim {
	public:
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		/*!
			@brief  Touch Event Type
		*/
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		enum class EVENT : uint8_t {
			DOWN,		///< Touch Down
			UP,			///< Touch Up
			CONTACT,	///< Contact
			NONE
		};

		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		/*!
			@brief  Touch Struct
		*/
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		struct touch_t {
			EVENT		before;	///< Touch Event (before)
			EVENT		event;	///< Touch Event
			uint8_t		id;		///< Touch ID
			vtx::spos	pos;	///< Touch Position
			vtx::spos	org;	///< Touch Down Position
			vtx::spos	end;	///< Touch Up Position

			//-------------------------------------------------------------//
			/*!
				@brief	コンストラクター
			 */
			//-------------------------------------------------------------//
			touch_t() noexcept : before(EVENT::NONE), event(EVENT::NONE), id(0),
				pos(0), org(0), end(0) { }


			//-------------------------------------------------------------//
			/*!
				@brief	ドラッグされた距離の二乗を返す
				@return ドラッグされた距離の二乗
			 */
			//-------------------------------------------------------------//
			uint32_t length_sqr() const noexcept
			{
				auto d = end - org;
				return d.x * d.x + d.y * d.y;
			} 
		};

	private:

		touch_t		t_[2];
		uint8_t		touch_num_;
		bool		touch_;

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief	コンストラクター
			@param[in]	i2c	i2c 制御クラスを参照で渡す
		 */
		//-----------------------------------------------------------------//
		touch_sim() noexcept : t_{ }, touch_num_(0), touch_(false) { }

#if 0
		//-----------------------------------------------------------------//
		/*!
			@brief	デバイスのバージョンを取得
			@return デバイスのバージョン
		 */
		//-----------------------------------------------------------------//
		uint16_t get_version() const noexcept { return version_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	チップ・コードを取得
			@return チップ・コード
		 */
		//-----------------------------------------------------------------//
		uint8_t get_chip() const noexcept { return chip_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	デバイス・リセット
			@param[in]	RES	リセット端子
		 */
		//-----------------------------------------------------------------//
		template <class RES>
		static void reset() noexcept {
			RES::DIR = 1;
			RES::P = 0;
			utils::delay::milli_second(5);
			RES::P = 1;
			utils::delay::milli_second(300);
		}
#endif


		//-----------------------------------------------------------------//
		/*!
			@brief	シュミレーターが呼ぶ仮想設定 @n
					マウスの操作をタッチパネルの動作として注入
			@param[in]	pos		位置
			@param[in]	toush	タッチの状態
			@param[in]	idx		二点目
		 */
		//-----------------------------------------------------------------//
		void set_pos(const vtx::spos& pos, bool touch, uint32_t idx = 0)
		{
			t_[0].pos = pos;

			if(!touch_ && touch) {
				t_[0].before = t_[0].event;
				t_[0].event = EVENT::DOWN;
				touch_num_ = 1;
			} else if(touch_ && !touch) {
				t_[0].before = t_[0].event;
				t_[0].event = EVENT::UP;
				touch_num_ = 0;
			} else if(touch_ && touch) {
				t_[0].before = t_[0].event;
				t_[0].event = EVENT::CONTACT;
				touch_num_ = 1;
			} else if(!touch) {
				t_[0].before = t_[0].event;
				t_[0].event = EVENT::NONE;
				touch_num_ = 0;
			}
			touch_ = touch;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	開始
			@return 成功なら「true」
		 */
		//-----------------------------------------------------------------//
		bool start() noexcept
		{
			touch_num_ = 0;
			return true;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	アップデート
		 */
		//-----------------------------------------------------------------//
		void update() noexcept
		{
			for(uint8_t i = 0; i < 2; ++i) {
				if(t_[i].event == EVENT::DOWN) {
					t_[i].org = t_[i].pos;
				} else if(t_[i].event == EVENT::UP) {
					t_[i].end = t_[i].pos;
				}
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	タッチ数を取得
			@return タッチ数
		 */
		//-----------------------------------------------------------------//
		uint8_t get_touch_num() const noexcept { return touch_num_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	タッチ位置を取得
			@param[in]	idx	タッチ・インデックス（０～１）
			@return タッチ位置
		 */
		//-----------------------------------------------------------------//
		const touch_t& get_touch_pos(uint8_t idx) const noexcept { return t_[idx & 1]; }
	};
}
