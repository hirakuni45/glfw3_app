//=====================================================================//
/*! @file
	@brief  BMC コア関係
	@author 平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include "bmc_core.hpp"
#include <boost/foreach.hpp>
#include <boost/format.hpp>

namespace app {

// タイトルの表示と簡単な説明
	void bmc_core::help() const
	{
		using namespace std;

		cout << "Bitmap image creator" << endl;
		cout << "Copyright (C) 2007, Hiramatsu Kunihito" << endl;
		cout << "Version " << boost::format("%1.2f") % version_ << endl;
		cout << "usage:" << endl;
		std::string cmd = argv_[0];
		cout << "	" << cmd << " [options] in-file [out-file]" << endl;
		cout << "	-preview -pre      preview image (OpenGL)" << endl;
		cout << "	-bdf               BDF file input" << endl;
		cout << "	-dither            Ditherring(50%)" << endl;
		cout << "	-no-header         No-Header(Width/Height) output" << endl;
		cout << "	-clip-x start len  Clipping X-line" << endl;
		cout << "	-clip-y	start len  Clipping Y-line" << endl;
		cout << "	-append            Append output file" << endl;
		cout << "	-verbose           verbose" << endl;
		cout << endl;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief  コマンドライン解析
		@param[in]	ss	コマンド郡
	*/
	//-----------------------------------------------------------------//
	bool bmc_core::analize(const utils::strings& ss)
	{
		bool noerr = true;
		BOOST_FOREACH(const std::string& s, ss) {
			if(s.empty()) continue;
			else if(s[0] == '-') {
				if(s == "-preview") option_.set(option::preview);
				else if(s == "-pre") option_.set(option::preview);
				else if(s == "-true-color") option_.set(option::true_color);
				else if(s == "-inverse") option_.set(option::inverse);
				else if(s == "-bdf") option_.set(option::bdf_type);
				else if(s == "-dither") option_.set(option::dither);
				else if(s == "-no-header") option_.set(option::header);
				else if(s == "-clipx") option_.set(option::clipx);
				else if(s == "-clipy") option_.set(option::clipy);
				else if(s == "-append") option_.set(option::append);
				else if(s == "-verbose") option_.set(option::verbose);
				else noerr = false;
			} else {
				if(option_[option::clipx]) {
//					area.x = area.w;
//					if(Arith((const char *)p, &area.w) != 0) {
//						clipx = false;
//					}
//					if(area.x >= 0 && area.w > 0) clipx = false;
					option_.reset(option::clipx);
				} else if(option_[option::clipy]) {
//					area.y = area.h;
//					if(Arith((const char *)p, &area.h) != 0) {
//						clipy = false;
//					}
//					if(area.y >= 0 && area.h > 0) clipy = false;
					option_.reset(option::clipy);
				} else {
					inp_fname_ = out_fname_;
					out_fname_ = s;
				}
			}
		}

		return noerr;
	}
}

#if 0
//-----------------------------------------------------------------//
/*!
	@breif	フレームバッファに点を描画
	@param[in]	x	描画位置Ｘ
	@param[in]	y	描画位置Ｙ
	@param[in]	c	描画色
*/
//-----------------------------------------------------------------//
void point_pixel(int x, int y, img::rgba& c)
{
	if(x >= 0 && x < FB_WIDTH && y >= 0 && y < FB_HEIGHT) {
		unsigned char* p = &m_fb[(y * 256 + x) * 4];
		*p++ = c.r;
		*p++ = c.g;
		*p++ = c.b;
		*p++ = c.a;
	}
}


//-----------------------------------------------------------------//
/*!
	@breif	フルカラーフレームバッファにレンダリング
	@param[in]	img	イメージクラスを参照渡しで
*/
//-----------------------------------------------------------------//
void render_true_img(img& srcimg)
{
	int ofsx = (FB_WIDTH  - srcimg.get_width()) / 2;
	int ofsy = (FB_HEIGHT - srcimg.get_height()) / 2;
	for(int y = 0; y < srcimg.get_height(); ++y) {
		for(int x = 0; x < srcimg.get_width(); ++x) {
			img::rgba	c;
			srcimg.get_pixel(x, y, c);
			point_pixel(ofsx + x, ofsy + y, c);
		}
	}
}


