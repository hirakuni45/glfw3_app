#pragma once
//=====================================================================//
/*!	@file
	@brief	GUI Widget ファイラー（ヘッダー）@n
			ファイル選択を行う GUI モジュール
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include <vector>
#include <boost/unordered_map.hpp>
#include "utils/files.hpp"
#include "utils/drive_info.hpp"
#include "utils/preference.hpp"
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

		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		/*!
			@brief	widget_filer パラメーター
		*/
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		struct param {
			plate_param		plate_param_;
			color_param		color_param_;

			std::string		path_;		///< 初期ファイル・パス
			std::string		filter_;	///< 初期ファイル・フィルター

			param(const std::string& path = "", const std::string& filter = "") :
				plate_param_(),
				color_param_(widget_director::default_frame_color_),
				path_(path), filter_(filter)
			{ }
		};

	private:
		widget_director&	wd_;

		param				param_;

		gl::glmobj::handle	objh_;

		utils::files		fsc_;
		utils::drive_info	drv_;

		short			path_height_;
		short			label_height_;

		widget_label*	path_;	///< パス・フレーム
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
				limit_
			};
		};
		info_state::type	info_state_;

		struct widget_file {
			widget*			base;
			widget_label*	name;
			widget_label*	info;
			size_t			size;
			time_t			time;
			mode_t			mode;
			bool			dir;
			widget_file() : base(0), name(0), info(0),
				size(0), time(0), mode(0), dir(false) { }
		};
		typedef std::vector<widget_file> widget_files;
		typedef std::vector<widget_file>::iterator widget_files_it;
		typedef std::vector<widget_file>::const_iterator widget_files_cit;
		widget_files	left_;
		widget_files	center_;
		widget_files	right_;

		vtx::fpos	speed_;
		vtx::fpos	position_;
		uint32_t	select_pos_;

		std::string	file_;

		struct file_t {
			vtx::spos	position_;
			uint32_t	select_pos_;
			file_t() : position_(0), select_pos_(0) { }
		};

		typedef boost::unordered_map<std::string, file_t>	file_map;
		typedef boost::unordered_map<std::string, file_t>::iterator	file_map_it;
		typedef std::pair<std::string, file_t> file_map_pair;
		file_map	file_map_;

		uint32_t	select_file_id_;

		void create_files_(widget_files& wfs, short ofs);
		widget_files_cit scan_select_in_file_(widget_files& wfs) const;
		widget_files_cit scan_select_file_(widget_files& wfs) const;
		widget_files_cit scan_selected_file_(widget_files& wfs) const;
		void resize_files_(widget_files& wfs, short width);
		void update_files_info_(widget_files& wfs);
		void destroy_files_(widget_files& wfs);
		void regist_state_();
		void destroy_();
	public:
		//-----------------------------------------------------------------//
		/*!
			@brief	コンストラクター
		*/
		//-----------------------------------------------------------------//
		widget_filer(widget_director& wd, const widget::param& bp, const param& p) :
			wd_(wd), widget(bp), param_(p), objh_(0),
			fsc_(),
			path_height_(32), label_height_(32),
			path_(0), info_(0), main_(0), files_(0),
			info_state_(info_state::NONE),
			speed_(0.0f), position_(0.0f), select_pos_(0),
			file_(),
			select_file_id_(0)
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
			@brief	レンダリング
		*/
		//-----------------------------------------------------------------//
		void render();


		//-----------------------------------------------------------------//
		/*!
			@brief	サービス
		*/
		//-----------------------------------------------------------------//
		void service();


		//-----------------------------------------------------------------//
		/*!
			@brief	ファイル選択シグナルを取得
			@return ファイル選択シグナル
		*/
		//-----------------------------------------------------------------//
		uint32_t get_select_file_id() const { return select_file_id_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	ファイルを返す
			@return ファイル
		*/
		//-----------------------------------------------------------------//
		const std::string& get_file() const { return file_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	ファイル・リストを取得
			@param[out]	ss	ファイル・リスト
			@param[in]	dir	ディレクトリーを含める場合「true」
			@return ファイル・パス・インデックス（負の値ならマッチ無し）
		*/
		//-----------------------------------------------------------------//
		int get_file_list(utils::strings& ss, bool dir = false);


		//-----------------------------------------------------------------//
		/*!
			@brief	ファイル郡を取得
			@return ファイル郡
		*/
		//-----------------------------------------------------------------//
		const utils::files& get_files() const { return fsc_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	ファイルを選択
			@param[in]	path	選択するファイルパス
			@return 該当するファイルが無い場合「false」
		*/
		//-----------------------------------------------------------------//
		bool select_file(const std::string& path);


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
		bool load(sys::preference& pre);
	};
}
