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

		std::string mode;
		if(option_[option::append]) mode = "ab";
		else mode = "wb";

		utils::file_io fio;
		if(!fio.open(out_fname_, mode)) {
			std::cerr << "Can't write open file: '" << out_fname_ << "'" << std::endl;
			return 0;
		}

		uint32_t n = 0;
		if(option_[option::text]) {
			utils::strings ss;
			utils::split_text(symbol_, ",", ss);
			std::string label;
			if(option_[option::c_style]) {
				if(ss.size() == 1) {
					label = "static const uint8_t " + ss[0] + "[] = {\n";
				} else if(ss.size() == 2) {
					label = "static const uint8_t " + ss[0] + "[] " + ss[1] + " = {\n";
				}
				fio.put(label);
			}
			for(uint32_t i = 0; i < bits_.byte_size(); ++i) {
				if((i % 16) == 0) {
					fio.put("    ");
				}
				fio.put((boost::format("0x%02x") % static_cast<uint32_t>(bits_.get_byte(i))).str());
				fio.put(",");
				if((i % 16) == 15) {
					fio.put("\n");
				}
			}
			if(option_[option::c_style]) {
				fio.put(" };");
			}
			fio.put("\n");
			n = fio.tell();
		} else {
			n = bits_.save_file(fio);
		}
		fio.close();
		return n;
	}


	void bmc_core::bitmap_convert_()
	{
		if(src_img_.empty()) return;

		if(option_[option::header] && header_size_) {
			bits_.put_bits(src_img_.get_size().x, header_size_);
			bits_.put_bits(src_img_.get_size().y, header_size_);
		}

		dst_img_.create(src_img_.get_size(), true);

		if(option_[option::dither]) {
			struct float_img {
				std::vector<float>	img_;
				int w_;
				int h_;
				bool clip_(int x, int y) const {
					if(static_cast<uint32_t>(x) >= w_) return false;
					if(static_cast<uint32_t>(y) >= h_) return false;
					return true;
				}
				void create(int w, int h) { w_ = w; h_ = h; img_.resize(w * h); }
				void put(int x, int y, float v) { if(clip_(x, y)) img_[y * w_ + x] = v; }
				void add(int x, int y, float v) { if(clip_(x, y)) img_[y * w_ + x] += v; }
				float get(int x, int y) const {
					if(clip_(x, y)) return img_[y * w_ + x];
					else return 0.0f;
				}
			} gray;
			int w = src_img_.get_size().x;
			int h = src_img_.get_size().y;
			gray.create(w, h);
			// Ditherring weight:
			// (Floyd-Steinberg)
			//       curr, 7/16
			// 3/16, 5/16, 1/16
			for(int y = 0; y < h; ++y) {
				for(int x = 0; x < w; ++x) {
					img::rgba8 c;
					src_img_.get_pixel(x, y, c);
					gray.put(x, y, static_cast<float>(c.getY()));
				}
			}
			for(int y = 0; y < h; ++y) {
				for(int x = 0; x < w; ++x) {
					float g = gray.get(x, y);
					bool f = false;
					float e;
					if(g > 127.0f) {
						gray.put(x, y, 255.0f);
						e = g - 255.0f;
						f = true;
					} else {
						gray.put(x, y, 0.0f);
						e = g;
					}
					gray.add(x + 1, y,     e * (7.0f/16.0f));
					gray.add(x - 1, y + 1, e * (3.0f/16.0f));
					gray.add(x + 0, y + 1, e * (5.0f/16.0f));
					gray.add(x + 1, y + 1, e * (1.0f/16.0f));

					if(option_[option::inverse]) f = !f;
					bits_.put_bit(f);
					img::rgba8 c;
					if(f) c.set(255, 255, 255, 255);
					else c.set(0, 0, 0, 255);
					dst_img_.put_pixel(x, y, c);
				}
			}
		} else {
			for(int y = 0; y < src_img_.get_size().y; ++y) {
				for(int x = 0; x < src_img_.get_size().x; ++x) {
					img::rgba8 c;
					src_img_.get_pixel(x, y, c);
					bool f = (c.getY() >= 128);
					if(option_[option::inverse]) f = !f;
					bits_.put_bit(f);
					if(f) c.set(255, 255, 255, 255);
					else c.set(0, 0, 0, 255);
					dst_img_.put_pixel(x, y, c);
				}
			}
		}
	}


	void bmc_core::bitmap_convert_(img::bdf_io& bdf)
	{
		if(option_[option::header]) {
			bits_.put_bits(bdf.get_width(), header_size_);
			bits_.put_bits(bdf.get_height(), header_size_);
		}

		// sjis に並んだものをそのまま出力
		for(uint32_t i = 0; i < bdf.size(); ++i) {
			bits_.put_bits(bdf.get_byte(i), 8);
		}

		// とりあえずプレビュー用に512ｘ512で画像を作成
		uint32_t n = bdf.size() / bdf.byte_size();
		dst_img_.create(vtx::spos(512), true);
		bits_.set_pos(16);
		for(uint32_t y = 0; y < 512; y += bdf.get_height()) {
			for(uint32_t x = 0; x < 512; x += bdf.get_width()) {
				for(uint32_t j = 0; j < bdf.get_height(); ++j) {
					for(uint32_t i = 0; i < bdf.get_width(); ++i) {
						bool f = bits_.get_bit();
						img::rgba8 c;
						if(f) c.set(255, 255, 255, 255);
						else c.set(0, 0, 0, 255);
						dst_img_.put_pixel(x + i, y + j, c);
					}
				}
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
					return false;
				}
				if(i == 0) pos.x = a.get_integer();
				else pos.y = a.get_integer();
			}
		} else {
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
		cout << "    " << cmd << " [options] in-file [out-file]" << endl;
		cout << "    -preview,-pre     preview image (OpenGL)" << endl;
		cout << "    -header size      output header" << endl;
		cout << "    -text             text base output" << endl;
		cout << "    -c-style symbol   C style table output" << endl;
		cout << "    -offset x,y       offset location" << endl;
		cout << "    -clip x,y         clipping area" << endl;
		cout << "    -bdf              BDF file input" << endl;
		cout << "    -append           append file" << endl;
		cout << "    -inverse          inverse mono color" << endl;
		cout << "    -dither           ditherring" << endl;
		cout << "    -verbose          verbose" << endl;
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
		bool header = false;
		bool symbol = false;
		bool offset = false;
		bool size = false;
		for(int i = 1; i < argc_; ++i) {
			string s = argv_[i];
			if(s[0] == '-') {
				if(s == "-preview") option_.set(option::preview);
				else if(s == "-pre") option_.set(option::preview);
				else if(s == "-verbose") option_.set(option::verbose);
				else if(s == "-header") { option_.set(option::header); header = true; }
				else if(s == "-text") option_.set(option::text);
				else if(s == "-c_style") { option_.set(option::c_style); symbol = true; }
				else if(s == "-offset") { option_.set(option::offset); offset = true; }
				else if(s == "-size") { option_.set(option::size); size = true; }
				else if(s == "-bdf") option_.set(option::bdf);
				else if(s == "-append") option_.set(option::append);
				else if(s == "-inverse") option_.set(option::inverse);
				else if(s == "-dither") option_.set(option::dither);
				else {
					no_err = false;
					cerr << "Option error: '" << s << "'" << endl;
				}
			} else {
				if(header) {
					utils::arith a;
 					if(!a.analize(s)) {
						cerr << "Option header error: '" << s << "'" << endl;
						return false;
					}
					header_size_ = a.get_integer();
					header = false;
				} else if(offset) {
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

		if(option_[option::bdf]) { // BDF ファイルの場合
			img::bdf_io bdf;

			bdf.initialize();

			if(!bdf.load(inp_fname_)) {
				cerr << "Can't decode BDF file: '" << inp_fname_ << "'" << endl;
				return false;
			}

			bitmap_convert_(bdf);

		} else { // 通常の画像ファイル
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
		}

		// ファイル出力
		uint32_t n = save_file_();

		if(option_[option::verbose]) {
			cout << "Source image size: " << src_img_.get_size().x << ", "
				<< src_img_.get_size().y << endl;
			if(option_[option::header]) {
				cout << "Output size header: " << header_size_ << endl;
			}
			if(option_[option::text]) {
				cout << "Text base output" << endl;
			}
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
			if(option_[option::bdf]) {
				cout << "BDF file input" << endl;
			}
			if(option_[option::append]) {
				cout << "Append file" << endl;
			}
			if(option_[option::dither]) {
				cout << "Ditherring" << endl;
			}
			cout << "Output size: " << n << " bytes" << endl;	
		}

		return true;
	}
}

