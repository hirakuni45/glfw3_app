#pragma once
//=====================================================================//
/*!	@file
	@brief	ターミナル・クラス（ヘッダー）
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include <string>
#include <vector>
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

	private:
		cha_t		cha_;

		vtx::spos	size_;
		vtx::spos	cursor_;

		typedef std::vector<cha_t>	line;
		typedef std::vector<line>	lines;
		line		line_;
		uint32_t	line_pos_;
		uint32_t	line_max_;
		lines		lines_;

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief	コンストラクター
		*/
		//-----------------------------------------------------------------//
		terminal() :
			cha_(' ', img::rgba8(255, 255, 255, 255), img::rgba8(0, 0, 0, 255)),
			size_(0), cursor_(0),
			line_(), line_pos_(0), line_max_(200), lines_() { }


		//-----------------------------------------------------------------//
		/*!
			@brief	デストラクター
		*/
		//-----------------------------------------------------------------//
		~terminal() { }


		//-----------------------------------------------------------------//
		/*!
			@brief	サイズを取得
			@return サイズ
		*/
		//-----------------------------------------------------------------//
		const vtx::spos& size() const { return size_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	リサイズ
			@param[in]	size	サイズ
			@param[in]	destroy	以前のデータを廃棄する場合「true」
		*/
		//-----------------------------------------------------------------//
		void resize(const vtx::spos& size, bool destroy = false) {
#if 0
			chaers tmp;
			if(!destroy) {
				tmp.swap(chaers_);
			}
			cha_t ch;
			ch = cha_;
			ch.cha_ = ' ';
			chaers_.resize(size.x * size.y, ch);
			if(!destroy && !tmp.empty()) {
				uint32_t xx = size.x < size_.x ? size.x : size_.x;
				uint32_t yy = size.y < size_.y ? size.y : size_.y;
				for(uint32_t y = 0; y < yy; ++y) {
					for(uint32_t x = 0; x < xx; ++x) {
						chaers_[y * size.x + x] = tmp[y * size_.x + x];
					}
				}
			}
			size_ = size;
			if(cursor_.x >= size_.x) cursor_.x = size_.x - 1;
			if(cursor_.y >= size_.y) cursor_.y = size_.y - 1;
#endif
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	ライン消去
			@param[in]	pos	開始位置
		*/
		//-----------------------------------------------------------------//
		void clear_line(const vtx::spos& pos) {
			if(pos.x < 0 || pos.x >= size_.x) return;
			if(pos.y < 0 || pos.y >= size_.y) return;
#if 0
			cha_.cha_ = ' ';
			for(int x = pos.x; x < size_.x; ++x) {
				chaers_[pos.y * size_.x + x] = cha_;
			}
#endif
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	領域消去
			@param[in]	rect	領域
		*/
		//-----------------------------------------------------------------//
		void clear(const vtx::srect& rect) {
			if(rect.org.x < 0 || rect.org.x >= size_.x) return;
			if(rect.org.y < 0 || rect.org.y >= size_.y) return;
			if(rect.end_x() >= size_.x) return;
			if(rect.end_y() >= size_.y) return;
			cha_.cha_ = ' ';
#if 0
			for(int y = rect.org.y; y < rect.end_y(); ++y) {
				for(int x = rect.org.x; x < rect.end_x(); ++x) {
					chaers_[y * size_.x + x] = cha_;
				}
			}
#endif
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	消去
		*/
		//-----------------------------------------------------------------//
		void clear() { clear(vtx::srect(vtx::spos(0), size_)); }


		//-----------------------------------------------------------------//
		/*!
			@brief	スクロール
		*/
		//-----------------------------------------------------------------//
		void scroll() {
#if 0
			for(int y = 1; y < size_.y; ++y) {
				for(int x = 0; x < size_.x; ++x) {
					chaers_[(y - 1) * size_.x + x] = chaers_[y * size_.x + x];
				}
			}
			clear_line(vtx::spos(0, size_.y - 1));
#endif
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	ライン・フィード
		*/
		//-----------------------------------------------------------------//
		void line_feed() {
			++cursor_.y;
			if(cursor_.y >= size_.y) {
				cursor_.y = size_.y - 1;
				scroll();
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	文字出力
			@param[in]	cha	文字
		*/
		//-----------------------------------------------------------------//
		void output(uint32_t cha) {
			if(cha == '\n') {
				if(lines_.size() < line_max_) {
					lines_.push_back(line_);
					line_.clear();
				}
			} else {
				line_.emplace_back(cha);
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
		const vtx::spos& cursor() const { return cursor_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	カーソル位置を設定
			@param[in]	pos	カーソル位置
			@return 設定出来たら「true」
		*/
		//-----------------------------------------------------------------//
		bool set_cursor(const vtx::spos& pos) {
			if(pos.x < 0 || pos.x >= size_.x || pos.y < 0 || pos.y >= size_.y) return false;
			cursor_ = pos;
			return true;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	キャラクターを取得
		*/
		//-----------------------------------------------------------------//
		const cha_t& get_char(const vtx::spos& pos) const {
			if(pos.x < 0 || pos.x >= size_.x || pos.y < 0 || pos.y >= size_.y) {
				static cha_t zero(0, img::rgba8(0, 0), img::rgba8(0, 0)); 
				return zero;
			}
///			return chaers_[pos.y * size_.x + pos.x];
			return cha_;
		}
	};
}
