#-----------------------------------------------#
# GNU-Makefile for "gcc"	 					#
#-----------------------------------------------#
TARGET		=	pmdv
PSOURCES	=	main.cpp \
				pmdv_main.cpp \
				../common/core/glcore.cpp \
				../common/core/device.cpp \
				../common/core/kfimg_ft2.cpp \
				../common/core/jfep_io.cpp \
				../common/gl_fw/glfonts.cpp \
				../common/gl_fw/glmobj.cpp \
				../common/gl_fw/gltexfb.cpp \
				../common/gl_fw/glterminal.cpp \
				../common/gl_fw/glcamera.cpp \
				../common/gl_fw/gllight.cpp \
				../common/gl_fw/glutils.cpp \
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
				../common/img_io/dds_io.cpp \
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
				../common/widgets/widget_terminal.cpp \
				../common/mdf/pmd_io.cpp \
				../common/mdf/pmx_io.cpp
#				../common/mdf/surface.cpp

CSOURCES	=	../common/minizip/ioapi.c \
				../common/minizip/unzip.c
STDLIBS		=
OPTLIBS		=	glfw3 glew32 opengl32 glu32 gdi32 imm32 \
				pthread \
				openal winmm dsound \
				fitz \
				png12 jpeg_x86 openjpeg jbig2dec \
				freetype \
				id3tag \
				z \
				mad \
				faad mp4ff

INC_SYS		=	/usr/boost_1_54_0 \
				/usr/local/include

INC_LIB		=	/usr/local/include/libpng12 \
				/usr/local/include/libjpeg_x86 \
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
CP	=	i686-w64-mingw32-g++
CC	=	i686-w64-mingw32-gcc
LK	=	i686-w64-mingw32-g++
RC	=

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

.SUFFIXES :
.SUFFIXES : .rc .hpp .h .c .cpp .o

$(TARGET).exe: $(OBJECTS) Makefile
	$(LK) $(LOF)$(TARGET).exe $(LFLAGS) $(LIBS) $(OBJECTS) $(LIBN)

.c.o:
	$(CC) $(COPT) $(CFLAGS) $(CINCS) $(CCWARN) -c $< $(COF)$@

.cpp.o:
	$(CP) $(POPT) $(PFLAGS) $(PINCS) $(CPWARN) -c $< $(COF)$@

.rc.c:
	$(RC) -i $< -o $@

all:
	make clean
	make depend
	make $(TARGET)

install:
	cp ./$(TARGET).exe /usr/local/bin/.

run:
	./$(TARGET).exe

