#pragma once
//=========================================================================//
/*!	@file
	@brief	GUI widget スピン・ボックス・クラス
    @author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2017, 2023 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/glfw_app/blob/master/LICENSE
*/
//=========================================================================//
#include "widgets/widget_director.hpp"
#include "widgets/widget_utils.hpp"

namespace gui {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	GUI widget_spinbox クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	struct widget_spinbox : public widget {

		typedef widget_spinbox value_type;

		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		/*!
			@brief	widget_spinbox ステート型
		*/
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		enum class state {
			none,	///< 何もしない
			initial,///< 初期化
			inc,	///< 進む
			select,	///< 選択
			dec,	///< 戻る
		};

		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		/*!
			@brief	選択関数型 @n
					数字を表示する場合、「newpos」の値を文字列に変換して戻す。
					Ex: @n
					  (boost::format("%d") % newpos).str();
			@param[in]	st		ステート
			@param[in]	before	整数、変更前の値
			@param[in]	newpos	整数、変更後の値
			@return 表示する文字列を返す
		*/
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		typedef std::function< std::string (state st, int before, int newpos) > select_func_type;


		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		/*!
			@brief	widget_spinbox パラメーター
		*/
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		struct param {
			plate_param		plate_param_;
			color_param		color_param_;	///< 頂点カラーで変調する場合のパラメーター
			text_param		text_param_;	///< テキスト描画のパラメータ
			const img::i_img*	image_;		///< 画像を使う場合
			gl::mobj::handle	handle_;	///< モーションオブジェクトを使う場合
			uint32_t			id_;		///< セレクト ID （押された回数）

			select_func_type	select_func_;	///< 選択関数

			int					min_pos_;	///< 最低位置
			int					sel_pos_;	///< 選択位置
			int					max_pos_;	///< 最大位置
			int					page_div_;	///< ページ移動分割数
			int					page_step_;	///< ページ移動数(page_div_ が０の場合）

			bool				scroll_ctrl_;	///< スクロール・コントロール（マウスのダイアル）

			bool				accel_btn_;			///< ボタンのアクセル・コントロール
			uint16_t			accel_btn_delay_;	///< アクセルが利くまでの遅延
			uint16_t			accel_btn_inter_;	///< アクセル時のインターバル（速度）
			bool				accel_key_;			///< キーのアクセルコントロール
			uint16_t			accel_key_delay_;	///< アクセルが利くまでの遅延
			uint16_t			accel_key_inter_;	///< アクセル時のインターバル（速度）

			param(int min = 0, int sel = 0, int max = 0) noexcept :
				plate_param_(), color_param_(widget_director::default_spinbox_color_),
				text_param_("", img::rgba8(255, 255), img::rgba8(0, 255)),
				image_(0), handle_(0), id_(0),
				select_func_(),
				min_pos_(min), sel_pos_(sel), max_pos_(max),
				page_div_(0), page_step_((max - min) / 10),
				scroll_ctrl_(true),
				accel_btn_(true), accel_btn_delay_(30), accel_btn_inter_(5),
				accel_key_(true), accel_key_delay_(30), accel_key_inter_(5)
				{ }
		};

	private:
		widget_director&	wd_;

		param				param_;

		gl::mobj::handle	objh_;
		gl::mobj::handle	up_objh_;
		gl::mobj::handle	dn_objh_;

		bool		initial_;

		uint16_t	delay_btn_cnt_;
		uint16_t	delay_key_cnt_;

		int			sel_pos_;

		state		state_;

		state get_button_state_(int& d) const noexcept
		{
			state st = state::none;
			auto x = get_param().in_point_.x;
			auto xs = get_rect().size.x;
			if(x < (xs / 3)) {  // inc
				d = 1;
				st = state::inc;
			} else if(x >= (xs * 2 / 3)) {  // dec
				d = -1;
				st = state::dec;
			} else {
				st = state::select;
			}
			return st;
		}


		bool service_key_accel_() noexcept
		{
			if(param_.accel_key_) {
				++delay_key_cnt_;
				if(delay_key_cnt_ >= param_.accel_key_delay_) {
					delay_key_cnt_ -= param_.accel_key_inter_;
					return true;
				}
			}
			return false;
		}


		void setup_title_() noexcept
		{
			std::string tmp;
			if(param_.select_func_ != nullptr) {
				tmp = param_.select_func_(state_, sel_pos_, param_.sel_pos_);
			} else {
				tmp = (boost::format("%d") % param_.sel_pos_).str();
			}
			param_.text_param_.set_text(tmp);
		}

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief	コンストラクター
		*/
		//-----------------------------------------------------------------//
		widget_spinbox(widget_director& wd, const widget::param& bp, const param& p) noexcept :
			widget(bp), wd_(wd), param_(p), objh_(0), up_objh_(0), dn_objh_(0),
		    initial_(false), delay_btn_cnt_(0), delay_key_cnt_(0), sel_pos_(0),
			state_(state::initial)
		{ }


		//-----------------------------------------------------------------//
		/*!
			@brief	デストラクター
		*/
		//-----------------------------------------------------------------//
		virtual ~widget_spinbox() { }


