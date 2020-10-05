#pragma once
//=====================================================================//
/*! @file
	@brief  スクリプト・イメージ・クラス @n
			簡単なスクリプトで、プリミティブを組み合わせて、画像を作成 @n
			Copyright 2020 Kunihito Hiramatsu
	@author 平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include <string>
#include <map>
#include "utils/file_io.hpp"
#include "utils/vtx.hpp"
#include "utils/arith.hpp"
#include "utils/format.hpp"
#include "img_io/paint.hpp"
#include "img_io/img_files.hpp"

namespace script {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	イメージ・クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	class image {

		uint32_t	size_x_;
		uint32_t	size_y_;

		typedef std::map<std::string, img::rgbaf> color_map;
		color_map	color_map_;

		enum class draw_type {
			none,
			box,
			box_scale_h,
			grid,
		};

		struct draw_t {
			draw_type	type_;
			std::string	color_;
			std::string	color2_;
			vtx::srect	area_;

			vtx::spos	grid_org_;
			vtx::spos	grid_size_;
			vtx::spos	grid_num_;
			vtx::spos	grid_line_;

			img::rgba8 get_color(color_map& map) const {
				auto v = map[color_];
				img::rgba8 c;
				c.r = static_cast<uint8_t>(v.r * 255.0f);
				c.g = static_cast<uint8_t>(v.g * 255.0f);
				c.b = static_cast<uint8_t>(v.b * 255.0f);
				c.a = static_cast<uint8_t>(v.a * 255.0f);
				return c;
			}

			img::rgba8 get_color2(color_map& map) const {
				auto v = map[color2_];
				img::rgba8 c;
				c.r = static_cast<uint8_t>(v.r * 255.0f);
				c.g = static_cast<uint8_t>(v.g * 255.0f);
				c.b = static_cast<uint8_t>(v.b * 255.0f);
				c.a = static_cast<uint8_t>(v.a * 255.0f);
				return c;
			}

			draw_t() : type_(draw_type::none), color_(), color2_(), area_(),
				grid_org_(), grid_size_(), grid_num_(), grid_line_()
			{ }
		};

		typedef std::vector<draw_t> DRAW_OPR;
		DRAW_OPR	draw_opr_;

		std::string	save_ext_;

		img::paint	paint_;

		img::img_files	img_files_;

		bool		verbose_;


		bool set_size_(const utils::strings& ss)
		{
			if(ss.size() != 3) return false;

			int32_t x;
			if(!utils::string_to_int(ss[1], x)) return false;
			int32_t y;
			if(!utils::string_to_int(ss[2], y)) return false;

			if(x >= 8 && x <= 2048 && y >= 8 && y <= 2048) {
				size_x_ = x;
				size_y_ = y;
				if(verbose_) {
					utils::format("size(%u, %u)\n") % size_x_ % size_y_;
				}
				return true;
			}

			return false;
		}


		bool set_color_(const utils::strings& ss)
		{
			if(ss.size() != 5) return false;

			float r;
			if(!utils::string_to_float(ss[2], r)) {
				return false;
			}
			float g;
			if(!utils::string_to_float(ss[3], g)) {
				return false;
			}
			float b;
			if(!utils::string_to_float(ss[4], b)) {
				return false;
			}
			if(r >= 0.0f && r <= 1.0f) ;
			else if(g >= 0.0f && g <= 1.0f) ;
			else if(b >= 0.0f && b <= 1.0f) ;
			else {
				return false;
			}

			img::rgbaf c(r, g, b);
			auto ret = color_map_.emplace(ss[1], c);
			if(ret.second) {
				utils::format("color(%12s, %3.2f, %3.2f, %3.2f)\n") % ss[1].c_str() % r % g % b;
			}
			return ret.second;
		}


		bool insert_draw_(const utils::strings& ss, const char* cmd, draw_type dt)
		{
			if(ss.size() != 6) return false;

			if(ss[0] != cmd) return false;

			draw_t t;
			t.type_ = dt;

			auto it = color_map_.find(ss[1]);
			if(it == color_map_.end()) {
				utils::format("color name undefined: '%s'\n") % ss[1].c_str();
				return false;
			}
			t.color_ = ss[1];

			int32_t org_x;
			if(!utils::string_to_int(ss[2], org_x)) return false;
			int32_t org_y;
			if(!utils::string_to_int(ss[3], org_y)) return false;
			int32_t size_x;
			if(!utils::string_to_int(ss[4], size_x)) return false;
			int32_t size_y;
			if(!utils::string_to_int(ss[5], size_y)) return false;

			t.area_.org.set(org_x, org_y);
			t.area_.size.set(size_x, size_y);

			draw_opr_.push_back(t);

			utils::format("%s(%s, %d, %d, %d, %d)\n") % ss[0].c_str() % ss[1].c_str()
				% org_x % org_y % size_x % size_y;

			return true;
		}


		bool insert_draw2_(const utils::strings& ss, const char* cmd, draw_type dt)
		{
			if(ss.size() != 7) return false;

			if(ss[0] != cmd) return false;

			draw_t t;
			t.type_ = dt;

			{
				auto it = color_map_.find(ss[1]);
				if(it == color_map_.end()) {
					utils::format("color name undefined: '%s'\n") % ss[1].c_str();
					return false;
				}
				t.color_ = ss[1];
			}
			{
				auto it = color_map_.find(ss[2]);
				if(it == color_map_.end()) {
					utils::format("color name undefined: '%s'\n") % ss[1].c_str();
					return false;
				}
				t.color2_ = ss[2];
			}

			int32_t org_x;
			if(!utils::string_to_int(ss[3], org_x)) return false;
			int32_t org_y;
			if(!utils::string_to_int(ss[4], org_y)) return false;
			int32_t size_x;
			if(!utils::string_to_int(ss[5], size_x)) return false;
			int32_t size_y;
			if(!utils::string_to_int(ss[6], size_y)) return false;

			t.area_.org.set(org_x, org_y);
			t.area_.size.set(size_x, size_y);

			draw_opr_.push_back(t);

			utils::format("%s(%s, %s, %d, %d, %d, %d)\n") % ss[0].c_str()
				% ss[1].c_str() % ss[2].c_str()
				% org_x % org_y % size_x % size_y;

			return true;
		}


		bool insert_grid_(const utils::strings& ss, const char* cmd, draw_type dt)
		{
			if(ss.size() != 10) return false;

			if(ss[0] != cmd) return false;

			draw_t t;
			t.type_ = dt;

			{
				auto it = color_map_.find(ss[1]);
				if(it == color_map_.end()) {
					utils::format("color name undefined: '%s'\n") % ss[1].c_str();
					return false;
				}
				t.color_ = ss[1];
			}

			int v;
			if(!utils::string_to_int(ss[2], v)) return false;
			t.grid_org_.x = v;
			if(!utils::string_to_int(ss[3], v)) return false;
			t.grid_org_.y = v;
			if(!utils::string_to_int(ss[4], v)) return false;
			t.grid_size_.x = v;
			if(!utils::string_to_int(ss[5], v)) return false;
			t.grid_size_.y = v;
			if(!utils::string_to_int(ss[6], v)) return false;
			t.grid_num_.x = v;
			if(!utils::string_to_int(ss[7], v)) return false;
			t.grid_num_.y = v;
			if(!utils::string_to_int(ss[8], v)) return false;
			t.grid_line_.x = v;
			if(!utils::string_to_int(ss[9], v)) return false;
			t.grid_line_.y = v;

			draw_opr_.push_back(t);

//			utils::format("%s(%s, %d, %d, %d, %d)\n") % ss[0].c_str() % ss[1].c_str()
//				% 

			return true;
		}


		bool set_output_(const utils::strings& ss)
		{
			if(ss.size() != 2) return false;

			if(ss[1] == "png") ;
			else if(ss[1] == "bmp") ;
			else if(ss[1] == "jpeg") ;
			else if(ss[1] == "jpg") ;
			else return false;

			save_ext_ = ss[1];

			return true;
		}


	public:
		//-------------------------------------------------------------//
		/*!
			@brief  コンストラクター
		*/
		//-------------------------------------------------------------//
		image() : size_x_(0), size_y_(0), verbose_(true) { }


		//-------------------------------------------------------------//
		/*!
			@brief  スクリプト・ファイルを開く
			@param[in]	file	ファイル名
			@return オープン出来たら「true」
		*/
		//-------------------------------------------------------------//
		bool open(const std::string& file) noexcept
		{
			utils::file_io fin;

			if(!fin.open(file, "rb")) {
				return false;
			}

			uint32_t lineno = 0;
			bool state = true;
			do {
				++lineno;
				auto lin = fin.get_line();
				auto ss = utils::split_text(lin, " \t");
				if(ss.size() >= 1) {
					if(ss[0][0] == '/' && ss[0][1]) { // comment
					} else {
						if(ss[0] == "size") {  // スクリーン・サイズ指定
							if(!set_size_(ss)) {
								utils::format("script(%u) '%s': size command fail\n")
									% lineno % lin.c_str();
								state = false;
							}
						} else if(ss[0] == "color") {  // カラー定義
							if(!set_color_(ss)) {
								utils::format("script(%u) '%s': color command fail\n")
									% lineno % lin.c_str();
								state = false;
							}
						} else if(ss[0] == "box") {  // box 描画
							if(!insert_draw_(ss, "box", draw_type::box)) {
								utils::format("script(%u) '%s': box command fail\n")
									% lineno % lin.c_str();
								state = false;
							}
						} else if(ss[0] == "box_scale_h") {  // box scale 描画
							if(!insert_draw2_(ss, "box_scale_h", draw_type::box_scale_h)) {
								utils::format("script(%u) '%s': box_scale_h command fail\n")
									% lineno % lin.c_str();
								state = false;
							}
						} else if(ss[0] == "grid") {  // grid 描画
							if(!insert_grid_(ss, "grid", draw_type::grid)) {
								utils::format("script(%u) '%s': grid command fail\n")
									% lineno % lin.c_str();
								state = false;
							}
						} else if(ss[0] == "output") {  // 出力形式
							if(!set_output_(ss)) {
								utils::format("script(%u) '%s': output command fail\n")
									% lineno % lin.c_str();
								state = false;
							}
						} else {
							utils::format("script(%u) '%s': command fail\n")
								% lineno % lin.c_str();
							state = false;
						}

//						utils::format("(%d)%s\n") % lineno % lin.c_str();
					}
					if(!state) break;
				}
			} while(!fin.eof()) ;

			fin.close();

			return state;
		}


		//-------------------------------------------------------------//
		/*!
			@brief  イメージ生成
			@return 作成出来たら「true」
		*/
		//-------------------------------------------------------------//
		bool create() noexcept
		{
			paint_.create(vtx::spos(size_x_, size_y_));

			for(auto t : draw_opr_) {

				auto c = t.get_color(color_map_);
				paint_.set_fore_color(c);

				switch(t.type_) {
				case draw_type::box:
					paint_.fill_rect(t.area_.org.x, t.area_.org.y, t.area_.size.x, t.area_.size.y);
					break;
				case draw_type::box_scale_h:
					{
						auto c = t.get_color2(color_map_);
						paint_.set_back_color(c);
					}
					paint_.fill_rect_scale_h(t.area_);
					break;
				case draw_type::grid:
					paint_.draw_grid(t.grid_org_, t.grid_size_, t.grid_num_, t.grid_line_);
					break;
				default:
					break;
				}
			}

			return true;			
		}


		//-------------------------------------------------------------//
		/*!
			@brief  イメージ・セーブ
			@param[in]	fname	ファイル名
			@return 作成出来たら「true」
		*/
		//-------------------------------------------------------------//
		bool save(const std::string& fname) noexcept
		{
			img::shared_img si(img::copy_image(&paint_));
			img_files_.set_image(si);

			return img_files_.save(fname, save_ext_);
		}
	};
}
