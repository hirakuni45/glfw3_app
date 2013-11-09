/*
 *  tga.c
 *
 *  Copyright (C) 2001-2002  Matthias Brueckner <matbrc@gmx.de>
 *  This file is part of the TGA library (libtga).
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Library General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */
 
#include <stdio.h>
#include <stdlib.h>
#include "tga.h"

static int getc_(void* context)
{
	return getc((FILE*)context);
}

static size_t read_(void *ptr, size_t size, void* context)
{
	return fread(ptr, 1, size, (FILE*)(context));
}

static int putc_(char ch, void* context)
{
	return putc(ch, (FILE*)context);
}

static size_t write_(const void *ptr, size_t size, void* context)
{
	return fwrite(ptr, 1, size, (FILE*)(context));
}

static int seek_(size_t offset, int mode, void* context)
{
	return fseek((FILE*)(context), offset, mode);
}

static size_t tell_(void* context)
{
	return ftell((FILE*)(context));
}


TGA*
TGAOpen(char *file, 
	char *mode)
{
 	TGA *tga;
	FILE *fd;

	tga = (TGA*)malloc(sizeof(TGA));
	if (!tga) {
		TGA_ERROR(tga, TGA_OOM);
		return NULL;
	}
	
	tga->off = 0;

	fd = fopen(file, mode);
	if (!fd) {
		TGA_ERROR(tga, TGA_OPEN_FAIL);
		free(tga);
		return NULL;
	}

	tga->file.getc = getc_;
	tga->file.read = read_;
	tga->file.putc = putc_;
	tga->file.write = write_;
	tga->file.seek = seek_;
	tga->file.tell = tell_;

	tga->fd = fd;
	tga->last = TGA_OK;
	return tga;
}

TGA*
TGAInit(void)
{
	TGA *tga;

	tga = (TGA*)malloc(sizeof(TGA));
	if (!tga) {
		TGA_ERROR(tga, TGA_OOM);
		return NULL;
	}

	tga->off = 0;
	tga->fd = NULL;
	tga->last = TGA_OK;
	return tga;
}

TGA*
TGAOpenFd(FILE *fd)
{
	TGA *tga;

	tga = (TGA*)malloc(sizeof(TGA));
	if (!tga) {
		TGA_ERROR(tga, TGA_OOM);
		return NULL;
	}

	if (!fd) {
		TGA_ERROR(tga, TGA_OPEN_FAIL);
		free(tga);
		return NULL;
	}

	tga->file.getc = getc_;
	tga->file.read = read_;
	tga->file.putc = putc_;
	tga->file.write = write_;
	tga->file.seek = seek_;
	tga->file.tell = tell_;

	tga->off = tga->file.tell(fd);
	if(tga->off == -1) {
		TGA_ERROR(tga, TGA_OPEN_FAIL);
		free(tga);
		return NULL;
	}

	tga->fd = fd;
	tga->last = TGA_OK;
	return tga;
}


void
TGAFree(TGA *tga)
{
	free(tga);
}


void
TGAClose(TGA *tga)
{
	if (tga) {
		fclose(tga->fd);
		free(tga);
	}
}


const char*
TGAStrError(tuint8 code)
{
	if (code >= TGA_ERRORS) code = TGA_ERROR;
	return tga_error_strings[code];
}


tlong
__TGASeek(TGA  *tga, 
	  tlong off, 
	  int   whence)
{
	tga->file.seek(off, whence, tga->fd);
	tga->off = tga->file.tell(tga->fd);
	return tga->off;
}


void
__TGAbgr2rgb(tbyte  *data, 
	     size_t  size, 
	     size_t  bytes)
{
	size_t i;
	tbyte tmp;
	
	for (i = 0; i < size; i += bytes) {
		tmp = data[i];
		data[i] = data[i + 2];
		data[i + 2] = tmp;
	}
}
