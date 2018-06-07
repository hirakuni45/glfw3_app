#pragma once
//=====================================================================//
/*!	@file
	@brief	ターミナル・クラス（ヘッダー）
    @author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2017 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/glfw_app/blob/master/LICENSE
*/
//=====================================================================//
#include <string>
#include <vector>
#include <deque>
#include <functional>
#include "img_io/img.hpp"
#include "utils/vtx.hpp"
#include "utils/string_utils.hpp"
#include "utils/format.hpp"
// #include <iostream>

namespace utils {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	ターミナル・クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	struct terminal {

		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		/*!
			@brief	文字トランク
		*/
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		struct cha_t {
			uint32_t	cha_;
			img::rgba8	fc_;
			img::rgba8	bc_;

			cha_t(uint32_t cha = 0,
				  const img::rgba8& fc = img::rgba8(255, 255, 255, 255),
				  const img::rgba8& bc = img::rgba8(  0,   0,   0, 255)) :
				cha_(cha), fc_(fc), bc_(bc) { }
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
		terminal(uint32_t max = 150) : cha_(), lines_(), max_(max), pos_(0), tmp_(' '),
			auto_crlf_(false), last_(), output_func_(nullptr)
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
			@brief	自動 CR/LF の設定
			@param[in]	f	自動機能を無効にする場合「false」
		*/
		//-----------------------------------------------------------------//
		void set_auto_crlf(bool f = true) { auto_crlf_ = f; }


		//-----------------------------------------------------------------//
		/*!
			@brief	消去
		*/
		//-----------------------------------------------------------------//
		void clear() {
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
		void clear_line(bool all = true) {
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
			@brief	文字出力関数設定
			@param[in]	func	関数
		*/
		//-----------------------------------------------------------------//
		void set_output_func(output_func func) {
			output_func_ = func;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	文字出力
			@param[in]	cha	文字
		*/
		//-----------------------------------------------------------------//
		void output(uint32_t cha) {

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
				if(pos_.x > 0) --pos_.x;
				break;
			case 0x11:  // Right
				{
//					line& l = lines_[pos_.y];
				}
				break;
			case 0x12:  // Left
				if(pos_.x > 0) --pos_.x;
				break;
			case 0x13:  // Down
				nl_();
				break;
			case 0x14:  // Up

				break;
			default:
				if(cha < 0x20) {
//				std::cout << boost::format("%02X") % cha << std::endl << std::flush;
				} else {
					line& l = lines_[pos_.y];
					if(pos_.x < l.size()) {
						l[pos_.x] = cha_;
						++pos_.x;
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
		void output(const std::string& str) {
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
		void output(const utils::lstring& str) {
			for(auto lch : str) {
				output(lch);
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	カーソル位置を取得
			@return カーソル位置
		*/
		//-----------------------------------------------------------------//
		const vtx::ipos& get_cursor() const { return pos_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	ライン数を取得
			@return ライン数
		*/
		//-----------------------------------------------------------------//
		uint32_t get_line_num() const { return lines_.size(); }


		//-----------------------------------------------------------------//
		/*!
			@brief	ラインを取得
			@param[in]	pos	ライン位置
			@return ライン
		*/
		//-----------------------------------------------------------------//
		const line& get_line(uint32_t pos) const { return lines_[pos]; }


		//-----------------------------------------------------------------//
		/*!
			@brief	ラスト・ラインを取得
			@return ラスト・ライン
		*/
		//-----------------------------------------------------------------//
		const line& get_last_line() const { return last_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	ラスト・テキストを取得 (UTF-32）
			@return ラスト・テキスト
		*/
		//-----------------------------------------------------------------//
		lstring get_last_text32() const {
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
		std::string get_last_text() const {
			return utf32_to_utf8(get_last_text32());
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	ラスト・キャラクターを取得
			@return ラスト・キャラクター
		*/
		//-----------------------------------------------------------------//
		const cha_t& get_last_cha() const { return cha_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	ラスト・キャラクターを取得
			@return ラスト・キャラクター
		*/
		//-----------------------------------------------------------------//
		uint32_t get_last_char() const { return cha_.cha_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	キャラクター・コンテナを取得
			@param[in]	pos	キャラクター位置
			@return キャラクター
		*/
		//-----------------------------------------------------------------//
		const cha_t& get_char(const vtx::ipos& pos) const {
			if(pos.y >= lines_.size()) return tmp_;
			const auto& l = lines_[pos.y];
			if(pos.x >= l.size()) return tmp_;
			return l[pos.x]; 
		}		
	};
}
