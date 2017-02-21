#pragma once
//=====================================================================//
/*!	@file
	@brief	GUI widget スピン・ボックス・クラス @n
			Copyright 2017 Kunihito Hiramatsu
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
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

		typedef std::function< void(const std::string& st, int pos) > select_func_type;

		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		/*!
			@brief	widget_spinbox パラメーター
		*/
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		struct param {
			plate_param		plate_param_;
			color_param		color_param_;	///< 頂点カラーで変調する場合のパラメーター
			text_param		text_param_;	///< テキスト描画のパラメータ
			const img::i_img*	image_;		///< 画像を使う場合
			gl::mobj::handle	handle_;	///< モーションオブジェクトを使う場合
			uint32_t			id_;		///< セレクト ID （押された回数）

			select_func_type	select_func_;	///< セレクト関数
			utils::strings		text_list_;		///< テキスト・リスト
			std::string			select_text_;	///< 選択テキスト
			int					select_pos_;	///< 選択位置

			param() :
				plate_param_(), color_param_(widget_director::default_spinbox_color_),
				text_param_("", img::rgba8(255, 255), img::rgba8(0, 255)),
				image_(0), handle_(0), id_(0),
				select_func_(nullptr),
				text_list_(),
				select_text_(), select_pos_(0)
				{ }
		};

	private:
		widget_director&	wd_;

		param				param_;

		gl::mobj::handle	objh_;
		gl::mobj::handle	up_objh_;
		gl::mobj::handle	dn_objh_;

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief	コンストラクター
		*/
		//-----------------------------------------------------------------//
		widget_spinbox(widget_director& wd, const widget::param& bp, const param& p) :
			widget(bp), wd_(wd), param_(p), objh_(0), up_objh_(0), dn_objh_(0) { }


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
		type_id type() const override { return get_type_id<value_type>(); }


		//-----------------------------------------------------------------//
		/*!
			@brief	widget 型の基本名称を取得
			@return widget 型の基本名称
		*/
		//-----------------------------------------------------------------//
		const char* type_name() const override { return "spinbox"; }


		//-----------------------------------------------------------------//
		/*!
			@brief	ハイブリッド・ウィジェットのサイン
			@return ハイブリッド・ウィジェットの場合「true」を返す。
		*/
		//-----------------------------------------------------------------//
		bool hybrid() const override { return true; }


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
			@brief	リストを設定
			@param[in]	ss	リスト
		*/
		//-----------------------------------------------------------------//
		void set_list(const utils::strings& ss) {
			param_.text_list_ = ss;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	リストを参照
			@return リスト
		*/
		//-----------------------------------------------------------------//
		utils::strings& at_list() { return param_.text_list_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	選択テキストの取得
		*/
		//-----------------------------------------------------------------//
		const std::string& get_select_text() const { return param_.select_text_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	選択位置の取得
		*/
		//-----------------------------------------------------------------//
		uint32_t get_select_pos() const { return param_.select_pos_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	初期化
		*/
		//-----------------------------------------------------------------//
		void initialize() override {
			// ボタンは標準的に固定、サイズ固定、選択時拡大
			at_param().state_.set(widget::state::SERVICE);
			at_param().state_.set(widget::state::POSITION_LOCK);
			at_param().state_.set(widget::state::SIZE_LOCK);
			at_param().state_.set(widget::state::MOVE_STALL);

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
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	アップデート
		*/
		//-----------------------------------------------------------------//
		void update() override {
			bool select = false;
			if(get_selected()) {
				++param_.id_;

				auto x = get_param().in_point_.x;
				auto xs = get_rect().size.x;
				if(param_.text_list_.size() > 0) {
					if(x < (xs / 3)) {  // up
						param_.select_pos_++;
						if(param_.select_pos_ >= param_.text_list_.size()) {
							param_.select_pos_ = param_.text_list_.size() - 1;
						}
					} else if(x >= (xs * 2 / 3)) {  // down
						param_.select_pos_--;
						if(param_.select_pos_ < 0) param_.select_pos_ = 0;
					}
				}
				select = true;
			}
			if(param_.text_list_.size() > 0) {
				param_.select_text_ = param_.text_list_[param_.select_pos_];
				param_.text_param_.set_text(param_.select_text_);
			}
			if(select && param_.select_func_ != nullptr) {
				param_.select_func_(param_.select_text_, param_.select_pos_);
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	サービス
		*/
		//-----------------------------------------------------------------//
		void service() override { }


		//-----------------------------------------------------------------//
		/*!
			@brief	レンダリング
		*/
		//-----------------------------------------------------------------//
		void render() override {
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
		bool save(sys::preference& pre) override {
			std::string path;
			path += '/';
			path += wd_.create_widget_name(this);

			int err = 0;
			if(!pre.put_integer(path + "/selector", param_.select_pos_)) ++err;
			return err == 0;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	状態のロード
			@param[in]	pre	プリファレンス参照
			@return エラーが無い場合「true」
		*/
		//-----------------------------------------------------------------//
		bool load(const sys::preference& pre) override {
			std::string path;
			path += '/';
			path += wd_.create_widget_name(this);

			int err = 0;
			if(!pre.get_integer(path + "/selector", param_.select_pos_)) ++err;
			return err == 0;
		}
	};

}
