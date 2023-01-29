# -*- tab-width : 4 -*-
#=======================================================================
#   @file
#   @brief  glfw_app
#   @author 平松邦仁 (hira@rvf-rc45.net)
#	@copyright	Copyright (C) 2019 Kunihito Hiramatsu @n
#				Released under the MIT license @n
#				https://github.com/hirakuni45/glfw_app/blob/master/LICENSE
#=======================================================================
TARGET		=	doom

# 'debug' or 'release'
BUILD		=	release

VPATH		=	../common DOOM

DOOM_PATH	=	DOOM

CSOURCES	=	minizip/ioapi.c \
				minizip/unzip.c \
				$(DOOM_PATH)/am_map.c \
				$(DOOM_PATH)/d_event.c \
				$(DOOM_PATH)/d_items.c \
				$(DOOM_PATH)/d_iwad.c \
				$(DOOM_PATH)/d_loop.c \
				$(DOOM_PATH)/d_main.c \
				$(DOOM_PATH)/d_mode.c \
				$(DOOM_PATH)/d_net.c \
				$(DOOM_PATH)/doomdef.c \
				$(DOOM_PATH)/doomgeneric.c \
				$(DOOM_PATH)/doomgeneric_TX.c \
				$(DOOM_PATH)/doomstat.c \
				$(DOOM_PATH)/dstrings.c \
				$(DOOM_PATH)/dummy.c \
				$(DOOM_PATH)/f_finale.c \
				$(DOOM_PATH)/f_wipe.c \
				$(DOOM_PATH)/hu_lib.c \
				$(DOOM_PATH)/hu_stuff.c \
				$(DOOM_PATH)/i_cdmus.c \
				$(DOOM_PATH)/i_endoom.c \
				$(DOOM_PATH)/i_input.c \
				$(DOOM_PATH)/i_joystick.c \
				$(DOOM_PATH)/i_main.c \
				$(DOOM_PATH)/i_scale.c \
				$(DOOM_PATH)/i_sound.c \
				$(DOOM_PATH)/i_system.c \
				$(DOOM_PATH)/i_timer.c \
				$(DOOM_PATH)/i_video.c \
				$(DOOM_PATH)/icon.c \
				$(DOOM_PATH)/info.c \
				$(DOOM_PATH)/m_argv.c \
				$(DOOM_PATH)/m_bbox.c \
				$(DOOM_PATH)/m_cheat.c \
				$(DOOM_PATH)/m_config.c \
				$(DOOM_PATH)/m_controls.c \
				$(DOOM_PATH)/m_fixed.c \
				$(DOOM_PATH)/m_menu.c \
				$(DOOM_PATH)/m_misc.c \
				$(DOOM_PATH)/m_random.c \
				$(DOOM_PATH)/memio.c \
				$(DOOM_PATH)/p_ceilng.c \
				$(DOOM_PATH)/p_doors.c \
				$(DOOM_PATH)/p_enemy.c \
				$(DOOM_PATH)/p_floor.c \
				$(DOOM_PATH)/p_inter.c \
				$(DOOM_PATH)/p_lights.c \
				$(DOOM_PATH)/p_map.c \
				$(DOOM_PATH)/p_maputl.c \
				$(DOOM_PATH)/p_mobj.c \
				$(DOOM_PATH)/p_plats.c \
				$(DOOM_PATH)/p_pspr.c \
				$(DOOM_PATH)/p_saveg.c \
				$(DOOM_PATH)/p_setup.c \
				$(DOOM_PATH)/p_sight.c \
				$(DOOM_PATH)/p_spec.c \
				$(DOOM_PATH)/p_switch.c \
				$(DOOM_PATH)/p_telept.c \
				$(DOOM_PATH)/p_tick.c \
				$(DOOM_PATH)/p_user.c \
				$(DOOM_PATH)/r_bsp.c \
				$(DOOM_PATH)/r_data.c \
				$(DOOM_PATH)/r_draw.c \
				$(DOOM_PATH)/r_main.c \
				$(DOOM_PATH)/r_plane.c \
				$(DOOM_PATH)/r_segs.c \
				$(DOOM_PATH)/r_sky.c \
				$(DOOM_PATH)/r_things.c \
				$(DOOM_PATH)/s_sound.c \
				$(DOOM_PATH)/sha1.c \
				$(DOOM_PATH)/sounds.c \
				$(DOOM_PATH)/st_lib.c \
				$(DOOM_PATH)/st_stuff.c \
				$(DOOM_PATH)/statdump.c \
				$(DOOM_PATH)/tables.c \
				$(DOOM_PATH)/v_video.c \
				$(DOOM_PATH)/w_checksum.c \
				$(DOOM_PATH)/w_file.c \
				$(DOOM_PATH)/w_main.c \
				$(DOOM_PATH)/w_wad.c \
				$(DOOM_PATH)/wi_stuff.c \
				$(DOOM_PATH)/z_zone.c

#				$(DOOM_PATH)/g_game.c
#				$(DOOM_PATH)/gusconf.c

PSOURCES	=	main.cpp \
				core/glcore.cpp \
				core/device.cpp \
				widgets/widget_director.cpp \
				widgets/widget_filer.cpp

# C++ version
CPP_VER		=	-std=c++17

# C++ include path for application
PINC_APP	=	. ../common ./$(DOOM_PATH)/include
# C include path for application
CINC_APP	=	. ../common ./$(DOOM_PATH)/include

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
