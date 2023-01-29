# -*- tab-width : 4 -*-
#=======================================================================
#   @file
#   @brief  glfw_app
#   @author 平松邦仁 (hira@rvf-rc45.net)
#	@copyright	Copyright (C) 2019, 2023 Kunihito Hiramatsu @n
#				Released under the MIT license @n
#				https://github.com/hirakuni45/glfw_app/blob/master/LICENSE
#=======================================================================
TARGET		=	dx7emu

# 'debug' or 'release'
BUILD		=	release

VPATH		=	../common

CSOURCES	=	minizip/ioapi.c \
				minizip/unzip.c \
				libfmsynth/src/fmsynth.c
#				src/miniz/miniz.c

PSOURCES	=	main.cpp \
				core/glcore.cpp \
				core/device.cpp \
				widgets/widget_director.cpp \
				widgets/widget_filer.cpp \
				MD_MIDI/MD_MIDIHelper.cpp \
				MD_MIDI/MD_MIDITrack.cpp \
				MD_MIDI/MD_MIDIFile.cpp

#				src/fir.cpp \
				src/fm_core.cpp \
				src/fm_op_kernel.cpp \
				src/resofilter.cpp \
				src/sawtooth.cpp

# C++ version
CPP_VER		=	-std=c++17

# C++ include path for application
PINC_APP	=	. ../common ./libfmsynth/include ./MD_MIDI
# C include path for application
CINC_APP	=	. ../common ./libfmsynth/include

# User include path
INC_USR		=
# User(optional) link library
LIBS_USR	=	winmm
# User library path 
LIB_DIR_USR	=
# cmpiler flags (-Dxxx)
CFLAGS		=
PFLAGS		=

-include $(VPATH)/makefile

-include $(DEPENDS)
