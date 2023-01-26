# -*- tab-width : 4 -*-
#=======================================================================
#   @file
#   @brief  glfw_app
#   @author 平松邦仁 (hira@rvf-rc45.net)
#	@copyright	Copyright (C) 2019, 2023 Kunihito Hiramatsu @n
#				Released under the MIT license @n
#				https://github.com/hirakuni45/glfw_app/blob/master/LICENSE
#=======================================================================
TARGET		=	ignitor_app

BUILD_ID	=	$(shell date +%Y%m%d%H)

# 'debug' or 'release'
BUILD		=	release

VPATH		=	../common

# 				./emu/nofrendo.c

CSOURCES	=	minizip/ioapi.c \
				minizip/unzip.c

PSOURCES	=	main.cpp \
				core/glcore.cpp \
				core/device.cpp \
				gl_fw/glmobj.cpp \
				gl_fw/glutils.cpp \
				gl_fw/glterminal.cpp \
				utils/vtx.cpp \
				utils/vmath.cpp \
				utils/sjis_utf16.cpp \
				utils/string_utils.cpp \
				utils/file_io.cpp \
				utils/file_info.cpp \
				utils/files.cpp \
				utils/keyboard.cpp \
				img_io/paint.cpp \
				img_io/bmp_io.cpp \
				img_io/tga_io.cpp \
				img_io/dds_io.cpp \
				img_io/png_io.cpp \
				img_io/jpeg_io.cpp \
				img_io/openjpeg_io.cpp \
				img_io/pvr_io.cpp \
				img_io/img_files.cpp \
				widgets/common_parts.cpp \
				widgets/widget_director.cpp \
				widgets/widget_utils.cpp \
				widgets/widget_filer.cpp

#				snd_io/audio_io.cpp \
				snd_io/pcm.cpp \
				snd_io/wav_io.cpp \
				snd_io/mp3_io.cpp \
				snd_io/snd_files.cpp \
				snd_io/sound.cpp
#				snd_io/aac_io.cpp
#				img_io/pdf_in.cpp

# PNG_FILES	=	graviton.png

ifeq ($(OS),Windows_NT)
	PFLAGS = -DWIN32_LEAN_AND_MEAN -DB_ID=$(BUILD_ID)
endif

STDLIBS		=	ws2_32 wsock32

-include $(VPATH)/makefile

-include $(DEPENDS)
