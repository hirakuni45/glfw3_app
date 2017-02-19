#pragma once
//=====================================================================//
/*!	@file
	@brief	GUI widget_label クラス @n
			Copyright 2017 Kunihito Hiramatsu
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include "widgets/widget_director.hpp"
#include "widgets/widget_utils.hpp"
#include "core/glcore.hpp"
#include <utility>

namespace gui {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	GUI widget_label クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	struct widget_label : public widget {

		typedef widget_label value_type;

		typedef std::function<void (const std::string&)> select_func_type;

		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		/*!
			@brief	Widget label パラメーター
		*/
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		struct param {
			plate_param	plate_param_;			///< プレート・パラメーター
			color_param	color_param_;			///< カラーパラメーター
			text_param	text_param_;			///< テキスト描画のパラメーター
			color_param	color_param_select_;	///< 選択時のカラーパラメーター

			utils::lstring	before_text_;	///< 以前のテキスト

			bool		read_only_;		///< 読み出し専用の場合
			bool		text_in_;		///< テキスト入力状態
			uint32_t	text_in_pos_;	///< テキスト入力位置
			uint32_t	text_in_limit_;	///< テキスト入力最大数

			shift_param	shift_param_;	///< 文字列スクロールパラメーター

			bool		menu_enable_;	///< メニュー許可

			select_func_type	select_func_;

			//-------------------------------------------------------------//
			/*!
				@brief	param コンストラクター
				@param[in]	text	ラベル・テキスト
				@param[in]	ro		テキスト変更の場合「false」
			*/
			//-------------------------------------------------------------//
			param(const std::string& text = "", bool ro = true) :
				plate_param_(),
				color_param_(widget_director::default_label_color_),
				text_param_(text, img::rgba8(255, 255), img::rgba8(0, 255),
				vtx::placement(vtx::placement::holizontal::LEFT,
				vtx::placement::vertical::CENTER)),
				color_param_select_(widget_director::default_label_color_select_),
				read_only_(ro), text_in_(false), text_in_pos_(0), text_in_limit_(0),
				shift_param_(),
				menu_enable_(false),
				select_func_(nullptr)
				{ }
		};

	private:
		widget_director&	wd_;

		param				param_;

		uint32_t			interval_;

		bool				focus_;

		gl::mobj::handle	objh_;
		gl::mobj::handle	select_objh_;

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief	コンストラクター
		*/
		//-----------------------------------------------------------------//
		widget_label(widget_director& wd, const widget::param& bp, const param& p) :
			widget(bp), wd_(wd), param_(p), interval_(0), focus_(false),
			objh_(0), select_objh_(0) { }


		//-----------------------------------------------------------------//
		/*!
			@brief	デストラクター
		*/
		//-----------------------------------------------------------------//
		virtual ~widget_label() { }


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
		const char* type_name() const override { return "label"; }


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
			@brief	テキストを設定
			@param[in]	text	テキスト
		*/
		//-----------------------------------------------------------------//
		void set_text(const std::string& text) {
			param_.text_param_.set_text(text);
			param_.text_in_pos_ = param_.text_param_.text_.size();
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	テキストを取得
			@return	テキスト
		*/
		//-----------------------------------------------------------------//
		std::string get_text() const {
			std::string s;
			utils::utf32_to_utf8(param_.text_param_.text_, s);
			return s;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	エイリアス・テキストを設定
			@param[in]	alias	テキスト
		*/
		//-----------------------------------------------------------------//
		void set_alias(const std::string& alias) {
			param_.text_param_.set_alias(alias);
			param_.text_param_.alias_enable_ = true;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	エイリアス・テキストを取得
			@return エイリアス・テキスト
		*/
		//-----------------------------------------------------------------//
		const std::string get_alias() const {
			std::string s;
			utils::utf32_to_utf8(param_.text_param_.alias_, s);
			return s;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	エイリアスの許可、不許可
			@param[in]	ena		不許可にする場合「false」
		*/
		//-----------------------------------------------------------------//
		void enable_alias(bool ena = true) {
			param_.text_param_.alias_enable_ = ena;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	初期化
		*/
		//-----------------------------------------------------------------//
		void initialize() override {
			// 標準的に固定
			at_param().state_.set(widget::state::POSITION_LOCK);
			at_param().state_.set(widget::state::SIZE_LOCK);
			at_param().state_.set(widget::state::SERVICE);
			at_param().state_.set(widget::state::MOVE_STALL);

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

			if(!param_.read_only_) {
				param_.text_in_pos_ = param_.text_param_.text_.size();
			}

			share_t t;
			t.size_ = size;
			t.color_param_ = param_.color_param_;
			t.plate_param_ = param_.plate_param_;
			objh_ = wd_.share_add(t);

			t.color_param_ = param_.color_param_select_;
			select_objh_ = wd_.share_add(t);
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	アップデート
		*/
		//-----------------------------------------------------------------//
		void update() override {
			if(param_.text_in_) return;

			// テキスト入力位置を調整
			if(param_.text_in_pos_ > param_.text_param_.text_.size()) {
				param_.text_in_pos_ = param_.text_param_.text_.size();
			}

			param_.shift_param_.size_ = get_rect().size.x - param_.plate_param_.frame_width_ * 2;
			shift_text_update(get_param(), param_.text_param_, param_.shift_param_);
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	サービス
		*/
		//-----------------------------------------------------------------//
		void service() override {
			if(get_select_in()) {
				if(!param_.read_only_) {
					param_.text_in_ = true;
					param_.before_text_ = param_.text_param_.text_;
				}
			}
			if(wd_.get_focus_widget() == this || wd_.get_focus_widget() == wd_.root_widget(this)) {
				focus_ = true;
			} else {
				param_.text_param_.cursor_ = -1;
				focus_ = false;
			}
			if(focus_) {
				if(!param_.read_only_ && param_.text_in_) {
					bool text_in = param_.text_in_;
					const std::string& ins = wd_.get_keyboard().input();
					for(char ch : ins) {
						if(param_.text_in_limit_ > 0 && param_.text_in_limit_ <= param_.text_in_pos_) {
							param_.text_in_ = false;
							continue;
						}
						if(ch == sys::keyboard::ctrl::DELETE) {
							if(param_.text_in_pos_ < param_.text_param_.text_.size()) {
								param_.text_param_.text_.erase(param_.text_in_pos_, 1);
							}
						} else if(ch < 0x20) {
							if(ch == sys::keyboard::ctrl::BS) {
								if(param_.text_in_pos_) {
									--param_.text_in_pos_;
									param_.text_param_.text_.erase(param_.text_in_pos_, 1);
								}
							} else if(ch == sys::keyboard::ctrl::CR) {
								if(param_.text_in_pos_ < param_.text_param_.text_.size()) {
									param_.text_param_.text_.erase(param_.text_in_pos_);
								}
								param_.text_param_.offset_.x = 0;
								param_.text_in_ = false;
							} else if(ch == sys::keyboard::ctrl::ESC) {
								param_.text_param_.offset_.x = 0;
								param_.text_in_ = false;
							} else if(ch == sys::keyboard::ctrl::RIGHT) {
								if(param_.text_in_pos_ < param_.text_param_.text_.size()) {
									++param_.text_in_pos_;
								}
							} else if(ch == sys::keyboard::ctrl::LEFT) {
								if(param_.text_in_pos_) {
									--param_.text_in_pos_;
								}
							}
						} else {
							if(param_.text_param_.text_.size() <= param_.text_in_pos_) {
								param_.text_param_.text_ += ch;
							} else {
								param_.text_param_.text_.insert(param_.text_in_pos_, 1, ch);
							}
							++param_.text_in_pos_;
						}
					}
					// 入力完了、ファンクション呼び出し
					if(text_in && !param_.text_in_) {
						if(param_.select_func_ != nullptr) param_.select_func_(param_.text_param_.get_text());
					}
				}

				// テキスト幅が、収容範囲を超える場合
				if(param_.text_in_) {
					gl::core& core = gl::core::get_instance();
					gl::fonts& fonts = core.at_fonts();

					if(!param_.text_param_.font_.empty()) {
						fonts.push_font_face();
						fonts.set_font_type(param_.text_param_.font_);
					}
					fonts.enable_proportional(param_.text_param_.proportional_);
					utils::lstring ls = param_.text_param_.text_;
					if(param_.text_in_pos_ < ls.size()) {
						ls.erase(param_.text_in_pos_);
					}
					ls += ' ';
					short fw = fonts.get_width(ls);
					if(!param_.text_param_.font_.empty()) {
						fonts.pop_font_face();
					}
					short w = get_rect().size.x - param_.plate_param_.frame_width_ * 2;
					if(fw >= w) {
						param_.text_param_.offset_.x = -(fw - w);
					} else {
						param_.text_param_.offset_.x = 0;
					}
				}
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	レンダリング
		*/
		//-----------------------------------------------------------------//
		void render() override {
			gl::mobj::handle h = objh_;
			if(get_select() || get_state(widget::state::SELECTED)) {
				h = select_objh_;
			}

			if(param_.plate_param_.resizeble_) {
				wd_.at_mobj().resize(h, get_param().rect_.size);
			}

			text_param tp = param_.text_param_;
			param_.text_param_.cursor_ = -1;
			if(param_.text_in_ && focus_) {
				if((interval_ % 40) < 20) {
					if(param_.text_param_.text_.size() <= param_.text_in_pos_) {
						param_.text_param_.cursor_ = param_.text_param_.text_.size();
						tp.text_ += ' ';
					} else {
						param_.text_param_.cursor_ = param_.text_in_pos_;
					}
				}
			}

			render_text(wd_, h, get_param(), tp, param_.plate_param_);
			++interval_;
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
			if(!pre.put_text(path + "/text", get_text())) ++err;
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

			std::string s;
			bool f = pre.get_text(path + "/text", s);
			if(f) {
				set_text(s);
			}
			return f;
		}
	};

	typedef std::vector<widget_label*>	widget_labels;
	typedef widget_labels::iterator		widget_labels_it;
	typedef widget_labels::const_iterator	widget_labels_cit;
}
