# -*- tab-width : 4 -*-
#=======================================================================
#   @file
#   @brief  glfw_app @n
#			target:Windows:MSYS2: clan64
#   @author 平松邦仁 (hira@rvf-rc45.net)
#	@copyright	Copyright (C) 2023 Kunihito Hiramatsu @n
#				Released under the MIT license @n
#				https://github.com/hirakuni45/glfw_app/blob/master/LICENSE
#=======================================================================
TARGET		=	basic

# 'debug' or 'release'
BUILD		=	release

VPATH		=	../common

CSOURCES	=	minizip/ioapi.c \
				minizip/unzip.c

PSOURCES	=	main.cpp \
				basic_gui.cpp \
				core/glcore.cpp \
				core/device.cpp \
				widgets/common_parts.cpp \
				widgets/widget_director.cpp \
				widgets/widget_filer.cpp

# C++ version
CPP_VER		=	-std=c++17

# C++ include path for application
PINC_APP	=	. ../common
# C include path for application
CINC_APP	=	. ../common

# User include path
INC_USR		=
# User(optional) link library
LIBS_USR	=	ws2_32 wsock32 iconv
# User library path 
LIB_DIR_USR	=
# cmpiler flags (-Dxxx)
CFLAGS		=
PFLAGS		=

-include $(VPATH)/makefile

-include $(DEPENDS)
