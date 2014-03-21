//=====================================================================//
/*!	@file
	@brief	漢字フォントイメージを freetype2 で扱うクラス（ヘッダー）
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include "core/kfimg_ft2.hpp"
#include "utils/string_utils.hpp"
#include <boost/foreach.hpp>
#include <ftoutln.h>

using namespace std;

namespace img {

	static kfimg_ft2* kanji_font_image_ = 0;

	//-----------------------------------------------------------------//
	/*!
		@brief	漢字フォントイメージのコンテキストを作成
	 */
	//-----------------------------------------------------------------//
	void create_kfimg()
	{
		if(kanji_font_image_ == 0) {
			kanji_font_image_ = new kfimg_ft2;
		}
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	漢字フォントイメージのコンテキストを取得
		@return クラスのポインター
	 */
	//-----------------------------------------------------------------//
	Ikfimg* get_kfimg()
	{
		Ikfimg* p = dynamic_cast<Ikfimg*>(kanji_font_image_);
		if(p == 0) {
///			std::cout << "Can't create kanji-font-image" << std::endl;
		}
		return p;
	}

	//-----------------------------------------------------------------//
	/*!
		@brief	漢字フォントイメージのコンテキストを廃棄
	 */
	//-----------------------------------------------------------------//
	void destroy_kfimg()
	{
		delete kanji_font_image_;
		kanji_font_image_ = 0;
	}


#if 0
void get_metrics(wchar_t code)
{
	FT_Error error;

	error = FT_Set_Pixel_Sizes(face_, w, h);

	FT_Vector	pen;
	pen.x = pen.y = 0;
	FT_Set_Transform(face_, &matrix_, &pen);

	if(antialias) {
		error = FT_Load_Char(face_, code, FT_LOAD_RENDER);
	} else {
		error = FT_Load_Char(face_, code, FT_LOAD_MONOCHROME);
	}

	FT_GlyphSlot slot = face_->glyph;
	FT_Bitmap* bitmap = &slot->bitmap;

	metrics_.bitmap_w = (float)bitmap->width;
	metrics_.bitmap_h = (float)bitmap->rows;
	metrics_.width    = (float)slot->metrics.width  / 64.0f;
	metrics_.height   = (float)slot->metrics.height / 64.0f;
	metrics_.hori_x   = (float)slot->metrics.horiBearingX / 64.0f;
	metrics_.hori_y   = (float)slot->metrics.horiBearingY / 64.0f;
	metrics_.vert_x   = (float)slot->metrics.vertBearingX / 64.0f;
	metrics_.vert_y   = (float)slot->metrics.vertBearingY / 64.0f;

}
#endif

	//-----------------------------------------------------------------//
	/*!
		@brief	フォントを指定
		@param[in]	fontfile	フォント・ファイル名
		@param[in]	alias		フォントの別名
		@return 成功した場合は「true」
	 */
	//-----------------------------------------------------------------//
	bool kfimg_ft2::install_font_type(const std::string& fontfile, const std::string& alias)
	{
		string name;

		if(fontfile.empty()) return false;

		if(alias.empty()) {
			utils::get_file_base(utils::get_file_name(fontfile), name);
		} else {
			name = alias;
		}

		face_map_it it = find_face_(name);
		if(it != face_map_.end()) {
			current_face_ = it;
			return true;
		}

		FT_Face face;
		FT_Error error = FT_New_Face(library_, fontfile.c_str(), 0, &face);
		if(error) {
			current_face_ = face_map_.end();
			return false;
		}

///		std::cout << "kfimg install: " << name << ", " << static_cast<int>(face_) << std::endl;

		face_t t(face);

		FT_Vector pen;
		pen.x = pen.y = 0;
		FT_Set_Transform(face, &matrix_, &pen);
		current_face_ = install_face_(name, t);

		return current_face_ != face_map_.end();
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	unicode に対応するビットマップを生成する。
		@param[in]	size	生成するビットマップのサイズ
		@param[in]	unicode	生成するビットマップの UNICODE
	 */
	//-----------------------------------------------------------------//
	void kfimg_ft2::create_bitmap(int size, wchar_t unicode)
	{
		if(current_face_ == face_map_.end()) return;

		// 基準点へのオフセットが無い場合
		face_t& t = current_face_->second;
		if(t.atr_map_.find(size) == t.atr_map_.end()) {
			struct met {
				int	ofs;
				int rows;
			};
			std::vector<met> mets;
			int offset = 0;
			for(int ch = 0x21; ch <= 0x7f; ++ch) {
				if(ch == 0x3f) continue;
				FT_Set_Pixel_Sizes(t.face_, size, size);
				FT_Load_Char(t.face_, ch, FT_LOAD_RENDER);
				FT_GlyphSlot slot = t.face_->glyph;
				met m;
				m.ofs = slot->metrics.horiBearingY / 64;
				FT_Bitmap* bitmap = &slot->bitmap;
				m.rows = bitmap->rows;
				mets.push_back(m);
				if(offset < m.ofs) offset = m.ofs;
			}
			int height = 0;
			BOOST_FOREACH(const met& m, mets) {
				int l = offset - m.ofs + m.rows + 1;
				if(height < l) height = l;
			}
			atr_t at;
			at.offset_ = offset;
			if(height < size) height = size;
			at.height_ = height;
			std::pair<int, atr_t> v(size, at);
/// std::cout << current_face_->first << ", Size: " << size << ", Height: " << at.height_ << std::endl;
			t.atr_map_.insert(v);
		}

		const atr_t& at = t.atr_map_[size];

		vtx::spos fs(size, at.height_);
		img_.create(fs);
		gray8 g(0);
		img_.fill(g);

		FT_Error error = FT_Set_Pixel_Sizes(t.face_, size, size);
		if(antialias_) {
			error = FT_Load_Char(t.face_, unicode, FT_LOAD_RENDER);
		} else {
			error = FT_Load_Char(t.face_, unicode, FT_LOAD_MONOCHROME);
		}
		FT_GlyphSlot slot = t.face_->glyph;
#if 0
		if(slot->format == FT_GLYPH_FORMAT_OUTLINE) {
			int strength = 2 << 6;
			FT_Outline_Embolden(&slot->outline, strength);
			FT_Render_Glyph(slot, FT_RENDER_MODE_NORMAL);
		}
#endif
		FT_Bitmap* bitmap = &slot->bitmap;
		metrics_.bitmap_w = static_cast<float>(bitmap->width);
		metrics_.bitmap_h = static_cast<float>(bitmap->rows);
		metrics_.width    = static_cast<float>(slot->metrics.width)  / 64.0f;
		metrics_.height   = static_cast<float>(slot->metrics.height) / 64.0f;
		metrics_.hori_x   = static_cast<float>(slot->metrics.horiBearingX) / 64.0f;
		metrics_.hori_y   = static_cast<float>(slot->metrics.horiBearingY) / 64.0f;
		metrics_.vert_x   = static_cast<float>(slot->metrics.vertBearingX) / 64.0f;
		metrics_.vert_y   = static_cast<float>(slot->metrics.vertBearingY) / 64.0f;

		vtx::spos ofs(static_cast<short>(metrics_.hori_x), at.offset_ - static_cast<short>(metrics_.hori_y) + 1);

	// グレイスケールでレンダリング出来なかった場合は、モノカラーとなる。
		if(bitmap->pixel_mode != FT_PIXEL_MODE_MONO) {		// gray-scale 0 to 255
			vtx::spos p;
			for(p.y = 0; p.y < bitmap->rows; p.y++) {
				for(p.x = 0; p.x < bitmap->width; p.x++) {
					img::gray8 c;
					c.g = bitmap->buffer[p.y * bitmap->width + p.x];
					img_.put_pixel(p + ofs, c);
				}
			}
		} else {	// monochrome
			int bitpos = 0;
			vtx::spos p;
			for(p.y = 0; p.y < bitmap->rows; p.y++) {
				for(p.x = 0; p.x < bitmap->width; p.x++) {
					img::gray8 c;
					if(bitmap->buffer[bitpos >> 3] & (1 << (~bitpos & 7))) c.g = 255; else c.g = 0;
					bitpos++;
					img_.put_pixel(p + ofs, c);
				}
				if(bitpos & 7) {
					bitpos |= 7;
					bitpos++;
				}
			}
		}
	}
}
