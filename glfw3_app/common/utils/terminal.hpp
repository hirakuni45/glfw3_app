#pragma once
//=====================================================================//
/*!	@file
	@brief	ターミナル・クラス（ヘッダー）
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include <string>
#include <vector>
#include <deque>
#include "img_io/img.hpp"
#include "utils/vtx.hpp"
#include "utils/string_utils.hpp"

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

	private:
		cha_t		cha_;

		uint32_t	crlf_;

		vtx::spos	max_;
		vtx::spos	pos_;

		uint32_t	line_max_;
		uint32_t	lines_max_;
		lines		lines_;

		const vtx::spos next_() const {
			vtx::spos p = pos_;
			if(lines_.empty()) {
				return p;
			}
			const line& l = lines_.back();
			if(!l.empty()) {
				++p.x;
			}
			return p;
		}

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief	コンストラクター
		*/
		//-----------------------------------------------------------------//
		terminal() :
			cha_(' ', img::rgba8(255, 255, 255, 255), img::rgba8(0, 0, 0, 255)),
			crlf_('\n'),
			max_(0), pos_(0),
			line_max_(500), lines_max_(200), lines_()
		{ }


		//-----------------------------------------------------------------//
		/*!
			@brief	デストラクター
		*/
		//-----------------------------------------------------------------//
		~terminal() { }


		//-----------------------------------------------------------------//
		/*!
			@brief	消去
		*/
		//-----------------------------------------------------------------//
		void clear() {
			max_.set(0);
			pos_.set(0);
			lines_.clear();
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	文字出力
			@param[in]	cha	文字
		*/
		//-----------------------------------------------------------------//
		void output(uint32_t cha) {
			if(cha == crlf_) {
				if(lines_.size() >= lines_max_) {
					lines_.pop_front();
				}
				if(lines_.size() < lines_max_) {
					line l;
					lines_.push_back(l);
				}
				pos_.x = 0;
				pos_.y = lines_.size() - 1;
				max_.y = std::max(max_.y, pos_.y);
			} else {
				if(lines_.empty()) {
					line l;
					lines_.push_back(l);
				}
				line& l = lines_.back();
				if(l.size() >= line_max_) {
					output(crlf_);
				}
				pos_.x = l.size();
				max_.x = std::max(max_.x, pos_.x);
				l.emplace_back(cha);
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	文字出力
			@param[in]	str	文字列
		*/
		//-----------------------------------------------------------------//
		void output(const std::string& str) {
			utils::lstring ls = utils::utf8_to_utf32(str);
			for(auto ch : ls) {
				output(ch);
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	カーソル位置を取得
			@return カーソル位置
		*/
		//-----------------------------------------------------------------//
		const vtx::spos get_cursor() const { return next_(); }


		//-----------------------------------------------------------------//
		/*!
			@brief	最大幅、高さを取得
			@return 最大幅、高さ
		*/
		//-----------------------------------------------------------------//
		const vtx::spos& get_max() const { return max_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	キャラクターを取得
			@param[in]	pos	位置
			@return キャラクターを参照で返す
		*/
		//-----------------------------------------------------------------//
		const cha_t& get_char(const vtx::spos& pos) const {
			if(pos.x < 0 || pos.y < 0) return cha_;
			if(pos.y >= lines_.size()) return cha_;
			const line& ln = lines_[pos.y];
			if(pos.x >= ln.size()) return cha_; 
			return ln[pos.x];
		}
	};
}
