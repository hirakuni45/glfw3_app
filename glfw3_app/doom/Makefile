# -*- tab-width : 4 -*-
#-----------------------------------------------#
# Application Binary Build Makefile             #
#-----------------------------------------------#
TARGET		=	doom

ifeq ($(OS),Windows_NT)
FEXT	=	.exe
# ICON_RC = icon.rc
SYSTEM := WIN
else
  UNAME := $(shell uname -s)
  ifeq ($(UNAME),Linux)
    SYSTEM := LINUX
  endif
  ifeq ($(UNAME),Darwin)
    SYSTEM := OSX
	OSX_VER := $(shell sw_vers -productVersion | sed 's/^\([0-9]*.[0-9]*\).[0-9]*/\1/')
  endif
FEXT	=
ICON_RC =
endif

# 'debug' or 'release'
BUILD		=	release

VPATH		=	../common

# 				./emu/nofrendo.c

CSOURCES	=	minizip/ioapi.c \
				minizip/unzip.c \
				prboom/am_map.c \
				prboom/d_client.c \
				prboom/d_deh.c \
				prboom/d_items.c \
				prboom/d_main.c \
				prboom/doomdef.c \
				prboom/doomstat.c \
				prboom/dstrings.c \
				prboom/f_finale.c \
				prboom/f_wipe.c \
				prboom/g_game.c \
				prboom/gl_main.c \
				prboom/gl_texture.c \
				prboom/hu_lib.c \
				prboom/hu_stuff.c \
				prboom/info.c \
				prboom/lprintf.c \
				prboom/m_argv.c \
				prboom/m_bbox.c \
				prboom/m_cheat.c \
				prboom/m_menu.c \
				prboom/m_misc.c \
				prboom/m_random.c \
				prboom/md5.c \
				prboom/mmus2mid.c \
				prboom/p_ceilng.c \
				prboom/p_checksum.c \
				prboom/p_doors.c \
				prboom/p_enemy.c \
				prboom/p_floor.c \
				prboom/p_genlin.c \
				prboom/p_inter.c \
				prboom/p_lights.c \
				prboom/p_map.c \
				prboom/p_maputl.c \
				prboom/p_mobj.c \
				prboom/p_plats.c \
				prboom/p_pspr.c \
				prboom/p_saveg.c \
				prboom/p_setup.c \
				prboom/p_sight.c \
				prboom/p_spec.c \
				prboom/p_switch.c \
				prboom/p_telept.c \
				prboom/p_tick.c \
				prboom/p_user.c \
				prboom/r_bsp.c \
				prboom/r_data.c \
				prboom/r_demo.c \
				prboom/r_draw.c \
				prboom/r_filter.c \
				prboom/r_fps.c \
				prboom/r_main.c \
				prboom/r_patch.c \
				prboom/r_plane.c \
				prboom/r_segs.c \
				prboom/r_sky.c \
				prboom/r_things.c \
				prboom/s_sound.c \
				prboom/sounds.c \
				prboom/st_lib.c \
				prboom/st_stuff.c \
				prboom/tables.c \
				prboom/v_video.c \
				prboom/version.c \
				prboom/w_mmap.c \
				prboom/w_wad.c \
				prboom/wi_stuff.c \
				prboom/z_bmalloc.c \
				prboom/z_zone.c \
				prboom-wad-tables/GAMMATBL.c \
				prboom-wad-tables/SINETABL.c \
				prboom-wad-tables/TANGTABL.c \
				prboom-wad-tables/TANTOANG.c \
				prboom-core/i_main.c \
				prboom-core/i_system.c \
				prboom-core/i_network.c \
				prboom-core/i_video.c \
				prboom-core/i_sound.c \
				prboom-core/gamepad.c

PSOURCES	=	main.cpp \
				doom.cpp \
				core/glcore.cpp \
				core/device.cpp \
				core/ftimg.cpp \
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

STDLIBS		=

ifeq ($(SYSTEM),WIN)
LOCAL_PATH	=	/mingw64
OPTLIBS		=	opengl32 glfw3 glew32 \
				pthread \
				png turbojpeg jpeg openjp2 \
				freetype \
				mad tag \
				z openal
else
LOCAL_PATH	=	/usr/local
OPTLIBS		=	glfw3 GLEW \
				pthread \
				png turbojpeg openjp2 \
				freetype \
				mad faad mp4ff tag \
				z
endif

INC_SYS		=	$(LOCAL_PATH)/include \
				$(LOCAL_PATH)/include/freetype2 \
				$(LOCAL_PATH)/include/openjpeg-2.3 \
				$(LOCAL_PATH)/include/taglib
INC_LIB		=
LIBDIR		=	$(LOCAL_PATH)/lib

ifeq ($(SYSTEM),OSX)
INC_SYS		+=	$(LOCAL_PATH)/opt/jpeg-turbo/include
LIBDIR		+=	$(LOCAL_PATH)/opt/jpeg-turbo/lib
endif

PINC_APP	=	. ../common prboom/include
CINC_APP	=	. ../common prboom/include prboom-wad-tables/include prboom-core/include

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

ifeq ($(SYSTEM),WIN)
CPMM	=	g++
CCMM	=	gcc
endif
ifeq ($(SYSTEM),OSX)
CPMM	=	clang++
CCMM	=	clang
endif

POPT	=	-O2 -std=c++14
COPT	=	-O2
LOPT	=

PFLAGS	=	-DHAVE_STDINT_H
CFLAGS	=

ifeq ($(SYSTEM),WIN)
	PFLAGS += -DWIN32 -DBOOST_USE_WINDOWS_H
	CFLAGS += -DWIN32
endif

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

# 	-static-libgcc -static-libstdc++
ifeq ($(SYSTEM),WIN)
LFLAGS	=
endif
ifeq ($(SYSTEM),OSX)
LFLAGS	=	-isysroot /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX$(OSX_VER).sdk \
			-Wl,-search_paths_first -Wl,-headerpad_max_install_names \
			-framework AGL -framework Cocoa -framework OpenGL -framework IOKit -framework CoreFoundation -framework CoreVideo -framework OpenAL
endif

# -Wuninitialized -Wunused -Werror -Wshadow
CCWARN	=	-Wimplicit -Wreturn-type -Wswitch \
			-Wformat
CPWARN	=	-Wall -Werror \
			-Wno-unused-private-field \
			-Wno-unused-variable

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
	$(LK) $(LFLAGS) $(LIBS) $(OBJECTS) $(ICON_OBJ) $(LIBN) -o $(TARGET)$(FEXT) 2>&1 | nkf -w

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
	objdump -p $(TARGET)$(FEXT) | grep "DLL Name"

tarball:
	tar cfvz $(TARGET)_$(shell date +%Y%m%d%H).tgz \
	*.[hc]pp Makefile ../common/*/*.[hc]pp ../common/*/*.[hc]

bin_zip:
	$(LK) $(LFLAGS) $(LIBS) $(OBJECTS) $(ICON_OBJ) $(LIBN) -mwindows -o $(TARGET)$(FEXT) 
	rm -f $(TARGET)_$(shell date +%Y%m%d%H)_bin.zip
	zip $(TARGET)_$(shell date +%Y%m%d%H)_bin.zip *.exe *.dll

-include $(DEPENDS)
