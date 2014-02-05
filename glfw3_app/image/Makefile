#-----------------------------------------------#
# GNU-Makefile for "gcc"	 					#
#-----------------------------------------------#
TARGET		=	image
PSOURCES	=	main.cpp \
				img_main.cpp \
				../common/core/glcore.cpp \
				../common/core/device.cpp \
				../common/core/kfimg_ft2.cpp \
				../common/core/jfep_io.cpp \
				../common/gl_fw/glfonts.cpp \
				../common/gl_fw/glmobj.cpp \
				../common/gl_fw/gltexfb.cpp \
				../common/gl_fw/glterminal.cpp \
				../common/utils/vtx.cpp \
				../common/utils/vmath.cpp \
				../common/utils/string_utils.cpp \
				../common/utils/file_io.cpp \
				../common/utils/file_info.cpp \
				../common/utils/files.cpp \
				../common/utils/preference.cpp \
				../common/img_io/paint.cpp \
				../common/img_io/png_io.cpp \
				../common/img_io/jpeg_io.cpp \
				../common/img_io/openjpeg_io.cpp \
				../common/img_io/bmp_io.cpp \
				../common/img_io/tga_io.cpp \
				../common/img_io/dds_io.cpp \
				../common/img_io/bdf_io.cpp \
				../common/img_io/pdf_in.cpp \
				../common/img_io/img_files.cpp \
				../common/img_io/img_utils.cpp \
				../common/snd_io/audio_io.cpp \
				../common/snd_io/pcm.cpp \
				../common/snd_io/wav_io.cpp \
				../common/snd_io/mp3_io.cpp \
				../common/snd_io/mp3_tag.cpp \
				../common/snd_io/aac_io.cpp \
				../common/snd_io/snd_files.cpp \
				../common/snd_io/sound.cpp \
				../common/widgets/common_parts.cpp \
				../common/widgets/widget.cpp \
				../common/widgets/widget_director.cpp \
				../common/widgets/widget_utils.cpp \
				../common/widgets/widget_image.cpp \
				../common/widgets/widget_text.cpp \
				../common/widgets/widget_frame.cpp \
				../common/widgets/widget_button.cpp \
				../common/widgets/widget_label.cpp \
				../common/widgets/widget_slider.cpp \
				../common/widgets/widget_list.cpp \
				../common/widgets/widget_check.cpp \
				../common/widgets/widget_radio.cpp \
				../common/widgets/widget_dialog.cpp \
				../common/widgets/widget_tree.cpp \
				../common/widgets/widget_filer.cpp \
				../common/widgets/widget_terminal.cpp

CSOURCES	=	../common/minizip/ioapi.c \
				../common/minizip/unzip.c
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
				/usr/local/include/libjpeg_x86 \
				/usr/local/include/openjpeg-2.0 \
				/usr/local/include/freetype2 \
				/usr/local/include/mupdf

PINC_APP	=	./ ../common
CINC_APP	=	./ ../common
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
# POPT	=	-g
# COPT	=	-g
LOPT	=

COF	=	-o
LOF	=	-o

PFLAGS	=	-DWIN32 -DHAVE_STDINT_H
# PFLAGS	=	-DWIN32 -mno-cygwin -DIPAD_EMU -DOPTION_MEMORY_ALOCATE
# PFLAGS	=	-DWIN32 -mno-cygwin -DIPAD_EMU
CFLAGS	=	-DWIN32
LFLAGS	=	-static-libgcc -static-libstdc++

# -Wuninitialized -Wunused -Werror -Wshadow
CCWARN	=	-Wimplicit -Wreturn-type -Wswitch \
			-Wformat
CPPWARN	=	-Wall

OBJECTS	=	$(PSOURCES:.cpp=.o) $(CSOURCES:.c=.o)
DEPENDS =   $(PSOURCES:.cpp=.d) $(CSOURCES:.c=.d)

.SUFFIXES :
.SUFFIXES : .res .hpp .h .c .cpp .o .d

$(TARGET).exe: $(OBJECTS) Makefile
	$(LK) $(LOF)$(TARGET).exe $(LFLAGS) $(LIBS) $(OBJECTS) $(LIBN)

.c.o:
	$(CC) $(COPT) $(CFLAGS) $(CINCS) $(CCWARN) -c $< $(COF)$@

.cpp.o:
	$(CP) $(POPT) $(PFLAGS) $(PINCS) $(CPWARN) -c $< $(COF)$@

.rc.c:
	$(RC) $< $@

%.d: %.c
	$(CC) -MM $(COPT) $(CFLAGS) $(INC_L) $(INC_C) $(CCWARN) $< > $@

%.d: %.cpp
	$(CP) -MM $(POPT) $(PFLAGS) $(INC_L) $(INC_P) $(CPWARN) $< > $@

all:
	make clean
	make depend
	make $(TARGET)

run:
	./$(TARGET).exe

clean:
	rm -f $(DEPENDS)
	rm -f $(OBJECTS)
	rm -f $(TARGET)

dllname:
	objdump -p $(TARGET).exe | grep "DLL Name"

tarball:
	chmod 644 \
			../common/core/*.[hc]pp \
			../common/gl_fw/*.[hc]pp \
			../common/utils/*.[hc]pp \
			../common/img_io/*.[hc]pp \
			../common/snd_io/*.[hc]pp \
			../common/widgets/*.[hc]pp \
			../common/minizip/*.[hc] \
			../gui_test/*.[hc]pp Makefile
	rm -f $(TARGET)_$(shell date +%Y%m%d%H)_src.tgz
	tar cfvz $(TARGET)_$(shell date +%Y%m%d%H)_src.tgz \
			../common/core/*.[hc]pp \
			../common/gl_fw/*.[hc]pp \
			../common/utils/*.[hc]pp \
			../common/img_io/*.[hc]pp \
			../common/snd_io/*.[hc]pp \
			../common/widgets/*.[hc]pp \
			../common/minizip/*.[hc] \
			../gui_test/*.[hc]pp \
			../gui_test/Makefile
	chmod 444 $(TARGET)_$(shell date +%Y%m%d%H)_src.tgz

bin_zip:
	$(LK) $(LOF)$(TARGET).exe $(LFLAGS) $(LIBS) -mwindows $(OBJECTS) $(LIBN)
	rm -f $(TARGET)_$(shell date +%Y%m%d%H)_bin.zip
	zip $(TARGET)_$(shell date +%Y%m%d%H)_bin.zip *.exe *.dll res/*.*

mylib_arc:
	tar cfvz mingw_pack.tgz /usr/local/include /usr/local/lib /usr/local/bin

clean_depend:
	rm -f $(DEPENDS)

-include $(DEPENDS)
