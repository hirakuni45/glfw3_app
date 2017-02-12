//=====================================================================//
/*! @file
	@brief  NES Emulator インターフェース・クラス
	@author 平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include "osd.hpp"

extern "C" {
    #include "emu/osd.h"
};

namespace {

	app::osd osd_;

}

extern "C" {

	char *osd_getromdata()
	{
		return osd_.getromdata();
	}


	void osd_setsound(void (*playfunc)(void *buffer, int size))
	{
	}


	void osd_getvideoinfo(vidinfo_t *info)
	{
		info->default_width  = 256;
		info->default_height = 240;
#if 0
typedef struct viddriver_s
{
   /* name of driver */
   const char *name;
   /* init function - return 0 on success, nonzero on failure */
   int       (*init)(int width, int height);
   /* clean up after driver (can be NULL) */
   void      (*shutdown)(void);
   /* set a video mode - return 0 on success, nonzero on failure */
   int       (*set_mode)(int width, int height);
   /* set up a palette */
   void      (*set_palette)(rgb_t *palette);
   /* custom bitmap clear (can be NULL) */
   void      (*clear)(uint8 color);
   /* lock surface for writing (required) */
   bitmap_t *(*lock_write)(void);
   /* free a locked surface (can be NULL) */
   void      (*free_write)(int num_dirties, rect_t *dirty_rects);
   /* custom blitter - num_dirties == -1 if full blit required */
   void      (*custom_blit)(bitmap_t *primary, int num_dirties, 
                            rect_t *dirty_rects);
   /* immediately invalidate the buffer, i.e. full redraw */
   bool      invalidate;
} viddriver_t;
#endif
//		info->driver = ;
	}


	void osd_getsoundinfo(sndinfo_t *info)
	{
		if(info == nullptr) return;
		info->sample_rate = 44100;
		info->bps = 16;
	}

	/* init / shutdown */
	int osd_init(void)
	{
		return osd_.init();
	}

	void osd_shutdown(void)
	{
		osd_.shutdown();
	}

	int osd_main(int argc, char *argv[])
	{
		return osd_.main(argc, argv);
	}

	int osd_installtimer(int frequency, void *func, int funcsize, void *counter, int countersize)
	{
		return 0;
	}

	/* filename manipulation */
	void osd_fullname(char *fullname, const char *shortname)
	{
	}

	char *osd_newextension(char *string, char *ext)
	{
		return nullptr;
	}

	/* input */
	void osd_getinput(void)
	{
	}

	void osd_getmouse(int *x, int *y, int *button)
	{
	}

	/* build a filename for a snapshot, return -ve for error */
	int osd_makesnapname(char *filename, int len)
	{
		return 0;
	}

}
