//=====================================================================//
/*! @file
	@brief  BMC コア関係
	@author 平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include "bmc_core.hpp"
#include <boost/foreach.hpp>
#include <boost/format.hpp>
#include <cstring>

namespace app {

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
		cout << "	-inverse           inverse mono color" << endl;
//		cout << "	-bdf               BDF file input" << endl;
//		cout << "	-dither            Ditherring(50%)" << endl;
//		cout << "	-no-header         No-Header(Width/Height) output" << endl;
//		cout << "	-clip-x start len  Clipping X-line" << endl;
//		cout << "	-clip-y	start len  Clipping Y-line" << endl;
//		cout << "	-append            Append output file" << endl;
		cout << "	-verbose           verbose" << endl;
		cout << endl;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief  verbose 表示
	*/
	//-----------------------------------------------------------------//
	void bmc_core::verbose() const
	{
		using namespace std;

		cout << "Input File:  '" << inp_fname_ << "'" << endl;
		cout << "Output File: '" << out_fname_ << "'" << endl;

		for(uint32_t i = 0; i < option::limit_; ++i) {
		}
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
		for(int i = 1; i < argc_; ++i) {
			string s = argv_[i];
			if(s[0] == '-') {
				if(s == "-preview") option_.set(option::preview);
				else if(s == "-pre") option_.set(option::preview);
				else if(s == "-verbose") option_.set(option::verbose);
				else if(s == "-inverse") option_.set(option::inverse);
#if 0
				else if(strcmp(p, "-true-color")==0) true_color = true;
				else if(strcmp(p, "-bdf")==0) { bdf_type = true; png_type = false; }
				else if(strcmp(p, "-dither")==0) dither = true;
				else if(strcmp(p, "-no-header")==0) header = false;
				else if(strcmp(p, "-clip-x")==0) { clipx = true;  clipy = false; }
				else if(strcmp(p, "-clip-y")==0) { clipy = false; clipy = true; }
				else if(strcmp(p, "-append")==0) { append = true; }
#endif
				else {
					no_err = false;
					cerr << "Option error: '" << s << "'" << endl;
				}
			} else {
#if 0
				if(clipx == true) {
					area.x = area.w;
					if(Arith((const char *)p, &area.w) != 0) {
						clipx = false;
					}
					if(area.x >= 0 && area.w > 0) clipx = false;
				} else if(clipy == true) {
					area.y = area.h;
					if(Arith((const char *)p, &area.h) != 0) {
						clipy = false;
					}
					if(area.y >= 0 && area.h > 0) clipy = false;
				} else {
#endif
				inp_fname_ = out_fname_;
				out_fname_ = s;
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