//-----------------------------------------------------------------//
/*!
	@breif	減色したモノカラーでレンダリング
	@param[in]	img	イメージクラスを参照渡しで
	@param[in]	reduction_num	減色数
	@param[in]	inverse	画像ピクセルの反転を行う場合「true」
	@param[in]	bitf	bitio クラス（参照渡し）
	@param[in]	area	クリッピングエリア
*/
//-----------------------------------------------------------------//
void render_mono_img(img& srcimg, int reduction_num, bool inverse, bitio& bitf, clip& area)
{
	// センターリングオフセット
	int ofsx = (FB_WIDTH  - srcimg.get_width()) / 2;
	int ofsy = (FB_HEIGHT - srcimg.get_height()) / 2;
	img::rgba fc, bc;
	clip cpa;

	if(inverse == true) {
		fc = m_back_color;
		bc = m_fore_color;
	} else {
		fc = m_fore_color;
		bc = m_back_color;
	}

	cpa = area;
	if(cpa.w < 0) cpa.w = srcimg.get_width() - cpa.x;
	if(cpa.h < 0) cpa.h = srcimg.get_height() - cpa.y;

	for(int y = cpa.y; y < (cpa.y + cpa.h); ++y) {
		for(int x = cpa.x; x < (cpa.x + cpa.w); ++x) {
			img::rgba c;
			srcimg.get_pixel(x, y, c);
			// R * 0.3 + G * 0.59 + B * 0.11 (NTSC)
			int	i = (19661 * c.r + 38666 * c.g + 7209 * c.b) >> 16;
			bool pc;
			if(reduction_num >= 3) {
				if(i < 85) pc = false;
				else if(i < 171) {	// 中間色をディザリング
					if((x ^ y) & 1) pc = true;
					else pc = false;
				} else pc = true;
			} else if(reduction_num <= 2) {	// 白黒
				if(i >= 128) pc = true; else pc = false;
			}
			if(pc == true) {
				c = fc;
				bitf.put_bit(1);
			} else {
				c = bc;
				bitf.put_bit(0);
			}
			point_pixel(ofsx + x, ofsy + y, c);
		}
	}
}


//-----------------------------------------------------------------//
/*!
	@breif	プレビュー描画タスク（glut 描画コールバック）
*/
//-----------------------------------------------------------------//
void preview_display_task(void)
{
	int	w, h;

	w = 768;
	h = 768;

	m_device.service();

	int	trg = m_device.getMousePositive();
	int lvl = m_device.getMouseLevel();
	if(lvl & MOUSE_LEFT) {
		gldev::xy	mpos;

		m_device.getMousePositionDiff(mpos);
		m_angY += (float)mpos.x;
		m_angX += (float)mpos.y;

		m_device.getMousePosition(mpos);
	}
	if(lvl & MOUSE_CENTER) {
		gldev::xy	mpos;

		m_device.getMousePositionDiff(mpos);
		m_positionX += (float)mpos.x * 0.01f;
		m_positionY -= (float)mpos.y * 0.01f;
	}
	if(lvl & MOUSE_RIGHT) {
		gldev::xy	mpos;

		m_device.getMousePositionDiff(mpos);
		m_scale += (float)mpos.y * 0.01f;
	}

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

#if 0
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0.0, 1.0, 1.0, 0.0, 0.0, 1.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
#else
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(40.0f, (GLfloat)w / (GLfloat)h, 0.1f, 20.0f);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
#endif

	glPushMatrix();

// マウス操作によるマトリックス
	glTranslatef(m_positionX, m_positionY, -12.0f);
//	glTranslatef(m_positionX, m_positionY, 0.0f);
	glRotatef(m_angX, 1.0, 0.0, 0.0);
	glRotatef(m_angY, 0.0, 1.0, 0.0);
	glScalef(m_scale, m_scale, m_scale);

	m_texfb.renderring(m_fb);
	m_texfb.flip();
	m_texfb.service(4.0f);

	glPopMatrix();

	glutSwapBuffers();

	if(m_device.getKeyLevel(32 + 0x1b) == true) {

		exit(0);
	}
}

