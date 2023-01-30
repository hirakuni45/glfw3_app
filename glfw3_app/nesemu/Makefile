# -*- tab-width : 4 -*-
#=======================================================================
#   @file
#   @brief  glfw_app
#   @author 平松邦仁 (hira@rvf-rc45.net)
#	@copyright	Copyright (C) 2019, 2023 Kunihito Hiramatsu @n
#				Released under the MIT license @n
#				https://github.com/hirakuni45/glfw_app/blob/master/LICENSE
#=======================================================================
TARGET		=	nesemu

ICON_RC		=	icon.rc

# 'debug' or 'release'
BUILD		=	release

VPATH		=	../common

CSOURCES	=	minizip/ioapi.c \
				minizip/unzip.c \
				./emu/log.c \
				./emu/bitmap.c \
				./emu/cpu/nes6502.c \
				./emu/nes/mmclist.c \
				./emu/nes/nes.c \
				./emu/nes/nes_mmc.c \
				./emu/nes/nes_pal.c \
				./emu/nes/nes_ppu.c \
				./emu/nes/nes_rom.c \
				./emu/nes/nesinput.c \
				./emu/nes/nesstate.c \
				./emu/sndhrdw/fds_snd.c \
				./emu/sndhrdw/mmc5_snd.c \
				./emu/sndhrdw/nes_apu.c \
				./emu/sndhrdw/vrcvisnd.c \
				./emu/mappers/map000.c \
				./emu/mappers/map001.c \
				./emu/mappers/map002.c \
				./emu/mappers/map003.c \
				./emu/mappers/map004.c \
				./emu/mappers/map005.c \
				./emu/mappers/map007.c \
				./emu/mappers/map008.c \
				./emu/mappers/map009.c \
				./emu/mappers/map011.c \
				./emu/mappers/map015.c \
				./emu/mappers/map016.c \
				./emu/mappers/map018.c \
				./emu/mappers/map019.c \
				./emu/mappers/map024.c \
				./emu/mappers/map032.c \
				./emu/mappers/map033.c \
				./emu/mappers/map034.c \
				./emu/mappers/map040.c \
				./emu/mappers/map041.c \
				./emu/mappers/map042.c \
				./emu/mappers/map046.c \
				./emu/mappers/map050.c \
				./emu/mappers/map064.c \
				./emu/mappers/map065.c \
				./emu/mappers/map066.c \
				./emu/mappers/map070.c \
				./emu/mappers/map073.c \
				./emu/mappers/map075.c \
				./emu/mappers/map078.c \
				./emu/mappers/map079.c \
				./emu/mappers/map085.c \
				./emu/mappers/map087.c \
				./emu/mappers/map093.c \
				./emu/mappers/map094.c \
				./emu/mappers/map099.c \
				./emu/mappers/map160.c \
				./emu/mappers/map229.c \
				./emu/mappers/map231.c \
				./emu/mappers/mapvrc.c \
				./emu/libsnss/libsnss.c

PSOURCES	=	main.cpp \
				tools.cpp \
				core/glcore.cpp \
				core/device.cpp \
				widgets/widget_director.cpp \
				widgets/widget_filer.cpp

# C++ version
CPP_VER		=	-std=c++17

# C++ include path for application
PINC_APP	=	. ../common
# C include path for application
CINC_APP	=	. ../common

# User include path
INC_USR		=	./emu ./emu/cpu ./emu/nes ./emu/mappers ./emu/libsnss ./emu/sndhrdw
# User(optional) link library
ifeq ($(OS),Windows_NT)
LIBS_USR	=
else
LIBS_USR	=
endif

# User library path 
LIB_DIR_USR	=
# cmpiler flags (-Dxxx)
CFLAGS		=
PFLAGS		=

-include $(VPATH)/makefile

-include $(DEPENDS)
