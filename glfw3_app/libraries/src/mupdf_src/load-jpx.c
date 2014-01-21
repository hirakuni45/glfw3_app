#include "mupdf/fitz.h"

/* Without the definition of OPJ_STATIC, compilation fails on windows
 * due to the use of __stdcall. We believe it is required on some
 * linux toolchains too. */
#define OPJ_STATIC
#ifndef _MSC_VER
#define OPJ_HAVE_STDINT_H
#endif

#include <openjpeg.h>

static void fz_opj_error_callback(const char *msg, void *client_data)
{
	fz_context *ctx = (fz_context *)client_data;
	fz_warn(ctx, "openjpeg error: %s", msg);
}

static void fz_opj_warning_callback(const char *msg, void *client_data)
{
	fz_context *ctx = (fz_context *)client_data;
	fz_warn(ctx, "openjpeg warning: %s", msg);
}

static void fz_opj_info_callback(const char *msg, void *client_data)
{
	/* fz_warn("openjpeg info: %s", msg); */
}

#if 0
typedef struct stream_block_s
{
	unsigned char *data;
	int size;
	int pos;
} stream_block;

size_t stream_read(void * p_buffer, size_t p_nb_bytes, void * p_user_data)
{
	stream_block *sb = (stream_block *)p_user_data;
	int len;

	len = sb->size - sb->pos;
	if (len < 0)
		len = 0;
	if (len == 0)
		return -1;  /* End of file! */
	if (len > p_nb_bytes)
		len = p_nb_bytes;
	memcpy(p_buffer, sb->data + sb->pos, len);
	sb->pos += len;
	return len;
}

size_t stream_skip(size_t skip, void * p_user_data)
{
	stream_block *sb = (stream_block *)p_user_data;

	if (skip > sb->size - sb->pos)
		skip = sb->size - sb->pos;
	sb->pos += skip;
	return sb->pos;
}

opj_bool stream_seek(size_t seek_pos, void * p_user_data)
{
	stream_block *sb = (stream_block *)p_user_data;

	if (seek_pos > sb->size)
		return OPJ_FALSE;
	sb->pos = seek_pos;
	return OPJ_TRUE;
}
#endif

fz_pixmap *
fz_load_jpx(fz_context *ctx, unsigned char *data, int size, fz_colorspace *defcs, int indexed)
{
	fz_colorspace *origcs;
	unsigned char *p;
	OPJ_CODEC_FORMAT format;
	int a, n, w, h, depth, sgnd;
	int x, y, k, v;

	if (size < 2)
		fz_throw(ctx, FZ_ERROR_GENERIC, "not enough data to determine image format");

	/* Check for SOC marker -- if found we have a bare J2K stream */
	if (data[0] == 0xFF && data[1] == 0x4F)
		format = CODEC_J2K;
	else
		format = CODEC_JP2;

	opj_dparameters_t params;
	opj_set_default_decoder_parameters(&params);
	if (indexed)
		params.flags |= OPJ_DPARAMETERS_IGNORE_PCLR_CMAP_CDEF_FLAG;

	opj_dinfo_t *dinfo = opj_create_decompress(format);
	opj_event_mgr_t mgr;
	mgr.error_handler = fz_opj_error_callback;
	mgr.warning_handler = fz_opj_warning_callback;
	mgr.info_handler = fz_opj_info_callback;
	opj_set_event_mgr((opj_common_ptr)dinfo, &mgr, ctx);

	opj_setup_decoder(dinfo, &params);

	opj_cio_t *cio = opj_cio_open((opj_common_ptr)dinfo, data, size);
	if(cio == NULL)
	{
		opj_destroy_decompress(dinfo);
		fz_throw(ctx, FZ_ERROR_GENERIC, "Failed to read JPX header");
	}

	opj_image_t *jpx = opj_decode(dinfo, cio);
	if (jpx == NULL)
	{
		opj_cio_close(cio);
		opj_destroy_decompress(dinfo);
		fz_throw(ctx, FZ_ERROR_GENERIC, "Failed to decode JPX image");
	}

	for (k = 1; k < (int)jpx->numcomps; k++)
	{
		if (jpx->comps[k].w != jpx->comps[0].w)
		{
			opj_image_destroy(jpx);
			fz_throw(ctx, FZ_ERROR_GENERIC, "image components have different width");
		}
		if (jpx->comps[k].h != jpx->comps[0].h)
		{
			opj_image_destroy(jpx);
			fz_throw(ctx, FZ_ERROR_GENERIC, "image components have different height");
		}
		if (jpx->comps[k].prec != jpx->comps[0].prec)
		{
			opj_image_destroy(jpx);
			fz_throw(ctx, FZ_ERROR_GENERIC, "image components have different precision");
		}
	}

	n = jpx->numcomps;
	w = jpx->comps[0].w;
	h = jpx->comps[0].h;
	depth = jpx->comps[0].prec;
	sgnd = jpx->comps[0].sgnd;

	if (jpx->color_space == CLRSPC_SRGB && n == 4) { n = 3; a = 1; }
	else if (jpx->color_space == CLRSPC_SYCC && n == 4) { n = 3; a = 1; }
	else if (n == 2) { n = 1; a = 1; }
	else if (n > 4) { n = 4; a = 1; }
	else { a = 0; }

	fz_colorspace *colorspace = 0;
	origcs = defcs;
	if (defcs)
	{
		if (defcs->n == n)
		{
			colorspace = defcs;
		}
		else
		{
			fz_warn(ctx, "jpx file and dict colorspaces do not match");
			defcs = NULL;
		}
	}

	if (!defcs)
	{
		switch (n)
		{
		case 1: colorspace = fz_device_gray(ctx); break;
		case 3: colorspace = fz_device_rgb(ctx); break;
		case 4: colorspace = fz_device_cmyk(ctx); break;
		}
	}

	fz_pixmap *img = 0;
	fz_try(ctx)
	{
		img = fz_new_pixmap(ctx, colorspace, w, h);
	}
	fz_catch(ctx)
	{
		opj_image_destroy(jpx);
		fz_rethrow_message(ctx, "out of memory loading jpx");
	}

	p = img->samples;
	for (y = 0; y < h; y++)
	{
		for (x = 0; x < w; x++)
		{
			for (k = 0; k < n + a; k++)
			{
				v = jpx->comps[k].data[y * w + x];
				if (sgnd)
					v = v + (1 << (depth - 1));
				if (depth > 8)
					v = v >> (depth - 8);
				*p++ = v;
			}
			if (!a)
				*p++ = 255;
		}
	}

	opj_image_destroy(jpx);
	opj_cio_close(cio);
	opj_destroy_decompress(dinfo);

	if (a)
	{
		if (n == 4)
		{
			fz_pixmap *tmp = fz_new_pixmap(ctx, fz_device_rgb(ctx), w, h);
			fz_convert_pixmap(ctx, tmp, img);
			fz_drop_pixmap(ctx, img);
			img = tmp;
		}
		fz_premultiply_pixmap(ctx, img);
	}

	if (origcs != defcs)
	{
		fz_pixmap *tmp = fz_new_pixmap(ctx, origcs, w, h);
		fz_convert_pixmap(ctx, tmp, img);
		fz_drop_pixmap(ctx, img);
		img = tmp;
	}

	return img;
}
