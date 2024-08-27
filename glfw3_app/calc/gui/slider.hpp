#pragma once
//=====================================================================//
/*!	@file
	@brief	スライダー表示と制御
    @author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2019, 2022 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/RX/blob/master/LICENSE
*/
//=====================================================================//
#include <functional>
#include "gui/widget.hpp"

namespace gui {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	スライダー・クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	struct slider : public widget {

		typedef slider value_type;

		typedef std::function<void(float ratio)> SELECT_FUNC_TYPE;

	private:

		SELECT_FUNC_TYPE	select_func_;
		vtx::spos			touch_org_;
		float				ratio_org_;
		float				ratio_;

		int16_t				handle_size_;

		bool				read_only_;

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief	コンストラクター
			@param[in]	loc		ロケーション
			@param[in]	inr		初期レシオ
		*/
		//-----------------------------------------------------------------//
		slider(const vtx::srect& loc = vtx::srect(0), float inr = 0.0f) noexcept :
			widget(loc, nullptr), select_func_(),
			touch_org_(0), ratio_org_(inr), ratio_(inr), handle_size_(DEF_SLIDER_HANDLE_SIZE), read_only_(false)
		{
			if(loc.size.x <= 0) {
				at_location().size.x = handle_size_;
			}
			if(loc.size.y <= 0) {
				at_location().size.y = handle_size_;
			}
			insert_widget(this);
		}


		slider(const slider& th) = delete;
		slider& operator = (const slider& th) = delete;


		//-----------------------------------------------------------------//
		/*!
			@brief	デストラクタ
		*/
		//-----------------------------------------------------------------//
		virtual ~slider() { remove_widget(this); }


		//-----------------------------------------------------------------//
		/*!
			@brief	型整数を取得
			@return 型整数
		*/
		//-----------------------------------------------------------------//
		const char* get_name() const override { return "Slider"; }


		//-----------------------------------------------------------------//
		/*!
			@brief	ID を取得
			@return ID
		*/
		//-----------------------------------------------------------------//
		ID get_id() const override { return ID::SLIDER; }


		//-----------------------------------------------------------------//
		/*!
			@brief	初期化
		*/
		//-----------------------------------------------------------------//
		void init() override { }


		//-----------------------------------------------------------------//
		/*!
			@brief	タッチ判定を更新
			@param[in]	pos		判定位置
			@param[in]	num		タッチ数
		*/
		//-----------------------------------------------------------------//
		void update_touch(const vtx::spos& pos, uint16_t num) noexcept override
		{
			update_touch_slider(pos, num);

			if(read_only_) {

			} else if(get_touch_state().positive_) {
				touch_org_ = get_touch_state().position_;
				ratio_org_ = ratio_;
			} else if(get_touch_state().level_) {
				auto d = get_touch_state().position_ - touch_org_;
				float ref = 1.0f;
				float val = 0.0f;
				const auto& loc = get_location();
				if(loc.size.x > loc.size.y) {
					ref = static_cast<float>(loc.size.x - DEF_SLIDER_FRAME_WIDTH * 2 - loc.size.y);
					val = static_cast<float>(d.x);
				} else {
					ref = static_cast<float>(loc.size.y - DEF_SLIDER_FRAME_WIDTH * 2 - loc.size.x);
					val = static_cast<float>(d.y);
				}
				ratio_ = ratio_org_ + (val / ref);
				if(ratio_ < 0.0f) ratio_ = 0.0f;
				else if(ratio_ > 1.0f) ratio_ = 1.0f;
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	選択推移
		*/
		//-----------------------------------------------------------------//
		void exec_select() noexcept override
		{
 			if(get_touch_state().negative_) {
				if(select_func_) {
					select_func_(ratio_);
				}
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	許可・不許可
			@param[in]	ena		不許可の場合「false」
		*/
		//-----------------------------------------------------------------//
		void enable(bool ena = true) noexcept override
		{
			if(ena) {
				set_state(STATE::ENABLE);
			} else {
				set_state(STATE::DISABLE);
				reset_touch_state();
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	スライダー・レシオの取得（移動量を正規化した値 0.0 to 1.0）
			@return	スライダー・レシオ
		*/
		//-----------------------------------------------------------------//
		float get_ratio() const noexcept { return ratio_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	スライダー・レシオを設定（移動量を正規化した値 0.0 to 1.0）
			@param	ratio	スライダー・レシオ
		*/
		//-----------------------------------------------------------------//
		void set_ratio(float ratio) noexcept {
			auto tmp = ratio_;
			if(ratio >= 0.0f && ratio <= 1.0f) {
				ratio_ = ratio;
			}
			if(tmp != ratio_) {
				set_update();
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	リード・オンリー（表示のみ）の設定
			@param[in]	ena		リード・オンリーを無効にする場合「false」
		*/
		//-----------------------------------------------------------------//
		void enable_read_only(bool ena = true) noexcept { read_only_ = ena; }


		//-----------------------------------------------------------------//
		/*!
			@brief	セレクト関数への参照
			@return	セレクト関数
		*/
		//-----------------------------------------------------------------//
		SELECT_FUNC_TYPE& at_select_func() noexcept { return select_func_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	描画
			@param[in] rdr	描画インスタンス
		*/
		//-----------------------------------------------------------------//
		template<class RDR>
		void draw(RDR& rdr) noexcept
		{
			auto org = get_final_position();
			auto r = vtx::srect(org, get_location().size);
			rdr.set_fore_color(get_base_color());
			rdr.round_box(r, DEF_SLIDER_ROUND_RADIUS);
			uint8_t inten = 64;
			if(!read_only_ && get_touch_state().level_) {  // 0.75
				inten = 192;
			}
			graphics::share_color sc(0, 0, 0);
			sc.set_color(get_base_color().rgba8, inten);
			rdr.set_fore_color(sc);

			r.org  += DEF_SLIDER_FRAME_WIDTH;
			r.size -= DEF_SLIDER_FRAME_WIDTH * 2;
			rdr.round_box(r, DEF_SLIDER_ROUND_RADIUS - DEF_SLIDER_FRAME_WIDTH);

			const auto& size = get_location().size;
			auto cen = org;
			int16_t rad = 0;
			if(size.x > size.y) {
				rad = size.y / 2;
				--rad;
				cen.x += DEF_SLIDER_FRAME_WIDTH + rad;
				cen.y += rad;
				cen.x += (size.x - DEF_SLIDER_FRAME_WIDTH * 2 - size.y) * ratio_;
			} else {
				rad = size.x / 2;
				--rad;
				cen.y += DEF_SLIDER_FRAME_WIDTH + rad;
				cen.x += rad;
				cen.y += (size.y - DEF_SLIDER_FRAME_WIDTH * 2 - size.x) * ratio_;
			}
			rdr.set_fore_color(get_base_color());
			rdr.fill_circle(cen, rad);
		}
	};
}
