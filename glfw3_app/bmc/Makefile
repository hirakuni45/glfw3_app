#-----------------------------------------------#
# Application Binary Build Makefile             #
#-----------------------------------------------#
TARGET		=	bmc.exe

#ICON_RC		=	icon.rc

# 'debug' or 'release'
BUILD		=	release

VPATH		=	../common

CSOURCES	=	minizip/ioapi.c \
				minizip/unzip.c

PSOURCES	=	main.cpp \
				bmc_core.cpp \
				bmc_main.cpp \
				core/glcore.cpp \
				core/device.cpp \
				core/kfimg_ft2.cpp \
				core/jfep_io.cpp \
				gl_fw/glfonts.cpp \
				gl_fw/glmobj.cpp \
				gl_fw/gltexfb.cpp \
				gl_fw/glterminal.cpp \
				utils/arith.cpp \
				utils/vtx.cpp \
				utils/vmath.cpp \
				utils/string_utils.cpp \
				utils/file_io.cpp \
				utils/file_info.cpp \
				utils/files.cpp \
				utils/preference.cpp \
				utils/keyboard.cpp \
				img_io/paint.cpp \
				img_io/png_io.cpp \
				img_io/jpeg_io.cpp \
				img_io/openjpeg_io.cpp \
				img_io/bmp_io.cpp \
				img_io/tga_io.cpp \
				img_io/dds_io.cpp \
				img_io/bdf_io.cpp \
				img_io/img_files.cpp \
				img_io/img_utils.cpp \
				snd_io/audio_io.cpp \
				snd_io/pcm.cpp \
				snd_io/wav_io.cpp \
				snd_io/mp3_io.cpp \
				snd_io/mp3_tag.cpp \
				snd_io/aac_io.cpp \
				snd_io/snd_files.cpp \
				snd_io/sound.cpp \
				widgets/common_parts.cpp \
				widgets/widget.cpp \
				widgets/widget_director.cpp \
				widgets/widget_utils.cpp \
				widgets/widget_image.cpp \
				widgets/widget_text.cpp \
				widgets/widget_frame.cpp \
				widgets/widget_button.cpp \
				widgets/widget_label.cpp \
				widgets/widget_slider.cpp \
				widgets/widget_list.cpp \
				widgets/widget_check.cpp \
				widgets/widget_radio.cpp \
				widgets/widget_dialog.cpp \
				widgets/widget_tree.cpp \
				widgets/widget_filer.cpp \
				widgets/widget_terminal.cpp

STDLIBS		=
OPTLIBS		=	glfw3 glew32 opengl32 glu32 gdi32 imm32 \
				pthread \
				openal winmm dsound \
				mupdf mupdf-js-none \
				png jpeg_x86 openjp2 jbig2dec \
				freetype \
				id3tag \
				z \
				mad \
				faad mp4ff

INC_SYS		=	/usr/local/boost_1_54_0

INC_LIB		=	/usr/local/include \
				/usr/local/include/libpng16 \
				/usr/local/include/libjpeg_x86 \
				/usr/local/include/openjpeg-2.0 \
				/usr/local/include/freetype2

PINC_APP	=	. ../common
CINC_APP	=	. ../common
LIBDIR		=	/usr/local/lib

INC_S	=	$(addprefix -I, $(INC_SYS))
INC_L	=	$(addprefix -I, $(INC_LIB))
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
CP	=	g++
CC	=	gcc
LK	=	g++
RC	=	windres

POPT	=	-O2 -std=gnu++11
COPT	=	-O2
LOPT	=

PFLAGS	=	-DWIN32 -DHAVE_STDINT_H
CFLAGS	=	-DWIN32

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
LFLAGS	=

# -Wuninitialized -Wunused -Werror -Wshadow
CCWARN	=	-Wimplicit -Wreturn-type -Wswitch \
			-Wformat
CPPWARN	=	-Wall

OBJECTS	=	$(addprefix $(BUILD)/,$(patsubst %.cpp,%.o,$(PSOURCES))) \
			$(addprefix $(BUILD)/,$(patsubst %.c,%.o,$(CSOURCES)))
DEPENDS =   $(patsubst %.o,%.d, $(OBJECTS))

ifdef ICON_RC
	ICON_OBJ =	$(addprefix $(BUILD)/,$(patsubst %.rc,%.o,$(ICON_RC)))
endif

.PHONY: all clean
.SUFFIXES :
.SUFFIXES : .rc .hpp .h .c .cpp .o

all: $(BUILD) $(TARGET)

$(TARGET): $(OBJECTS) $(ICON_OBJ) Makefile
	$(LK) $(LFLAGS) $(LIBS) $(OBJECTS) $(ICON_OBJ) $(LIBN) -o $(TARGET)

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
	$(CC) -MM -DDEPEND_ESCAPE $(COPT) $(CFLAGS) $(INC_C) $< \
	| sed 's/$(notdir $*)\.o:/$(subst /,\/,$(patsubst %.d,%.o,$@) $@):/' > $@ ; \
	[ -s $@ ] || rm -f $@

$(BUILD)/%.d : %.cpp
	mkdir -p $(dir $@); \
	$(CP) -MM -DDEPEND_ESCAPE $(POPT) $(PFLAGS) $(INC_P) $< \
	| sed 's/$(notdir $*)\.o:/$(subst /,\/,$(patsubst %.d,%.o,$@) $@):/' > $@ ; \
	[ -s $@ ] || rm -f $@

run:
	./$(TARGET)

clean:
	rm -rf $(BUILD) $(TARGET)

clean_depend:
	rm -f $(DEPENDS)

dllname:
	objdump -p $(TARGET) | grep "DLL Name"

tarball:
	tar cfvz $(subst .exe,,$(TARGET))_$(shell date +%Y%m%d%H).tgz \
	*.[hc]pp Makefile ../common/*/*.[hc]pp ../common/*/*.[hc]

bin_zip:
	$(LK) $(LFLAGS) $(LIBS) $(OBJECTS) $(ICON_OBJ) $(LIBN) -mwindows -o $(TARGET) 
	rm -f $(subst .exe,,$(TARGET))_$(shell date +%Y%m%d%H)_bin.zip
	zip $(subst .exe,,$(TARGET))_$(shell date +%Y%m%d%H)_bin.zip *.exe *.dll

-include $(DEPENDS)
