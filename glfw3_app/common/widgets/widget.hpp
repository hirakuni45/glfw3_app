#pragma once
//=====================================================================//
/*!	@file
	@brief	GUI widget 基底クラス
    @author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2017, 2018 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/glfw_app/blob/master/LICENSE
*/
//=====================================================================//
#include <vector>
#include <string>
#include <bitset>
#include "img_io/img.hpp"
#include "img_io/paint.hpp"
#include "utils/vtx.hpp"
#include "utils/vmath.hpp"
#include "utils/preference.hpp"

namespace gui {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	gui widget の「型」情報を取得する仕組み
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	namespace any_detail {
		template<class value_type> struct dummy_ { static char p; };
		template<class value_type> char dummy_<value_type>::p;
	}
	typedef void* type_id;
	template<class value_type>
	type_id get_type_id() { return &any_detail::dummy_<value_type>::p; }


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	gui widget 基底クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	struct widget {

		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		/*!
			@brief	カラー・パラメーター
		*/
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		struct color_param {
			img::rgba8		fore_color_;			///< 前面色
			img::rgba8		back_color_;			///< 背景色
			img::paint::intensity_rect	inten_rect_;
			bool			ir_enable_;
			explicit color_param(const img::rgba8& fc = img::rgba8(255),
				const img::rgba8& bc = img::rgba8(0)) :
				fore_color_(fc), back_color_(bc), inten_rect_(), ir_enable_(false)
			{ }

			void swap_color() { fore_color_.swap(back_color_); }

			size_t hash() const {
				size_t h = fore_color_.hash();
				h ^= back_color_.hash();
				h ^= inten_rect_.hash();
				boost::hash_combine(h, ir_enable_);
				return h;
			}

			bool operator == (const color_param& c) const {
				return c.fore_color_ == fore_color_ &&
					c.back_color_ == back_color_ &&
					c.inten_rect_ == inten_rect_ &&
					c.ir_enable_ == ir_enable_;
			}
		};


		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		/*!
			@brief	プレート・パラメーター
		*/
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		struct plate_param {

			enum class round_style : uint16_t {
				NONE,		///< ラウンド無し
				ALL,		///< 全てラウンド
				TOP,		///< 上側をラウンド
				BOTTOM,		///< 下側をラウンド
				UPPER_OPEN,	///< 上がオープン（シート用）
				UNDER_OPEN,	///< 下がオープン（シート用）
			};

			round_style	round_style_;	///< ラウンド・スタイル
			short		round_radius_;	///< ラウンド半径
			short		frame_width_;	///< フレーム幅
			short		caption_width_;	///< キャプション幅
			vtx::ipos	grid_;			///< リサイズ・グリッド
			bool		resizeble_;		///< リサイズが可能な場合

			plate_param(short rr = 8, short fw = 4) : round_style_(round_style::ALL),
				round_radius_(rr), frame_width_(fw), caption_width_(0),
				grid_(16), resizeble_(false)
			{ }

			void set_caption(short width) {
				if(width <= 0) return;
				caption_width_ = width;
				// アンチエリアスを解消する為の隙間を取る
				short g = width + frame_width_ + round_radius_ - frame_width_;
				if(g & 15) { g |= 15; ++g; }
				grid_.y = g;
			}

			size_t hash() const {
				size_t h = grid_.hash();
				boost::hash_combine(h, round_style_);
				boost::hash_combine(h, round_radius_);
				boost::hash_combine(h, frame_width_);
				boost::hash_combine(h, caption_width_);
				boost::hash_combine(h, resizeble_);
				return h;
			}

			bool operator == (const plate_param& pp) const {
				return pp.round_style_ == round_style_ &&
					pp.round_radius_ == round_radius_ &&
					pp.frame_width_ == frame_width_ &&
					pp.caption_width_ == caption_width_ &&
					pp.grid_ == grid_ &&
					pp.resizeble_ == resizeble_;
			}
		};


		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		/*!
			@brief	テキスト・パラメーター
		*/
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		struct text_param {
			utils::lstring	text_;			///< テキスト
			std::string		font_;			///< フォントセット
			short			font_size_;		///< フォントサイズ
			bool			proportional_;	///< プロポーショナル・フォントの場合「true」
			bool			alias_enable_;	///< テキストの別名を有効にする
			utils::lstring	alias_;			///< テキストの別名（置き換え用）

			img::rgba8		fore_color_;	///< テキスト色
			img::rgba8		shadow_color_;	///< 影色
			vtx::ipos		shadow_offset_;	///< 影の相対位置
			vtx::placement	placement_;		///< 配置方法
			vtx::ipos		offset_;		///< 描画オフセット（シフト表示用）

			int				cursor_;		///< カーソル位置反転表示（エリアスは無効になる）

			text_param() :
				text_(), font_(), font_size_(24), proportional_(true),
				alias_enable_(false), alias_(),
				fore_color_(255, 255), shadow_color_(0, 255),
				shadow_offset_(1),
				placement_(vtx::placement::holizontal::CENTER,
					vtx::placement::vertical::CENTER),
				offset_(0), cursor_(-1) { }

			explicit text_param(const std::string& text,
				const img::rgba8& fc, const img::rgba8& sc,
				const vtx::placement& pl = vtx::placement(
				vtx::placement::holizontal::CENTER,
				vtx::placement::vertical::CENTER)) :
					text_(), font_(), font_size_(24), proportional_(true),
					alias_enable_(false), alias_(),
					fore_color_(fc), shadow_color_(sc), shadow_offset_(1),
					placement_(pl),
					offset_(0), cursor_(-1) { utils::utf8_to_utf32(text, text_); }

			void set_text(const std::string& text) {
				text_.clear();
				utils::utf8_to_utf32(text, text_);
			}

			std::string get_text() const {
				std::string s;
				utils::utf32_to_utf8(text_, s);
				return s;
			}

			void set_alias(const std::string& alias) {
				alias_.clear();
				utils::utf8_to_utf32(alias, alias_);
			}

			const std::string get_alias() const {
				std::string s;
				utils::utf32_to_utf8(alias_, s);
				return s;
			}
		};


		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		/*!
			@brief	シフト・パラメーター（文字列の横スクロール）
		*/
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		struct shift_param {
			bool		enable_;		///< シフト表示を有効にする場合
			bool		every_;			///< 常にシフト動作の場合
			int16_t		size_;			///< シフト領域
			float		offset_;		///< シフト表示のオフセット
			float		speed_;			///< シフト表示の速度
			uint16_t	hold_frame_;	///< シフト表示までのホールドフレーム
			uint16_t	org_wait_frame_;	///< シフト先頭の場合の待ちフレーム
			uint16_t	org_wait_count_;	///< シフト先頭の場合の待ちカウンタ

			shift_param(bool ena = true) :
				enable_(ena), every_(false), size_(0),
				offset_(0.0f), speed_(0.5f),
				hold_frame_(2 * 60),
				org_wait_frame_(0), org_wait_count_(0)
				{ }
		};


		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		/*!
			@brief	スライダー・パラメーター
		*/
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		struct slider_param {

			//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
			/*!
				@brief	スライダー方向型
			*/
			//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
			enum class direction {
				HOLIZONTAL,	///< 水平スライダー
				VERTICAL	///< 垂直スライダー
			};

			direction	direction_;		///< 方向性
			float		position_;		///< 位置
			float		handle_ratio_;	///< ハンドル割合
			float		grid_;			///< グリッド
			float		step_;			///< ステップ
			bool		accelerator_;	///< アクセレーターを有効にする場合
			bool		handle_resize_;	///< ハンドル・リサイズ
			slider_param(float pos = 0.0f, direction dir = direction::HOLIZONTAL) :
				direction_(dir), position_(pos),
				handle_ratio_(0.1f), grid_(0.0f), step_(0.1f),
				accelerator_(true), handle_resize_(true)
			{ }
		};


		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		/*!
			@brief	アクション@n
					レンダリング時に行うアクション
		*/
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		struct action {
			enum type {
				FOCUS_HIGHLIGHT,		///< ハイライト
				FOCUS_ALPHA25,			///< 25% 半透明にする
				FOCUS_ALPHA50,			///< 50% 半透明にする
				FOCUS_SCALE,			///< スケール

				SELECT_HIGHLIGHT,		///< ハイライト
				SELECT_CYCLE_COLOR,		///< サイクルカラー
				SELECT_ALPHA,			///< 半透明にする
				SELECT_SCALE,			///< スケール

				limit_
			};
		};
		typedef std::bitset<action::limit_> action_types;


		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		/*!
			@brief	widget 状態制御
		*/
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		struct state {
			enum type {
				ENABLE,					///< 全体の許可・不許可（処理しない）

				STALL,					///< 停止中（アップデートは無効）

				RENDER_ENABLE,			///< widget の描画
			   	FOCUS_ENABLE,			///< フォーカス許可
				UPDATE_ENABLE,			///< アップデート許可

				POSITION_LOCK,			///< 位置固定
				CLIP_PARENTS,			///< ペアレントの領域でクリッピングを行う
				DRAG_UNSELECT,			///< ドラッグされたらセレクトを外す
				FOCUS_CHILDS,			///< 子供全てにフォーカスを与える

				SELECT_PARENTS,			///< 選択権を「親」に移動
				SELECT_CHILDS,			///< 子供を全て選択
				SELECT_ROOT,			///< 選択権をルートに与える
				MOVE_ROOT,				///< 移動の選択権をルートに与える
				MOVE_STALL,             ///< 移動の選択権を停止する
				RESIZE_ROOT,			///< リサイズの選択権をルートに与える

				AREA_ROOT,				///< ルートの描画領域を継承

				MOVE_TOP,				///< 移動時、最上位にする。
				RESIZE_TOP,				///< リサイズ時、最上位にする。
				ENABLE_TOP,				///< 有効時、最上位にする。

				RESIZE_H_ENABLE,		///< 水平リサイズ許可
				RESIZE_V_ENABLE,		///< 垂直リサイズ許可
				RESIZE_EDGE_ENABLE,		///< エッジ領域でのリサイズ許可
				SIZE_LOCK,				///< サイズ固定

				SERVICE,				///< サービスを呼び出す

				BEFORE_DRAG,			///< １フレーム前ドラッグの状態
				DRAG,					///< ドラッグの状態
				BEFORE_RESIZE,			///< １フレーム前リサイズの状態
				RESIZE,					///< リサイズの状態

				/// 以下、読み出し専用フラグ
				SELECTED,				///< 選択済
				SYSTEM_STALL,			///< システム用ストール

				BEFORE_FOCUS,			///< １フレーム前フォーカスの状態
				IS_FOCUS,				///< フォーカスの状態
				FOCUS,					///< フォーカス（テンポラリー）

				BEFORE_SELECT,			///< １フレーム前選択状態
				IS_SELECT,				///< 選択状態
				SELECT,					///< セレクト（テンポラリー）

				limit_
			};
		};
		typedef std::bitset<state::limit_>	state_types;


		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		/*!
			@brief	widget ベース・パラメーター
		*/
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		struct param {
			vtx::irect			rect_;			///< 領域
			vtx::irect			clip_;			///< クリップ領域
			vtx::ipos			rpos_;			///< レンダリング開始
			vtx::ipos			move_org_;		///< 移動基準位置
			vtx::ipos			move_pos_;		///< 移動位置
			vtx::ipos			resize_sign_;	///< リサイズ符号
			vtx::ipos			resize_min_;	///< リサイズ最小サイズ
			vtx::ipos			resize_org_;	///< リサイズ基準位置
			vtx::ipos			resize_pos_;	///< リサイズ位置
			vtx::ipos			resize_ref_;	///< リサイズ基準サイズ
			vtx::ipos			speed_;			///< 慣性速度
			vtx::ipos			in_point_;		///< 内包ポイント
			uint32_t			hold_frame_;	///< ホールド・フレーム
			uint32_t			holded_frame_;	///< ホールドしてたフレーム
			widget*				parents_;		///< 親ウィジェット
			action_types		action_;		///< アクション
			state_types			state_;			///< 状態制御
			uint32_t			pre_group_;		///< プリファレンス・グループ

			param(const vtx::irect& r = vtx::irect(0), widget* parents = nullptr) :
				rect_(r), clip_(r), rpos_(r.org),
				move_org_(0), move_pos_(0),
				resize_sign_(0), resize_min_(16 * 3), resize_org_(0), resize_pos_(0), resize_ref_(0),
				speed_(0), in_point_(0),
				hold_frame_(0), holded_frame_(0),
				parents_(parents),
				action_(), state_(),
				pre_group_(0)
			{
				state_.set(state::ENABLE);
				state_.set(state::RENDER_ENABLE);
				state_.set(state::FOCUS_ENABLE);
				state_.set(state::UPDATE_ENABLE);
			}
		};

	private:
		param		param_;

		std::string	symbol_;

		uint32_t	serial_;

		bool		mark_;

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief	コンストラクター
			@param[in]	para	パラメーター
			@param[in]	sym		シンボル
		*/
		//-----------------------------------------------------------------//
		widget(const param& para, const std::string& sym = "") :
			param_(para), symbol_(sym),
			serial_(0), mark_(false)
			{ }


		//-----------------------------------------------------------------//
		/*!
			@brief	デストラクター
		*/
		//-----------------------------------------------------------------//
		virtual ~widget() { }


		//-----------------------------------------------------------------//
		/*!
			@brief	パラメーターの取得(RO)
			@return 基本パラメーターの参照
		*/
		//-----------------------------------------------------------------//
		const param& get_param() const { return param_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	パラメーターの取得(RW)
			@return 基本パラメーターの参照
		*/
		//-----------------------------------------------------------------//
		param& at_param() { return param_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	領域への参照(RO)
			@return 領域の参照
		*/
		//-----------------------------------------------------------------//
		const vtx::irect& get_rect() const { return param_.rect_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	領域への参照
			@return 領域の参照
		*/
		//-----------------------------------------------------------------//
		vtx::irect& at_rect() { return param_.rect_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	マーキングを設定
			@param[in]	f	フラグ
		*/
		//-----------------------------------------------------------------//
		void set_mark(bool f = true) { mark_ = f; }


		//-----------------------------------------------------------------//
		/*!
			@brief	マーキングを取得
			@return	マーク状態
		*/
		//-----------------------------------------------------------------//
		bool get_mark() const { return mark_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	widget 型を取得
			@return widget 型
		*/
		//-----------------------------------------------------------------//
		virtual type_id type() const = 0;


		//-----------------------------------------------------------------//
		/*!
			@brief	widget 型の基本名称を取得
			@return widget 型の基本名称
		*/
		//-----------------------------------------------------------------//
		virtual const char* type_name() const = 0;


		//-----------------------------------------------------------------//
		/*!
			@brief	ハイブリッド・ウィジェットの検査
			@return ハイブリッド・ウィジェットの場合「true」を返す。
		*/
		//-----------------------------------------------------------------//
		virtual bool hybrid() const = 0;


		//-----------------------------------------------------------------//
		/*!
			@brief	初期化
		*/
		//-----------------------------------------------------------------//
		virtual void initialize() = 0;


		//-----------------------------------------------------------------//
		/*!
			@brief	アップデート
		*/
		//-----------------------------------------------------------------//
		virtual void update() = 0;


		//-----------------------------------------------------------------//
		/*!
			@brief	サービス処理
		*/
		//-----------------------------------------------------------------//
		virtual void service() = 0;


		//-----------------------------------------------------------------//
		/*!
			@brief	レンダリング
		*/
		//-----------------------------------------------------------------//
		virtual void render() = 0;


		//-----------------------------------------------------------------//
		/*!
			@brief	状態のセーブ
			@param[in]	pre	プリファレンス参照
			@return エラーが無い場合「true」
		*/
		//-----------------------------------------------------------------//
		virtual bool save(sys::preference& pre) = 0;


		//-----------------------------------------------------------------//
		/*!
			@brief	状態のロード
			@param[in]	pre	プリファレンス参照
			@return エラーが無い場合「true」
		*/
		//-----------------------------------------------------------------//
		virtual bool load(const sys::preference& pre) = 0;


		//-----------------------------------------------------------------//
		/*!
			@brief	ストールの設定
			@param[in]	f	不許可の場合「false」
		*/
		//-----------------------------------------------------------------//
		void set_stall(bool f = true) { param_.state_[state::STALL] = f; }


		//-----------------------------------------------------------------//
		/*!
			@brief	ステートの設定
			@param[in]	t	制御タイプ
			@param[in]	f	不許可の場合「false」
		*/
		//-----------------------------------------------------------------//
		void set_state(state::type t, bool f = true) { param_.state_[t] = f; }


		//-----------------------------------------------------------------//
		/*!
			@brief	ステートの取得
			@param[in]	t	制御タイプ
			@return	許可なら「true」
		*/
		//-----------------------------------------------------------------//
		bool get_state(state::type t) const { return param_.state_[t]; }


		//-----------------------------------------------------------------//
		/*!
			@brief	アクションの設定
			@param[in]	t	制御タイプ
			@param[in]	f	不許可の場合「false」
		*/
		//-----------------------------------------------------------------//
		void set_action(action::type t, bool f = true) { param_.action_[t] = f; }


		//-----------------------------------------------------------------//
		/*!
			@brief	アクションの取得
			@param[in]	t	制御タイプ
			@return	許可なら「true」
		*/
		//-----------------------------------------------------------------//
		bool get_action(action::type t) const { return param_.action_[t]; }


		//-----------------------------------------------------------------//
		/*!
			@brief	許可の状態を取得する
			@return 許可の状態
		*/
		//-----------------------------------------------------------------//
		bool get_enable() const {
			return get_state(state::ENABLE);
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	フォーカス・インを取得する
			@return 状態
		*/
		//-----------------------------------------------------------------//
		bool get_focus_in() const {
			return !get_state(state::BEFORE_FOCUS) && get_state(state::IS_FOCUS);
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	フォーカスを取得する
			@return 状態
		*/
		//-----------------------------------------------------------------//
		bool get_focus() const {
			return get_state(state::IS_FOCUS);
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	フォーカス・アウトを取得する
			@return 状態
		*/
		//-----------------------------------------------------------------//
		bool get_focus_out() const {
			return get_state(state::BEFORE_FOCUS) && !get_state(state::IS_FOCUS);
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	セレクト・インを取得する
			@return 状態
		*/
		//-----------------------------------------------------------------//
		bool get_select_in() const {
			return !get_state(state::BEFORE_SELECT) && get_state(state::IS_SELECT);
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	セレクトを取得する
			@return 状態
		*/
		//-----------------------------------------------------------------//
		bool get_select() const {
			return get_state(state::IS_SELECT);
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	セレクト・アウトを取得する
			@return 状態
		*/
		//-----------------------------------------------------------------//
		bool get_select_out() const {
			return get_state(state::BEFORE_SELECT) && !get_state(state::IS_SELECT);
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	選択された
			@return 状態
		*/
		//-----------------------------------------------------------------//
		bool get_selected() const {
			return get_focus() && get_select_out();
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	シンボルを設定する
			@param[in] シンボル
		*/
		//-----------------------------------------------------------------//
		void set_symbol(const std::string& sym) { symbol_ = sym; }


		//-----------------------------------------------------------------//
		/*!
			@brief	シンボルを取得する
			@return シンボル
		*/
		//-----------------------------------------------------------------//
		const std::string& get_symbol() const { return symbol_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	シリアル番号を設定する
			@param[in] シリアル番号
		*/
		//-----------------------------------------------------------------//
		void set_serial(uint32_t serial) { serial_ = serial; }


		//-----------------------------------------------------------------//
		/*!
			@brief	シリアル番号を取得する
			@return シリアル番号
		*/
		//-----------------------------------------------------------------//
		uint32_t get_serial() const { return serial_; }

	};

	typedef std::vector<widget*> widgets;
	typedef std::vector<widget*>::iterator widgets_it;
	typedef std::vector<widget*>::const_iterator widgets_cit;


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	share 構造体
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	struct share_t {
		vtx::ipos	size_;
		widget::color_param	color_param_;
		widget::plate_param	plate_param_;
		share_t() : size_(0), color_param_(), plate_param_() { }

		inline size_t hash() const {
			size_t hs = size_.hash();
			hs ^= color_param_.hash();
			hs ^= plate_param_.hash();
			return hs;
		}

		inline bool operator == (const share_t& k) const {
			return k.size_ == size_ &&
				k.color_param_ == color_param_ &&
				k.plate_param_ == plate_param_;
		}
	};
	inline size_t hash_value(const share_t& k) { return k.hash(); }

}
