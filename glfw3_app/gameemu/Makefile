# -*- tab-width : 4 -*-
#=======================================================================
#   @file
#   @brief  glfw_app
#   @author 平松邦仁 (hira@rvf-rc45.net)
#	@copyright	Copyright (C) 2019, 2023 Kunihito Hiramatsu @n
#				Released under the MIT license @n
#				https://github.com/hirakuni45/glfw_app/blob/master/LICENSE
#=======================================================================
TARGET		=	gameemu

# 'debug' or 'release'
BUILD		=	release

VPATH		=	../common

CSOURCES	=	minizip/ioapi.c \
				minizip/unzip.c

PSOURCES	=	main.cpp \
				core/glcore.cpp \
				core/device.cpp \
				widgets/widget_director.cpp \
				widgets/widget_filer.cpp \
				src/devices/cpu/z80/z80.cpp \
				src/devices/machine/gen_latch.cpp \
				src/devices/machine/watchdog.cpp \
				src/devices/sound/samples.cpp \
				src/emu/config.cpp \
				src/emu/ioport.cpp \
				src/emu/tilemap.cpp \
				src/mame/drivers/rallyx.cpp \
				src/mame/video/rallyx.cpp

# C++ version
CPP_VER		=	-std=c++17

# C++ include path for application
PINC_APP	=	. ../common ./src/emu ./src/osd ./src/device ./src/name
# C include path for application
CINC_APP	=	. ../common

# User include path
INC_USR		=
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

-include ../common/makefile

-include $(DEPENDS)