		//-----------------------------------------------------------------//
		/*!
			@brief	型を取得
		*/
		//-----------------------------------------------------------------//
		type_id type() const noexcept override { return get_type_id<value_type>(); }


		//-----------------------------------------------------------------//
		/*!
			@brief	widget 型の基本名称を取得
			@return widget 型の基本名称
		*/
		//-----------------------------------------------------------------//
		const char* type_name() const noexcept override { return "spinbox"; }


		//-----------------------------------------------------------------//
		/*!
			@brief	ハイブリッド・ウィジェットのサイン
			@return ハイブリッド・ウィジェットの場合「true」を返す。
		*/
		//-----------------------------------------------------------------//
		bool hybrid() const noexcept override { return true; }


		//-----------------------------------------------------------------//
		/*!
			@brief	個別パラメーターの取得
			@return 個別パラメーター
		*/
		//-----------------------------------------------------------------//
		const param& get_local_param() const noexcept { return param_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	個別パラメーターの参照
			@return 個別パラメーター
		*/
		//-----------------------------------------------------------------//
		param& at_local_param() noexcept { return param_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	選択テキストの取得
			@return 選択テキスト
		*/
		//-----------------------------------------------------------------//
		std::string get_select_text() const noexcept { return param_.text_param_.get_text(); }


		//-----------------------------------------------------------------//
		/*!
			@brief	数値の取得
			@return 数値
		*/
		//-----------------------------------------------------------------//
		int get_select_pos() const noexcept { return param_.sel_pos_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	選択位置の設定
			@param[in]	pos		設定位置
		*/
		//-----------------------------------------------------------------//
		void set_select_pos(int pos) noexcept
		{
			if(param_.min_pos_ <= pos && pos <= param_.max_pos_) { 
				param_.sel_pos_ = pos;
			}
			initial_ = false;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	最小値の取得
			@return 数値
		*/
		//-----------------------------------------------------------------//
		int get_select_min() const noexcept { return param_.min_pos_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	選択位置の最小値を設定
			@param[in]	pos	設定位置
		*/
		//-----------------------------------------------------------------//
		void set_select_min(int pos) noexcept
		{
			param_.min_pos_ = pos;
			if(param_.sel_pos_ < pos) {
				param_.sel_pos_ = pos;
			}
			initial_ = false;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	最大値の取得
			@return 数値
		*/
		//-----------------------------------------------------------------//
		int get_select_max() const noexcept { return param_.max_pos_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	選択位置の最大値を設定
			@param[in]	pos	設定位置
		*/
		//-----------------------------------------------------------------//
		void set_select_max(int pos) noexcept
		{
			param_.max_pos_ = pos;
			if(param_.sel_pos_ > pos) {
				param_.sel_pos_ = pos;
			}
			initial_ = false;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	実行
			@param[in]	st	ステート（標準では「選択」値の変化はしない）
		*/
		//-----------------------------------------------------------------//
		void exec(state st = state::select) noexcept
		{
			++param_.id_;
			switch(st) {
			case state::inc:
				++param_.sel_pos_;
				if(param_.sel_pos_ > param_.max_pos_) param_.sel_pos_ = param_.max_pos_;
				break;
			case state::dec:
				param_.sel_pos_--;
				if(param_.sel_pos_ < param_.min_pos_) param_.sel_pos_ = param_.min_pos_;
				break;
			default:
				break;
			}
			state_ = st;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	初期化
		*/
		//-----------------------------------------------------------------//
		void initialize() noexcept override
		{
			// ボタンは標準的に固定、サイズ固定、選択時拡大
			set_state(widget::state::SERVICE);
			set_state(widget::state::POSITION_LOCK);
			set_state(widget::state::SIZE_LOCK);
			set_state(widget::state::MOVE_STALL);
			at_param().action_.set(widget::action::SELECT_SCALE);

			using namespace img;

			if(param_.handle_) {
				at_rect().size = wd_.at_mobj().get_size(param_.handle_);
				objh_ = param_.handle_;
			} else if(param_.image_) {
				paint pa;
				const vtx::spos& size = get_rect().size;
				create_image_base(param_.image_, size, pa);
				at_rect().size = pa.get_size();
				objh_ = wd_.at_mobj().install(&pa);
			} else {
				vtx::spos size;
				if(param_.plate_param_.resizeble_) {
					vtx::spos rsz = param_.plate_param_.grid_ * 3;
					if(get_param().rect_.size.x >= rsz.x) size.x = rsz.x;
					else size.x = get_param().rect_.size.x;
					if(get_param().rect_.size.y >= rsz.y) size.y = rsz.y;
					else size.y = get_param().rect_.size.y;
				} else {
					size = get_param().rect_.size;
				}
				share_t t;
				t.size_ = size;
				t.color_param_ = param_.color_param_;
				t.plate_param_ = param_.plate_param_;
				objh_ = wd_.share_add(t);
			}

			sel_pos_ = param_.sel_pos_;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	アップデート
		*/
		//-----------------------------------------------------------------//
		void update() noexcept override
		{
			if(!initial_) {
				setup_title_();
				initial_ = true;
			}

			state st = state::none;
			int d = 0;
			if(get_selected()) {
				st = get_button_state_(d);
				delay_btn_cnt_ = 0;
			} else if(get_focus() && param_.scroll_ctrl_) {
				if(get_select()) {
					st = get_button_state_(d);
					if(st == state::inc || st == state::dec) {
						++delay_btn_cnt_;
						if(delay_btn_cnt_ >= param_.accel_btn_delay_) {
							delay_btn_cnt_ -= param_.accel_btn_inter_;
						} else {
							st = state::none;
						}
					}
				} else {
					delay_btn_cnt_ = 0;
					const vtx::spos& scr = wd_.get_scroll();
					if(scr.y != 0) {
						d = -scr.y;
						if(d > 0) st = state::inc;
						else if(d < 0) st = state::dec;
					}
				}
			}

			if(get_focus()) {
				gl::core& core = gl::core::get_instance();
				const gl::device& dev = core.get_device();
				int step = param_.page_step_;
				if(param_.page_div_ != 0) {
					step = (param_.max_pos_ - param_.min_pos_) / param_.page_div_;
				}
				bool trg = false;
				bool lvl = false;
				if(dev.get_level(gl::device::key::PAGE_UP)) {
					lvl = true;
					d =  step;
					st = state::inc;
					if(dev.get_positive(gl::device::key::PAGE_UP)) trg = true;
				} else if(dev.get_level(gl::device::key::PAGE_DOWN)) {
					lvl = true;
					d = -step;
					st = state::dec;
					if(dev.get_positive(gl::device::key::PAGE_DOWN)) trg = true;
				} else if(dev.get_level(gl::device::key::UP)) {
					lvl = true;
					d = step > 0 ?  1 : 0;
					st = state::inc;
					if(dev.get_positive(gl::device::key::UP)) trg = true;
				} else if(dev.get_level(gl::device::key::DOWN)) {
					lvl = true;
					d = step > 0 ? -1 : 0;
					st = state::dec;
					if(dev.get_positive(gl::device::key::DOWN)) trg = true;
				}
				if(lvl) {
					if(!trg && !service_key_accel_()) {
						st = state::none;
					}
				} else {
					delay_key_cnt_ = 0;
				}
			}

			if(st != state::none || sel_pos_ != param_.sel_pos_) {
				++param_.id_;
				if(d > 0) {
					param_.sel_pos_ += d;
					if(param_.sel_pos_ > param_.max_pos_) param_.sel_pos_ = param_.max_pos_;
					st = state::inc;
				} else if(d < 0) {
					param_.sel_pos_ += d;
					if(param_.sel_pos_ < param_.min_pos_) param_.sel_pos_ = param_.min_pos_;
					st = state::dec;
				}
				state_ = st;
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	サービス
		*/
		//-----------------------------------------------------------------//
		void service() noexcept override
		{
			if(!get_enable()) return;

			if(state_ != state::none) {
				setup_title_();
				state_ = state::none;
			}
			sel_pos_ = param_.sel_pos_;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	レンダリング
		*/
		//-----------------------------------------------------------------//
		void render() noexcept override
		{
			if(objh_ == 0) return;

			using namespace gl;

			core& core = core::get_instance();
			const vtx::spos& siz = core.get_rect().size;

			if(param_.plate_param_.resizeble_) {
				wd_.at_mobj().resize(objh_, get_param().rect_.size);
			}

			render_text(wd_, objh_, get_param(), param_.text_param_, param_.plate_param_);

			// チップの描画
			wd_.at_mobj().setup_matrix(siz.x, siz.y);
			wd_.set_TSC();

			gl::mobj::handle uph = wd_.get_share_image().up_box_;
			gl::mobj::handle dnh = wd_.get_share_image().down_box_;
			const vtx::spos& bs = wd_.at_mobj().get_size(uph);
			const vtx::spos& size = get_rect().size;
			short wf = param_.plate_param_.frame_width_;
			short space = 2;
			vtx::spos pos(size.x - bs.x - wf - space, (size.y - bs.y) / 2);
			wd_.at_mobj().draw(dnh, gl::mobj::attribute::normal, pos);
			pos.x = wf + space;
			wd_.at_mobj().draw(uph, gl::mobj::attribute::normal, pos);
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	状態のセーブ
			@param[in]	pre	プリファレンス参照
			@return エラーが無い場合「true」
		*/
		//-----------------------------------------------------------------//
		bool save(sys::preference& pre) noexcept override
		{
			std::string path;
			path += '/';
			path += wd_.create_widget_name(this);

			int err = 0;
			if(!pre.put_integer(path + "/selector", param_.sel_pos_)) ++err;
			return err == 0;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	状態のロード
			@param[in]	pre	プリファレンス参照
			@return エラーが無い場合「true」
		*/
		//-----------------------------------------------------------------//
		bool load(const sys::preference& pre) noexcept override
		{
			std::string path;
			path += '/';
			path += wd_.create_widget_name(this);

			int err = 0;
			if(!pre.get_integer(path + "/selector", param_.sel_pos_)) ++err;
			initial_ = false;
			return err == 0;
		}
	};
}
