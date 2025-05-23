#pragma once
//=========================================================================//
/*!	@file
	@brief	ターミナル・クラス（ヘッダー）
    @author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2017, 2024 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/glfw_app/blob/master/LICENSE
*/
//=========================================================================//
#include <string>
#include <vector>
#include <deque>
#include <functional>
#include "img_io/img.hpp"
#include "utils/vtx.hpp"
#include "utils/string_utils.hpp"
#include "utils/format.hpp"

namespace utils {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	ターミナル・クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	struct terminal {

		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		/*!
			@brief	文字トランク
		*/
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		struct cha_t {
			uint32_t	cha_;
			img::rgba8	fc_;
			img::rgba8	bc_;
			bool		select_;

			cha_t(uint32_t cha = 0,
				  const img::rgba8& fc = img::rgba8(255, 255, 255, 255),
				  const img::rgba8& bc = img::rgba8(  0,   0,   0, 255)) noexcept :
				cha_(cha), fc_(fc), bc_(bc), select_(false)
				{ }
		};

		typedef std::vector<cha_t>	line;
		typedef std::deque<line>	lines;

		typedef std::function< void (uint32_t ch) > output_func;

	private:
		cha_t		cha_;

		lines		lines_;
		uint32_t	max_;

		vtx::ipos	pos_;

		cha_t		tmp_;

		bool		auto_crlf_;
		bool		insert_;

		line		last_;

		output_func	output_func_;

		void nl_() {
			if(lines_.size() >= max_) {
				lines_.pop_front();
			} else {
				++pos_.y;
			}
			last_ = lines_.back();
			line l;
			lines_.push_back(l);
		}

