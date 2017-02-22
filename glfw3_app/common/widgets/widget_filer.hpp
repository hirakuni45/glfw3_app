#pragma once
//=====================================================================//
/*!	@file
	@brief	GUI Widget ファイラー（ヘッダー）@n
			ファイル選択を行う GUI モジュール @n
			Copyright 2017 Kunihito Hiramatsu
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include <vector>
#include <boost/unordered_map.hpp>
#include <boost/optional.hpp>
#include "utils/files.hpp"
#include "utils/drive_info.hpp"
#include "widgets/widget_director.hpp"
#include "widgets/widget_null.hpp"
#include "widgets/widget_label.hpp"
#include "widgets/widget_button.hpp"

namespace gui {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	GUI filer クラス（ファイル選択）
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	struct widget_filer : public widget {

		typedef widget_filer value_type;

		typedef std::function< void (const std::string&) > select_file_func_type;

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

			param(const std::string& path = "", const std::string& filter = "", bool new_file = false) :
				plate_param_(),
				color_param_(widget_director::default_frame_color_),
				text_param_(), shift_param_(),
				path_(path), filter_(filter),
				path_height_(32), label_height_(32),
				every_top_(true), acc_focus_(true),
				new_file_(new_file),
				select_file_func_(nullptr)
			{ }
		};

	private:
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

		static const char* key_path_;
		static const char* key_locate_;
		static const char* key_size_;

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
		void destroy_files_(widget_files& wfs);
		void get_regist_state_();
		void set_regist_state_();
		void set_select_pos_(uint32_t pos);
		void destroy_();
		widget_file_copt scan_item_(const std::string& path) const;
		bool focus_(const std::string& fn);
		std::string make_path_(const std::string path);
		void select_path_(const std::string& n);

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief	コンストラクター
		*/
		//-----------------------------------------------------------------//
		widget_filer(widget_director& wd, const widget::param& bp, const param& p) :
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
		type_id type() const { return get_type_id<value_type>(); }


		//-----------------------------------------------------------------//
		/*!
			@brief	widget 型の基本名称を取得
			@return widget 型の基本名称
		*/
		//-----------------------------------------------------------------//
		const char* type_name() const { return "filer"; }


		//-----------------------------------------------------------------//
		/*!
			@brief	ハイブリッド・ウィジェットのサイン
			@return ハイブリッド・ウィジェットの場合「true」を返す。
		*/
		//-----------------------------------------------------------------//
		bool hybrid() const { return true; }


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
			@brief	ファイラーの状態を取得
			@return ファイラーの状態「true」なら有効
		*/
		//-----------------------------------------------------------------//
		bool get_enable() const { return get_state(state::ENABLE); }


		//-----------------------------------------------------------------//
		/*!
			@brief	許可
			@param[in]	f	「false」を指定すると不許可
		*/
		//-----------------------------------------------------------------//
		void enable(bool f = true) {
			wd_.enable(this, f, true);
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	初期化
		*/
		//-----------------------------------------------------------------//
		void initialize();


		//-----------------------------------------------------------------//
		/*!
			@brief	アップデート
		*/
		//-----------------------------------------------------------------//
		void update();


		//-----------------------------------------------------------------//
		/*!
			@brief	サービス
		*/
		//-----------------------------------------------------------------//
		void service();


		//-----------------------------------------------------------------//
		/*!
			@brief	レンダリング
		*/
		//-----------------------------------------------------------------//
		void render();


		//-----------------------------------------------------------------//
		/*!
			@brief	状態のセーブ
			@param[in]	pre	プリファレンス参照
			@return エラーが無い場合「true」
		*/
		//-----------------------------------------------------------------//
		bool save(sys::preference& pre);


		//-----------------------------------------------------------------//
		/*!
			@brief	状態のロード
			@param[in]	pre	プリファレンス参照
			@return エラーが無い場合「true」
		*/
		//-----------------------------------------------------------------//
		bool load(const sys::preference& pre);


		//-----------------------------------------------------------------//
		/*!
			@brief	ファイル選択 ID を取得
			@return ファイル選択 ID
		*/
		//-----------------------------------------------------------------//
		uint32_t get_select_file_id() const { return select_file_id_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	ファイル取得ステータス
			@return 「true」なら取得中、「false」で取得済み
		*/
		//-----------------------------------------------------------------//
		bool get_file_state() const { return fsc_wait_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	選択されたファイルを返す
			@return 選択されたファイル
		*/
		//-----------------------------------------------------------------//
		const std::string& get_file() const { return file_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	ファイル・リストを取得
			@param[in]	dir	ディレクトリーを含める場合「true」
			@return ファイル・リスト
		*/
		//-----------------------------------------------------------------//
		utils::strings get_file_list(bool dir = false) const;


		//-----------------------------------------------------------------//
		/*!
			@brief	ファイルをフォーカスする
			@param[in]	path	選択するファイルパス
			@return 該当するファイルが無い場合「false」
		*/
		//-----------------------------------------------------------------//
		bool focus_file(const std::string& path);


		//-----------------------------------------------------------------//
		/*!
			@brief	代替テキスト（エリアス）を設定
			@param[in]	path	選択するファイルパス
			@param[in]	alias	代替テキスト
		*/
		//-----------------------------------------------------------------//
		void set_alias(const std::string& path, const std::string& alias);


		//-----------------------------------------------------------------//
		/*!
			@brief	代替テキスト（エリアス）を有効、無効
			@param[in]	path	選択するファイルパス
			@param[in]	ena		無効にする場合「false」
		*/
		//-----------------------------------------------------------------//
		void enable_alias(const std::string& path, bool ena = true);
	};
}
