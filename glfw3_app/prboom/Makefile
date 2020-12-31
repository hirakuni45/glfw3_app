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

# 				$(DOOM_PATH)/src/i_sound.c
#				$(DOOM_PATH)/src/i_video.c
#				$(DOOM_PATH)/src/i_net.c
#				$(DOOM_PATH)/src/d_net.c
#				$(DOOM_PATH)/src/m_fixed.c
#				$(DOOM_PATH)/src/m_swap.c

CSOURCES	=	minizip/ioapi.c \
				minizip/unzip.c \
				$(DOOM_PATH)/src/doomdef.c \
				$(DOOM_PATH)/src/doomstat.c \
				$(DOOM_PATH)/src/dstrings.c \
				$(DOOM_PATH)/src/POSIX/i_system.c \
				$(DOOM_PATH)/src/tables.c \
				$(DOOM_PATH)/src/f_finale.c \
				$(DOOM_PATH)/src/f_wipe.c \
				$(DOOM_PATH)/src/d_main.c \
				$(DOOM_PATH)/src/d_items.c \
				$(DOOM_PATH)/src/g_game.c \
				$(DOOM_PATH)/src/m_menu.c \
				$(DOOM_PATH)/src/m_misc.c \
				$(DOOM_PATH)/src/m_argv.c \
				$(DOOM_PATH)/src/m_bbox.c \
				$(DOOM_PATH)/src/m_cheat.c \
				$(DOOM_PATH)/src/m_random.c \
				$(DOOM_PATH)/src/am_map.c \
				$(DOOM_PATH)/src/p_ceilng.c \
				$(DOOM_PATH)/src/p_doors.c \
				$(DOOM_PATH)/src/p_enemy.c \
				$(DOOM_PATH)/src/p_floor.c \
				$(DOOM_PATH)/src/p_inter.c \
				$(DOOM_PATH)/src/p_lights.c \
				$(DOOM_PATH)/src/p_map.c \
				$(DOOM_PATH)/src/p_maputl.c \
				$(DOOM_PATH)/src/p_plats.c \
				$(DOOM_PATH)/src/p_pspr.c \
				$(DOOM_PATH)/src/p_setup.c \
				$(DOOM_PATH)/src/p_sight.c \
				$(DOOM_PATH)/src/p_spec.c \
				$(DOOM_PATH)/src/p_switch.c \
				$(DOOM_PATH)/src/p_mobj.c \
				$(DOOM_PATH)/src/p_telept.c \
				$(DOOM_PATH)/src/p_tick.c \
				$(DOOM_PATH)/src/p_saveg.c \
				$(DOOM_PATH)/src/p_user.c \
				$(DOOM_PATH)/src/r_bsp.c \
				$(DOOM_PATH)/src/r_data.c \
				$(DOOM_PATH)/src/r_draw.c \
				$(DOOM_PATH)/src/r_main.c \
				$(DOOM_PATH)/src/r_plane.c \
				$(DOOM_PATH)/src/r_segs.c \
				$(DOOM_PATH)/src/r_sky.c \
				$(DOOM_PATH)/src/r_things.c \
				$(DOOM_PATH)/src/w_wad.c \
				$(DOOM_PATH)/src/wi_stuff.c \
				$(DOOM_PATH)/src/v_video.c \
				$(DOOM_PATH)/src/st_lib.c \
				$(DOOM_PATH)/src/st_stuff.c \
				$(DOOM_PATH)/src/hu_stuff.c \
				$(DOOM_PATH)/src/hu_lib.c \
				$(DOOM_PATH)/src/s_sound.c \
				$(DOOM_PATH)/src/z_zone.c \
				$(DOOM_PATH)/src/info.c \
				$(DOOM_PATH)/src/sounds.c


# $(DOOM_PATH)/src/i_main.c

