//=====================================================================//
/*!	@file
	@brief	OpenGL ターミナル・クラス
    @author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2017 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/glfw3_app/blob/master/LICENSE
*/
//=====================================================================//
#include <cstring>
#include <boost/foreach.hpp>
#include "gl_fw/glterminal.hpp"
#include "core/glcore.hpp"

using namespace std;
using namespace img;

namespace gl {

	//-----------------------------------------------------------------//
	/*!
		@brief	初期化
		@param[in]	w	横幅
		@param[in]	h	高さ
	 */
	//-----------------------------------------------------------------//
	void terminal::initialize(int w, int h)
	{
		core& core = core::get_instance();

		buff_.clear();
		limit_pos_.x = w;
		limit_pos_.y = h;
		buff_.resize(w * h);

		code c;
		c.cha = 0x0020;
		c.fore_color = fore_color_;
		c.back_color = back_color_;
		c.atr = attribute_;
		BOOST_FOREACH(code& cd, buff_) {
			cd = c;
		}

		// 半角文字中で一番広い場合の幅検出
		fonts& fonts = core.at_fonts();
		int max = 0;
		for(int i = 0x20; i < 128; ++i) {
			int ww = fonts.get_width(i);
			if(ww > max) max = ww;
		}
		font_size_.x = max;
		font_size_.y = fonts.get_height();
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	リサイズ
		@param[in]	w	横幅
		@param[in]	h	高さ
	*/
	//-----------------------------------------------------------------//
	void terminal::resize(int w, int h)
	{
		buff_.clear();
		limit_pos_.set(w, h);
		buff_.resize(w * h);
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	画面消去
	*/
	//-----------------------------------------------------------------//
	void terminal::clear()
	{
		if(limit_pos_.y > 1 && limit_pos_.x > 0) {
			code c;
			c.cha = 0x0020;
			c.fore_color = fore_color_;
			c.back_color = back_color_;
			c.atr = attribute_;
			BOOST_FOREACH(code& cd, buff_) {
				cd = c;
			}
		}
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	スクロール
	*/
	//-----------------------------------------------------------------//
	void terminal::scroll()
	{
		if(scroll_ == true && limit_pos_.y > 1 && limit_pos_.x > 0) {
			memcpy(&buff_[0], &buff_[limit_pos_.x],
				limit_pos_.x * (limit_pos_.y - 1) * sizeof(code));
			int o = (limit_pos_.y - 1) * limit_pos_.x;
			code& c = buff_[o];
			c.cha = 0x0020;
			c.fore_color = fore_color_;
			c.back_color = back_color_;
			c.atr = attribute_;
			memcpy(&buff_[o + 1], &buff_[o], (limit_pos_.x - 1) * sizeof(code));
		}
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	文字出力
		@param[in]	ch	UTF-16 文字コード
	*/
	//-----------------------------------------------------------------//
	void terminal::put(uint32_t ch)
	{
		if(ch < 0x20) {
			if(ch == 0x0a) {
				cursor_pos_.y++;
				if(cursor_pos_.y >= limit_pos_.y) {
					cursor_pos_.y = limit_pos_.y - 1;
					scroll();
				}
				cursor_pos_.x = 0;
			}
		} else {
			code& c = buff_[cursor_pos_.x + cursor_pos_.y * limit_pos_.x];
			c.cha = ch;
			c.fore_color = fore_color_;
			c.back_color = back_color_;
			c.atr = attribute_;

			++cursor_pos_.x;
			if(cursor_pos_.x >= limit_pos_.x) {
				cursor_pos_.x = 0;
				cursor_pos_.y++;
				if(cursor_pos_.y >= limit_pos_.y) {
					cursor_pos_.y = limit_pos_.y - 1;
					scroll();
				}
			}
		}
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	レンダリングサービス
	 */
	//-----------------------------------------------------------------//
	void terminal::service()
	{
		core& core = core::get_instance();
		fonts& fonts = core.at_fonts();

		fonts.set_fore_color(fore_color_);
		fonts.set_back_color(back_color_);

		for(int y = 0; y < limit_pos_.y; ++y) {
			int xx = 0;
			for(int x = 0; x < limit_pos_.x; ++x) {
				code& c = buff_[y * limit_pos_.x + x];
				if(cursor_ == true && cursor_pos_.x == x && cursor_pos_.y == y) {
					if((frame_count_ & 7) > 3) {
						c.cha = 0x007f;
					}
				}
				// プロポーショナルフォントを等幅で表示
				int kn = 0;
				int fw = fonts.get_width(c.cha);
				if(proportional_ == false) {
					if(c.cha < 0x80) {		// 半角文字
						kn = (font_size_.x - fw) / 2;
						fw = font_size_.x;
					} else {	// 全角文字
						kn = ((font_size_.x * 2) - fw) / 2;
						fw = font_size_.x * 2;
					}
				}
				vtx::spos pos(xx + kn, (limit_pos_.y - y - 1) * font_size_.y);
				fonts.draw(pos, c.cha);
				xx += fw;
			}
		}
		++frame_count_;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	廃棄
	*/
	//-----------------------------------------------------------------//
	void terminal::destroy()
	{
		codes().swap(buff_);
		limit_pos_.set(0, 0);
	}
}