clean:
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
			../common/mdf/*.[hc]pp \
			../common/minizip/*.[hc] \
			../pmdv/*.[hc]pp Makefile
	rm -f $(TARGET)_$(shell date +%Y%m%d%H)_src.tgz
	tar cfvz $(TARGET)_$(shell date +%Y%m%d%H)_src.tgz \
			../common/core/*.[hc]pp \
			../common/gl_fw/*.[hc]pp \
			../common/utils/*.[hc]pp \
			../common/img_io/*.[hc]pp \
			../common/snd_io/*.[hc]pp \
			../common/widgets/*.[hc]pp \
			../common/mdf/*.[hc]pp \
			../common/minizip/*.[hc] \
			../pmdv/*.[hc]pp \
			../pmdv/Makefile
	chmod 444 $(TARGET)_$(shell date +%Y%m%d%H)_src.tgz

bin_zip:
	$(LK) $(LOF)$(TARGET).exe $(LFLAGS) $(LIBS) -mwindows $(OBJECTS) $(LIBN)
	rm -f $(TARGET)_$(shell date +%Y%m%d%H)_bin.zip
	zip $(TARGET)_$(shell date +%Y%m%d%H)_bin.zip *.exe *.dll res/*.*

lib_arc:
	tar cfvz mingw_pack.tgz /usr/local/include /usr/local/lib /usr/local/bin

depend:
	makedepend -Y -- $(CFLAGS) $(INC_P) -- $(CSOURCES) $(PSOURCES)

# DO NOT DELETE

../common/minizip/ioapi.o: ../common/minizip/ioapi.h
../common/minizip/unzip.o: ../common/minizip/unzip.h
../common/minizip/unzip.o: ../common/minizip/ioapi.h
main.o: main.hpp ../common/snd_io/sound.hpp ../common/snd_io/audio_io.hpp
main.o: ../common/utils/file_io.hpp ../common/utils/string_utils.hpp
main.o: ../common/utils/mtx.hpp ../common/utils/vtx.hpp
main.o: ../common/snd_io/i_audio.hpp ../common/snd_io/audio.hpp
main.o: ../common/snd_io/snd_files.hpp ../common/snd_io/i_snd_io.hpp
main.o: ../common/snd_io/tag.hpp ../common/img_io/i_img.hpp
main.o: ../common/img_io/img.hpp ../common/widgets/widget_director.hpp
main.o: ../common/widgets/widget.hpp ../common/img_io/img.hpp
main.o: ../common/img_io/paint.hpp ../common/img_io/i_img.hpp
main.o: ../common/img_io/img_rgba8.hpp ../common/img_io/img_idx8.hpp
main.o: ../common/img_io/img_clut.hpp ../common/img_io/img_gray8.hpp
main.o: ../common/utils/vmath.hpp ../common/utils/preference.hpp
main.o: ../common/widgets/common_parts.hpp ../common/gl_fw/glmobj.hpp
main.o: ../common/gl_fw/gl_info.hpp ../common/img_io/img_rgba8.hpp
main.o: ../common/img_io/img_files.hpp ../common/img_io/i_img_io.hpp
main.o: ../common/core/glcore.hpp ../common/gl_fw/IGLcore.hpp
main.o: ../common/core/device.hpp ../common/gl_fw/glfonts.hpp
main.o: ../common/gl_fw/Ikfimg.hpp ../common/core/device.hpp
main.o: ../common/utils/director.hpp ../common/utils/i_scene.hpp
main.o: pmdv_main.hpp ../common/utils/i_scene.hpp
main.o: ../common/widgets/widget_filer.hpp ../common/utils/files.hpp
main.o: ../common/utils/drive_info.hpp ../common/utils/file_info.hpp
main.o: ../common/utils/handle_set.hpp ../common/widgets/widget_null.hpp
main.o: ../common/widgets/widget_director.hpp
main.o: ../common/widgets/widget_label.hpp
main.o: ../common/widgets/widget_button.hpp
main.o: ../common/widgets/widget_frame.hpp ../common/widgets/widget_check.hpp
main.o: ../common/mdf/pmd_io.hpp ../common/gl_fw/gllight.hpp
main.o: ../common/utils/quat.hpp ../common/mdf/pmx_io.hpp
main.o: ../common/gl_fw/glcamera.hpp ../common/gl_fw/glmatrix.hpp
pmdv_main.o: ../common/core/glcore.hpp ../common/gl_fw/IGLcore.hpp
pmdv_main.o: ../common/core/device.hpp ../common/utils/vtx.hpp
pmdv_main.o: ../common/gl_fw/gl_info.hpp ../common/gl_fw/glfonts.hpp
pmdv_main.o: ../common/gl_fw/Ikfimg.hpp ../common/img_io/i_img.hpp
pmdv_main.o: ../common/img_io/img.hpp ../common/utils/string_utils.hpp
pmdv_main.o: ../common/utils/mtx.hpp ../common/utils/file_io.hpp
pmdv_main.o: ../common/core/device.hpp ../common/gl_fw/glutils.hpp
pmdv_main.o: ../common/img_io/img_rgba8.hpp ../common/img_io/i_img.hpp
pmdv_main.o: ../common/img_io/img_idx8.hpp ../common/img_io/img_clut.hpp
pmdv_main.o: ../common/img_io/img_gray8.hpp pmdv_main.hpp main.hpp
pmdv_main.o: ../common/snd_io/sound.hpp ../common/snd_io/audio_io.hpp
pmdv_main.o: ../common/snd_io/i_audio.hpp ../common/snd_io/audio.hpp
pmdv_main.o: ../common/snd_io/snd_files.hpp ../common/snd_io/i_snd_io.hpp
pmdv_main.o: ../common/snd_io/tag.hpp ../common/widgets/widget_director.hpp
pmdv_main.o: ../common/widgets/widget.hpp ../common/img_io/img.hpp
pmdv_main.o: ../common/img_io/paint.hpp ../common/img_io/img_rgba8.hpp
pmdv_main.o: ../common/utils/vmath.hpp ../common/utils/preference.hpp
pmdv_main.o: ../common/widgets/common_parts.hpp ../common/gl_fw/glmobj.hpp
pmdv_main.o: ../common/img_io/img_files.hpp ../common/img_io/i_img_io.hpp
pmdv_main.o: ../common/utils/i_scene.hpp ../common/utils/director.hpp
pmdv_main.o: ../common/utils/i_scene.hpp ../common/widgets/widget_filer.hpp
pmdv_main.o: ../common/utils/files.hpp ../common/utils/drive_info.hpp
pmdv_main.o: ../common/utils/file_info.hpp ../common/utils/handle_set.hpp
pmdv_main.o: ../common/widgets/widget_null.hpp
pmdv_main.o: ../common/widgets/widget_director.hpp
pmdv_main.o: ../common/widgets/widget_label.hpp
pmdv_main.o: ../common/widgets/widget_button.hpp
pmdv_main.o: ../common/widgets/widget_frame.hpp
pmdv_main.o: ../common/widgets/widget_check.hpp ../common/mdf/pmd_io.hpp
pmdv_main.o: ../common/gl_fw/gllight.hpp ../common/utils/quat.hpp
pmdv_main.o: ../common/mdf/pmx_io.hpp ../common/gl_fw/glcamera.hpp
pmdv_main.o: ../common/gl_fw/glmatrix.hpp
../common/core/glcore.o: ../common/core/glcore.hpp
../common/core/glcore.o: ../common/gl_fw/IGLcore.hpp
../common/core/glcore.o: ../common/core/device.hpp ../common/utils/vtx.hpp
../common/core/glcore.o: ../common/gl_fw/gl_info.hpp
../common/core/glcore.o: ../common/gl_fw/glfonts.hpp
../common/core/glcore.o: ../common/gl_fw/Ikfimg.hpp
../common/core/glcore.o: ../common/img_io/i_img.hpp ../common/img_io/img.hpp
../common/core/glcore.o: ../common/utils/string_utils.hpp
../common/core/glcore.o: ../common/utils/mtx.hpp ../common/utils/file_io.hpp
../common/core/glcore.o: ../common/core/device.hpp
../common/core/device.o: ../common/core/device.hpp ../common/utils/vtx.hpp
../common/core/kfimg_ft2.o: ../common/core/kfimg_ft2.hpp
../common/core/kfimg_ft2.o: ../common/gl_fw/Ikfimg.hpp
../common/core/kfimg_ft2.o: ../common/img_io/i_img.hpp
../common/core/kfimg_ft2.o: ../common/img_io/img.hpp ../common/utils/vtx.hpp
../common/core/kfimg_ft2.o: ../common/img_io/img_gray8.hpp
../common/core/kfimg_ft2.o: ../common/utils/string_utils.hpp
../common/core/kfimg_ft2.o: ../common/utils/mtx.hpp
../common/core/jfep_io.o: ../common/core/jfep_io.hpp
../common/gl_fw/glfonts.o: ../common/gl_fw/glfonts.hpp
../common/gl_fw/glfonts.o: ../common/gl_fw/gl_info.hpp
../common/gl_fw/glfonts.o: ../common/gl_fw/Ikfimg.hpp
../common/gl_fw/glfonts.o: ../common/img_io/i_img.hpp
../common/gl_fw/glfonts.o: ../common/img_io/img.hpp ../common/utils/vtx.hpp
../common/gl_fw/glfonts.o: ../common/utils/string_utils.hpp
../common/gl_fw/glfonts.o: ../common/utils/mtx.hpp
../common/gl_fw/glfonts.o: ../common/img_io/img_utils.hpp
../common/gl_fw/glfonts.o: ../common/img_io/i_img.hpp
../common/gl_fw/glfonts.o: ../common/img_io/img_idx8.hpp
../common/gl_fw/glfonts.o: ../common/img_io/img_clut.hpp
../common/gl_fw/glfonts.o: ../common/img_io/img_gray8.hpp
../common/gl_fw/glfonts.o: ../common/img_io/img_rgba8.hpp
../common/gl_fw/glmobj.o: ../common/gl_fw/glmobj.hpp
../common/gl_fw/glmobj.o: ../common/gl_fw/gl_info.hpp
../common/gl_fw/glmobj.o: ../common/img_io/i_img.hpp ../common/img_io/img.hpp
../common/gl_fw/glmobj.o: ../common/utils/vtx.hpp
../common/gl_fw/glmobj.o: ../common/img_io/img_rgba8.hpp
../common/gl_fw/glmobj.o: ../common/img_io/i_img.hpp
../common/gl_fw/glmobj.o: ../common/img_io/img_idx8.hpp
../common/gl_fw/glmobj.o: ../common/img_io/img_clut.hpp
../common/gl_fw/glmobj.o: ../common/img_io/img_gray8.hpp
../common/gl_fw/glmobj.o: ../common/gl_fw/glutils.hpp
../common/gl_fw/glmobj.o: ../common/img_io/img_utils.hpp
../common/gl_fw/glmobj.o: ../common/img_io/img_rgba8.hpp
../common/gl_fw/gltexfb.o: ../common/gl_fw/gltexfb.hpp
../common/gl_fw/gltexfb.o: ../common/gl_fw/gl_info.hpp
../common/gl_fw/gltexfb.o: ../common/utils/vtx.hpp
../common/gl_fw/glterminal.o: ../common/gl_fw/glterminal.hpp
../common/gl_fw/glterminal.o: ../common/gl_fw/IGLcore.hpp
../common/gl_fw/glterminal.o: ../common/core/device.hpp
../common/gl_fw/glterminal.o: ../common/utils/vtx.hpp
../common/gl_fw/glterminal.o: ../common/gl_fw/gl_info.hpp
../common/gl_fw/glterminal.o: ../common/gl_fw/glfonts.hpp
../common/gl_fw/glterminal.o: ../common/gl_fw/Ikfimg.hpp
../common/gl_fw/glterminal.o: ../common/img_io/i_img.hpp
../common/gl_fw/glterminal.o: ../common/img_io/img.hpp
../common/gl_fw/glterminal.o: ../common/utils/string_utils.hpp
../common/gl_fw/glterminal.o: ../common/utils/mtx.hpp
../common/gl_fw/glterminal.o: ../common/utils/file_io.hpp
../common/gl_fw/glterminal.o: ../common/img_io/img.hpp
../common/gl_fw/glcamera.o: ../common/gl_fw/IGLcore.hpp
../common/gl_fw/glcamera.o: ../common/core/device.hpp ../common/utils/vtx.hpp
../common/gl_fw/glcamera.o: ../common/gl_fw/gl_info.hpp
../common/gl_fw/glcamera.o: ../common/gl_fw/glfonts.hpp
../common/gl_fw/glcamera.o: ../common/gl_fw/Ikfimg.hpp
../common/gl_fw/glcamera.o: ../common/img_io/i_img.hpp
../common/gl_fw/glcamera.o: ../common/img_io/img.hpp
../common/gl_fw/glcamera.o: ../common/utils/string_utils.hpp
../common/gl_fw/glcamera.o: ../common/utils/mtx.hpp
../common/gl_fw/glcamera.o: ../common/utils/file_io.hpp
../common/gl_fw/glcamera.o: ../common/gl_fw/glcamera.hpp
../common/gl_fw/glcamera.o: ../common/utils/quat.hpp
../common/gl_fw/glcamera.o: ../common/gl_fw/glmatrix.hpp
../common/gl_fw/gllight.o: ../common/gl_fw/gllight.hpp
../common/gl_fw/gllight.o: ../common/gl_fw/gl_info.hpp
../common/gl_fw/gllight.o: ../common/utils/vtx.hpp ../common/img_io/img.hpp
../common/gl_fw/glutils.o: ../common/gl_fw/glutils.hpp
../common/gl_fw/glutils.o: ../common/gl_fw/gl_info.hpp
../common/gl_fw/glutils.o: ../common/utils/vtx.hpp ../common/img_io/i_img.hpp
../common/gl_fw/glutils.o: ../common/img_io/img.hpp
../common/gl_fw/glutils.o: ../common/img_io/img_rgba8.hpp
../common/gl_fw/glutils.o: ../common/img_io/i_img.hpp
../common/gl_fw/glutils.o: ../common/img_io/img_idx8.hpp
../common/gl_fw/glutils.o: ../common/img_io/img_clut.hpp
../common/gl_fw/glutils.o: ../common/img_io/img_gray8.hpp
../common/gl_fw/glutils.o: ../common/gl_fw/IGLcore.hpp
../common/gl_fw/glutils.o: ../common/core/device.hpp
../common/gl_fw/glutils.o: ../common/gl_fw/glfonts.hpp
../common/gl_fw/glutils.o: ../common/gl_fw/Ikfimg.hpp
../common/gl_fw/glutils.o: ../common/utils/string_utils.hpp
../common/gl_fw/glutils.o: ../common/utils/mtx.hpp
../common/gl_fw/glutils.o: ../common/utils/file_io.hpp
../common/utils/vtx.o: ../common/utils/vtx.hpp
../common/utils/vmath.o: ../common/utils/vmath.hpp ../common/utils/vtx.hpp
../common/utils/string_utils.o: ../common/utils/string_utils.hpp
../common/utils/string_utils.o: ../common/utils/mtx.hpp
../common/utils/string_utils.o: ../common/utils/vtx.hpp
../common/utils/file_io.o: ../common/utils/file_io.hpp
../common/utils/file_io.o: ../common/utils/string_utils.hpp
../common/utils/file_io.o: ../common/utils/mtx.hpp ../common/utils/vtx.hpp
../common/utils/file_info.o: ../common/utils/file_info.hpp
../common/utils/file_info.o: ../common/utils/file_io.hpp
../common/utils/file_info.o: ../common/utils/string_utils.hpp
../common/utils/file_info.o: ../common/utils/mtx.hpp ../common/utils/vtx.hpp
../common/utils/files.o: ../common/utils/files.hpp
../common/utils/files.o: ../common/utils/drive_info.hpp
../common/utils/files.o: ../common/utils/file_info.hpp
../common/utils/files.o: ../common/utils/file_io.hpp
../common/utils/files.o: ../common/utils/string_utils.hpp
../common/utils/files.o: ../common/utils/mtx.hpp ../common/utils/vtx.hpp
../common/utils/files.o: ../common/utils/handle_set.hpp
../common/utils/preference.o: ../common/Utils/preference.hpp
../common/utils/preference.o: ../common/utils/vtx.hpp
../common/utils/preference.o: ../common/utils/file_io.hpp
../common/utils/preference.o: ../common/utils/string_utils.hpp
../common/utils/preference.o: ../common/utils/mtx.hpp
../common/img_io/paint.o: ../common/img_io/paint.hpp ../common/img_io/img.hpp
../common/img_io/paint.o: ../common/img_io/i_img.hpp ../common/utils/vtx.hpp
../common/img_io/paint.o: ../common/img_io/img_rgba8.hpp
../common/img_io/paint.o: ../common/img_io/img_idx8.hpp
../common/img_io/paint.o: ../common/img_io/i_img.hpp
../common/img_io/paint.o: ../common/img_io/img_clut.hpp
../common/img_io/paint.o: ../common/img_io/img_gray8.hpp
../common/img_io/paint.o: ../common/utils/string_utils.hpp
../common/img_io/paint.o: ../common/utils/mtx.hpp ../common/gl_fw/Ikfimg.hpp
../common/img_io/png_io.o: ../common/img_io/png_io.hpp
../common/img_io/png_io.o: ../common/img_io/i_img_io.hpp
../common/img_io/png_io.o: ../common/utils/file_io.hpp
../common/img_io/png_io.o: ../common/utils/string_utils.hpp
../common/img_io/png_io.o: ../common/utils/mtx.hpp ../common/utils/vtx.hpp
../common/img_io/png_io.o: ../common/img_io/i_img.hpp
../common/img_io/png_io.o: ../common/img_io/img.hpp
../common/img_io/png_io.o: ../common/img_io/img_idx8.hpp
../common/img_io/png_io.o: ../common/img_io/i_img.hpp
../common/img_io/png_io.o: ../common/img_io/img_clut.hpp
../common/img_io/png_io.o: ../common/img_io/img_rgba8.hpp
../common/img_io/png_io.o: ../common/img_io/img_gray8.hpp
../common/img_io/jpeg_io.o: ../common/utils/file_io.hpp
../common/img_io/jpeg_io.o: ../common/utils/string_utils.hpp
../common/img_io/jpeg_io.o: ../common/utils/mtx.hpp ../common/utils/vtx.hpp
../common/img_io/jpeg_io.o: ../common/img_io/jpeg_io.hpp
../common/img_io/jpeg_io.o: ../common/img_io/i_img_io.hpp
../common/img_io/jpeg_io.o: ../common/img_io/i_img.hpp
../common/img_io/jpeg_io.o: ../common/img_io/img.hpp
../common/img_io/jpeg_io.o: ../common/img_io/img_rgba8.hpp
../common/img_io/jpeg_io.o: ../common/img_io/img_idx8.hpp
../common/img_io/jpeg_io.o: ../common/img_io/i_img.hpp
../common/img_io/jpeg_io.o: ../common/img_io/img_clut.hpp
../common/img_io/jpeg_io.o: ../common/img_io/img_gray8.hpp
../common/img_io/openjpeg_io.o: ../common/utils/file_io.hpp
../common/img_io/openjpeg_io.o: ../common/utils/string_utils.hpp
../common/img_io/openjpeg_io.o: ../common/utils/mtx.hpp
../common/img_io/openjpeg_io.o: ../common/utils/vtx.hpp
../common/img_io/openjpeg_io.o: ../common/img_io/openjpeg_io.hpp
../common/img_io/openjpeg_io.o: ../common/img_io/i_img_io.hpp
../common/img_io/openjpeg_io.o: ../common/img_io/i_img.hpp
../common/img_io/openjpeg_io.o: ../common/img_io/img.hpp
../common/img_io/openjpeg_io.o: ../common/img_io/img_rgba8.hpp
../common/img_io/openjpeg_io.o: ../common/img_io/img_idx8.hpp
../common/img_io/openjpeg_io.o: ../common/img_io/i_img.hpp
../common/img_io/openjpeg_io.o: ../common/img_io/img_clut.hpp
../common/img_io/openjpeg_io.o: ../common/img_io/img_gray8.hpp
../common/img_io/bmp_io.o: ../common/img_io/bmp_io.hpp
../common/img_io/bmp_io.o: ../common/img_io/i_img_io.hpp
../common/img_io/bmp_io.o: ../common/utils/file_io.hpp
../common/img_io/bmp_io.o: ../common/utils/string_utils.hpp
../common/img_io/bmp_io.o: ../common/utils/mtx.hpp ../common/utils/vtx.hpp
../common/img_io/bmp_io.o: ../common/img_io/i_img.hpp
../common/img_io/bmp_io.o: ../common/img_io/img.hpp
../common/img_io/bmp_io.o: ../common/img_io/img_idx8.hpp
../common/img_io/bmp_io.o: ../common/img_io/i_img.hpp
../common/img_io/bmp_io.o: ../common/img_io/img_clut.hpp
../common/img_io/bmp_io.o: ../common/img_io/img_rgba8.hpp
../common/img_io/bmp_io.o: ../common/img_io/img_gray8.hpp
../common/img_io/dds_io.o: ../common/img_io/dds_io.hpp
../common/img_io/dds_io.o: ../common/img_io/i_img_io.hpp
../common/img_io/dds_io.o: ../common/utils/file_io.hpp
../common/img_io/dds_io.o: ../common/utils/string_utils.hpp
../common/img_io/dds_io.o: ../common/utils/mtx.hpp ../common/utils/vtx.hpp
../common/img_io/dds_io.o: ../common/img_io/i_img.hpp
../common/img_io/dds_io.o: ../common/img_io/img.hpp
../common/img_io/dds_io.o: ../common/img_io/img_idx8.hpp
../common/img_io/dds_io.o: ../common/img_io/i_img.hpp
../common/img_io/dds_io.o: ../common/img_io/img_clut.hpp
../common/img_io/dds_io.o: ../common/img_io/img_rgba8.hpp
../common/img_io/dds_io.o: ../common/img_io/img_gray8.hpp
../common/img_io/pdf_in.o: ../common/img_io/pdf_in.hpp
../common/img_io/pdf_in.o: ../common/img_io/i_img_io.hpp
../common/img_io/pdf_in.o: ../common/utils/file_io.hpp
../common/img_io/pdf_in.o: ../common/utils/string_utils.hpp
../common/img_io/pdf_in.o: ../common/utils/mtx.hpp ../common/utils/vtx.hpp
../common/img_io/pdf_in.o: ../common/img_io/i_img.hpp
../common/img_io/pdf_in.o: ../common/img_io/img.hpp
../common/img_io/pdf_in.o: ../common/img_io/img_rgba8.hpp
../common/img_io/pdf_in.o: ../common/img_io/img_idx8.hpp
../common/img_io/pdf_in.o: ../common/img_io/i_img.hpp
../common/img_io/pdf_in.o: ../common/img_io/img_clut.hpp
../common/img_io/pdf_in.o: ../common/img_io/img_gray8.hpp
../common/img_io/img_files.o: ../common/img_io/img_files.hpp
../common/img_io/img_files.o: ../common/img_io/i_img_io.hpp
../common/img_io/img_files.o: ../common/utils/file_io.hpp
../common/img_io/img_files.o: ../common/utils/string_utils.hpp
../common/img_io/img_files.o: ../common/utils/mtx.hpp ../common/utils/vtx.hpp
../common/img_io/img_files.o: ../common/img_io/i_img.hpp
../common/img_io/img_files.o: ../common/img_io/img.hpp
../common/img_io/img_files.o: ../common/img_io/jpeg_io.hpp
../common/img_io/img_files.o: ../common/img_io/i_img_io.hpp
../common/img_io/img_files.o: ../common/img_io/img_rgba8.hpp
../common/img_io/img_files.o: ../common/img_io/img_idx8.hpp
../common/img_io/img_files.o: ../common/img_io/i_img.hpp
../common/img_io/img_files.o: ../common/img_io/img_clut.hpp
../common/img_io/img_files.o: ../common/img_io/img_gray8.hpp
../common/img_io/img_files.o: ../common/img_io/png_io.hpp
../common/img_io/img_files.o: ../common/img_io/bmp_io.hpp
../common/img_io/img_files.o: ../common/img_io/dds_io.hpp
../common/img_io/img_files.o: ../common/img_io/openjpeg_io.hpp
../common/img_io/img_utils.o: ../common/img_io/img_utils.hpp
../common/img_io/img_utils.o: ../common/img_io/img.hpp
../common/img_io/img_utils.o: ../common/img_io/i_img.hpp
../common/img_io/img_utils.o: ../common/utils/vtx.hpp
../common/img_io/img_utils.o: ../common/img_io/img_idx8.hpp
../common/img_io/img_utils.o: ../common/img_io/i_img.hpp
../common/img_io/img_utils.o: ../common/img_io/img_clut.hpp
../common/img_io/img_utils.o: ../common/img_io/img_gray8.hpp
../common/img_io/img_utils.o: ../common/img_io/img_rgba8.hpp
../common/snd_io/audio_io.o: ../common/snd_io/audio_io.hpp
../common/snd_io/audio_io.o: ../common/utils/file_io.hpp
../common/snd_io/audio_io.o: ../common/utils/string_utils.hpp
../common/snd_io/audio_io.o: ../common/utils/mtx.hpp ../common/utils/vtx.hpp
../common/snd_io/audio_io.o: ../common/snd_io/i_audio.hpp
../common/snd_io/audio_io.o: ../common/snd_io/audio.hpp
../common/snd_io/audio_io.o: ../common/snd_io/pcm.hpp
../common/snd_io/pcm.o: ../common/snd_io/pcm.hpp ../common/snd_io/i_audio.hpp
../common/snd_io/pcm.o: ../common/utils/file_io.hpp
../common/snd_io/pcm.o: ../common/utils/string_utils.hpp
../common/snd_io/pcm.o: ../common/utils/mtx.hpp ../common/utils/vtx.hpp
../common/snd_io/pcm.o: ../common/snd_io/audio.hpp
../common/snd_io/wav_io.o: ../common/snd_io/wav_io.hpp
../common/snd_io/wav_io.o: ../common/snd_io/i_snd_io.hpp
../common/snd_io/wav_io.o: ../common/utils/file_io.hpp
../common/snd_io/wav_io.o: ../common/utils/string_utils.hpp
../common/snd_io/wav_io.o: ../common/utils/mtx.hpp ../common/utils/vtx.hpp
../common/snd_io/wav_io.o: ../common/snd_io/i_audio.hpp
../common/snd_io/wav_io.o: ../common/snd_io/audio.hpp
../common/snd_io/wav_io.o: ../common/snd_io/tag.hpp
../common/snd_io/wav_io.o: ../common/img_io/i_img.hpp
../common/snd_io/wav_io.o: ../common/img_io/img.hpp ../common/snd_io/pcm.hpp
../common/snd_io/mp3_io.o: ../common/snd_io/mp3_io.hpp
../common/snd_io/mp3_io.o: ../common/snd_io/i_snd_io.hpp
../common/snd_io/mp3_io.o: ../common/utils/file_io.hpp
../common/snd_io/mp3_io.o: ../common/utils/string_utils.hpp
../common/snd_io/mp3_io.o: ../common/utils/mtx.hpp ../common/utils/vtx.hpp
../common/snd_io/mp3_io.o: ../common/snd_io/i_audio.hpp
../common/snd_io/mp3_io.o: ../common/snd_io/audio.hpp
../common/snd_io/mp3_io.o: ../common/snd_io/tag.hpp
../common/snd_io/mp3_io.o: ../common/img_io/i_img.hpp
../common/snd_io/mp3_io.o: ../common/img_io/img.hpp
../common/snd_io/mp3_io.o: ../common/snd_io/mp3_tag.hpp
../common/snd_io/mp3_io.o: ../common/img_io/jpeg_io.hpp
../common/snd_io/mp3_io.o: ../common/img_io/i_img_io.hpp
../common/snd_io/mp3_io.o: ../common/img_io/i_img.hpp
../common/snd_io/mp3_io.o: ../common/img_io/img_rgba8.hpp
../common/snd_io/mp3_io.o: ../common/img_io/img_idx8.hpp
../common/snd_io/mp3_io.o: ../common/img_io/img_clut.hpp
../common/snd_io/mp3_io.o: ../common/img_io/img_gray8.hpp
../common/snd_io/mp3_io.o: ../common/img_io/png_io.hpp
../common/snd_io/mp3_io.o: ../common/snd_io/pcm.hpp
../common/snd_io/mp3_io.o: ../common/img_io/img_utils.hpp
../common/snd_io/mp3_tag.o: ../common/snd_io/mp3_tag.hpp
../common/snd_io/mp3_tag.o: ../common/utils/file_io.hpp
../common/snd_io/mp3_tag.o: ../common/utils/string_utils.hpp
../common/snd_io/mp3_tag.o: ../common/utils/mtx.hpp ../common/utils/vtx.hpp
../common/snd_io/mp3_tag.o: ../common/img_io/i_img.hpp
../common/snd_io/mp3_tag.o: ../common/img_io/img.hpp
../common/snd_io/mp3_tag.o: ../common/img_io/jpeg_io.hpp
../common/snd_io/mp3_tag.o: ../common/img_io/i_img_io.hpp
../common/snd_io/mp3_tag.o: ../common/img_io/i_img.hpp
../common/snd_io/mp3_tag.o: ../common/img_io/img_rgba8.hpp
../common/snd_io/mp3_tag.o: ../common/img_io/img_idx8.hpp
../common/snd_io/mp3_tag.o: ../common/img_io/img_clut.hpp
../common/snd_io/mp3_tag.o: ../common/img_io/img_gray8.hpp
../common/snd_io/mp3_tag.o: ../common/img_io/png_io.hpp
../common/snd_io/aac_io.o: ../common/snd_io/aac_io.hpp
../common/snd_io/aac_io.o: ../common/snd_io/i_snd_io.hpp
../common/snd_io/aac_io.o: ../common/utils/file_io.hpp
../common/snd_io/aac_io.o: ../common/utils/string_utils.hpp
../common/snd_io/aac_io.o: ../common/utils/mtx.hpp ../common/utils/vtx.hpp
../common/snd_io/aac_io.o: ../common/snd_io/i_audio.hpp
../common/snd_io/aac_io.o: ../common/snd_io/audio.hpp
../common/snd_io/aac_io.o: ../common/snd_io/tag.hpp
../common/snd_io/aac_io.o: ../common/img_io/i_img.hpp
../common/snd_io/aac_io.o: ../common/img_io/img.hpp ../common/snd_io/pcm.hpp
../common/snd_io/aac_io.o: ../common/img_io/img_files.hpp
../common/snd_io/aac_io.o: ../common/img_io/i_img_io.hpp
../common/snd_io/aac_io.o: ../common/img_io/i_img.hpp
../common/snd_io/aac_io.o: ../common/img_io/img_utils.hpp
../common/snd_io/aac_io.o: ../common/img_io/img_idx8.hpp
../common/snd_io/aac_io.o: ../common/img_io/img_clut.hpp
../common/snd_io/aac_io.o: ../common/img_io/img_gray8.hpp
../common/snd_io/aac_io.o: ../common/img_io/img_rgba8.hpp
../common/snd_io/snd_files.o: ../common/snd_io/snd_files.hpp
../common/snd_io/snd_files.o: ../common/snd_io/i_snd_io.hpp
../common/snd_io/snd_files.o: ../common/utils/file_io.hpp
../common/snd_io/snd_files.o: ../common/utils/string_utils.hpp
../common/snd_io/snd_files.o: ../common/utils/mtx.hpp ../common/utils/vtx.hpp
../common/snd_io/snd_files.o: ../common/snd_io/i_audio.hpp
../common/snd_io/snd_files.o: ../common/snd_io/audio.hpp
../common/snd_io/snd_files.o: ../common/snd_io/tag.hpp
../common/snd_io/snd_files.o: ../common/img_io/i_img.hpp
../common/snd_io/snd_files.o: ../common/img_io/img.hpp
../common/snd_io/snd_files.o: ../common/snd_io/wav_io.hpp
../common/snd_io/snd_files.o: ../common/snd_io/mp3_io.hpp
../common/snd_io/snd_files.o: ../common/snd_io/mp3_tag.hpp
../common/snd_io/snd_files.o: ../common/img_io/jpeg_io.hpp
../common/snd_io/snd_files.o: ../common/img_io/i_img_io.hpp
../common/snd_io/snd_files.o: ../common/img_io/i_img.hpp
../common/snd_io/snd_files.o: ../common/img_io/img_rgba8.hpp
../common/snd_io/snd_files.o: ../common/img_io/img_idx8.hpp
../common/snd_io/snd_files.o: ../common/img_io/img_clut.hpp
../common/snd_io/snd_files.o: ../common/img_io/img_gray8.hpp
../common/snd_io/snd_files.o: ../common/img_io/png_io.hpp
../common/snd_io/snd_files.o: ../common/snd_io/aac_io.hpp
../common/snd_io/sound.o: ../common/snd_io/sound.hpp
../common/snd_io/sound.o: ../common/snd_io/audio_io.hpp
../common/snd_io/sound.o: ../common/utils/file_io.hpp
../common/snd_io/sound.o: ../common/utils/string_utils.hpp
../common/snd_io/sound.o: ../common/utils/mtx.hpp ../common/utils/vtx.hpp
../common/snd_io/sound.o: ../common/snd_io/i_audio.hpp
../common/snd_io/sound.o: ../common/snd_io/audio.hpp
../common/snd_io/sound.o: ../common/snd_io/snd_files.hpp
../common/snd_io/sound.o: ../common/snd_io/i_snd_io.hpp
../common/snd_io/sound.o: ../common/snd_io/tag.hpp ../common/img_io/i_img.hpp
../common/snd_io/sound.o: ../common/img_io/img.hpp ../common/snd_io/pcm.hpp
../common/snd_io/sound.o: ../common/utils/file_info.hpp
../common/widgets/common_parts.o: ../common/widgets/common_parts.hpp
../common/widgets/common_parts.o: ../common/gl_fw/glmobj.hpp
../common/widgets/common_parts.o: ../common/gl_fw/gl_info.hpp
../common/widgets/common_parts.o: ../common/img_io/i_img.hpp
../common/widgets/common_parts.o: ../common/img_io/img.hpp
../common/widgets/common_parts.o: ../common/utils/vtx.hpp
../common/widgets/common_parts.o: ../common/img_io/img_rgba8.hpp
../common/widgets/common_parts.o: ../common/img_io/i_img.hpp
../common/widgets/common_parts.o: ../common/img_io/img_idx8.hpp
../common/widgets/common_parts.o: ../common/img_io/img_clut.hpp
../common/widgets/common_parts.o: ../common/img_io/img_gray8.hpp
../common/widgets/common_parts.o: ../common/widgets/widget.hpp
../common/widgets/common_parts.o: ../common/img_io/img.hpp
../common/widgets/common_parts.o: ../common/img_io/paint.hpp
../common/widgets/common_parts.o: ../common/img_io/img_rgba8.hpp
../common/widgets/common_parts.o: ../common/utils/string_utils.hpp
../common/widgets/common_parts.o: ../common/utils/mtx.hpp
../common/widgets/common_parts.o: ../common/utils/vmath.hpp
../common/widgets/common_parts.o: ../common/utils/preference.hpp
../common/widgets/common_parts.o: ../common/utils/file_io.hpp
../common/widgets/common_parts.o: ../common/widgets/widget_utils.hpp
../common/widgets/common_parts.o: ../common/widgets/widget_director.hpp
../common/widgets/common_parts.o: ../common/img_io/img_files.hpp
../common/widgets/common_parts.o: ../common/img_io/i_img_io.hpp
../common/widgets/widget.o: ../common/widgets/widget.hpp
../common/widgets/widget.o: ../common/img_io/img.hpp
../common/widgets/widget.o: ../common/img_io/paint.hpp
../common/widgets/widget.o: ../common/img_io/img.hpp
../common/widgets/widget.o: ../common/img_io/i_img.hpp
../common/widgets/widget.o: ../common/utils/vtx.hpp
../common/widgets/widget.o: ../common/img_io/img_rgba8.hpp
../common/widgets/widget.o: ../common/img_io/img_idx8.hpp
../common/widgets/widget.o: ../common/img_io/i_img.hpp
../common/widgets/widget.o: ../common/img_io/img_clut.hpp
../common/widgets/widget.o: ../common/img_io/img_gray8.hpp
../common/widgets/widget.o: ../common/utils/string_utils.hpp
../common/widgets/widget.o: ../common/utils/mtx.hpp ../common/utils/vmath.hpp
../common/widgets/widget.o: ../common/utils/preference.hpp
../common/widgets/widget.o: ../common/utils/file_io.hpp
../common/widgets/widget_director.o: ../common/gl_fw/gl_info.hpp
../common/widgets/widget_director.o: ../common/gl_fw/IGLcore.hpp
../common/widgets/widget_director.o: ../common/core/device.hpp
../common/widgets/widget_director.o: ../common/utils/vtx.hpp
../common/widgets/widget_director.o: ../common/gl_fw/glfonts.hpp
../common/widgets/widget_director.o: ../common/gl_fw/Ikfimg.hpp
../common/widgets/widget_director.o: ../common/img_io/i_img.hpp
../common/widgets/widget_director.o: ../common/img_io/img.hpp
../common/widgets/widget_director.o: ../common/utils/string_utils.hpp
../common/widgets/widget_director.o: ../common/utils/mtx.hpp
../common/widgets/widget_director.o: ../common/utils/file_io.hpp
../common/widgets/widget_director.o: ../common/widgets/widget_director.hpp
../common/widgets/widget_director.o: ../common/widgets/widget.hpp
../common/widgets/widget_director.o: ../common/img_io/img.hpp
../common/widgets/widget_director.o: ../common/img_io/paint.hpp
../common/widgets/widget_director.o: ../common/img_io/i_img.hpp
../common/widgets/widget_director.o: ../common/img_io/img_rgba8.hpp
../common/widgets/widget_director.o: ../common/img_io/img_idx8.hpp
../common/widgets/widget_director.o: ../common/img_io/img_clut.hpp
../common/widgets/widget_director.o: ../common/img_io/img_gray8.hpp
../common/widgets/widget_director.o: ../common/utils/vmath.hpp
../common/widgets/widget_director.o: ../common/utils/preference.hpp
../common/widgets/widget_director.o: ../common/widgets/common_parts.hpp
../common/widgets/widget_director.o: ../common/gl_fw/glmobj.hpp
../common/widgets/widget_director.o: ../common/img_io/img_rgba8.hpp
../common/widgets/widget_director.o: ../common/img_io/img_files.hpp
../common/widgets/widget_director.o: ../common/img_io/i_img_io.hpp
../common/widgets/widget_director.o: ../common/widgets/widget_utils.hpp
../common/widgets/widget_director.o: ../common/widgets/widget_null.hpp
../common/widgets/widget_director.o: ../common/widgets/widget_director.hpp
../common/widgets/widget_director.o: ../common/widgets/widget_image.hpp
../common/widgets/widget_director.o: ../common/widgets/widget_text.hpp
../common/widgets/widget_director.o: ../common/widgets/widget_frame.hpp
../common/widgets/widget_director.o: ../common/widgets/widget_button.hpp
../common/widgets/widget_director.o: ../common/widgets/widget_label.hpp
../common/widgets/widget_director.o: ../common/widgets/widget_slider.hpp
../common/widgets/widget_director.o: ../common/widgets/widget_check.hpp
../common/widgets/widget_director.o: ../common/widgets/widget_radio.hpp
../common/widgets/widget_director.o: ../common/widgets/widget_list.hpp
../common/widgets/widget_director.o: ../common/widgets/widget_dialog.hpp
../common/widgets/widget_director.o: ../common/widgets/widget_tree.hpp
../common/widgets/widget_director.o: ../common/utils/tree_unit.hpp
../common/widgets/widget_director.o: ../common/widgets/widget_filer.hpp
../common/widgets/widget_director.o: ../common/utils/files.hpp
../common/widgets/widget_director.o: ../common/utils/drive_info.hpp
../common/widgets/widget_director.o: ../common/utils/file_info.hpp
../common/widgets/widget_director.o: ../common/utils/handle_set.hpp
../common/widgets/widget_director.o: ../common/widgets/widget_terminal.hpp
../common/widgets/widget_director.o: ../common/gl_fw/glterminal.hpp
../common/widgets/widget_utils.o: ../common/gl_fw/IGLcore.hpp
../common/widgets/widget_utils.o: ../common/core/device.hpp
../common/widgets/widget_utils.o: ../common/utils/vtx.hpp
../common/widgets/widget_utils.o: ../common/gl_fw/gl_info.hpp
../common/widgets/widget_utils.o: ../common/gl_fw/glfonts.hpp
../common/widgets/widget_utils.o: ../common/gl_fw/Ikfimg.hpp
../common/widgets/widget_utils.o: ../common/img_io/i_img.hpp
../common/widgets/widget_utils.o: ../common/img_io/img.hpp
../common/widgets/widget_utils.o: ../common/utils/string_utils.hpp
../common/widgets/widget_utils.o: ../common/utils/mtx.hpp
../common/widgets/widget_utils.o: ../common/utils/file_io.hpp
../common/widgets/widget_utils.o: ../common/img_io/paint.hpp
../common/widgets/widget_utils.o: ../common/img_io/i_img.hpp
../common/widgets/widget_utils.o: ../common/img_io/img_rgba8.hpp
../common/widgets/widget_utils.o: ../common/img_io/img_idx8.hpp
../common/widgets/widget_utils.o: ../common/img_io/img_clut.hpp
../common/widgets/widget_utils.o: ../common/img_io/img_gray8.hpp
../common/widgets/widget_utils.o: ../common/img_io/img_utils.hpp
../common/widgets/widget_utils.o: ../common/widgets/widget_utils.hpp
../common/widgets/widget_utils.o: ../common/widgets/widget.hpp
../common/widgets/widget_utils.o: ../common/img_io/img.hpp
../common/widgets/widget_utils.o: ../common/utils/vmath.hpp
../common/widgets/widget_utils.o: ../common/utils/preference.hpp
../common/widgets/widget_utils.o: ../common/widgets/widget_director.hpp
../common/widgets/widget_utils.o: ../common/widgets/common_parts.hpp
../common/widgets/widget_utils.o: ../common/gl_fw/glmobj.hpp
../common/widgets/widget_utils.o: ../common/img_io/img_rgba8.hpp
../common/widgets/widget_utils.o: ../common/img_io/img_files.hpp
../common/widgets/widget_utils.o: ../common/img_io/i_img_io.hpp
../common/widgets/widget_utils.o: ../common/widgets/widget_button.hpp
../common/widgets/widget_utils.o: ../common/widgets/widget_label.hpp
../common/widgets/widget_image.o: ../common/gl_fw/IGLcore.hpp
../common/widgets/widget_image.o: ../common/core/device.hpp
../common/widgets/widget_image.o: ../common/utils/vtx.hpp
../common/widgets/widget_image.o: ../common/gl_fw/gl_info.hpp
../common/widgets/widget_image.o: ../common/gl_fw/glfonts.hpp
../common/widgets/widget_image.o: ../common/gl_fw/Ikfimg.hpp
../common/widgets/widget_image.o: ../common/img_io/i_img.hpp
../common/widgets/widget_image.o: ../common/img_io/img.hpp
../common/widgets/widget_image.o: ../common/utils/string_utils.hpp
../common/widgets/widget_image.o: ../common/utils/mtx.hpp
../common/widgets/widget_image.o: ../common/utils/file_io.hpp
../common/widgets/widget_image.o: ../common/widgets/widget_image.hpp
../common/widgets/widget_image.o: ../common/widgets/widget_director.hpp
../common/widgets/widget_image.o: ../common/widgets/widget.hpp
../common/widgets/widget_image.o: ../common/img_io/img.hpp
../common/widgets/widget_image.o: ../common/img_io/paint.hpp
../common/widgets/widget_image.o: ../common/img_io/i_img.hpp
../common/widgets/widget_image.o: ../common/img_io/img_rgba8.hpp
../common/widgets/widget_image.o: ../common/img_io/img_idx8.hpp
../common/widgets/widget_image.o: ../common/img_io/img_clut.hpp
../common/widgets/widget_image.o: ../common/img_io/img_gray8.hpp
../common/widgets/widget_image.o: ../common/utils/vmath.hpp
../common/widgets/widget_image.o: ../common/utils/preference.hpp
../common/widgets/widget_image.o: ../common/widgets/common_parts.hpp
../common/widgets/widget_image.o: ../common/gl_fw/glmobj.hpp
../common/widgets/widget_image.o: ../common/img_io/img_rgba8.hpp
../common/widgets/widget_image.o: ../common/img_io/img_files.hpp
../common/widgets/widget_image.o: ../common/img_io/i_img_io.hpp
../common/widgets/widget_image.o: ../common/widgets/widget_utils.hpp
../common/widgets/widget_image.o: ../common/img_io/img_utils.hpp
../common/widgets/widget_text.o: ../common/gl_fw/IGLcore.hpp
../common/widgets/widget_text.o: ../common/core/device.hpp
../common/widgets/widget_text.o: ../common/utils/vtx.hpp
../common/widgets/widget_text.o: ../common/gl_fw/gl_info.hpp
../common/widgets/widget_text.o: ../common/gl_fw/glfonts.hpp
../common/widgets/widget_text.o: ../common/gl_fw/Ikfimg.hpp
../common/widgets/widget_text.o: ../common/img_io/i_img.hpp
../common/widgets/widget_text.o: ../common/img_io/img.hpp
../common/widgets/widget_text.o: ../common/utils/string_utils.hpp
../common/widgets/widget_text.o: ../common/utils/mtx.hpp
../common/widgets/widget_text.o: ../common/utils/file_io.hpp
../common/widgets/widget_text.o: ../common/widgets/widget_text.hpp
../common/widgets/widget_text.o: ../common/widgets/widget_director.hpp
../common/widgets/widget_text.o: ../common/widgets/widget.hpp
../common/widgets/widget_text.o: ../common/img_io/img.hpp
../common/widgets/widget_text.o: ../common/img_io/paint.hpp
../common/widgets/widget_text.o: ../common/img_io/i_img.hpp
../common/widgets/widget_text.o: ../common/img_io/img_rgba8.hpp
../common/widgets/widget_text.o: ../common/img_io/img_idx8.hpp
../common/widgets/widget_text.o: ../common/img_io/img_clut.hpp
../common/widgets/widget_text.o: ../common/img_io/img_gray8.hpp
../common/widgets/widget_text.o: ../common/utils/vmath.hpp
../common/widgets/widget_text.o: ../common/utils/preference.hpp
../common/widgets/widget_text.o: ../common/widgets/common_parts.hpp
../common/widgets/widget_text.o: ../common/gl_fw/glmobj.hpp
../common/widgets/widget_text.o: ../common/img_io/img_rgba8.hpp
../common/widgets/widget_text.o: ../common/img_io/img_files.hpp
../common/widgets/widget_text.o: ../common/img_io/i_img_io.hpp
../common/widgets/widget_text.o: ../common/widgets/widget_utils.hpp
../common/widgets/widget_frame.o: ../common/widgets/widget_frame.hpp
../common/widgets/widget_frame.o: ../common/widgets/widget_director.hpp
../common/widgets/widget_frame.o: ../common/widgets/widget.hpp
../common/widgets/widget_frame.o: ../common/img_io/img.hpp
../common/widgets/widget_frame.o: ../common/img_io/paint.hpp
../common/widgets/widget_frame.o: ../common/img_io/img.hpp
../common/widgets/widget_frame.o: ../common/img_io/i_img.hpp
../common/widgets/widget_frame.o: ../common/utils/vtx.hpp
../common/widgets/widget_frame.o: ../common/img_io/img_rgba8.hpp
../common/widgets/widget_frame.o: ../common/img_io/img_idx8.hpp
../common/widgets/widget_frame.o: ../common/img_io/i_img.hpp
../common/widgets/widget_frame.o: ../common/img_io/img_clut.hpp
../common/widgets/widget_frame.o: ../common/img_io/img_gray8.hpp
../common/widgets/widget_frame.o: ../common/utils/string_utils.hpp
../common/widgets/widget_frame.o: ../common/utils/mtx.hpp
../common/widgets/widget_frame.o: ../common/utils/vmath.hpp
../common/widgets/widget_frame.o: ../common/utils/preference.hpp
../common/widgets/widget_frame.o: ../common/utils/file_io.hpp
../common/widgets/widget_frame.o: ../common/widgets/common_parts.hpp
../common/widgets/widget_frame.o: ../common/gl_fw/glmobj.hpp
../common/widgets/widget_frame.o: ../common/gl_fw/gl_info.hpp
../common/widgets/widget_frame.o: ../common/img_io/img_rgba8.hpp
../common/widgets/widget_frame.o: ../common/img_io/img_files.hpp
../common/widgets/widget_frame.o: ../common/img_io/i_img_io.hpp
../common/widgets/widget_frame.o: ../common/widgets/widget_utils.hpp
../common/widgets/widget_button.o: ../common/gl_fw/IGLcore.hpp
../common/widgets/widget_button.o: ../common/core/device.hpp
../common/widgets/widget_button.o: ../common/utils/vtx.hpp
../common/widgets/widget_button.o: ../common/gl_fw/gl_info.hpp
../common/widgets/widget_button.o: ../common/gl_fw/glfonts.hpp
../common/widgets/widget_button.o: ../common/gl_fw/Ikfimg.hpp
../common/widgets/widget_button.o: ../common/img_io/i_img.hpp
../common/widgets/widget_button.o: ../common/img_io/img.hpp
../common/widgets/widget_button.o: ../common/utils/string_utils.hpp
../common/widgets/widget_button.o: ../common/utils/mtx.hpp
../common/widgets/widget_button.o: ../common/utils/file_io.hpp
../common/widgets/widget_button.o: ../common/img_io/paint.hpp
../common/widgets/widget_button.o: ../common/img_io/i_img.hpp
../common/widgets/widget_button.o: ../common/img_io/img_rgba8.hpp
../common/widgets/widget_button.o: ../common/img_io/img_idx8.hpp
../common/widgets/widget_button.o: ../common/img_io/img_clut.hpp
../common/widgets/widget_button.o: ../common/img_io/img_gray8.hpp
../common/widgets/widget_button.o: ../common/widgets/widget_button.hpp
../common/widgets/widget_button.o: ../common/widgets/widget_director.hpp
../common/widgets/widget_button.o: ../common/widgets/widget.hpp
../common/widgets/widget_button.o: ../common/img_io/img.hpp
../common/widgets/widget_button.o: ../common/utils/vmath.hpp
../common/widgets/widget_button.o: ../common/utils/preference.hpp
../common/widgets/widget_button.o: ../common/widgets/common_parts.hpp
../common/widgets/widget_button.o: ../common/gl_fw/glmobj.hpp
../common/widgets/widget_button.o: ../common/img_io/img_rgba8.hpp
../common/widgets/widget_button.o: ../common/img_io/img_files.hpp
../common/widgets/widget_button.o: ../common/img_io/i_img_io.hpp
../common/widgets/widget_button.o: ../common/widgets/widget_utils.hpp
../common/widgets/widget_label.o: ../common/widgets/widget_label.hpp
../common/widgets/widget_label.o: ../common/widgets/widget_director.hpp
../common/widgets/widget_label.o: ../common/widgets/widget.hpp
../common/widgets/widget_label.o: ../common/img_io/img.hpp
../common/widgets/widget_label.o: ../common/img_io/paint.hpp
../common/widgets/widget_label.o: ../common/img_io/img.hpp
../common/widgets/widget_label.o: ../common/img_io/i_img.hpp
../common/widgets/widget_label.o: ../common/utils/vtx.hpp
../common/widgets/widget_label.o: ../common/img_io/img_rgba8.hpp
../common/widgets/widget_label.o: ../common/img_io/img_idx8.hpp
../common/widgets/widget_label.o: ../common/img_io/i_img.hpp
../common/widgets/widget_label.o: ../common/img_io/img_clut.hpp
../common/widgets/widget_label.o: ../common/img_io/img_gray8.hpp
../common/widgets/widget_label.o: ../common/utils/string_utils.hpp
../common/widgets/widget_label.o: ../common/utils/mtx.hpp
../common/widgets/widget_label.o: ../common/utils/vmath.hpp
../common/widgets/widget_label.o: ../common/utils/preference.hpp
../common/widgets/widget_label.o: ../common/utils/file_io.hpp
../common/widgets/widget_label.o: ../common/widgets/common_parts.hpp
../common/widgets/widget_label.o: ../common/gl_fw/glmobj.hpp
../common/widgets/widget_label.o: ../common/gl_fw/gl_info.hpp
../common/widgets/widget_label.o: ../common/img_io/img_rgba8.hpp
../common/widgets/widget_label.o: ../common/img_io/img_files.hpp
../common/widgets/widget_label.o: ../common/img_io/i_img_io.hpp
../common/widgets/widget_label.o: ../common/widgets/widget_utils.hpp
../common/widgets/widget_label.o: ../common/gl_fw/IGLcore.hpp
../common/widgets/widget_label.o: ../common/core/device.hpp
../common/widgets/widget_label.o: ../common/gl_fw/glfonts.hpp
../common/widgets/widget_label.o: ../common/gl_fw/Ikfimg.hpp
../common/widgets/widget_slider.o: ../common/gl_fw/IGLcore.hpp
../common/widgets/widget_slider.o: ../common/core/device.hpp
../common/widgets/widget_slider.o: ../common/utils/vtx.hpp
../common/widgets/widget_slider.o: ../common/gl_fw/gl_info.hpp
../common/widgets/widget_slider.o: ../common/gl_fw/glfonts.hpp
../common/widgets/widget_slider.o: ../common/gl_fw/Ikfimg.hpp
../common/widgets/widget_slider.o: ../common/img_io/i_img.hpp
../common/widgets/widget_slider.o: ../common/img_io/img.hpp
../common/widgets/widget_slider.o: ../common/utils/string_utils.hpp
../common/widgets/widget_slider.o: ../common/utils/mtx.hpp
../common/widgets/widget_slider.o: ../common/utils/file_io.hpp
../common/widgets/widget_slider.o: ../common/widgets/widget_slider.hpp
../common/widgets/widget_slider.o: ../common/widgets/widget_director.hpp
../common/widgets/widget_slider.o: ../common/widgets/widget.hpp
../common/widgets/widget_slider.o: ../common/img_io/img.hpp
../common/widgets/widget_slider.o: ../common/img_io/paint.hpp
../common/widgets/widget_slider.o: ../common/img_io/i_img.hpp
../common/widgets/widget_slider.o: ../common/img_io/img_rgba8.hpp
../common/widgets/widget_slider.o: ../common/img_io/img_idx8.hpp
../common/widgets/widget_slider.o: ../common/img_io/img_clut.hpp
../common/widgets/widget_slider.o: ../common/img_io/img_gray8.hpp
../common/widgets/widget_slider.o: ../common/utils/vmath.hpp
../common/widgets/widget_slider.o: ../common/utils/preference.hpp
../common/widgets/widget_slider.o: ../common/widgets/common_parts.hpp
../common/widgets/widget_slider.o: ../common/gl_fw/glmobj.hpp
../common/widgets/widget_slider.o: ../common/img_io/img_rgba8.hpp
../common/widgets/widget_slider.o: ../common/img_io/img_files.hpp
../common/widgets/widget_slider.o: ../common/img_io/i_img_io.hpp
../common/widgets/widget_slider.o: ../common/widgets/widget_utils.hpp
../common/widgets/widget_slider.o: ../common/widgets/widget_director.hpp
../common/widgets/widget_list.o: ../common/gl_fw/IGLcore.hpp
../common/widgets/widget_list.o: ../common/core/device.hpp
../common/widgets/widget_list.o: ../common/utils/vtx.hpp
../common/widgets/widget_list.o: ../common/gl_fw/gl_info.hpp
../common/widgets/widget_list.o: ../common/gl_fw/glfonts.hpp
../common/widgets/widget_list.o: ../common/gl_fw/Ikfimg.hpp
../common/widgets/widget_list.o: ../common/img_io/i_img.hpp
../common/widgets/widget_list.o: ../common/img_io/img.hpp
../common/widgets/widget_list.o: ../common/utils/string_utils.hpp
../common/widgets/widget_list.o: ../common/utils/mtx.hpp
../common/widgets/widget_list.o: ../common/utils/file_io.hpp
../common/widgets/widget_list.o: ../common/widgets/widget_list.hpp
../common/widgets/widget_list.o: ../common/widgets/widget_director.hpp
../common/widgets/widget_list.o: ../common/widgets/widget.hpp
../common/widgets/widget_list.o: ../common/img_io/img.hpp
../common/widgets/widget_list.o: ../common/img_io/paint.hpp
../common/widgets/widget_list.o: ../common/img_io/i_img.hpp
../common/widgets/widget_list.o: ../common/img_io/img_rgba8.hpp
../common/widgets/widget_list.o: ../common/img_io/img_idx8.hpp
../common/widgets/widget_list.o: ../common/img_io/img_clut.hpp
../common/widgets/widget_list.o: ../common/img_io/img_gray8.hpp
../common/widgets/widget_list.o: ../common/utils/vmath.hpp
../common/widgets/widget_list.o: ../common/utils/preference.hpp
../common/widgets/widget_list.o: ../common/widgets/common_parts.hpp
../common/widgets/widget_list.o: ../common/gl_fw/glmobj.hpp
../common/widgets/widget_list.o: ../common/img_io/img_rgba8.hpp
../common/widgets/widget_list.o: ../common/img_io/img_files.hpp
../common/widgets/widget_list.o: ../common/img_io/i_img_io.hpp
../common/widgets/widget_list.o: ../common/widgets/widget_label.hpp
../common/widgets/widget_list.o: ../common/widgets/widget_null.hpp
../common/widgets/widget_list.o: ../common/widgets/widget_director.hpp
../common/widgets/widget_list.o: ../common/widgets/widget_utils.hpp
../common/widgets/widget_check.o: ../common/gl_fw/IGLcore.hpp
../common/widgets/widget_check.o: ../common/core/device.hpp
../common/widgets/widget_check.o: ../common/utils/vtx.hpp
../common/widgets/widget_check.o: ../common/gl_fw/gl_info.hpp
../common/widgets/widget_check.o: ../common/gl_fw/glfonts.hpp
../common/widgets/widget_check.o: ../common/gl_fw/Ikfimg.hpp
../common/widgets/widget_check.o: ../common/img_io/i_img.hpp
../common/widgets/widget_check.o: ../common/img_io/img.hpp
../common/widgets/widget_check.o: ../common/utils/string_utils.hpp
../common/widgets/widget_check.o: ../common/utils/mtx.hpp
../common/widgets/widget_check.o: ../common/utils/file_io.hpp
../common/widgets/widget_check.o: ../common/widgets/widget_check.hpp
../common/widgets/widget_check.o: ../common/widgets/widget_director.hpp
../common/widgets/widget_check.o: ../common/widgets/widget.hpp
../common/widgets/widget_check.o: ../common/img_io/img.hpp
../common/widgets/widget_check.o: ../common/img_io/paint.hpp
../common/widgets/widget_check.o: ../common/img_io/i_img.hpp
../common/widgets/widget_check.o: ../common/img_io/img_rgba8.hpp
../common/widgets/widget_check.o: ../common/img_io/img_idx8.hpp
../common/widgets/widget_check.o: ../common/img_io/img_clut.hpp
../common/widgets/widget_check.o: ../common/img_io/img_gray8.hpp
../common/widgets/widget_check.o: ../common/utils/vmath.hpp
../common/widgets/widget_check.o: ../common/utils/preference.hpp
../common/widgets/widget_check.o: ../common/widgets/common_parts.hpp
../common/widgets/widget_check.o: ../common/gl_fw/glmobj.hpp
../common/widgets/widget_check.o: ../common/img_io/img_rgba8.hpp
../common/widgets/widget_check.o: ../common/img_io/img_files.hpp
../common/widgets/widget_check.o: ../common/img_io/i_img_io.hpp
../common/widgets/widget_check.o: ../common/widgets/widget_utils.hpp
../common/widgets/widget_radio.o: ../common/gl_fw/IGLcore.hpp
../common/widgets/widget_radio.o: ../common/core/device.hpp
../common/widgets/widget_radio.o: ../common/utils/vtx.hpp
../common/widgets/widget_radio.o: ../common/gl_fw/gl_info.hpp
../common/widgets/widget_radio.o: ../common/gl_fw/glfonts.hpp
../common/widgets/widget_radio.o: ../common/gl_fw/Ikfimg.hpp
../common/widgets/widget_radio.o: ../common/img_io/i_img.hpp
../common/widgets/widget_radio.o: ../common/img_io/img.hpp
../common/widgets/widget_radio.o: ../common/utils/string_utils.hpp
../common/widgets/widget_radio.o: ../common/utils/mtx.hpp
../common/widgets/widget_radio.o: ../common/utils/file_io.hpp
../common/widgets/widget_radio.o: ../common/widgets/widget_radio.hpp
../common/widgets/widget_radio.o: ../common/widgets/widget_director.hpp
../common/widgets/widget_radio.o: ../common/widgets/widget.hpp
../common/widgets/widget_radio.o: ../common/img_io/img.hpp
../common/widgets/widget_radio.o: ../common/img_io/paint.hpp
../common/widgets/widget_radio.o: ../common/img_io/i_img.hpp
../common/widgets/widget_radio.o: ../common/img_io/img_rgba8.hpp
../common/widgets/widget_radio.o: ../common/img_io/img_idx8.hpp
../common/widgets/widget_radio.o: ../common/img_io/img_clut.hpp
../common/widgets/widget_radio.o: ../common/img_io/img_gray8.hpp
../common/widgets/widget_radio.o: ../common/utils/vmath.hpp
../common/widgets/widget_radio.o: ../common/utils/preference.hpp
../common/widgets/widget_radio.o: ../common/widgets/common_parts.hpp
../common/widgets/widget_radio.o: ../common/gl_fw/glmobj.hpp
../common/widgets/widget_radio.o: ../common/img_io/img_rgba8.hpp
../common/widgets/widget_radio.o: ../common/img_io/img_files.hpp
../common/widgets/widget_radio.o: ../common/img_io/i_img_io.hpp
../common/widgets/widget_radio.o: ../common/widgets/widget_utils.hpp
../common/widgets/widget_dialog.o: ../common/widgets/widget_dialog.hpp
../common/widgets/widget_dialog.o: ../common/widgets/widget_director.hpp
../common/widgets/widget_dialog.o: ../common/widgets/widget.hpp
../common/widgets/widget_dialog.o: ../common/img_io/img.hpp
../common/widgets/widget_dialog.o: ../common/img_io/paint.hpp
../common/widgets/widget_dialog.o: ../common/img_io/img.hpp
../common/widgets/widget_dialog.o: ../common/img_io/i_img.hpp
../common/widgets/widget_dialog.o: ../common/utils/vtx.hpp
../common/widgets/widget_dialog.o: ../common/img_io/img_rgba8.hpp
../common/widgets/widget_dialog.o: ../common/img_io/img_idx8.hpp
../common/widgets/widget_dialog.o: ../common/img_io/i_img.hpp
../common/widgets/widget_dialog.o: ../common/img_io/img_clut.hpp
../common/widgets/widget_dialog.o: ../common/img_io/img_gray8.hpp
../common/widgets/widget_dialog.o: ../common/utils/string_utils.hpp
../common/widgets/widget_dialog.o: ../common/utils/mtx.hpp
../common/widgets/widget_dialog.o: ../common/utils/vmath.hpp
../common/widgets/widget_dialog.o: ../common/utils/preference.hpp
../common/widgets/widget_dialog.o: ../common/utils/file_io.hpp
../common/widgets/widget_dialog.o: ../common/widgets/common_parts.hpp
../common/widgets/widget_dialog.o: ../common/gl_fw/glmobj.hpp
../common/widgets/widget_dialog.o: ../common/gl_fw/gl_info.hpp
../common/widgets/widget_dialog.o: ../common/img_io/img_rgba8.hpp
../common/widgets/widget_dialog.o: ../common/img_io/img_files.hpp
../common/widgets/widget_dialog.o: ../common/img_io/i_img_io.hpp
../common/widgets/widget_dialog.o: ../common/widgets/widget_button.hpp
../common/widgets/widget_dialog.o: ../common/widgets/widget_text.hpp
../common/widgets/widget_dialog.o: ../common/widgets/widget_utils.hpp
../common/widgets/widget_tree.o: ../common/widgets/widget_tree.hpp
../common/widgets/widget_tree.o: ../common/widgets/widget_director.hpp
../common/widgets/widget_tree.o: ../common/widgets/widget.hpp
../common/widgets/widget_tree.o: ../common/img_io/img.hpp
../common/widgets/widget_tree.o: ../common/img_io/paint.hpp
../common/widgets/widget_tree.o: ../common/img_io/img.hpp
../common/widgets/widget_tree.o: ../common/img_io/i_img.hpp
../common/widgets/widget_tree.o: ../common/utils/vtx.hpp
../common/widgets/widget_tree.o: ../common/img_io/img_rgba8.hpp
../common/widgets/widget_tree.o: ../common/img_io/img_idx8.hpp
../common/widgets/widget_tree.o: ../common/img_io/i_img.hpp
../common/widgets/widget_tree.o: ../common/img_io/img_clut.hpp
../common/widgets/widget_tree.o: ../common/img_io/img_gray8.hpp
../common/widgets/widget_tree.o: ../common/utils/string_utils.hpp
../common/widgets/widget_tree.o: ../common/utils/mtx.hpp
../common/widgets/widget_tree.o: ../common/utils/vmath.hpp
../common/widgets/widget_tree.o: ../common/utils/preference.hpp
../common/widgets/widget_tree.o: ../common/utils/file_io.hpp
../common/widgets/widget_tree.o: ../common/widgets/common_parts.hpp
../common/widgets/widget_tree.o: ../common/gl_fw/glmobj.hpp
../common/widgets/widget_tree.o: ../common/gl_fw/gl_info.hpp
../common/widgets/widget_tree.o: ../common/img_io/img_rgba8.hpp
../common/widgets/widget_tree.o: ../common/img_io/img_files.hpp
../common/widgets/widget_tree.o: ../common/img_io/i_img_io.hpp
../common/widgets/widget_tree.o: ../common/widgets/widget_label.hpp
../common/widgets/widget_tree.o: ../common/utils/tree_unit.hpp
../common/widgets/widget_tree.o: ../common/widgets/widget_utils.hpp
../common/widgets/widget_filer.o: ../common/widgets/widget_utils.hpp
../common/widgets/widget_filer.o: ../common/widgets/widget.hpp
../common/widgets/widget_filer.o: ../common/img_io/img.hpp
../common/widgets/widget_filer.o: ../common/img_io/paint.hpp
../common/widgets/widget_filer.o: ../common/img_io/img.hpp
../common/widgets/widget_filer.o: ../common/img_io/i_img.hpp
../common/widgets/widget_filer.o: ../common/utils/vtx.hpp
../common/widgets/widget_filer.o: ../common/img_io/img_rgba8.hpp
../common/widgets/widget_filer.o: ../common/img_io/img_idx8.hpp
../common/widgets/widget_filer.o: ../common/img_io/i_img.hpp
../common/widgets/widget_filer.o: ../common/img_io/img_clut.hpp
../common/widgets/widget_filer.o: ../common/img_io/img_gray8.hpp
../common/widgets/widget_filer.o: ../common/utils/string_utils.hpp
../common/widgets/widget_filer.o: ../common/utils/mtx.hpp
../common/widgets/widget_filer.o: ../common/utils/vmath.hpp
../common/widgets/widget_filer.o: ../common/utils/preference.hpp
../common/widgets/widget_filer.o: ../common/utils/file_io.hpp
../common/widgets/widget_filer.o: ../common/widgets/widget_director.hpp
../common/widgets/widget_filer.o: ../common/widgets/common_parts.hpp
../common/widgets/widget_filer.o: ../common/gl_fw/glmobj.hpp
../common/widgets/widget_filer.o: ../common/gl_fw/gl_info.hpp
../common/widgets/widget_filer.o: ../common/img_io/img_rgba8.hpp
../common/widgets/widget_filer.o: ../common/img_io/img_files.hpp
../common/widgets/widget_filer.o: ../common/img_io/i_img_io.hpp
../common/widgets/widget_filer.o: ../common/widgets/widget_filer.hpp
../common/widgets/widget_filer.o: ../common/utils/files.hpp
../common/widgets/widget_filer.o: ../common/utils/drive_info.hpp
../common/widgets/widget_filer.o: ../common/utils/file_info.hpp
../common/widgets/widget_filer.o: ../common/utils/handle_set.hpp
../common/widgets/widget_filer.o: ../common/widgets/widget_null.hpp
../common/widgets/widget_filer.o: ../common/widgets/widget_director.hpp
../common/widgets/widget_filer.o: ../common/widgets/widget_label.hpp
../common/widgets/widget_filer.o: ../common/widgets/widget_button.hpp
../common/widgets/widget_terminal.o: ../common/widgets/widget_terminal.hpp
../common/widgets/widget_terminal.o: ../common/widgets/widget_director.hpp
../common/widgets/widget_terminal.o: ../common/widgets/widget.hpp
../common/widgets/widget_terminal.o: ../common/img_io/img.hpp
../common/widgets/widget_terminal.o: ../common/img_io/paint.hpp
../common/widgets/widget_terminal.o: ../common/img_io/img.hpp
../common/widgets/widget_terminal.o: ../common/img_io/i_img.hpp
../common/widgets/widget_terminal.o: ../common/utils/vtx.hpp
../common/widgets/widget_terminal.o: ../common/img_io/img_rgba8.hpp
../common/widgets/widget_terminal.o: ../common/img_io/img_idx8.hpp
../common/widgets/widget_terminal.o: ../common/img_io/i_img.hpp
../common/widgets/widget_terminal.o: ../common/img_io/img_clut.hpp
../common/widgets/widget_terminal.o: ../common/img_io/img_gray8.hpp
../common/widgets/widget_terminal.o: ../common/utils/string_utils.hpp
../common/widgets/widget_terminal.o: ../common/utils/mtx.hpp
../common/widgets/widget_terminal.o: ../common/utils/vmath.hpp
../common/widgets/widget_terminal.o: ../common/utils/preference.hpp
../common/widgets/widget_terminal.o: ../common/utils/file_io.hpp
../common/widgets/widget_terminal.o: ../common/widgets/common_parts.hpp
../common/widgets/widget_terminal.o: ../common/gl_fw/glmobj.hpp
../common/widgets/widget_terminal.o: ../common/gl_fw/gl_info.hpp
../common/widgets/widget_terminal.o: ../common/img_io/img_rgba8.hpp
../common/widgets/widget_terminal.o: ../common/img_io/img_files.hpp
../common/widgets/widget_terminal.o: ../common/img_io/i_img_io.hpp
../common/widgets/widget_terminal.o: ../common/widgets/widget_frame.hpp
../common/widgets/widget_terminal.o: ../common/gl_fw/glterminal.hpp
../common/widgets/widget_terminal.o: ../common/gl_fw/IGLcore.hpp
../common/widgets/widget_terminal.o: ../common/core/device.hpp
../common/widgets/widget_terminal.o: ../common/gl_fw/glfonts.hpp
../common/widgets/widget_terminal.o: ../common/gl_fw/Ikfimg.hpp
../common/widgets/widget_terminal.o: ../common/widgets/widget_utils.hpp
../common/mdf/pmd_io.o: ../common/mdf/pmd_io.hpp ../common/gl_fw/IGLcore.hpp
../common/mdf/pmd_io.o: ../common/core/device.hpp ../common/utils/vtx.hpp
../common/mdf/pmd_io.o: ../common/gl_fw/gl_info.hpp
../common/mdf/pmd_io.o: ../common/gl_fw/glfonts.hpp
../common/mdf/pmd_io.o: ../common/gl_fw/Ikfimg.hpp ../common/img_io/i_img.hpp
../common/mdf/pmd_io.o: ../common/img_io/img.hpp
../common/mdf/pmd_io.o: ../common/utils/string_utils.hpp
../common/mdf/pmd_io.o: ../common/utils/mtx.hpp ../common/utils/file_io.hpp
../common/mdf/pmd_io.o: ../common/gl_fw/gllight.hpp ../common/img_io/img.hpp
../common/mdf/pmd_io.o: ../common/utils/quat.hpp
../common/mdf/pmd_io.o: ../common/img_io/img_files.hpp
../common/mdf/pmd_io.o: ../common/img_io/i_img_io.hpp
../common/mdf/pmd_io.o: ../common/img_io/i_img.hpp
../common/mdf/pmd_io.o: ../common/gl_fw/glutils.hpp
../common/mdf/pmd_io.o: ../common/img_io/img_rgba8.hpp
../common/mdf/pmd_io.o: ../common/img_io/img_idx8.hpp
../common/mdf/pmd_io.o: ../common/img_io/img_clut.hpp
../common/mdf/pmd_io.o: ../common/img_io/img_gray8.hpp
../common/mdf/pmx_io.o: ../common/mdf/pmx_io.hpp ../common/gl_fw/IGLcore.hpp
../common/mdf/pmx_io.o: ../common/core/device.hpp ../common/utils/vtx.hpp
../common/mdf/pmx_io.o: ../common/gl_fw/gl_info.hpp
../common/mdf/pmx_io.o: ../common/gl_fw/glfonts.hpp
../common/mdf/pmx_io.o: ../common/gl_fw/Ikfimg.hpp ../common/img_io/i_img.hpp
../common/mdf/pmx_io.o: ../common/img_io/img.hpp
../common/mdf/pmx_io.o: ../common/utils/string_utils.hpp
../common/mdf/pmx_io.o: ../common/utils/mtx.hpp ../common/utils/file_io.hpp
../common/mdf/pmx_io.o: ../common/gl_fw/gllight.hpp ../common/img_io/img.hpp
../common/mdf/pmx_io.o: ../common/utils/quat.hpp