PSOURCES	=	main.cpp \
				core/glcore.cpp \
				core/device.cpp \
				gl_fw/glmobj.cpp \
				gl_fw/glutils.cpp \
				gl_fw/glterminal.cpp \
				utils/vtx.cpp \
				utils/vmath.cpp \
				utils/sjis_utf16.cpp \
				utils/string_utils.cpp \
				utils/file_io.cpp \
				utils/file_info.cpp \
				utils/files.cpp \
				utils/keyboard.cpp \
				img_io/paint.cpp \
				img_io/bmp_io.cpp \
				img_io/tga_io.cpp \
				img_io/dds_io.cpp \
				img_io/png_io.cpp \
				img_io/jpeg_io.cpp \
				img_io/openjpeg_io.cpp \
				img_io/pvr_io.cpp \
				img_io/img_files.cpp \
				snd_io/audio_io.cpp \
				snd_io/pcm.cpp \
				snd_io/wav_io.cpp \
				snd_io/mp3_io.cpp \
				snd_io/snd_files.cpp \
				snd_io/sound.cpp \
				widgets/common_parts.cpp \
				widgets/widget_director.cpp \
				widgets/widget_utils.cpp \
				widgets/widget_filer.cpp

# platform switcher (Windows, Linux, OS-X)
ifeq ($(OS),Windows_NT)
    FEXT	= .exe
#   ICON_RC = icon.rc
    SYSTEM := WIN
	CPP_VER := -std=c++17
    LOCAL_INC_PATH := /mingw64/include
    LOCAL_LIB_PATH := /mingw64/lib
	OPTLIBS = opengl32 glew32 openal glfw3
	CPMM	= g++
	CCMM	= gcc
	CFLAGS  = -DWIN32
	PFLAGS  = -DWIN32 -DBOOST_USE_WINDOWS_H
	LFLAGS =
else
  FEXT	=
  ICON_RC =
  UNAME := $(shell uname -s)
  ifeq ($(UNAME),Linux)
    SYSTEM := LINUX
	CPP_VER := -std=c++17
	CFLAGS =
	PFLAGS =
	LFLAGS =
  endif
  ifeq ($(UNAME),Darwin)
    SYSTEM := OSX
	OSX_VER := $(shell sw_vers -productVersion | sed 's/^\([0-9]*.[0-9]*\).[0-9]*/\1/')
	CPP_VER := -std=c++17
	CFLAGS = -DOS_X
	PFLAGS =
	LFLAGS = -isysroot /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk \
		-Wl,-search_paths_first -Wl,-headerpad_max_install_names \
		-framework AGL -framework Cocoa -framework OpenGL -framework IOKit \
		-framework CoreFoundation -framework CoreVideo -framework OpenAL
    LOCAL_INC_PATH := /opt/local/include
    LOCAL_LIB_PATH := /opt/local/lib
    OPTLIBS = glfw GLEW
  else
    LOCAL_INC_PATH := /usr/local/include
    LOCAL_LIB_PATH := /usr/local/lib
    OPTLIBS = glfw GLEW
  endif
  CPMM	=	clang++
  CCMM	=	clang
endif

# build openjpeg any version path
OPENJPEG_PATH := $(shell ls -d $(LOCAL_INC_PATH)/openjpeg*)

STDLIBS		=

OPTLIBS += 	pthread \
			png jpeg openjp2 \
			freetype \
			mad tag \
			z

INC_SYS		=	$(LOCAL_INC_PATH) \
				$(LOCAL_INC_PATH)/freetype2 \
				$(OPENJPEG_PATH)
INC_LIB		=
LIBDIR		=	$(LOCAL_LIB_PATH)

PINC_APP	=	. ../common
CINC_APP	=	. ../common $(DOOM_PATH) $(DOOM_PATH)/src

INC_S	=	$(addprefix -isystem , $(INC_SYS))
INC_L	=	$(addprefix -isystem , $(INC_LIB))
INC_P	=	$(addprefix -I, $(PINC_APP))
INC_C	=	$(addprefix -I, $(CINC_APP))
CINCS	=	$(INC_S) $(INC_L) $(INC_C)
PINCS	=	$(INC_S) $(INC_L) $(INC_P)
LIBS	=	$(addprefix -L, $(LIBDIR))
LIBN	=	$(addprefix -l, $(STDLIBS))
LIBN	+=	$(addprefix -l, $(OPTLIBS))

#
# Compiler, Linker Options, Resource_compiler
#
CP	=	clang++
CC	=	clang
LK	=	clang++
RC	=	windres

POPT	=	-O2 $(CPP_VER)
COPT	=	-O2
LOPT	=

PFLAGS +=	-DHAVE_STDINT_H
CFLAGS +=	-DHAVE_CONFIG_H

