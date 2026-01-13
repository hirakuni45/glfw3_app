#pragma once
//=========================================================================//
/*!	@file
	@brief	GUI Widget ファイラー（ヘッダー）@n
			ファイル選択を行う GUI モジュール
    @author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2017, 2026 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/glfw_app/blob/master/LICENSE
*/
//=========================================================================//
#include <vector>
#include <boost/unordered_map.hpp>
#include <boost/optional.hpp>
#include "utils/files.hpp"
#include "utils/drive_info.hpp"
#include "widgets/widget_director.hpp"
#include "widgets/widget_null.hpp"
#include "widgets/widget_label.hpp"
#include "widgets/widget_button.hpp"
#include "widgets/spring_damper.hpp"

namespace gui {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	GUI filer クラス（ファイル選択）
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	struct widget_filer : public widget {

		typedef widget_filer value_type;

		typedef std::function< void (const std::string&) > select_file_func_type;
		typedef std::function< void (void) > cancel_file_func_type;

		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		/*!
			@brief	widget_filer パラメーター
		*/
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		struct param {
			plate_param		plate_param_;
			color_param		color_param_;
			text_param		text_param_;
			shift_param		shift_param_;

			std::string		path_;		///< 初期ファイル・パス
			std::string		filter_;	///< 初期ファイル・フィルター

			short			path_height_;
			short			label_height_;

			bool			every_top_;		///< 有効なら、常に手前
			bool			acc_focus_;		///< アクセレーターキーによるフォーカス

			bool			new_file_;		///< 新規ファイル作成

			select_file_func_type	select_file_func_;	///< セレクト関数
			cancel_file_func_type	cancel_file_func_;	///< 選択キャンセル関数

			param(const std::string& path = "", const std::string& filter = "", bool new_file = false) :
				plate_param_(),
				color_param_(widget_director::default_frame_color_),
				text_param_(), shift_param_(),
				path_(path), filter_(filter),
				path_height_(32), label_height_(32),
				every_top_(true), acc_focus_(true),
				new_file_(new_file),
				select_file_func_(nullptr), cancel_file_func_(nullptr)
			{ }
		};

	private:

		static constexpr char NEW_FILE_TEXT[] = "New File...";
		static constexpr float DEF_GAIN   = 0.85f;
		static constexpr float SLIP_GAIN  = 0.5f;
		static constexpr float SPEED_GAIN = 0.95f;
		static constexpr float SPEED_MOVE = 38.0f;	/// 横スクロールの初期速度

		widget_director&	wd_;

		param				param_;

		gl::mobj::handle	objh_;

		utils::files		fsc_;
		std::string			fsc_path_;
		bool				fsc_wait_;
		utils::file_infos	file_infos_;
		utils::drive_info	drv_;

		widget_button*	info_;	///< インフォメーション切り替えボタン
		widget*			main_;	///< メイン・フレーム
		widget*			files_;	///< ファイル・フレーム

		// 情報の状態
		struct info_state {
			enum type {
				NONE,	///< 情報無し
				SIZE,	///< サイズ表示
				TIME,	///< 時間表示
				MODE,	///< モード表示
				ALIAS,	///< エイリアス表示
				limit_
			};
		};
		info_state::type	info_state_;

		struct widget_file {
			widget_null*	base;
			widget_label*	name;
			widget_label*	info;
			size_t			size;
			time_t			time;
			mode_t			mode;
			bool			dir;
			widget_file() : base(0), name(0), info(0),
							size(0), time(0), mode(0),
							dir(false) { }
		};
		typedef std::vector<widget_file> widget_files;
		typedef std::vector<widget_file>::iterator widget_files_it;
		typedef std::vector<widget_file>::const_iterator widget_files_cit;

		typedef boost::unordered_map<std::string, uint32_t>	name_map;

		typedef boost::optional<const widget_file&>  widget_file_copt;

		widget_files	left_;
		widget_files	center_;
		widget_files	right_;

		bool			request_right_;

		vtx::fpos	speed_;
		vtx::fpos	position_;
		float		move_speed_;
		uint32_t	select_pos_;

		std::string	file_;

		struct file_t {
			vtx::ipos	position_;
			uint32_t	select_pos_;
			file_t() : position_(0), select_pos_(0) { }
		};

		typedef boost::unordered_map<std::string, file_t>	file_map;
		file_map	file_map_;

		uint32_t	select_file_id_;

		std::string	focus_path_;

		static const char*	key_path_;
		static const char*	key_locate_;
		static const char*	key_size_;

		char				acc_key_;
		char				acc_key_ch_;
		uint32_t			acc_cnt_;

		bool		back_directory_;
		bool		center_update_;

		void create_file_(widget_file& wf, const vtx::irect& rect, int ofs, const std::string& str);
		void create_files_(widget_files& wfs, int ofs);
		widget_files_cit scan_select_in_file_(widget_files& wfs) const;
		widget_files_cit scan_select_file_(widget_files& wfs) const;
		widget_files_cit scan_selected_file_(widget_files& wfs) const;
		void un_selected_(widget_files& wfs);
		void resize_files_(widget_files& wfs, short ofs, short width);
		void update_files_info_(widget_files& wfs);
		void update_files_alias_(widget_files& wfs);
		void get_regist_state_();
		void set_regist_state_();
		void set_select_pos_(uint32_t pos);
		widget_file_copt scan_item_(const std::string& path) const;
		bool focus_(const std::string& fn);

		void destroy_files_(widget_files& wfs) noexcept
		{
			// ラベル郡を破棄
			for(const auto& wf : wfs) {
				wd_.del_widget(wf.info);
				wd_.del_widget(wf.name);
				wd_.del_widget(wf.base);
			}
			wfs.clear();
		}

		std::string make_path_(const std::string path) noexcept
		{
			std::string fin;
			if(utils::probe_full_path(path)) {
				auto root = utils::get_file_path(path);
				if(param_.path_ != root) {
					return fin;
				}
				auto s = utils::get_file_name(path);
				if(s.empty()) {
					return fin;
				}
				fin = s;
			} else {
				fin = path;
			}
			return fin;
		}

		void select_path_(const std::string& n) noexcept
		{
			// 移動中は無視！
			if(move_speed_ != 0.0f) return;

			if(n == "..") {  // 一つ前に戻る
				request_right_ = false;
				move_speed_ =  SPEED_MOVE;
			} else if(n.back() == '/') {
				request_right_ = true;
				move_speed_ = -SPEED_MOVE;
				std::string ap;
				if(n.size() > 2 && 'A' <= n[0] && n[0] <= 'Z' && n[1] == ':') {
					param_.path_ = n;
				} else {
					ap = utils::append_path(param_.path_, n);
					param_.path_ = utils::strip_last_of_delimita_path(ap);
				}
				file_infos_.clear();
				fsc_path_.clear();
				fsc_.set_path(param_.path_, param_.filter_);
				fsc_wait_ = true;
				destroy_files_(right_);
			} else {
				file_ = utils::append_path(param_.path_, n);
				++select_file_id_;
				enable(false);
				if(param_.select_file_func_ != nullptr) {
					param_.select_file_func_(file_);
				}
			}
		}

		void destroy_() noexcept
		{
			destroy_files_(left_);
			destroy_files_(center_);
			destroy_files_(right_);
			wd_.del_widget(files_);
			wd_.del_widget(main_);
		}

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief	コンストラクター
		*/
		//-----------------------------------------------------------------//
		widget_filer(widget_director& wd, const widget::param& bp, const param& p) noexcept :
			widget(bp), wd_(wd), param_(p), objh_(0),
			fsc_(), fsc_path_(), fsc_wait_(false),
			info_(0), main_(0), files_(0),
			info_state_(info_state::NONE),
			request_right_(false),
			speed_(0.0f), position_(0.0f), move_speed_(0.0f), select_pos_(0),
			file_(),
			select_file_id_(0),
			acc_key_(0), acc_key_ch_(0), acc_cnt_(0),
			back_directory_(false), center_update_(false)
			{ }


		//-----------------------------------------------------------------//
		/*!
			@brief	デストラクター
		*/
		//-----------------------------------------------------------------//
		virtual ~widget_filer() { destroy_(); }


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
		const char* type_name() const noexcept override { return "filer"; }


		//-----------------------------------------------------------------//
		/*!
			@brief	ハイブリッド・ウィジェットのサイン
			@return ハイブリッド・ウィジェットの場合「true」を返す。
		*/
		//-----------------------------------------------------------------//
		bool hybrid() const noexcept override { return true; }


		//-----------------------------------------------------------------//
		/*!
			@brief	個別パラメーターへの取得(ro)
			@return 個別パラメーター
		*/
		//-----------------------------------------------------------------//
		const param& get_local_param() const noexcept { return param_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	個別パラメーターへの取得
			@return 個別パラメーター
		*/
		//-----------------------------------------------------------------//
		param& at_local_param() noexcept { return param_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	ファイラーの状態を取得
			@return ファイラーの状態「true」なら有効
		*/
		//-----------------------------------------------------------------//
		bool get_enable() const noexcept { return get_state(state::ENABLE); }


		//-----------------------------------------------------------------//
		/*!
			@brief	許可
			@param[in]	f	「false」を指定すると不許可
		*/
		//-----------------------------------------------------------------//
		void enable(bool f = true) noexcept {
			wd_.enable(this, f, true);
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	初期化
		*/
		//-----------------------------------------------------------------//
		void initialize() noexcept override;


		//-----------------------------------------------------------------//
		/*!
			@brief	アップデート
		*/
		//-----------------------------------------------------------------//
		void update() noexcept override;


		//-----------------------------------------------------------------//
		/*!
			@brief	サービス
		*/
		//-----------------------------------------------------------------//
		void service() noexcept override;


		//-----------------------------------------------------------------//
		/*!
			@brief	レンダリング
		*/
		//-----------------------------------------------------------------//
		void render() noexcept override
		{
			if(objh_ == 0) return;

			wd_.at_mobj().resize(objh_, get_param().rect_.size);
			glEnable(GL_TEXTURE_2D);
			wd_.at_mobj().draw(objh_, gl::mobj::attribute::normal, vtx::spos(0));

			shift_text_render(wd_, get_param(), param_.text_param_, param_.plate_param_);
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
			if(!pre.put_text(path + "/current_path", param_.path_)) ++err;
			if(!pre.put_position(path + "/locate", vtx::ipos(get_rect().org))) ++err;
			if(!pre.put_position(path + "/size", vtx::ipos(get_rect().size))) ++err;
///			if(!pre.put_integer(key_info_, info_state_) ++err;
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
			std::string s;
			if(pre.get_text(path + "/current_path", s)) {
				if(utils::probe_file(s, true)) {
					param_.path_ = s;
				} else {
					param_.path_ = "/";
				}
			} else {
				++err;
			}

			rescan_center();

			vtx::ipos p;
			if(pre.get_position(path + "/locate", p)) {
				at_rect().org = p;
			} else {
				++err;
			}
			if(pre.get_position(path + "/size", p)) {
				at_rect().size = p;
			} else {
				++err;
			}
#if 0
			int i;
			if(pre.get_integer(key_info_, i)) {
				info_state_ = static_cast<info_state::type>(i);
			} else {
				++err;
			}
#endif
			return err == 0;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	ファイル選択 ID を取得
			@return ファイル選択 ID
		*/
		//-----------------------------------------------------------------//
		uint32_t get_select_file_id() const noexcept { return select_file_id_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	ファイル取得ステータス
			@return 「true」なら取得中、「false」で取得済み
		*/
		//-----------------------------------------------------------------//
		bool get_file_state() const noexcept { return fsc_wait_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	選択されたファイルを返す
			@return 選択されたファイル
		*/
		//-----------------------------------------------------------------//
		const std::string& get_file() const noexcept { return file_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	ファイル・リストを取得
			@param[in]	dir	ディレクトリーを含める場合「true」
			@return ファイル・リスト
		*/
		//-----------------------------------------------------------------//
		utils::strings get_file_list(bool dir = false) const noexcept
		{
			utils::strings ss;
			const widget_files& wfs = center_;
			for(widget_files_cit cit = wfs.begin(); cit != wfs.end(); ++cit) {
				const widget_file& wf = *cit;
				if(wf.size == 0 && wf.time == 0 && wf.mode == 0) continue;
				const std::string& fp = wf.name->get_text();
				if(fp == "..") continue;
				std::string fn = utils::append_path(param_.path_, fp);
				if(dir) {
					fn += '/';
					ss.push_back(fn);
				} else {
					if(wf.dir) continue;
					ss.push_back(fn);
				}
			}
			return ss;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	ファイルをフォーカスする
			@param[in]	path	選択するファイルパス
			@return 該当するファイルが無い場合「false」
		*/
		//-----------------------------------------------------------------//
		bool focus_file(const std::string& path) noexcept
		{
			focus_path_ = make_path_(path);
			return focus_(focus_path_);
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	代替テキスト（エリアス）を設定
			@param[in]	path	選択するファイルパス
			@param[in]	alias	代替テキスト
		*/
		//-----------------------------------------------------------------//
		void set_alias(const std::string& path, const std::string& alias) noexcept
		{
			widget_file_copt wfo = scan_item_(path);
			if(wfo) {
				const widget_file& wf = *wfo;
				if(wf.name) {
					wf.name->set_alias(alias);
				}
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	代替テキスト（エリアス）を有効、無効
			@param[in]	path	選択するファイルパス
			@param[in]	ena		無効にする場合「false」
		*/
		//-----------------------------------------------------------------//
		void enable_alias(const std::string& path, bool ena = true) noexcept
		{
			widget_file_copt wfo = scan_item_(path);
			if(wfo) {
				const widget_file& wf = *wfo;
				if(wf.name) {
					wf.name->enable_alias(ena);
				}
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	ファイル情報収集をやり直す
		*/
		//-----------------------------------------------------------------//
		void rescan_center() noexcept
		{
			file_infos_.clear();
			fsc_path_.clear();
			fsc_.set_path(param_.path_, param_.filter_);
			fsc_wait_ = true;
			destroy_files_(left_);
			destroy_files_(center_);
			destroy_files_(right_);
		}
	};
}
