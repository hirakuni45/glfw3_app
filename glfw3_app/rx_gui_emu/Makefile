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

ifeq ($(OS),Windows_NT)
FEXT	=	.exe
ICON_RC =
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

VPATH		=	../common ./RX

CSOURCES	=	minizip/ioapi.c \
				minizip/unzip.c

#				RX/CALC_app/calc_gui.cpp
#				RX/CALC_app/resource.cpp

PSOURCES	=	main.cpp \
				RX/DSOS_app/dso_gui.cpp \
				RX/DSOS_app/resource.cpp \
				RX/graphics/font8x16.cpp \
				RX/graphics/kfont16.cpp \
				RX/graphics/color.cpp \
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

STDLIBS		=

ifeq ($(SYSTEM),WIN)
LOCAL_PATH	=	/mingw64
OPTLIBS		=	opengl32 glfw3 glew32 \
				pthread \
				png turbojpeg jpeg openjp2 \
				freetype \
				mad \
				z openal mpfr gmp
else
LOCAL_PATH	=	/usr/local
OPTLIBS		=	glfw3 GLEW \
				pthread \
				png turbojpeg openjp2 \
				freetype \
				mad \
				z mpfr gmp
endif

INC_SYS		=	$(LOCAL_PATH)/include \
				$(LOCAL_PATH)/include/freetype2 \
				$(LOCAL_PATH)/include/openjpeg-2.3
INC_LIB		=
LIBDIR		=	$(LOCAL_PATH)/lib
ifeq ($(SYSTEM),OSX)
INC_SYS		+=	$(LOCAL_PATH)/opt/jpeg-turbo/include
LIBDIR		+=	$(LOCAL_PATH)/opt/jpeg-turbo/lib
endif

PINC_APP	=	. ../common ./RX
CINC_APP	=	. ../common

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
else
CPMM	=	clang++
CCMM	=	clang
endif

POPT	=	-O2 -std=c++17
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
			-Wno-unused-variable \
			-Wno-unused-function

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
	objdump -p $(TARGET)$(FEXT) | grep "DLL Name"

tarball:
	tar cfvz $(TARGET)_$(shell date +%Y%m%d%H).tgz \
	*.[hc]pp Makefile ../common/*/*.[hc]pp ../common/*/*.[hc]

bin_zip:
	$(LK) $(LFLAGS) $(LIBS) $(OBJECTS) $(ICON_OBJ) $(LIBN) -mwindows -o $(TARGET)$(FEXT) 
	rm -f $(TARGET)_$(shell date +%Y%m%d%H)_bin.zip
	zip $(TARGET)_$(shell date +%Y%m%d%H)_bin.zip *.exe *.dll

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
