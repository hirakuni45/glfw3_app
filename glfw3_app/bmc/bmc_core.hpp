#pragma once
//=====================================================================//
/*! @file
	@brief  BMC コア関係
	@author 平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include <bitset>
#include <boost/format.hpp>
#include "utils/string_utils.hpp"
#include "utils/bit_array.hpp"
#include "img_io/img_files.hpp"
#include "img_io/img_utils.hpp"
#include "img_io/img_rgba8.hpp"
#include "img_io/bdf_io.hpp"
#include "utils/arith.hpp"
#include "utils/format.hpp"

#include "script_img.hpp"

namespace app {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  BMC コア・クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	class bmc_core {
	public:

		struct option {
			enum type {
				preview,	///< プレビューを有効
				verbose,	///< 詳細なメッセージ出力
				header,		///< サイズヘッダー出力
				text,		///< テキストベース出力
				c_style,	///< C99 スタイルのテキスト出力
				cpp_style,	///< C++11 スタイルのテキスト出力
				offset,		///< オフセット
				size,		///< サイズ
				bdf,		///< BDF ファイル入力
				append,		///< 追加出力
				inverse,	///< 画像反転
				dither,		///< ディザリング
				compress4,	///< RLE4 圧縮フォーマット
				script,		///< 描画スクリプトファイル

				limit_
			};
		};

	private:
		static const uint32_t bdf_prev_x_ = 32;
		static const uint32_t bdf_prev_y_ = 32;

		std::bitset<option::limit_>	option_;
		std::string	inp_fname_;
		std::string out_fname_;
		uint32_t	header_size_;
		std::string	symbol_;
		vtx::srect	clip_;

		float		version_;

		img::img_rgba8	src_img_;
		img::img_rgba8	dst_img_;

		utils::bit_array	bits_;

		uint32_t		bdf_num_;
		uint32_t		bdf_pages_;
		vtx::spos		bdf_fsize_;


		void bitmap_convert_()
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
					vtx::spos	size_;

					bool clip_(const vtx::spos& p) const {
						if(static_cast<uint16_t>(p.x) >= static_cast<uint16_t>(size_.x)) {
							return false;
						}
						if(static_cast<uint16_t>(p.y) >= static_cast<uint16_t>(size_.y)) {
							return false;
						}
						return true;
					}

					void create(const vtx::spos& size) {
						size_ = size; img_.resize(size_.x * size_.y);
					}

					void put(const vtx::spos& pos, float v) {
						if(clip_(pos)) img_[pos.y * size_.x + pos.x] = v;
					}

					void add(const vtx::spos& pos, float v) {
						if(clip_(pos)) img_[pos.y * size_.x + pos.x] += v;
					}

					float get(const vtx::spos& pos) const {
						if(clip_(pos)) return img_[pos.y * size_.x + pos.x];
						else return 0.0f;
					}
				} gray;

				gray.create(src_img_.get_size());
				// Ditherring weight:
				// (Floyd-Steinberg)
				//       curr, 7/16
				// 3/16, 5/16, 1/16
				vtx::spos pos;
				for(pos.y = 0; pos.y < src_img_.get_size().y; ++pos.y) {
					for(pos.x = 0; pos.x < src_img_.get_size().x; ++pos.x) {
						img::rgba8 c;
						src_img_.get_pixel(pos, c);
						gray.put(pos, static_cast<float>(c.getY()));
					}
				}
				for(pos.y = 0; pos.y < src_img_.get_size().y; ++pos.y) {
					for(pos.x = 0; pos.x < src_img_.get_size().x; ++pos.x) {
						float g = gray.get(pos);
						bool f = false;
						float e;
						if(g > 127.0f) {
							gray.put(pos, 255.0f);
							e = g - 255.0f;
							f = true;
						} else {
							gray.put(pos, 0.0f);
							e = g;
						}
						gray.add(vtx::spos(pos.x + 1, pos.y + 0), e * (7.0f/16.0f));
						gray.add(vtx::spos(pos.x - 1, pos.y + 1), e * (3.0f/16.0f));
						gray.add(vtx::spos(pos.x + 0, pos.y + 1), e * (5.0f/16.0f));
						gray.add(vtx::spos(pos.x + 1, pos.y + 1), e * (1.0f/16.0f));

						if(option_[option::inverse]) f = !f;
						bits_.put_bit(f);
						img::rgba8 c;
						if(f) c.set(255, 255, 255, 255);
						else c.set(0, 0, 0, 255);
						dst_img_.put_pixel(pos, c);
					}
				}
			} else {
				vtx::spos pos;
				for(pos.y = 0; pos.y < src_img_.get_size().y; ++pos.y) {
					for(pos.x = 0; pos.x < src_img_.get_size().x; ++pos.x) {
						img::rgba8 c;
						src_img_.get_pixel(pos, c);
						bool f = (c.getY() >= 128);
						if(option_[option::inverse]) f = !f;
						bits_.put_bit(f);
						if(f) c.set(255, 255, 255, 255);
						else c.set(0, 0, 0, 255);
						dst_img_.put_pixel(pos, c);
					}
				}
			}
		}


		void bitmap_convert_(img::bdf_io& bdf)
		{
			if(option_[option::header]) {
				bits_.put_bits(bdf.get_width(), header_size_);
				bits_.put_bits(bdf.get_height(), header_size_);
			}

			// sjis に並んだものをそのまま出力
			for(uint32_t i = 0; i < bdf.size(); ++i) {
				bits_.put_bits(bdf.get_byte(i), 8);
			}

			// 最大ページ数を求めておく
			bdf_num_ = bdf.size() / bdf.byte_size();	// 全フォント数
			bdf_pages_ = bdf_num_ / (bdf_prev_x_ * bdf_prev_y_);
			if(bdf_num_ % (bdf_prev_x_ * bdf_prev_y_)) ++bdf_pages_;

			bdf_fsize_.set(bdf.get_width(), bdf.get_height());

			create_bdf_image(0);
		}


		uint32_t save_file_()
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
				utils::strings ss = utils::split_text(symbol_, ",");
				std::string label;
				if(option_[option::c_style]) {
					if(ss.size() == 1) {
						label = "static const uint8_t " + ss[0] + "[] = {\n";
					} else if(ss.size() == 2) {
						label = "static const uint8_t " + ss[0] + "[] " + ss[1] + " = {\n";
					}
					fio.put(label);
				} else if(option_[option::cpp_style]) {
					if(ss.size() == 1) {
						label = "static constexpr uint8_t " + ss[0] + "[] = {\n";
					} else if(ss.size() == 2) {
						label = "static constexpr uint8_t " + ss[0] + "[] " + ss[1] + " = {\n";
					}
					fio.put(label);
				}
				for(uint32_t i = 0; i < bits_.byte_size(); ++i) {
					if((i % 16) == 0) {
						fio.put("    ");
					}
					fio.put(
						(boost::format("0x%02x") % static_cast<uint32_t>(bits_.get_byte(i))).str()
					);
					fio.put(",");
					if((i % 16) == 15) {
						fio.put("\n");
					}
				}
				if(option_[option::c_style] || option_[option::cpp_style]) {
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


		static bool scan_pos_(const std::string& str, vtx::spos& pos)
		{
			utils::strings ss = utils::split_text(str, ",");
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


	public:
		//-----------------------------------------------------------------//
		/*!
			@brief  コンストラクター
		*/
		//-----------------------------------------------------------------//
		bmc_core() :
			header_size_(0), clip_(0),
			version_(1.05f), bdf_num_(0), bdf_pages_(0), bdf_fsize_(0) { }


		//-----------------------------------------------------------------//
		/*!
			@brief  help 表示
			@param[in]	cmd	コマンド文字列
		*/
		//-----------------------------------------------------------------//
		void help(const char* cmd) const
		{
			using namespace std;

			utils::format("BitMap Converter\n");
			utils::format("Copyright (C) 2013, 2020, Hiramatsu Kunihito\n");
			utils::format("Version %3.2f\n") % version_;
			utils::format("usage:\n");
			auto c = utils::get_file_base(cmd);
			cout << "    " << c << " [options] in-file [out-file]" << endl;
			cout << "    -preview,-pre     preview image (OpenGL)" << endl;
			cout << "    -header bits      output width,height" << endl;
			cout << "    -text             text base output" << endl;
			cout << "    -c_style symbol   C style table output (const)" << endl;
			cout << "    -cpp_style symbol C++ style table output (constexpr)" << endl;
			cout << "    -offset x,y       offset location" << endl;
			cout << "    -size x,y         clipping size" << endl;
			cout << "    -bdf              BDF file input" << endl;
			cout << "    -append           append file" << endl;
			cout << "    -inverse          inverse mono color" << endl;
			cout << "    -dither           ditherring" << endl;
			cout << "    -compress4        RLE4 compress" << endl;
			cout << "    -script           render SCRIPT file input" << endl;
			cout << "    -verbose          verbose" << endl;
			cout << endl;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  コマンドライン解析
			@return エラーが無ければ「true」
		*/
		//-----------------------------------------------------------------//
		bool analize(int argc, char** argv)
		{
			using namespace std;

			bool no_err = true;
			bool header = false;
			bool symbol = false;
			bool offset = false;
			bool size = false;
			for(int i = 1; i < argc; ++i) {
				string s = argv[i];
				if(s[0] == '-') {
					if(s == "-preview") option_.set(option::preview);
					else if(s == "-pre") option_.set(option::preview);
					else if(s == "-verbose") option_.set(option::verbose);
					else if(s == "-header") { option_.set(option::header); header = true; }
					else if(s == "-text") option_.set(option::text);
					else if(s == "-c_style") { option_.set(option::c_style); symbol = true; }
					else if(s == "-cpp_style") { option_.set(option::cpp_style); symbol = true; }
					else if(s == "-offset") { option_.set(option::offset); offset = true; }
					else if(s == "-size") { option_.set(option::size); size = true; }
					else if(s == "-bdf") option_.set(option::bdf);
					else if(s == "-append") option_.set(option::append);
					else if(s == "-inverse") option_.set(option::inverse);
					else if(s == "-dither") option_.set(option::dither);
					else if(s == "-compress4") option_.set(option::compress4);
					else if(s == "-script") option_.set(option::script);
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
		bool execute()
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

			} else if(option_[option::script]) { // スクリプト・ファイルの場合
				script::image simg;

				if(!simg.open(inp_fname_)) {
					cerr << "Can't decode SCRIPT file: '" << inp_fname_ << "'" << endl;
					return false;
				}

				simg.create();

				if(!simg.save(out_fname_)) {
					cerr << "Can't save SCRIPT/output file: '" << out_fname_ << "'" << endl;
					return false;
				}

				return true;
				
			} else { // 通常の画像ファイル
				img::img_files	imfs;

				if(!imfs.load(inp_fname_)) {
					cerr << "Can't load source image: " << inp_fname_ << "'" << endl;
					return false;
				}

				// ソース画像をコピー
				vtx::srect sr(vtx::spos(0), imfs.get_image()->get_size());
				if(option_[option::offset]) {
					sr.org = clip_.org;
				}
				if(option_[option::size]) {
					sr.size = clip_.size;
				}
				src_img_.create(sr.size, true);
				img::copy_to_rgba8(imfs.get_image().get(), sr, src_img_, vtx::spos(0)); 

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
				if(option_[option::cpp_style]) {
					cout << "C++-Style output symbol: '" << symbol_ << "'" << endl; 
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
					cout << "BDF font size: " << static_cast<int>(bdf_fsize_.x)
						<< ", " << static_cast<int>(bdf_fsize_.y) << endl;
					cout << "BDF font num: " << bdf_num_ << endl;
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


		//-----------------------------------------------------------------//
		/*!
			@brief  オプションの取得
			@param[in]	t	オプション・タイプ
			@return 状態
		*/
		//-----------------------------------------------------------------//
		bool get_option(option::type t) const { return option_[t]; }


		//-----------------------------------------------------------------//
		/*!
			@brief  入力ファイルの取得
			@return 入力ファイル名
		*/
		//-----------------------------------------------------------------//
		const std::string& get_inp_file() const { return inp_fname_; }


		//-----------------------------------------------------------------//
		/*!
			@brief  出力ファイルの取得
			@return 出力ファイル名
		*/
		//-----------------------------------------------------------------//
		const std::string& get_out_file() const { return out_fname_; }


		//-----------------------------------------------------------------//
		/*!
			@brief  ソース画像の参照を取得
			@return ソース画像の参照
		*/
		//-----------------------------------------------------------------//
		const img::img_rgba8& get_src_image() const { return src_img_; }


		//-----------------------------------------------------------------//
		/*!
			@brief  ソース画像の参照を取得
			@return ソース画像の参照
		*/
		//-----------------------------------------------------------------//
		const img::img_rgba8& get_dst_image() const { return dst_img_; }


		//-----------------------------------------------------------------//
		/*!
			@brief  BDF の画像のページ数を取得
			@return 最大ページ数
		*/
		//-----------------------------------------------------------------//
		uint32_t get_bdf_pages() const { return bdf_pages_; }


		//-----------------------------------------------------------------//
		/*!
			@brief  BDF の画像を生成
			@param[in]	page	ページ
		*/
		//-----------------------------------------------------------------//
		void create_bdf_image(uint32_t page)
		{
			if(page >= bdf_pages_) {
				return;
			}

			uint32_t ofs = 0;
			// ヘッダーの分を seek
			if(option_[option::header]) {
				ofs = header_size_ * 2;
			}

			uint32_t fb = (((bdf_fsize_.x * bdf_fsize_.y) + 7) >> 3) << 3;
			ofs += bdf_prev_x_ * bdf_prev_y_ * page * fb;
			bits_.set_pos(ofs); // seek

			vtx::spos ssz(bdf_fsize_.x * bdf_prev_x_, bdf_fsize_.y * bdf_prev_y_);
			dst_img_.create(ssz, true);
			for(uint32_t y = 0; y < ssz.y; y += bdf_fsize_.y) {
				for(uint32_t x = 0; x < ssz.x; x += bdf_fsize_.x) {
					for(uint32_t j = 0; j < bdf_fsize_.y; ++j) {
						for(uint32_t i = 0; i < bdf_fsize_.x; ++i) {
							bool f = bits_.get_bit();
							img::rgba8 c;
							if(f) c.set(255, 255, 255, 255);
							else c.set(0, 0, 0, 255);
							dst_img_.put_pixel(vtx::spos(x + i, y + j), c);
						}
					}
				}
			}
		}
	};
}
