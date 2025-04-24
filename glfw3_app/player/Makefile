# -*- tab-width : 4 -*-
#=======================================================================
#   @file
#   @brief  glfw_app (Windows: MSYS2/CLANG64) @n
#			faad(faad2) はソース中に含まれる mp4ff ライブラリが含まれない為 @n
#           独自にソースを取得して libmp4ff.a を生成する必要があります。
#   @author 平松邦仁 (hira@rvf-rc45.net)
#	@copyright	Copyright (C) 2019, 2025 Kunihito Hiramatsu @n
#				Released under the MIT license @n
#				https://github.com/hirakuni45/glfw_app/blob/master/LICENSE
#=======================================================================
TARGET		=	player

ICON_RC		=	icon.rc

# 'debug' or 'release'
BUILD		=	release

VPATH		=	../common

CSOURCES	=	minizip/ioapi.c \
				minizip/unzip.c

PSOURCES	=	main.cpp \
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
INC_USR		=	../libraries/inc
# User(optional) link library
ifeq ($(OS),Windows_NT)
LIBS_USR	=	faad mp4ff
else
LIBS_USR	=	faad mp4ff
endif

# User library path 
LIB_DIR_USR	=	../libraries/lib
# cmpiler flags (-Dxxx)
CFLAGS		=
PFLAGS		=

-include $(VPATH)/makefile

-include $(DEPENDS)
