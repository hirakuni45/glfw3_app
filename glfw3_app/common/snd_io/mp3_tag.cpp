//=====================================================================//
/*!	@file
	@brief	MP3 ID3 TAG クラス@n
			libid3tag を利用
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include <io.h>
#include <fcntl.h>
#include "mp3_tag.hpp"
#include "utils/file_io.hpp"

// #define MSG_

namespace mp3 {

	// ローカルな変換
	static void ucs4_to_sjis_(const id3_ucs4_t* ucs4, std::string& dst)
	{
		id3_ucs4_t c;
		while((c = *ucs4++) != 0) {
			if(c <= 0xff) {
				dst += c;
			} else {
#ifdef MSG_
std::cout << boost::format("%x ") % c;
#endif
			}
		}
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	初期化
	*/
	//-----------------------------------------------------------------//
	void mp3_tag::initialize()
	{
		destroy();

		open_ = false;
		id3_file_ = 0;
		id3_tag_ = 0;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	オープン
		@param[in]	fin	ファイル入力
		@return 成功なら「true」が返る
	*/
	//-----------------------------------------------------------------//
	bool mp3_tag::open(utils::file_io& fin)
	{
		int fd = fin.file_handle();

		id3_file_ = id3_file_fdopen(fd, ID3_FILE_MODE_READONLY);
		if(id3_file_ == 0) return false;

		id3_tag_ = id3_file_tag(id3_file_);
		if(id3_tag_ == 0) {
			id3_file_close(id3_file_);
			id3_file_ = 0;
			return false;
		}
		open_ = true;

		value_ = 0;

		for(int i = 0; i < id3_t::limit_; ++i) {
			text_[i].clear();
		}

		return true;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	TAG 内の全てのフレームをパース
		@return 成功なら「true」が返る
	*/
	//-----------------------------------------------------------------//
	bool mp3_tag::decode()
	{
		if(open_ == false) return false;
		if(id3_tag_ == 0) return false;

//		std::cout << boost::format("Tag frame num: %d\n") % id3_tag_->nframes;
//		std::cout << boost::format("Tag version: %d\n") % id3_tag_->version;
//		std::cout << boost::format("Tag size: %d (%X)\n") % static_cast<int>(id3_tag_->paddedsize)
//			% static_cast<int>(id3_tag_->paddedsize);

		int skip = id3_tag_->paddedsize;
///		if(skip & 3) { skip |= 3; ++skip; }
		skip_head_ = skip;

		for(int i = 0; i < id3_tag_->nframes; ++i) {
			id3_frame* fm = id3_tag_->frames[i];
			frame_t_ = id3_t::none;
			bool picture = false;
			if(strcmp(fm->id, "APIC") == 0) picture = true;
			else if(strcmp(fm->id, "TALB") == 0) frame_t_ = id3_t::album;
			else if(strcmp(fm->id, "TIT2") == 0) frame_t_ = id3_t::title;
			else if(strcmp(fm->id, "TPE1") == 0) frame_t_ = id3_t::artist;
			else if(strcmp(fm->id, "TPE2") == 0) frame_t_ = id3_t::artist2;
			else if(strcmp(fm->id, "TCON") == 0) frame_t_ = id3_t::content;
			else if(strcmp(fm->id, "TRCK") == 0) frame_t_ = id3_t::track;
			else if(strcmp(fm->id, "TPOS") == 0) frame_t_ = id3_t::disc;
			else if(strcmp(fm->id, "TDRC") == 0) frame_t_ = id3_t::release_year;
			else {
			}
#ifdef MSG_
			std::cout << fm->id << " ";
#endif
			for(int j = 0; j < fm->nfields; ++j) {
				id3_field* fd = &fm->fields[j];

				switch(fd->type) {
				case ID3_FIELD_TYPE_TEXTENCODING:
					text_code_ = id3_field_gettextencoding(fd);
#ifdef MSG_
					if(text_code_ == ID3_FIELD_TEXTENCODING_ISO_8859_1) {
						std::cout << "ISO_8859" << std::endl;
					} else if(text_code_ == ID3_FIELD_TEXTENCODING_UTF_16) {
						std::cout << "UTF_16  " << std::endl;
					} else if(text_code_ == ID3_FIELD_TEXTENCODING_UTF_16BE) {
						std::cout << "UTF_16BE" << std::endl;
					} else if(text_code_ == ID3_FIELD_TEXTENCODING_UTF_8) {
						std::cout << "UTF_8   " << std::endl; 
					}
#endif
					break;
				case ID3_FIELD_TYPE_LATIN1:
				{
					const id3_latin1_t* t = id3_field_getlatin1(fd);
				}
					break;
				case ID3_FIELD_TYPE_LATIN1FULL:
					break;
// ID3_FIELD_TYPE_LATIN1LIST,
				case ID3_FIELD_TYPE_STRING:
				{
					const id3_ucs4_t* ucs4 = id3_field_getstring(fd);
					if(picture) {
						if(ucs4[0] == 0xff && ucs4[1] == 0xd8 && ucs4[2] == 0xff && ucs4[3] == 0xe0) {
							apic_string_[0] = ucs4[0];
							apic_string_[1] = ucs4[1];
							apic_string_[2] = ucs4[2];
							apic_string_[3] = ucs4[3];
							apic_string_len_ = fd->string.length;
						} else {
							apic_string_len_ = 0;
						}
					} else {
						std::string dst;
						bool cnv = true;
						if(text_code_ == ID3_FIELD_TEXTENCODING_ISO_8859_1) {
							std::string tmp;
							ucs4_to_sjis_(ucs4, tmp);
							utils::sjis_to_utf8(tmp, dst);
						} else if(text_code_ == ID3_FIELD_TEXTENCODING_UTF_16) {
							id3_utf16_t* utf16 = id3_ucs4_utf16duplicate(ucs4);
							utils::utf16_to_utf8((const char16_t*)utf16, dst);
							free(utf16);
						} else if(text_code_ == ID3_FIELD_TEXTENCODING_UTF_8) {
							id3_utf8_t* utf8 = id3_ucs4_utf8duplicate(ucs4);
							dst = (char*)(utf8);
							free(utf8);
						} else {
							cnv = false;
						}
						if(cnv) {
							text_[frame_t_] = dst;
#ifdef MSG_
							std::cout << "STRING: '" << dst << "'";
#endif
						}
					}
				}
					break;
				case ID3_FIELD_TYPE_STRINGFULL:
					break;
				case ID3_FIELD_TYPE_STRINGLIST:
					{
						for(int n = 0; n < id3_field_getnstrings(fd); ++n) {
							const id3_ucs4_t* ucs4 = id3_field_getstrings(fd, n);

							std::string dst;
							bool cnv = true;
							if(text_code_ == ID3_FIELD_TEXTENCODING_ISO_8859_1) {
								id3_latin1_t* sjis = id3_ucs4_latin1duplicate(ucs4);
								utils::sjis_to_utf8((const char*)sjis, dst);
								free(sjis);
							} else if(text_code_ == ID3_FIELD_TEXTENCODING_UTF_16) {
								id3_utf16_t* utf16 = id3_ucs4_utf16duplicate(ucs4);
								utils::utf16_to_utf8((char16_t*)utf16, dst);
								free(utf16);
							} else if(text_code_ == ID3_FIELD_TEXTENCODING_UTF_8) {
								id3_utf8_t* utf8 = id3_ucs4_utf8duplicate(ucs4);
								dst = (char*)utf8;
								free(utf8);
							} else {
								cnv = false;
							}
							if(cnv) {
								text_[frame_t_] = dst;
#ifdef MSG_
								std::cout << boost::format("STRINGS(%d): '%s'\n") % n % dst;
#endif
							}
						}
					}
					break;
// ID3_FIELD_TYPE_LANGUAGE,
				case ID3_FIELD_TYPE_FRAMEID:
					{
						const char* id = id3_field_getframeid(fd);
						if(id) {
						}
					}
					break;
				case ID3_FIELD_TYPE_DATE:
					break;
				case ID3_FIELD_TYPE_INT8:
				{
					value_ = id3_field_getint(fd);
				}
					break;
// ID3_FIELD_TYPE_INT16,
// ID3_FIELD_TYPE_INT24,
// ID3_FIELD_TYPE_INT32,
// ID3_FIELD_TYPE_INT32PLUS,
				case ID3_FIELD_TYPE_BINARYDATA:
					{
						id3_length_t len;
						const id3_byte_t* p = id3_field_getbinarydata(fd, &len);
						if(picture == true && len > 0) {
							id3_byte_t* tmp = 0;
							if(apic_string_len_ == (len + 5)) {
								tmp = new id3_byte_t[len + 5];
								tmp[0] = apic_string_[0];
								tmp[1] = apic_string_[1];
								tmp[2] = apic_string_[2];
								tmp[3] = apic_string_[3];
								tmp[4] = 0;
								memcpy(tmp + 5, p, len);
							}
							jpegio_.destroy();
							pngio_.destroy();
							utils::file_io mem;
							if(tmp) {
								mem.open((const char*)tmp, len + 5);
							} else {
								mem.open((const char*)p, len);
							}
							if(jpegio_.probe(mem)) {
								jpegio_.load(mem);
							} else if(pngio_.probe(mem)) {
								pngio_.load(mem);
							} else {
//								std::cout << "APIC can't decode picture format: ";
//								for(int i = 0; i < 4; ++i) {
//									std::cout << boost::format("%02X,") % p[i];
//								}
//								std::cout << std::endl;
							}
							mem.close();
							delete[] tmp;
						}
					}
					break;
				default:
					break;
				}
			}
		}
		return true;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	クローズ
	*/
	//-----------------------------------------------------------------//
	void mp3_tag::close()
	{
		if(open_) {
			id3_file_close(id3_file_);
			id3_file_ = 0;
			id3_tag_ = 0;
			jpegio_.destroy();
			pngio_.destroy();
		}
		open_ = false;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	廃棄
	*/
	//-----------------------------------------------------------------//
	void mp3_tag::destroy()
	{
		for(int i = 0; i < id3_t::limit_; ++i) {
			text_[i] = "";
		}
		close();
	}

}