ifeq ($(BUILD),debug)
	POPT += -g
	COPT += -g
	PFLAGS += -DDEBUG
	CFLAGS += -DDEBUG
endif

ifeq ($(BUILD),release)
	PFLAGS += -DNDEBUG
	CFLAGS += -DNDEBUG
endif

# -Wuninitialized -Wunused -Werror -Wshadow
CCWARN	=	-Wall -Werror -Wimplicit -Wreturn-type -Wswitch \
			-Wformat \
			-Wno-unused-const-variable -Wno-unused-variable \
			-Wno-switch \
			-Wno-sizeof-pointer-memaccess \
			-Wno-sometimes-uninitialized \
			-Wno-unused-function -Wno-unused-value

CPWARN	=	-Wall -Werror -Wno-unused-private-field \
			-Wno-unused-variable -Wno-unused-function

OBJECTS	=	$(addprefix $(BUILD)/,$(patsubst %.cpp,%.o,$(PSOURCES))) \
			$(addprefix $(BUILD)/,$(patsubst %.c,%.o,$(CSOURCES)))
DEPENDS =   $(patsubst %.o,%.d, $(OBJECTS))

ifdef ICON_RC
	ICON_OBJ =	$(addprefix $(BUILD)/,$(patsubst %.rc,%.o,$(ICON_RC)))
endif

.PHONY: all clean
.SUFFIXES :
.SUFFIXES : .rc .hpp .h .c .cpp .o

all: $(BUILD) $(TARGET)$(FEXT)

$(TARGET)$(FEXT): $(OBJECTS) $(ICON_OBJ) Makefile
	$(LK) $(LFLAGS) $(LIBS) $(OBJECTS) $(ICON_OBJ) $(LIBN) -o $(TARGET)$(FEXT)

$(BUILD)/%.o : %.c
	mkdir -p $(dir $@); \
	$(CC) -c $(COPT) $(CFLAGS) $(CINCS) $(CCWARN) -o $@ $<

$(BUILD)/%.o : %.cpp
	mkdir -p $(dir $@); \
	$(CP) -c $(POPT) $(PFLAGS) $(PINCS) $(CPWARN) -o $@ $<

$(ICON_OBJ): $(ICON_RC)
	$(RC) -i $< -o $@

$(BUILD)/%.d : %.c
	mkdir -p $(dir $@); \
	$(CCMM) -MM -DDEPEND_ESCAPE $(COPT) $(CFLAGS) $(CINCS) $< \
	| sed 's/$(notdir $*)\.o:/$(subst /,\/,$(patsubst %.d,%.o,$@) $@):/' > $@ ; \
	[ -s $@ ] || rm -f $@

$(BUILD)/%.d : %.cpp
	mkdir -p $(dir $@); \
	$(CPMM) -MM -DDEPEND_ESCAPE $(POPT) $(PFLAGS) $(PINCS) $< \
	| sed 's/$(notdir $*)\.o:/$(subst /,\/,$(patsubst %.d,%.o,$@) $@):/' > $@ ; \
	[ -s $@ ] || rm -f $@

ifeq ($(SYSTEM),WIN)
strip:
	$(LK) $(LFLAGS) $(LIBS) $(OBJECTS) $(ICON_OBJ) $(LIBN) -mwindows -o $(TARGET)$(FEXT)
endif

run:
	./$(TARGET)

clean:
	rm -rf $(BUILD) $(TARGET)$(FEXT)

clean_depend:
	rm -f $(DEPENDS)

dllname:
	objdump -p $(TARGET)$(FEXT) | grep --text "DLL Name"

tarball:
	tar cfvz $(TARGET)_$(shell date +%Y%m%d%H).tgz \
	*.[hc]pp Makefile ../common/*/*.[hc]pp ../common/*/*.[hc]

bin_zip:
	$(LK) $(LFLAGS) $(LIBS) $(OBJECTS) $(ICON_OBJ) $(LIBN) -mwindows -o $(TARGET)$(FEXT) 
	rm -f $(TARGET)_$(shell date +%Y%m%d%H)_bin.zip
	zip $(TARGET)_$(shell date +%Y%m%d%H)_bin.zip *.exe *.dll

-include $(DEPENDS)
