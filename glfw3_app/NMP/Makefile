# -*- tab-width : 4 -*-
#===========================================================================
#   @file
#   @brief  glfw_app Necronomicon Pinball
#   @author 平松邦仁 (hira@rvf-rc45.net)
#	@copyright	Copyright (C) 2026 Kunihito Hiramatsu @n
#				Released under the MIT license @n
#				https://github.com/hirakuni45/glfw_app/blob/master/LICENSE
#===========================================================================
TARGET		=	nmp

# 'debug' or 'release'
BUILD		=	release

VPATH		=	../common src

SRC_PATH	=	src

CSOURCES	=	minizip/ioapi.c \
				minizip/unzip.c \
				$(SRC_PATH)/top.c \
				$(SRC_PATH)/sim.c \
				$(SRC_PATH)/vect.c \
				$(SRC_PATH)/fnc.c \
				$(SRC_PATH)/mobj.c \
				$(SRC_PATH)/pinball.c \
				$(SRC_PATH)/task.c \
				$(SRC_PATH)/dex.c \
				$(SRC_PATH)/mw.c \
				$(SRC_PATH)/saturn.c \
				$(SRC_PATH)/genr.c \
				$(SRC_PATH)/arkham.c \
				$(SRC_PATH)/bloody.c \
				$(SRC_PATH)/dreaml.c \
				$(SRC_PATH)/cdfs.c \
				$(SRC_PATH)/etc.c \
				$(SRC_PATH)/info.c \
				$(SRC_PATH)/flags.c \
				$(SRC_PATH)/tracks.c

#				$(SRC_PATH)/sblcld.c
# OBJS	=	launch/start.o launch/stack.o launch/entry.o
#		top.o
#		sim.o
#		vect.o
#		fnc.o fnca.o
#		mobj.o
#		pinball.o
#		task.o
#		dex.o
#		mw.o
#		saturn.o
#		genr.o
#		arkham.o
#		bloody.o
#		dreaml.o
#		cdfs.o
#		etc.o
#		info.o
#		flags.o
#		tracks.o
#		sblcld.o

PSOURCES	=	main.cpp \
				core/glcore.cpp \
				core/device.cpp \
				widgets/widget_director.cpp \
				widgets/widget_filer.cpp

# C++ version
CPP_VER		=	-std=c++20

# C++ include path for application
PINC_APP	=	. ../common ./$(DOOM_PATH) ./$(DOOM_PATH)/src
# C include path for application
CINC_APP	=	. ../common ./$(DOOM_PATH) ./$(DOOM_PATH)/src

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