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

VPATH		=	../common DOOM-master

DOOM_PATH	=	DOOM-master/linuxdoom-1.10

CSOURCES	=	minizip/ioapi.c \
				minizip/unzip.c \
				$(DOOM_PATH)/doomdef.c \
				$(DOOM_PATH)/doomstat.c \
				$(DOOM_PATH)/dstrings.c \
				$(DOOM_PATH)/i_system.c \
				$(DOOM_PATH)/i_sound.c \
				$(DOOM_PATH)/i_video.c \
				$(DOOM_PATH)/i_net.c \
				$(DOOM_PATH)/tables.c \
				$(DOOM_PATH)/f_finale.c \
				$(DOOM_PATH)/f_wipe.c \
				$(DOOM_PATH)/d_main.c \
				$(DOOM_PATH)/d_net.c \
				$(DOOM_PATH)/d_items.c \
				$(DOOM_PATH)/g_game.c \
				$(DOOM_PATH)/m_menu.c \
				$(DOOM_PATH)/m_misc.c \
				$(DOOM_PATH)/m_argv.c \
				$(DOOM_PATH)/m_bbox.c \
				$(DOOM_PATH)/m_fixed.c \
				$(DOOM_PATH)/m_swap.c \
				$(DOOM_PATH)/m_cheat.c \
				$(DOOM_PATH)/m_random.c \
				$(DOOM_PATH)/am_map.c \
				$(DOOM_PATH)/p_ceilng.c \
				$(DOOM_PATH)/p_doors.c \
				$(DOOM_PATH)/p_enemy.c \
				$(DOOM_PATH)/p_floor.c \
				$(DOOM_PATH)/p_inter.c \
				$(DOOM_PATH)/p_lights.c \
				$(DOOM_PATH)/p_map.c \
				$(DOOM_PATH)/p_maputl.c \
				$(DOOM_PATH)/p_plats.c \
				$(DOOM_PATH)/p_pspr.c \
				$(DOOM_PATH)/p_setup.c \
				$(DOOM_PATH)/p_sight.c \
				$(DOOM_PATH)/p_spec.c \
				$(DOOM_PATH)/p_switch.c \
				$(DOOM_PATH)/p_mobj.c \
				$(DOOM_PATH)/p_telept.c \
				$(DOOM_PATH)/p_tick.c \
				$(DOOM_PATH)/p_saveg.c \
				$(DOOM_PATH)/p_user.c \
				$(DOOM_PATH)/r_bsp.c \
				$(DOOM_PATH)/r_data.c \
				$(DOOM_PATH)/r_draw.c \
				$(DOOM_PATH)/r_main.c \
				$(DOOM_PATH)/r_plane.c \
				$(DOOM_PATH)/r_segs.c \
				$(DOOM_PATH)/r_sky.c \
				$(DOOM_PATH)/r_things.c \
				$(DOOM_PATH)/w_wad.c \
				$(DOOM_PATH)/wi_stuff.c \
				$(DOOM_PATH)/v_video.c \
				$(DOOM_PATH)/st_lib.c \
				$(DOOM_PATH)/st_stuff.c \
				$(DOOM_PATH)/hu_stuff.c \
				$(DOOM_PATH)/hu_lib.c \
				$(DOOM_PATH)/s_sound.c \
				$(DOOM_PATH)/z_zone.c \
				$(DOOM_PATH)/info.c \
				$(DOOM_PATH)/sounds.c \
				$(DOOM_PATH)/i_main.c

PSOURCES	=	main.cpp \
				core/glcore.cpp \
				core/device.cpp \
				gl_fw/glfonts.cpp \
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
				img_io/img_utils.cpp \
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
	OPTLIBS = opengl32 glew32 openal
	CPMM	=	g++
	CCMM	=	gcc
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
	CPP_VER := -std=c++14
	CFLAGS =
	PFLAGS =
	LFLAGS = -isysroot /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk \
		-Wl,-search_paths_first -Wl,-headerpad_max_install_names \
		-framework AGL -framework Cocoa -framework OpenGL -framework IOKit \
		-framework CoreFoundation -framework CoreVideo -framework OpenAL
  endif
  LOCAL_INC_PATH := /usr/local/include
  LOCAL_LIB_PATH := /usr/local/lib
  OPTLIBS = GLEW
  CPMM	=	clang++
  CCMM	=	clang
endif

# build openjpeg any version path
OPENJPEG_PATH := $(shell ls -d $(LOCAL_INC_PATH)/openjpeg*)

STDLIBS		=

OPTLIBS += 	glfw3 pthread \
			png jpeg openjp2 \
			freetype \
			mad tag \
			z

INC_SYS		=	$(LOCAL_INC_PATH) \
				$(LOCAL_INC_PATH)/freetype2 \
				$(OPENJPEG_PATH) \
				$(LOCAL_INC_PATH)/taglib
INC_LIB		=
LIBDIR		=	$(LOCAL_LIB_PATH)

PINC_APP	=	. ../common
CINC_APP	=	. ../common $(DOOM_PATH)

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
CFLAGS +=	-DNORMALUNIX

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
CCWARN	=	-Wimplicit -Wreturn-type -Wswitch \
			-Wformat
CPWARN	=	-Wall -Werror -Wno-unused-private-field -Wno-unused-variable

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
