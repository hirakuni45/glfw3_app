# -*- tab-width : 4 -*-
#=======================================================================
#   @file
#   @brief  glfw_app
#   @author 平松邦仁 (hira@rvf-rc45.net)
#	@copyright	Copyright (C) 2019 Kunihito Hiramatsu @n
#				Released under the MIT license @n
#				https://github.com/hirakuni45/glfw_app/blob/master/LICENSE
#=======================================================================
TARGET		=	gbemu

# 'debug' or 'release'
BUILD		=	release

VPATH		=	../common src

CSOURCES	=	minizip/ioapi.c \
				minizip/unzip.c
#				src/miniz/miniz.c

PSOURCES	=	main.cpp \
				core/glcore.cpp \
				core/device.cpp \
				widgets/widget_director.cpp \
				widgets/widget_filer.cpp \
				src/Audio.cpp \
				src/Cartridge.cpp \
				src/CommonMemoryRule.cpp \
				src/GearboyCore.cpp \
				src/Input.cpp \
				src/IORegistersMemoryRule.cpp \
				src/MBC1MemoryRule.cpp \
				src/MBC2MemoryRule.cpp \
				src/MBC3MemoryRule.cpp \
				src/MBC5MemoryRule.cpp \
				src/Memory.cpp \
				src/MemoryRule.cpp \
				src/MultiMBC1MemoryRule.cpp \
				src/opcodes.cpp \
				src/opcodes_cb.cpp \
				src/Processor.cpp \
				src/RomOnlyMemoryRule.cpp \
				src/Video.cpp \
				src/audio/Blip_Buffer.cpp \
				src/audio/Effects_Buffer.cpp \
				src/audio/Gb_Apu.cpp \
				src/audio/Gb_Apu_State.cpp \
				src/audio/Gb_Oscs.cpp \
				src/audio/Multi_Buffer.cpp

# C++ version
CPP_VER		=	-std=c++17

# C++ include path for application
PINC_APP	=	. ../common ./src
# C include path for application
CINC_APP	=	. ../common

# User include path
INC_USR		=
# User(optional) link library
LIBS_USR	=
# User library path 
LIB_DIR_USR	=
# cmpiler flags (-Dxxx)
CFLAGS		=
PFLAGS		=

-include ../common/makefile

-include $(DEPENDS)
