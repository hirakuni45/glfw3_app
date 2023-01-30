# -*- tab-width : 4 -*-
#=======================================================================
#   @file
#   @brief  glfw_app
#   @author 平松邦仁 (hira@rvf-rc45.net)
#	@copyright	Copyright (C) 2019 Kunihito Hiramatsu @n
#				Released under the MIT license @n
#				https://github.com/hirakuni45/glfw_app/blob/master/LICENSE
#=======================================================================
TARGET		=	prboom

# 'debug' or 'release'
BUILD		=	release

VPATH		=	../common prboom-2.5.0

DOOM_PATH	=	prboom-2.5.0

#				$(DOOM_PATH)/src/i_net.c
#				$(DOOM_PATH)/src/d_net.c
#				$(DOOM_PATH)/src/m_fixed.c
#				$(DOOM_PATH)/src/m_swap.c

CSOURCES	=	minizip/ioapi.c \
				minizip/unzip.c \
				$(DOOM_PATH)/src/GLFW/i_system.c \
				$(DOOM_PATH)/src/GLFW/i_video.c \
				$(DOOM_PATH)/src/GLFW/i_sound.c \
				$(DOOM_PATH)/src/GLFW/i_joy.c \
				$(DOOM_PATH)/src/GLFW/i_main.c \
				$(DOOM_PATH)/src/GLFW/lprintf.c \
				$(DOOM_PATH)/src/mmus2mid.c \
				$(DOOM_PATH)/src/am_map.c \
				$(DOOM_PATH)/src/g_game.c \
				$(DOOM_PATH)/src/p_mobj.c \
				$(DOOM_PATH)/src/r_demo.c \
				$(DOOM_PATH)/src/r_segs.c \
				$(DOOM_PATH)/src/hu_lib.c \
				$(DOOM_PATH)/src/p_plats.c \
				$(DOOM_PATH)/src/r_sky.c \
				$(DOOM_PATH)/src/d_deh.c \
				$(DOOM_PATH)/src/hu_stuff.c \
				$(DOOM_PATH)/src/m_argv.c \
				$(DOOM_PATH)/src/p_pspr.c \
				$(DOOM_PATH)/src/m_bbox.c \
				$(DOOM_PATH)/src/p_saveg.c \
				$(DOOM_PATH)/src/r_things.c \
				$(DOOM_PATH)/src/d_items.c \
				$(DOOM_PATH)/src/m_cheat.c \
				$(DOOM_PATH)/src/p_setup.c \
				$(DOOM_PATH)/src/s_sound.c \
				$(DOOM_PATH)/src/d_main.c \
				$(DOOM_PATH)/src/p_sight.c \
				$(DOOM_PATH)/src/sounds.c \
				$(DOOM_PATH)/src/m_menu.c \
				$(DOOM_PATH)/src/p_spec.c \
				$(DOOM_PATH)/src/info.c \
				$(DOOM_PATH)/src/st_lib.c \
				$(DOOM_PATH)/src/m_misc.c \
				$(DOOM_PATH)/src/p_switch.c \
				$(DOOM_PATH)/src/p_telept.c \
				$(DOOM_PATH)/src/st_stuff.c \
				$(DOOM_PATH)/src/m_random.c \
				$(DOOM_PATH)/src/p_tick.c \
				$(DOOM_PATH)/src/tables.c \
				$(DOOM_PATH)/src/p_user.c \
				$(DOOM_PATH)/src/p_ceilng.c \
				$(DOOM_PATH)/src/v_video.c \
				$(DOOM_PATH)/src/doomdef.c \
				$(DOOM_PATH)/src/p_doors.c \
				$(DOOM_PATH)/src/p_enemy.c \
				$(DOOM_PATH)/src/r_bsp.c \
				$(DOOM_PATH)/src/version.c \
				$(DOOM_PATH)/src/doomstat.c \
				$(DOOM_PATH)/src/p_floor.c \
				$(DOOM_PATH)/src/r_data.c \
				$(DOOM_PATH)/src/w_wad.c \
				$(DOOM_PATH)/src/p_genlin.c \
				$(DOOM_PATH)/src/dstrings.c \
				$(DOOM_PATH)/src/p_inter.c \
				$(DOOM_PATH)/src/wi_stuff.c \
				$(DOOM_PATH)/src/r_draw.c \
				$(DOOM_PATH)/src/f_finale.c \
				$(DOOM_PATH)/src/p_lights.c \
				$(DOOM_PATH)/src/z_bmalloc.c \
				$(DOOM_PATH)/src/p_map.c \
				$(DOOM_PATH)/src/r_main.c \
				$(DOOM_PATH)/src/f_wipe.c \
				$(DOOM_PATH)/src/z_zone.c \
				$(DOOM_PATH)/src/p_maputl.c \
				$(DOOM_PATH)/src/r_plane.c \
				$(DOOM_PATH)/src/md5.c \
				$(DOOM_PATH)/src/p_checksum.c \
				$(DOOM_PATH)/src/r_patch.c \
				$(DOOM_PATH)/src/r_fps.c \
				$(DOOM_PATH)/src/r_filter.c \
				$(DOOM_PATH)/src/w_memcache.c

#				$(DOOM_PATH)/src/gl_main.c \
				$(DOOM_PATH)/src/gl_texture.c \
#				$(DOOM_PATH)/src/d_client.c
#				$(DOOM_PATH)/src/w_mmap.c

PSOURCES	=	main.cpp \
				core/glcore.cpp \
				core/device.cpp \
				widgets/widget_director.cpp \
				widgets/widget_filer.cpp

# C++ version
CPP_VER		=	-std=c++17

# C++ include path for application
PINC_APP	=	. ../common ./prboom-2.5.0 ./prboom-2.5.0/src
# C include path for application
CINC_APP	=	. ../common ./prboom-2.5.0 ./prboom-2.5.0/src

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