//-----------------------------------------------------------------//
/*!
	@breif	アイドル・タスク（glut アイドル・コールバック）
*/
//-----------------------------------------------------------------//
static void preview_idle_task(void)
{
	glutPostRedisplay();
}

//-----------------------------------------------------------------//
/*!
	@breif	プレビュー初期化（glut）
*/
//-----------------------------------------------------------------//
void preview_init(int *argc, char *argv[])
{
	glutInit(argc, argv);

	int	w = 768;
	int	h = 768;

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(w, h);
	glutInitWindowPosition(0, 0);
	glutCreateWindow("LCD Emulater");

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	glEnable(GL_DEPTH_TEST);
//	glDepthFunc(GL_LEQUAL);

// Alpha blending...
//	glEnable(GL_BLEND);
//	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//	glEnable(GL_ALPHA_TEST);
//	glAlphaFunc(GL_LESS, 1.0f);

	glutDisplayFunc(preview_display_task);
	glutIdleFunc(preview_idle_task);

//	glutTimerFunc(100, TimerTask, 0);

	m_device.initialize();
	m_texfb.initialize(FB_WIDTH, FB_HEIGHT, 32);
}

void preview_main(void)
{
	glutMainLoop();
}

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
	char* in_fname = NULL;
	char* out_fname = NULL;
	bool preview = false;
	bool verbose = false;
	bool true_color = false;
	bool inverse = false;
	bool png_type = false;
	bool bdf_type = false;
	bool dither = false;
	bool header = true;
	bool clipx = false;
	bool clipy = false;
	bool append = false;
	bool opterr = false;
	clip area = { 0, 0, -1, -1 };

	for(int i = 1; i < argc; ++i) {
		char* p = argv[i];
		if(p[0] == '-') {
			if(strcmp(p, "-preview")==0) preview = true;
			else if(strcmp(p, "-pre")==0) preview = true;
			else if(strcmp(p, "-true-color")==0) true_color = true;
			else if(strcmp(p, "-inverse")==0) inverse = true;
			else if(strcmp(p, "-png")==0) { png_type = true; bdf_type = false; }
			else if(strcmp(p, "-bdf")==0) { bdf_type = true; png_type = false; }
			else if(strcmp(p, "-dither")==0) dither = true;
			else if(strcmp(p, "-no-header")==0) header = false;
			else if(strcmp(p, "-clip-x")==0) { clipx = true;  clipy = false; }
			else if(strcmp(p, "-clip-y")==0) { clipy = false; clipy = true; }
			else if(strcmp(p, "-append")==0) { append = true; }
			else if(strcmp(p, "-verbose")==0) verbose = true;
			else {
				opterr = true;
			}
		} else {
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
				in_fname = out_fname;
				out_fname = p;
			}
		}
	}

	if(opterr == true) {
		fprintf(stderr, "Illegual option...\n\n");
		title((const char *)argv[0]);
		return 1;
	}

	if(in_fname == NULL) {
		in_fname = out_fname;
		out_fname = NULL;
	}

	if(in_fname == NULL) {
		title((const char *)argv[0]);
		return 1;
	}

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

// PNG ファイルフォーマットの読み込み
	if(png_type) {
		pngio png;
		try {
			png.load(in_fname); 
		} catch(const char *msg) {
			fprintf(stderr, "PNG load error: '%s'\n\n", msg);
			file_read = false;
		} catch(...) {
			fprintf(stderr, "PNG load error: 'other'\n\n");
			file_read = false;
		}
		if(file_read) {
			image = png;
		}
	} else if(bdf_type) {
		bdf.initialize();
		if(bdf.load(in_fname)) {
			file_read = true;
		}

	} else {
// （通常）バイナリーベースのファイル入力


	}

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
