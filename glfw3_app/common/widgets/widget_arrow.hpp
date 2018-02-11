#pragma once
//=====================================================================//
/*!	@file
	@brief	GUI widget_arrow クラス @n
			※矢印ボタンクラス
    @author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2018 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/glfw_app/blob/master/LICENSE
*/
//=====================================================================//
#include "widgets/widget_director.hpp"
#include "widgets/widget_utils.hpp"

namespace gui {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	GUI widget_arrow クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	struct widget_arrow : public widget {

		typedef widget_arrow value_type;

		typedef std::function< void(uint32_t) > select_func_type;

		typedef std::function< void(int32_t) > level_func_type;

		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		/*!
			@brief	widget_arrow 方向型
		*/
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		enum class direction {
			none,	///< 無し
			up,		///< 上
			down,	///< 下
			left,	///< 左
			right,	///< 右
		};


		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		/*!
			@brief	widget_arrow パラメーター
		*/
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		struct param {
			plate_param		plate_param_;	///< 平面描画パラメーター
			color_param		color_param_;	///< 頂点カラーで変調する場合のパラメーター
			const img::i_img*	image_;		///< ボタンに画像を使う場合
			gl::mobj::handle	handle_;	///< ボタンにモーションオブジェクトを使う場合
			uint32_t			id_;		///< セレクト ID （押された回数に相当）
			int32_t				level_;		///< レベル
			int32_t				min_;		///< 最小値
			int32_t				max_;		///< 最大値
			uint32_t			delay_;		///< カウントアップ・ディレイ（フレーム単位）
			widget_arrow*		master_;	///< カウンター・マスター

			select_func_type	select_func_;	///< セレクト関数
			level_func_type		level_func_;	///< レベル関数

			direction			dir_;

			param(direction dir = direction::none) :
				plate_param_(), color_param_(widget_director::default_button_color_),
				image_(nullptr), handle_(0), id_(0),
				level_(0), min_(0), max_(100), delay_(16), master_(nullptr),
				select_func_(nullptr), level_func_(nullptr), dir_(dir)
				{ }
		};

	private:
		widget_director&	wd_;

		param				param_;

		gl::mobj::handle	objh_;

		uint32_t			id_;
		int32_t				level_;
		uint32_t			count_;

	void update_level_()
	{
		if(param_.master_ == nullptr) {
			++param_.level_;
			if(param_.level_ > param_.max_) {
				param_.level_ = param_.max_;
			}
		} else {
			int32_t& level = param_.master_->at_local_param().level_;
			--level;
			if(level < param_.master_->get_local_param().min_) {
				level = param_.master_->get_local_param().min_;
			}
		}
	}

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief	コンストラクター
		*/
		//-----------------------------------------------------------------//
		widget_arrow(widget_director& wd, const widget::param& bp, const param& p) :
			widget(bp), wd_(wd), param_(p), objh_(0), id_(0), level_(0), count_(0) { }


		//-----------------------------------------------------------------//
		/*!
			@brief	デストラクター
		*/
		//-----------------------------------------------------------------//
		virtual ~widget_arrow() { }


		//-----------------------------------------------------------------//
		/*!
			@brief	型を取得
		*/
		//-----------------------------------------------------------------//
		type_id type() const override { return get_type_id<value_type>(); }


		//-----------------------------------------------------------------//
		/*!
			@brief	widget 型の基本名称を取得
			@return widget 型の基本名称
		*/
		//-----------------------------------------------------------------//
		const char* type_name() const override { return "arrow"; }


		//-----------------------------------------------------------------//
		/*!
			@brief	ハイブリッド・ウィジェットのサイン
			@return ハイブリッド・ウィジェットの場合「true」を返す。
		*/
		//-----------------------------------------------------------------//
		bool hybrid() const override { return false; }


		//-----------------------------------------------------------------//
		/*!
			@brief	個別パラメーターへの取得(ro)
			@return 個別パラメーター
		*/
		//-----------------------------------------------------------------//
		const param& get_local_param() const { return param_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	個別パラメーターへの取得
			@return 個別パラメーター
		*/
		//-----------------------------------------------------------------//
		param& at_local_param() { return param_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	初期化
		*/
		//-----------------------------------------------------------------//
		void initialize() override
		{
			// ボタンは標準的に固定、サイズ固定、選択時拡大
			at_param().state_.set(widget::state::SERVICE);
			at_param().state_.set(widget::state::POSITION_LOCK);
			at_param().state_.set(widget::state::SIZE_LOCK);
			at_param().state_.set(widget::state::MOVE_STALL);
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
				switch(param_.dir_) {
				case direction::up:
					objh_ = wd_.get_share_image().up_box_;
					break;
				case direction::down:
					objh_ = wd_.get_share_image().down_box_;
					break;
				case direction::left:
					objh_ = wd_.get_share_image().left_box_;
					break;
				case direction::right:
					objh_ = wd_.get_share_image().right_box_;
					break;
				default:
					break;
				}
				if(objh_ != 0) {
					at_rect().size = wd_.at_mobj().get_size(objh_);
				}
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	アップデート
		*/
		//-----------------------------------------------------------------//
		void update() override
		{
			if(!get_state(widget::state::ENABLE)) {
				return;
			}

			if(get_select()) {
				++count_;
				if(count_ >= param_.delay_) {
					count_ = 0;
					update_level_();
				}
			} else {
				count_ = 0;
			}
			if(get_selected()) {
				++param_.id_;
				update_level_();
				count_ = 0;
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	サービス
		*/
		//-----------------------------------------------------------------//
		void service() override
		{
			if(!get_state(state::ENABLE)) {
				return;
			}

			if(level_ != param_.level_) {
				if(param_.level_func_ != nullptr) {
					param_.level_func_(param_.level_);
				}
				level_ = param_.level_;
			}
			if(id_ != param_.id_) {
				if(param_.select_func_ != nullptr) {
					param_.select_func_(param_.id_);
				}
				id_ = param_.id_;
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	レンダリング
		*/
		//-----------------------------------------------------------------//
		void render() override
		{
			if(objh_ == 0) return;

			if(param_.plate_param_.resizeble_) {
				wd_.at_mobj().resize(objh_, get_param().rect_.size);
			}

			vtx::spos pos(0);
			wd_.at_mobj().draw(objh_, gl::mobj::attribute::normal, pos);
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	状態のセーブ
			@param[in]	pre	プリファレンス参照
			@return エラーが無い場合「true」
		*/
		//-----------------------------------------------------------------//
		bool save(sys::preference& pre) override {
			return true;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	状態のロード
			@param[in]	pre	プリファレンス参照
			@return エラーが無い場合「true」
		*/
		//-----------------------------------------------------------------//
		bool load(const sys::preference& pre) override {
			return true;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	レベルを取得
			@return レベル
		*/
		//-----------------------------------------------------------------//
		int32_t get_level() const { return param_.level_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	レベルを設定
			@param[in]	level	レベル
		*/
		//-----------------------------------------------------------------//
		void set_level(int32_t level) {
			param_.level_ = level;
			level_ = level;
			count_ = 0;
		}
	};
}
