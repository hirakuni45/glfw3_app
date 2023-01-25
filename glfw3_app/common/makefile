# platform switcher (Windows, Linux, OS-X)
ifeq ($(OS),Windows_NT)
    FEXT	= .exe
#    ICON_RC = icon.rc
    SYSTEM := WIN
	CPP_VER := -std=c++17
    LOCAL_INC_PATH := /clang64/include
    LOCAL_LIB_PATH :=
	OPTLIBS = opengl32 glew32 openal glfw3 \
			  comdlg32 hid setupapi ksguid
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
	CFLAGS = -DWIN32
	PFLAGS = -DWIN32 -DBOOST_USE_WINDOWS_H
	LFLAGS =
  endif
  ifeq ($(UNAME),Darwin)
    SYSTEM := OSX
	OSX_VER := $(shell sw_vers -productVersion | sed 's/^\([0-9]*.[0-9]*\).[0-9]*/\1/')
	CPP_VER := -std=c++17
	CFLAGS =
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
  endif
  CPMM	=	clang++
  CCMM	=	clang
endif


# build openjpeg any version path
OPENJPEG_PATH := $(shell ls -r -d $(LOCAL_INC_PATH)/openjpeg*)

STDLIBS		=

OPTLIBS += 	pthread \
			png jpeg openjp2 \
			freetype \
			mad \
			z

INC_SYS		=	$(LOCAL_INC_PATH)/freetype2 \
				$(OPENJPEG_PATH)
INC_LIB		=
LIBDIR		=	$(LOCAL_LIB_PATH)

PINC_APP	=	. ../common
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

# -Wuninitialized -Wunused -Werror -Wshadow
CCWARN	=	-Wimplicit -Wreturn-type -Wswitch \
			-Wformat
CPWARN	=	-Wall -Werror -Wno-unused-private-field \
			-Wno-unused-variable \
			-Wno-unused-function \
			-Wno-unused-but-set-variable

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

