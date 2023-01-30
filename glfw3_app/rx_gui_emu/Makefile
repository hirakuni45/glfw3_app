# -*- tab-width : 4 -*-
#=======================================================================
#   @file
#   @brief  glfw_app Makefile
#   @author 平松邦仁 (hira@rvf-rc45.net)
#   @copyright  Copyright (C) 2019 Kunihito Hiramatsu @n
#               Released under the MIT license @n
#               https://github.com/hirakuni45/glfw_app/blob/master/LICENSE
#=======================================================================
TARGET		=	rx_gui_emu

ICON_RC		=

# 'debug' or 'release'
BUILD		=	release

VPATH		=	../common ./RX

CSOURCES	=	minizip/ioapi.c \
				minizip/unzip.c

#				RX/CALC_app/calc_gui.cpp
#				RX/CALC_app/resource.cpp

#				RX/DSOS_app/dso_gui.cpp
#				RX/DSOS_app/resource.cpp

#				RX/SYNTH_app/synth_gui.cpp

PSOURCES	=	main.cpp \
				RX/LOGGER_app/root_menu.cpp \
				RX/graphics/font8x16.cpp \
				RX/graphics/kfont16.cpp \
				RX/graphics/color.cpp \
				core/glcore.cpp \
				core/device.cpp \
				widgets/widget_director.cpp \
				widgets/widget_filer.cpp

# C++ version
CPP_VER		=	-std=c++17

# C++ include path for application
PINC_APP	=	. ../common ./RX
# C include path for application
CINC_APP	=	. ../common

# User include path
INC_USR		=
# User(optional) link library
ifeq ($(OS),Windows_NT)
LIBS_USR	=	mpfr gmp
else
LIBS_USR	=	mpfr gmp
endif

# User library path 
LIB_DIR_USR	=
# cmpiler flags (-Dxxx)
CFLAGS		=
PFLAGS		=

-include ../common/makefile

copy:
	cp RX/common/basic_arith.hpp /d/Git/RX/common/.
	cp RX/common/mpfr.hpp /d/Git/RX/common/.
	cp RX/CALC_app/calc_gui.hpp /d/Git/RX/CALC_sample/.
	cp RX/CALC_app/calc_func.hpp /d/Git/RX/CALC_sample/.
	cp RX/CALC_app/calc_symbol.hpp /d/Git/RX/CALC_sample/.
	cp RX/CALC_app/resource.[hc]pp /d/Git/RX/CALC_sample/.
	cp -r RX/CALC_app/resource /d/Git/RX/CALC_sample/. 
	cp RX/graphics/color.[hc]pp /d/Git/RX/graphics/.
	cp RX/graphics/button.hpp /d/Git/RX/graphics/.
	cp RX/graphics/widget.hpp /d/Git/RX/graphics/.
	cp RX/graphics/graphics.hpp /d/Git/RX/graphics/.
	cp RX/graphics/widget_director.hpp /d/Git/RX/graphics/.

-include $(DEPENDS)
