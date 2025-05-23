# -*- tab-width : 4 -*-
#=======================================================================
#	glfw3_app makefile (shear makefile) @n
#		Windows: MSYS2/CLANG64
#   @author 平松邦仁 (hira@rvf-rc45.net)
#	@copyright	Copyright (C) 2023, 2025 Kunihito Hiramatsu @n
#				Released under the MIT license @n
#				https://github.com/hirakuni45/RX/blob/master/LICENSE
#=======================================================================
# platform switcher (Windows, Linux, OS-X)
ifeq ($(OS),Windows_NT)
    FEXT	= .exe
    SYSTEM := WIN
    LOCAL_INC_PATH := /clang64/include
    LOCAL_LIB_PATH :=
	MP4FF_PATH := ../libraries/inc
	MP4FF_LIB_PATH := ../libraries/lib
	MP4FF_LIB := faad mp4ff
	OPTLIBS = opengl32 glu32 glew32 openal glfw3 \
			  comdlg32 hid setupapi ksguid
	CPMM	=	clang++
	CCMM	=	clang
	CFLAGS  += -DWIN32
	PFLAGS  += -DWIN32 -DBOOST_USE_WINDOWS_H
	LFLAGS	+=
else
  FEXT	=
  UNAME := $(shell uname -s)
  ifeq ($(UNAME),Linux)
    SYSTEM := LINUX
	CFLAGS +=
	PFLAGS +=
	LFLAGS +=
  endif
  ifeq ($(UNAME),Darwin)
    SYSTEM := OSX
	OSX_VER := $(shell sw_vers -productVersion | sed 's/^\([0-9]*.[0-9]*\).[0-9]*/\1/')
	CFLAGS +=
	PFLAGS +=
	LFLAGS += -isysroot /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk \
		-Wl,-search_paths_first -Wl,-headerpad_max_install_names \
		-framework AGL -framework Cocoa -framework OpenGL -framework IOKit \
		-framework CoreFoundation -framework CoreVideo -framework OpenAL
    LOCAL_INC_PATH := /opt/local/include
    LOCAL_LIB_PATH := /opt/local/lib
    OPTLIBS = glfw GLEW
  else
    LOCAL_INC_PATH := /usr/local/include
    LOCAL_LIB_PATH := /usr/local/lib
  endif
  CPMM	=	clang++
  CCMM	=	clang
endif

# build openjpeg any version path
OPENJPEG_PATH := $(shell ls -r -d $(LOCAL_INC_PATH)/openjpeg*)

OPTLIBS += 	pthread \
			png jpeg openjp2 \
			freetype \
			mad $(MP4FF_LIB)\
			z $(LIBS_USR) 

STDLIBS		=
INC_SYS		=	$(LOCAL_INC_PATH)/freetype2 $(OPENJPEG_PATH) $(MP4FF_PATH) $(INC_USR)
LIBDIR		=	$(LOCAL_LIB_PATH) $(MP4FF_LIB_PATH) $(LIB_DIR_USR)

INC_S	=	$(addprefix -isystem , $(INC_SYS))
INC_L	=	$(addprefix -isystem , $(INC_LIB))
INC_P	=	$(addprefix -I, $(PINC_APP))
INC_C	=	$(addprefix -I, $(CINC_APP))
CINCS	=	$(INC_S) $(INC_L) $(INC_C)
PINCS	=	$(INC_S) $(INC_L) $(INC_P)
LIBS	=	$(addprefix -L, $(LIBDIR))
LIBN	=	$(addprefix -l, $(STDLIBS))
LIBN	+=	$(addprefix -l, $(OPTLIBS))

# -Wuninitialized -Wunused -Werror -Wshadow
CCWARN	=	-Wimplicit -Wreturn-type -Wswitch \
			-Wformat
CPWARN	=	-Wall -Werror -Wno-unused-private-field \
			-Wno-unused-variable \
			-Wno-unused-function \
			-Wno-unused-but-set-variable \
			-Wno-vla-cxx-extension \
			-Wno-deprecated-declarations

CCWARN	+=	$(USR_CCWAN)
CPWARN	+=	$(USR_CPWAN)

#
# Compiler, Linker Options, Resource_compiler
#
CP	=	clang++
CC	=	clang
LK	=	clang++
RC	=	windres

ifeq ($(SYSTEM),WIN)
CPMM	=	clang++
CCMM	=	clang
else
CPMM	=	clang++
CCMM	=	clang
endif

POPT	=	-O2 -std=c++17
COPT	=	-O2
LOPT	=

PFLAGS	+=	-DHAVE_STDINT_H
CFLAGS	+=	

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

$(TARGET)$(FEXT): $(OBJECTS) $(ICON_OBJ) Makefile ../common/makefile
	$(LK) $(LFLAGS) $(LIBS) $(OBJECTS) $(ICON_OBJ) $(LIBN) -o $(TARGET)$(FEXT)

$(BUILD)/%.o : %.c
	mkdir -p $(dir $@); \
	$(CC) -c $(COPT) $(CFLAGS) $(CINCS) $(CCWARN) -o $@ $<

$(BUILD)/%.o : %.cpp
	mkdir -p $(dir $@); \
	$(CP) -c $(POPT) $(PFLAGS) $(PINCS) $(CPWARN) -o $@ $<

$(ICON_OBJ): $(ICON_RC)
	$(RC) -i $< -o $@

$(BUILD)/%.d : %.c Makefile ../common/makefile
	mkdir -p $(dir $@); \
	$(CCMM) -MM -DDEPEND_ESCAPE $(COPT) $(CFLAGS) $(CINCS) $< \
	| sed 's/$(notdir $*)\.o:/$(subst /,\/,$(patsubst %.d,%.o,$@) $@):/' > $@ ; \
	[ -s $@ ] || rm -f $@

$(BUILD)/%.d : %.cpp Makefile ../common/makefile
	mkdir -p $(dir $@); \
	$(CPMM) -MM -DDEPEND_ESCAPE $(POPT) $(PFLAGS) $(PINCS) $< \
	| sed 's/$(notdir $*)\.o:/$(subst /,\/,$(patsubst %.d,%.o,$@) $@):/' > $@ ; \
	[ -s $@ ] || rm -f $@

ifeq ($(SYSTEM),WIN)
strip:
	$(LK) $(LFLAGS) $(LIBS) $(OBJECTS) $(ICON_OBJ) $(LIBN) -mwindows -o $(TARGET)$(FEXT)
endif

run:
	./$(TARGET) $(RUN_PARA)

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

