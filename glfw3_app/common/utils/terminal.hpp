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
			wchar_t		cha;
			img::rgba8	fc;
			img::rgba8	bc;

			cha_t() { }
			cha_t(wchar_t cha_, const img::rgba8& fc_, const img::rgba8& bc_) : cha(cha_), fc(fc_), bc(bc_) { } 
		};

	private:
		cha_t		cha_;

		typedef std::vector<cha_t>	chaers;
		chaers		chaers_;

		vtx::spos	size_;
		vtx::spos	cursor_;

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief	コンストラクター
		*/
		//-----------------------------------------------------------------//
		terminal() : cha_(' ', img::rgba8(255, 255, 255, 255), img::rgba8(0, 0, 0, 255)),
			chaers_(),
			size_(0), cursor_(0) { }


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
			chaers tmp;
			if(!destroy) {
				tmp.swap(chaers_);
			}
			cha_t ch;
			ch = cha_;
			ch.cha = ' ';
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

			cha_.cha = ' ';
			for(uint32_t x = pos.x; x < size_.x; ++x) {
				chaers_[pos.y * size_.x + x] = cha_;
			}			
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
			cha_.cha = ' ';
			for(uint32_t y = rect.org.y; y < rect.end_y(); ++y) {
				for(uint32_t x = rect.org.x; x < rect.end_x(); ++x) {
					chaers_[y * size_.x + x] = cha_;
				}
			}
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
			for(uint32_t y = 1; y < size_.y; ++y) {
				for(uint32_t x = 0; x < size_.x; ++x) {
					chaers_[(y - 1) * size_.x + x] = chaers_[y * size_.x + x];
				}
			}
			clear_line(vtx::spos(0, size_.y - 1));
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	文字出力
			@param[in]	cha	文字
		*/
		//-----------------------------------------------------------------//
		void output(wchar_t cha) {
			if(chaers_.empty()) return;
			cha_.cha = cha;
			chaers_[cursor_.y * size_.x + cursor_.x] = cha_;
			++cursor_.x;
			if(cursor_.x >= size_.x) {
				cursor_.x = 0;
				++cursor_.y;
				if(cursor_.y >= size_.y) {
					cursor_.y = size_.y - 1;
					scroll();
				}
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
			return chaers_[pos.y * size_.x + pos.x];
		}
	};
}
