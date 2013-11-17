//=====================================================================//
/*! @file
	@brief  BMC コア関係
	@author 平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include "bmc_core.hpp"
#include <boost/foreach.hpp>
#include <boost/format.hpp>
#include "img_io/img_files.hpp"
#include "img_io/img_utils.hpp"
#include "utils/arith.hpp"
#include "utils/string_utils.hpp"

namespace app {

	uint32_t bmc_core::save_file_()
	{
		if(out_fname_.empty()) {
			return 0;
		}

		uint32_t n = 0;
		if(option_[option::c_style]) {
			utils::file_io fio;
			if(fio.open(out_fname_, "wb")) {
				utils::strings ss;
				utils::split_text(symbol_, ",", ss);
				std::string label;
				if(ss.size() == 1) {
					label = "static const uint8_t " + ss[0] + "[] = {\n";
				} else if(ss.size() == 2) {
					label = "static const uint8_t " + ss[0] + "[] " + ss[1] + " = {\n";
				}
				fio.put(label);
				for(uint32_t i = 0; i < bits_.byte_size(); ++i) {
					if(i) {
						fio.put(",");
						if((i % 16) == 0) {
							fio.put("\n");
						}
					}
					if((i % 16) == 0) {
						fio.put("    ");
					}
					fio.put((boost::format("0x%02x") % static_cast<uint32_t>(bits_.get_byte(i))).str());
				}
				fio.put(" };\n");
				n = fio.tell();
				fio.close();
			} else {
				std::cerr << "Can't write open file: '" << out_fname_ << "'" << std::endl;
			}
		} else {
			n = bits_.save_file(out_fname_);
		}
		return n;
	}

	void bmc_core::bitmap_convert_()
	{
		if(src_img_.empty()) return;

		if(!option_[option::no_header]) {
			bits_.put_bits(src_img_.get_size().x, 8);
			bits_.put_bits(src_img_.get_size().y, 8);
		}

		dst_img_.create(src_img_.get_size(), true);

		for(int y = 0; y < src_img_.get_size().y; ++y) {
			for(int x = 0; x < src_img_.get_size().x; ++x) {
				img::rgba8 c;
				src_img_.get_pixel(x, y, c);
				bool f = (c.getY() >= 128);
				bits_.put_bit(f);
				if(f) c.set(255, 255, 255, 255);
				else c.set(0, 0, 0, 255);
				dst_img_.put_pixel(x, y, c);
			}
		}
	}


	static bool scan_pos_(const std::string& str, vtx::spos& pos)
	{
		utils::strings ss;
		utils::split_text(str, ",", ss);
		if(ss.size() == 2) {
			for(int i = 0; i < 2; ++i) {
				utils::arith a;
 				if(!a.analize(ss[i])) {
					std::cerr << "Number error: '" << str << "'" << std::endl;
					return false;
				}
				if(i == 0) pos.x = a.get_integer();
				else pos.y = a.get_integer();
			}
		} else {
			std::cerr << "Number error: '" << str << "'" << std::endl;
			return false;
		}
		return true;
	}


	// タイトルの表示と簡単な説明
	void bmc_core::help() const
	{
		using namespace std;

		cout << "BitMap Converter" << endl;
		cout << "Copyright (C) 2013, Hiramatsu Kunihito" << endl;
		cout << "Version " << boost::format("%1.2f") % version_ << endl;
		cout << "usage:" << endl;
		string cmd;
		const char* p = strrchr(argv_[0], '\\');
		if(p) { cmd = p + 1; }
		cout << "	" << cmd << " [options] in-file [out-file]" << endl;
		cout << "	-preview -pre      preview image (OpenGL)" << endl;
		cout << "	-no-header         no output size header" << endl;
		cout << "	-c-style symbol    C style table output" << endl;
		cout << "	-offset x,y        Offset location" << endl;
		cout << "	-clip	x,y        Clipping length" << endl;
//		cout << "	-inverse           inverse mono color" << endl;
//		cout << "	-bdf               BDF file input" << endl;
//		cout << "	-dither            Ditherring(50%)" << endl;
		cout << "	-verbose           verbose" << endl;
		cout << endl;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief  コマンドライン解析
		@return エラーが無ければ「true」
	*/
	//-----------------------------------------------------------------//
	bool bmc_core::analize()
	{
		using namespace std;

		bool no_err = true;
		bool symbol = false;
		bool offset = false;
		bool size = false;
		for(int i = 1; i < argc_; ++i) {
			string s = argv_[i];
			if(s[0] == '-') {
				if(s == "-preview") option_.set(option::preview);
				else if(s == "-pre") option_.set(option::preview);
				else if(s == "-verbose") option_.set(option::verbose);
				else if(s == "-no-header") option_.set(option::no_header);
				else if(s == "-c_style") { option_.set(option::c_style); symbol = true; }
				else if(s == "-offset") { option_.set(option::offset); offset = true; }
				else if(s == "-size") { option_.set(option::size); size = true; }
//				else if(s == "-inverse") option_.set(option::inverse);
//				else if(s == "-bdf") option_.set(option::bdf_type);
//				else if(s == "-dither") option_.set(option::dither);
				else {
					no_err = false;
					cerr << "Option error: '" << s << "'" << endl;
				}
			} else {
				if(offset) {
					if(!scan_pos_(s, clip_.org)) {
						cerr << "Option offset error: '" << s << "'" << endl;
					}
					offset = false;
				} else if(size) {
					if(!scan_pos_(s, clip_.size)) {
						cerr << "Option size error: '" << s << "'" << endl;
					}
					size = false;
				} else if(symbol) {
					symbol_ = s;
					symbol = false;
				} else {
					inp_fname_ = out_fname_;
					out_fname_ = s;
				}
			}
		}

		if(inp_fname_.empty()) {
			inp_fname_ = out_fname_;
			out_fname_.clear();
		}

		if(!no_err) {
			cerr << endl;
		}
		return no_err;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief  実行
		@return エラーが無ければ「true」
	*/
	//-----------------------------------------------------------------//
	bool bmc_core::execute()
	{
		using namespace std;

		if(option_[option::verbose]) {
			cout << "Input File:  '" << inp_fname_ << "'" << endl;
			cout << "Output File: '" << out_fname_ << "'" << endl;
		}

		if(inp_fname_.empty()) {
			cerr << "Input file empty..." << endl;
			return false;
		}
		img::img_files	imfs;
		imfs.initialize();

		if(!imfs.load(inp_fname_)) {
			cerr << "Can't load source image: " << inp_fname_ << "'" << endl;
			return false;
		}

		// ソース画像をコピー
		vtx::srect sr(vtx::spos(0), imfs.get_image_if()->get_size());
		if(option_[option::offset]) {
			sr.org = clip_.org;
		}
		if(option_[option::size]) {
			sr.size = clip_.size;
		}
		src_img_.create(sr.size, true);
		img::copy_to_rgba8(imfs.get_image_if(), sr.org.x, sr.org.y, sr.size.x, sr.size.y,
			src_img_, 0, 0); 

		// モノクロ変換
		bitmap_convert_();

		// ファイル出力
		uint32_t n = save_file_();
		if(option_[option::verbose]) {
			cout << "Source image size: " << src_img_.get_size().x << ", "
				<< src_img_.get_size().y << endl;
			if(option_[option::c_style]) {
				cout << "C-Style output symbol: '" << symbol_ << "'" << endl; 
			}
			if(option_[option::offset]) {
				cout << "Offset: " << static_cast<int>(clip_.org.x) << " ,"
					<< static_cast<int>(clip_.org.y) << endl;
			}
			if(option_[option::size]) {
				cout << "Size: " << static_cast<int>(clip_.size.x) << " ,"
					<< static_cast<int>(clip_.size.y) << endl;
			}
			cout << "Output size: " << n << " bytes" << endl;		
		}

		return true;
	}


#if 0
static void set_bitmap12x12(const unsigned char* bitmap12, int x, int y, img& image)
{
	int pos = 0;
	unsigned char bits = *bitmap12++;
	for(int j = 0; j < 12; ++j) {
		for(int i = 0; i < 12; ++i) {
			img::rgba c;
			if(bits & 1) c.set(255, 255, 255);
			else c.set(0, 0, 0);
			image.put_pixel(x + i, y + j, c);
			bits >>= 1;
			++pos;
			pos &= 7;
			if(pos == 0) bits = *bitmap12++;
		}
	}
}


//-----------------------------------------------------------------//
/*!
	@breif	main 関数
	@param[in]	argc	コマンド入力パラメーターの数	
	@param[in]	argv	コマンド入力パラメーターのリスト
	@return	常に０
*/
//-----------------------------------------------------------------//
int main(int argc, char *argv[])
{
// Verbose
	if(verbose == true) {
		printf("Input image file:  '%s'\n", in_fname);
		printf("Output image file: '%s'\n", out_fname);
		printf("Clipping X location(start, length): %d, %d\n", area.x, area.w);
		printf("Clipping Y location(start, length): %d, %d\n", area.y, area.h);
	}

	bool file_read = true;
	img	image;
	bdfio bdf;

	if(bdf_type) {
		if(file_read == true && out_fname != NULL) {
			bdf.save(out_fname);
		}

		if(file_read == true && preview == true) {
			image.create(256, 256);

			const unsigned char* p = bdf.get_array();
			p += 18 * 300;
			for(int y = 0; y < 256; y += 16) {
				for(int x = 0; x < 256; x += 16) {
					set_bitmap12x12(p, x, y, image);
					p += 18;
				}
			}
			bitio	bitfo;
			int		num;
			if(dither == true) num = 3;
			else num = 2;
			render_mono_img(image, num, inverse, bitfo, area);
		}

	} else if(true_color == true) {
		render_true_img(image);
	} else {
		bitio	bitfo;
		int		num;
		if(dither == true) num = 3;
		else num = 2;

		if(header == true) {
			int	w = image.get_width();
			int	h = image.get_height();
			if(area.w > 0) w = area.w;
			if(area.h > 0) h = area.h;
			bitfo.put_bits(w, 8);
			bitfo.put_bits(h, 8);
		}
		render_mono_img(image, num, inverse, bitfo, area);

// バイナリーファイル出力
		if(out_fname != NULL) {
			if(append == true) {
				bitfo.save_append_file((const char *)out_fname);
			} else {
				bitfo.save_file((const char *)out_fname);
			}
		}
	}

	if(file_read == true && preview == true) {
		preview_init(&argc, argv);
		preview_main();
	}

	return 0;
}
#endif

}