		void bl_() {  // back line
		}

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief	コンストラクター
			@param[in]	max	最大ライン数
		*/
		//-----------------------------------------------------------------//
		terminal(uint32_t max = 150) noexcept : cha_(), lines_(), max_(max), pos_(0), tmp_(' '),
			auto_crlf_(false), insert_(true), last_(), output_func_(nullptr)
		{
			line l;
			lines_.push_back(l);
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	デストラクター
		*/
		//-----------------------------------------------------------------//
		~terminal() { }


		//-----------------------------------------------------------------//
		/*!
			@brief	自動 CR/LF の許可／不許可
			@param[in]	f	自動機能を無効にする場合「false」
		*/
		//-----------------------------------------------------------------//
		void enable_crlf(bool f = true) noexcept { auto_crlf_ = f; }


		//-----------------------------------------------------------------//
		/*!
			@brief	insert モードの許可／不許可
			@param[in]	f	不許可にする場合「false」
		*/
		//-----------------------------------------------------------------//
		void enable_insert(bool f = true) noexcept { insert_ = f; }


		//-----------------------------------------------------------------//
		/*!
			@brief	消去
		*/
		//-----------------------------------------------------------------//
		void clear() noexcept
		{
			lines_.clear();
			line l;
			lines_.push_back(l);
			pos_.set(0);
			last_.clear();
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	ライン・クリア
		*/
		//-----------------------------------------------------------------//
		void clear_line(bool all = true) noexcept
		{
			line& l = lines_[pos_.y];
			if(all) l.clear();
			else {
				if(pos_.x < l.size()) {
					l.resize(pos_.x);
				}
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	全セレクト
			@param[in] ena		選択状態
		*/
		//-----------------------------------------------------------------//
		void select_all(bool ena) noexcept
		{
			for(auto& l : lines_) {
				for(auto& ch : l) {
					ch.select_ = ena;
				}
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	文字出力関数設定
			@param[in]	func	関数
		*/
		//-----------------------------------------------------------------//
		void set_output_func(output_func func) noexcept
		{
			output_func_ = func;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	文字出力
			@param[in]	cha	文字
		*/
		//-----------------------------------------------------------------//
		void output(uint32_t cha) noexcept
		{
			if(output_func_ != nullptr) output_func_(cha);

			cha_.cha_ = cha;
			switch(cha) {
			case '\r':  // CR
				pos_.x = 0;
				break;
			case '\n':  // LF
				nl_();
				if(auto_crlf_) {
					pos_.x = 0;
				}
				break;
			case 0x08:  // Back Space
				if(pos_.x > 0) {
					auto& l = lines_[pos_.y];
					--pos_.x;
					if(pos_.x < l.size()) {
						l.erase(l.begin() + pos_.x);
					}
				}
				break;
			case 0x7f:  // DEL
				{
					auto& l = lines_[pos_.y];
					if(pos_.x < l.size()) {
						l.erase(l.begin() + pos_.x);
					}
				}
				break;
			case 0x11:  // Right
				if(pos_.x < lines_[pos_.y].size()) {
					++pos_.x;
				}
				break;
			case 0x12:  // Left
				if(pos_.x > 0) --pos_.x;
				break;
			case 0x13:  // Down
				if(pos_.y < (lines_.size() - 1)) {
					++pos_.y;
					auto& l = lines_[pos_.y];
					if(l.size() <= pos_.x) {
						pos_.x = l.size();
					}
				}
				break;
			case 0x14:  // Up
				if(pos_.y > 0) {
					pos_.y--;
					auto& l = lines_[pos_.y];
					if(l.size() <= pos_.x) {
						pos_.x = l.size();
					}
				}
				break;
			default:
				if(cha < 0x20) {
//				std::cout << boost::format("%02X") % cha << std::endl << std::flush;
				} else {
					line& l = lines_[pos_.y];
					if(pos_.x < l.size()) {
						if(insert_) {
							l.insert(l.begin() + pos_.x, cha_);
						} else {
							l[pos_.x] = cha_;
							++pos_.x;
						}
					} else {
						l.push_back(cha_);
						pos_.x = l.size();
					}
				}
				break;
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	文字出力（UTF-8)
			@param[in]	str	文字列
		*/
		//-----------------------------------------------------------------//
		void output(const std::string& str) noexcept
		{
			auto ls = utils::utf8_to_utf32(str);
			for(auto lch : ls) {
				output(lch);
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	文字出力（UTF-32）
			@param[in]	str	文字列
		*/
		//-----------------------------------------------------------------//
		void output(const utils::lstring& str) noexcept
		{
			for(auto lch : str) {
				output(lch);
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	フォア・カラーの設定
			@param[in]	pos		位置
			@param[in]	col		カラー
		*/
		//-----------------------------------------------------------------//
		void set_fore_color(const vtx::ipos& pos, const img::rgba8& col) noexcept
		{
			if(pos.y >= 0 && pos.y < lines_.size()) {
				auto& l = lines_[pos.y];
				if(pos.x >= 0 && pos.x < l.size()) {
					l[pos.x].fc_ = col;
				}
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	バック・カラーの設定
			@param[in]	pos		位置
			@param[in]	col		カラー
		*/
		//-----------------------------------------------------------------//
		void set_back_color(const vtx::ipos& pos, const img::rgba8& col) noexcept
		{
			if(pos.y >= 0 && pos.y < lines_.size()) {
				auto& l = lines_[pos.y];
				if(pos.x >= 0 && pos.x < l.size()) {
					l[pos.x].bc_ = col;
				}
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	選択状態の設定
			@param[in]	pos		位置
			@param[in]	ena		非選択の場合は「false」
		*/
		//-----------------------------------------------------------------//
		void set_select(const vtx::ipos& pos, bool ena = true) noexcept
		{
			if(pos.y >= 0 && pos.y < lines_.size()) {
				auto& l = lines_[pos.y];
				if(pos.x >= 0 && pos.x < l.size()) {
					l[pos.x].select_ = ena;
				}
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	カーソル位置を取得
			@return カーソル位置
		*/
		//-----------------------------------------------------------------//
		const vtx::ipos& get_cursor() const noexcept { return pos_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	ライン数を取得
			@return ライン数
		*/
		//-----------------------------------------------------------------//
		uint32_t get_line_num() const noexcept { return lines_.size(); }


		//-----------------------------------------------------------------//
		/*!
			@brief	ラインを取得
			@param[in]	pos	ライン位置
			@return ライン
		*/
		//-----------------------------------------------------------------//
		const line& get_line(uint32_t pos) const noexcept { return lines_[pos]; }


		//-----------------------------------------------------------------//
		/*!
			@brief	ラスト・ラインを取得
			@return ラスト・ライン
		*/
		//-----------------------------------------------------------------//
		const line& get_last_line() const noexcept { return last_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	ラスト・テキストを取得 (UTF-32）
			@return ラスト・テキスト
		*/
		//-----------------------------------------------------------------//
		lstring get_last_text32() const noexcept
		{
			lstring ls;
			for(auto ch : last_) {
				ls += ch.cha_;
			}
			return ls;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	ラスト・テキストを取得
			@return ラスト・テキスト
		*/
		//-----------------------------------------------------------------//
		std::string get_last_text() const noexcept
		{
			return utf32_to_utf8(get_last_text32());
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	ラスト・キャラクターを取得
			@return ラスト・キャラクター
		*/
		//-----------------------------------------------------------------//
		const cha_t& get_last_cha() const noexcept { return cha_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	ラスト・キャラクターを取得
			@return ラスト・キャラクター
		*/
		//-----------------------------------------------------------------//
		uint32_t get_last_char() const noexcept { return cha_.cha_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	キャラクター・コンテナを取得
			@param[in]	pos	キャラクター位置
			@return キャラクター
		*/
		//-----------------------------------------------------------------//
		const cha_t& get_char(const vtx::ipos& pos) const noexcept
		{
			if(pos.y >= lines_.size()) return tmp_;
			const auto& l = lines_[pos.y];
			if(pos.x >= l.size()) return tmp_;
			return l[pos.x]; 
		}		
	};
}
