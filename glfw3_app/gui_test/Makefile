#-----------------------------------------------#
# Application Binary Build Makefile             #
#-----------------------------------------------#
TARGET		=	gui_test

#ICON_RC		=	icon.rc

# 'debug' or 'release'
BUILD		=	release

VPATH		=	../common

CSOURCES	=
#		minizip/ioapi.c \
#				minizip/unzip.c

PSOURCES	=	main.cpp \
				gui_test.cpp \
				core/glcore.cpp \
				core/device.cpp \
				core/ftimg.cpp \
				gl_fw/glfonts.cpp \
				gl_fw/glmobj.cpp \
				gl_fw/gltexfb.cpp \
				gl_fw/glterminal.cpp \
				snd_io/audio_io.cpp \
				snd_io/sound.cpp \
				snd_io/pcm.cpp \
				snd_io/wav_io.cpp \
				snd_io/mp3_io.cpp \
				snd_io/aac_io.cpp \
				snd_io/snd_files.cpp \
				utils/vtx.cpp \
				utils/vmath.cpp \
				utils/sjis_utf16.cpp \
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
				img_io/img_files.cpp \
				img_io/img_utils.cpp \
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
				widgets/widget_menu.cpp \
				widgets/widget_dialog.cpp \
				widgets/widget_tree.cpp \
				widgets/widget_filer.cpp \
				widgets/widget_terminal.cpp

STDLIBS		=
OPTLIBS		=	glfw3 GLEW \
				pthread \
				png turbojpeg openjp2 \
				mad faad mp4ff tag \
				freetype \
				z

INC_SYS		=	/opt/local/include/boost

INC_LIB		=	/opt/libjpeg-turbo/include \
				/usr/local/include \
				/opt/local/include \
				/opt/local/include/freetype2 \
				/opt/local/include/openjpeg-2.1 \
				/opt/local/include/taglib

PINC_APP	=	. ../common
CINC_APP	=	. ../common
LIBDIR		=	/opt/libjpeg-turbo/lib \
				/usr/local/lib \
				/opt/local/lib

INC_S	=	$(addprefix -isystem, $(INC_SYS))
INC_L	=	$(addprefix -isystem, $(INC_LIB))
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
RC	=

POPT	=	-O2 -std=gnu++11
COPT	=	-O2
LOPT	=

PFLAGS	=	-DHAVE_STDINT_H
CFLAGS	=

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
LFLAGS	=	-isysroot /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.9.sdk \
-Wl,-search_paths_first -Wl,-headerpad_max_install_names \
-framework AGL -framework Cocoa -framework OpenGL -framework IOKit -framework CoreFoundation -framework CoreVideo -framework OpenAL

CCWARN	=	-Wimplicit -Wreturn-type -Wswitch \
			-Wformat
CPWARN	=	-Wall -Werror

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
	$(CC) -MM -DDEPEND_ESCAPE $(COPT) $(CFLAGS) $(CINCS) $< \
	| sed 's/$(notdir $*)\.o:/$(subst /,\/,$(patsubst %.d,%.o,$@) $@):/' > $@ ; \
	[ -s $@ ] || rm -f $@

$(BUILD)/%.d : %.cpp
	mkdir -p $(dir $@); \
	$(CP) -MM -DDEPEND_ESCAPE $(POPT) $(PFLAGS) $(PINCS) $< \
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
