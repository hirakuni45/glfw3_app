# -*- tab-width : 4 -*-
#=======================================================================
#   @file
#   @brief  glfw_app
#   @author 平松邦仁 (hira@rvf-rc45.net)
#	@copyright	Copyright (C) 2019 Kunihito Hiramatsu @n
#				Released under the MIT license @n
#				https://github.com/hirakuni45/glfw_app/blob/master/LICENSE
#=======================================================================
TARGET		=	nesemu

# 'debug' or 'release'
BUILD		=	release

VPATH		=	../common emu

CSOURCES	=	minizip/ioapi.c \
				minizip/unzip.c \
				./emu/log.c \
				./emu/bitmap.c \
				./emu/cpu/nes6502.c \
				./emu/nes/mmclist.c \
				./emu/nes/nes.c \
				./emu/nes/nes_mmc.c \
				./emu/nes/nes_pal.c \
				./emu/nes/nes_ppu.c \
				./emu/nes/nes_rom.c \
				./emu/nes/nesinput.c \
				./emu/nes/nesstate.c \
				./emu/sndhrdw/fds_snd.c \
				./emu/sndhrdw/mmc5_snd.c \
				./emu/sndhrdw/nes_apu.c \
				./emu/sndhrdw/vrcvisnd.c \
				./emu/mappers/map000.c \
				./emu/mappers/map001.c \
				./emu/mappers/map002.c \
				./emu/mappers/map003.c \
				./emu/mappers/map004.c \
				./emu/mappers/map005.c \
				./emu/mappers/map007.c \
				./emu/mappers/map008.c \
				./emu/mappers/map009.c \
				./emu/mappers/map011.c \
				./emu/mappers/map015.c \
				./emu/mappers/map016.c \
				./emu/mappers/map018.c \
				./emu/mappers/map019.c \
				./emu/mappers/map024.c \
				./emu/mappers/map032.c \
				./emu/mappers/map033.c \
				./emu/mappers/map034.c \
				./emu/mappers/map040.c \
				./emu/mappers/map041.c \
				./emu/mappers/map042.c \
				./emu/mappers/map046.c \
				./emu/mappers/map050.c \
				./emu/mappers/map064.c \
				./emu/mappers/map065.c \
				./emu/mappers/map066.c \
				./emu/mappers/map070.c \
				./emu/mappers/map073.c \
				./emu/mappers/map075.c \
				./emu/mappers/map078.c \
				./emu/mappers/map079.c \
				./emu/mappers/map085.c \
				./emu/mappers/map087.c \
				./emu/mappers/map093.c \
				./emu/mappers/map094.c \
				./emu/mappers/map099.c \
				./emu/mappers/map160.c \
				./emu/mappers/map229.c \
				./emu/mappers/map231.c \
				./emu/mappers/mapvrc.c \
				./emu/libsnss/libsnss.c

PSOURCES	=	main.cpp \
				tools.cpp \
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
    ICON_RC = icon.rc
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

PINC_APP	=	. ../common ./emu ./emu/cpu ./emu/nes ./emu/mappers ./emu/sndhrdw ./emu/libsnss
CINC_APP	=	. ../common ./emu ./emu/cpu ./emu/nes ./emu/mappers ./emu/sndhrdw ./emu/libsnss

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
CFLAGS +=

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
CPWARN	=	-Wall -Werror -Wno-unused-private-field

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
