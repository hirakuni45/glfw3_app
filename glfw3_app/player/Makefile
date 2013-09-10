#-----------------------------------------------#
# GNU-Makefile for "gcc"	 					#
#-----------------------------------------------#
TARGET		=	player
PSOURCES	=	main.cpp \
				player.cpp \
				../core/glcore.cpp \
				../core/device.cpp \
				../core/kfimg_ft2.cpp \
				../core/jfep_io.cpp \
				../gl_fw/glfonts.cpp \
				../gl_fw/glmobj.cpp \
				../gl_fw/gltexfb.cpp \
				../gl_fw/glterminal.cpp \
				../utils/vtx.cpp \
				../utils/vmath.cpp \
				../utils/string_utils.cpp \
				../utils/file_io.cpp \
				../utils/file_info.cpp \
				../utils/files.cpp \
				../utils/preference.cpp \
				../img_io/paint.cpp \
				../img_io/png_io.cpp \
				../img_io/jpeg_io.cpp \
				../img_io/openjpeg_io.cpp \
				../img_io/bmp_io.cpp \
				../img_io/dds_io.cpp \
				../img_io/pdf_in.cpp \
				../img_io/img_files.cpp \
				../img_io/img_utils.cpp \
				../snd_io/audio_io.cpp \
				../snd_io/pcm.cpp \
				../snd_io/wav_io.cpp \
				../snd_io/mp3_io.cpp \
				../snd_io/mp3_tag.cpp \
				../snd_io/aac_io.cpp \
				../snd_io/snd_files.cpp \
				../snd_io/sound.cpp \
				../widgets/common_parts.cpp \
				../widgets/widget.cpp \
				../widgets/widget_director.cpp \
				../widgets/widget_utils.cpp \
				../widgets/widget_group.cpp \
				../widgets/widget_image.cpp \
				../widgets/widget_frame.cpp \
				../widgets/widget_button.cpp \
				../widgets/widget_label.cpp \
				../widgets/widget_slider.cpp \
				../widgets/widget_list.cpp \
				../widgets/widget_check.cpp \
				../widgets/widget_radio.cpp \
				../widgets/widget_zero.cpp \
				../widgets/widget_filer.cpp \
				../widgets/widget_terminal.cpp

CSOURCES	=	../minizip/ioapi.c \
				../minizip/unzip.c
STDLIBS		=
OPTLIBS		=	glfw3 opengl32 glu32 gdi32 imm32 \
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

PINC_APP	=	./ ../
CINC_APP	=	./ ../
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
.SUFFIXES : .res .hpp .h .c .cpp .o

$(TARGET).exe: $(OBJECTS) Makefile
	$(LK) $(LOF)$(TARGET).exe $(LFLAGS) $(LIBS) $(OBJECTS) $(LIBN)

.c.o:
	$(CC) $(COPT) $(CFLAGS) $(CINCS) $(CCWARN) -c $< $(COF)$@

.cpp.o:
	$(CP) $(POPT) $(PFLAGS) $(PINCS) $(CPWARN) -c $< $(COF)$@

.rc.c:
	$(RC) $< $@

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
			../core/*.[hc]pp \
			../gl_fw/*.[hc]pp \
			../utils/*.[hc]pp \
			../img_io/*.[hc]pp \
			../snd_io/*.[hc]pp \
			../widgets/*.[hc]pp \
			../minizip/*.[hc] \
			../player/*.[hc]pp Makefile
	rm -f $(TARGET)_$(shell date +%Y%m%d%H)_src.tgz
	tar cfvz $(TARGET)_$(shell date +%Y%m%d%H)_src.tgz \
			../core/*.[hc]pp \
			../gl_fw/*.[hc]pp \
			../utils/*.[hc]pp \
			../img_io/*.[hc]pp \
			../snd_io/*.[hc]pp \
			../widgets/*.[hc]pp \
			../minizip/*.[hc] \
			../player/*.[hc]pp \
			../player/res/*.png \
			../player/res/*.ttf
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

../minizip/ioapi.o: ../minizip/ioapi.h
../minizip/unzip.o: ../minizip/unzip.h ../minizip/ioapi.h
main.o: main.hpp ../snd_io/sound.hpp ../snd_io/audio_io.hpp
main.o: ../utils/file_io.hpp ../utils/string_utils.hpp ../snd_io/i_audio.hpp
main.o: ../snd_io/audio.hpp ../snd_io/snd_files.hpp ../snd_io/i_snd_io.hpp
main.o: ../snd_io/tag.hpp ../img_io/i_img.hpp ../img_io/img.hpp
main.o: ../utils/vtx.hpp ../widgets/widget_director.hpp ../widgets/widget.hpp
main.o: ../img_io/img.hpp ../img_io/paint.hpp ../img_io/i_img.hpp
main.o: ../img_io/img_rgba8.hpp ../img_io/img_idx8.hpp ../img_io/img_clut.hpp
main.o: ../img_io/img_gray8.hpp ../utils/vmath.hpp ../utils/vtx.hpp
main.o: ../widgets/common_parts.hpp ../gl_fw/glmobj.hpp ../gl_fw/gl_info.hpp
main.o: ../img_io/img_rgba8.hpp ../img_io/img_files.hpp
main.o: ../img_io/i_img_io.hpp ../widgets/widget_filer.hpp ../utils/files.hpp
main.o: ../utils/drive_info.hpp ../utils/file_info.hpp
main.o: ../utils/handle_set.hpp ../utils/preference.hpp
main.o: ../widgets/widget_director.hpp ../widgets/widget.hpp
main.o: ../widgets/widget_null.hpp ../widgets/widget_label.hpp
main.o: ../core/glcore.hpp ../gl_fw/IGLcore.hpp ../core/device.hpp
main.o: ../gl_fw/glfonts.hpp ../gl_fw/Ikfimg.hpp ../core/device.hpp
main.o: ../utils/director.hpp ../utils/i_scene.hpp player.hpp
main.o: ../utils/i_scene.hpp ../widgets/widget_label.hpp
main.o: ../widgets/widget_slider.hpp ../widgets/widget_check.hpp
main.o: ../gl_fw/glutils.hpp
player.o: player.hpp main.hpp ../snd_io/sound.hpp ../snd_io/audio_io.hpp
player.o: ../utils/file_io.hpp ../utils/string_utils.hpp
player.o: ../snd_io/i_audio.hpp ../snd_io/audio.hpp ../snd_io/snd_files.hpp
player.o: ../snd_io/i_snd_io.hpp ../snd_io/tag.hpp ../img_io/i_img.hpp
player.o: ../img_io/img.hpp ../utils/vtx.hpp ../widgets/widget_director.hpp
player.o: ../widgets/widget.hpp ../img_io/img.hpp ../img_io/paint.hpp
player.o: ../img_io/i_img.hpp ../img_io/img_rgba8.hpp ../img_io/img_idx8.hpp
player.o: ../img_io/img_clut.hpp ../img_io/img_gray8.hpp ../utils/vmath.hpp
player.o: ../utils/vtx.hpp ../widgets/common_parts.hpp ../gl_fw/glmobj.hpp
player.o: ../gl_fw/gl_info.hpp ../img_io/img_rgba8.hpp
player.o: ../img_io/img_files.hpp ../img_io/i_img_io.hpp
player.o: ../widgets/widget_filer.hpp ../utils/files.hpp
player.o: ../utils/drive_info.hpp ../utils/file_info.hpp
player.o: ../utils/handle_set.hpp ../utils/preference.hpp
player.o: ../widgets/widget_director.hpp ../widgets/widget.hpp
player.o: ../widgets/widget_null.hpp ../widgets/widget_label.hpp
player.o: ../utils/i_scene.hpp ../utils/director.hpp ../utils/i_scene.hpp
player.o: ../widgets/widget_label.hpp ../widgets/widget_slider.hpp
player.o: ../widgets/widget_check.hpp ../core/glcore.hpp ../gl_fw/IGLcore.hpp
player.o: ../core/device.hpp ../gl_fw/glfonts.hpp ../gl_fw/Ikfimg.hpp
player.o: ../core/device.hpp ../widgets/widget_utils.hpp
player.o: ../widgets/widget_null.hpp ../widgets/widget_button.hpp
player.o: ../widgets/widget_image.hpp
../core/glcore.o: ../core/glcore.hpp ../gl_fw/IGLcore.hpp ../core/device.hpp
../core/glcore.o: ../utils/vtx.hpp ../gl_fw/gl_info.hpp ../gl_fw/glfonts.hpp
../core/glcore.o: ../gl_fw/Ikfimg.hpp ../img_io/i_img.hpp ../img_io/img.hpp
../core/glcore.o: ../utils/string_utils.hpp ../utils/file_io.hpp
../core/glcore.o: ../core/device.hpp
../core/device.o: ../core/device.hpp ../utils/vtx.hpp
../core/kfimg_ft2.o: ../core/kfimg_ft2.hpp ../gl_fw/Ikfimg.hpp
../core/kfimg_ft2.o: ../img_io/i_img.hpp ../img_io/img.hpp ../utils/vtx.hpp
../core/kfimg_ft2.o: ../img_io/img_gray8.hpp ../utils/string_utils.hpp
../core/jfep_io.o: ../core/jfep_io.hpp
../gl_fw/glfonts.o: ../gl_fw/glfonts.hpp ../gl_fw/gl_info.hpp
../gl_fw/glfonts.o: ../gl_fw/Ikfimg.hpp ../img_io/i_img.hpp ../img_io/img.hpp
../gl_fw/glfonts.o: ../utils/vtx.hpp ../utils/string_utils.hpp
../gl_fw/glfonts.o: ../img_io/img_utils.hpp ../img_io/i_img.hpp
../gl_fw/glfonts.o: ../img_io/img_idx8.hpp ../img_io/img_clut.hpp
../gl_fw/glfonts.o: ../img_io/img_gray8.hpp ../img_io/img_rgba8.hpp
../gl_fw/glmobj.o: ../gl_fw/glmobj.hpp ../gl_fw/gl_info.hpp
../gl_fw/glmobj.o: ../img_io/i_img.hpp ../img_io/img.hpp ../utils/vtx.hpp
../gl_fw/glmobj.o: ../img_io/img_rgba8.hpp ../img_io/i_img.hpp
../gl_fw/glmobj.o: ../img_io/img_idx8.hpp ../img_io/img_clut.hpp
../gl_fw/glmobj.o: ../img_io/img_gray8.hpp ../gl_fw/glutils.hpp
../gl_fw/glmobj.o: ../img_io/img_utils.hpp ../img_io/img_rgba8.hpp
../gl_fw/gltexfb.o: ../gl_fw/gltexfb.hpp ../gl_fw/gl_info.hpp
../gl_fw/gltexfb.o: ../utils/vtx.hpp
../gl_fw/glterminal.o: ../gl_fw/glterminal.hpp ../gl_fw/IGLcore.hpp
../gl_fw/glterminal.o: ../core/device.hpp ../utils/vtx.hpp
../gl_fw/glterminal.o: ../gl_fw/gl_info.hpp ../gl_fw/glfonts.hpp
../gl_fw/glterminal.o: ../gl_fw/Ikfimg.hpp ../img_io/i_img.hpp
../gl_fw/glterminal.o: ../img_io/img.hpp ../utils/string_utils.hpp
../gl_fw/glterminal.o: ../utils/file_io.hpp ../img_io/img.hpp
../utils/vtx.o: ../utils/vtx.hpp
../utils/vmath.o: ../utils/vmath.hpp ../utils/vtx.hpp
../utils/string_utils.o: ../utils/string_utils.hpp
../utils/file_io.o: ../utils/file_io.hpp ../utils/string_utils.hpp
../utils/file_info.o: ../utils/file_info.hpp ../utils/file_io.hpp
../utils/file_info.o: ../utils/string_utils.hpp
../utils/files.o: ../utils/files.hpp ../utils/drive_info.hpp
../utils/files.o: ../utils/file_info.hpp ../utils/file_io.hpp
../utils/files.o: ../utils/string_utils.hpp ../utils/handle_set.hpp
../utils/preference.o: ../Utils/preference.hpp ../utils/vtx.hpp
../utils/preference.o: ../utils/file_io.hpp ../utils/string_utils.hpp
../img_io/paint.o: ../img_io/paint.hpp ../img_io/img.hpp ../img_io/i_img.hpp
../img_io/paint.o: ../utils/vtx.hpp ../img_io/img_rgba8.hpp
../img_io/paint.o: ../img_io/img_idx8.hpp ../img_io/i_img.hpp
../img_io/paint.o: ../img_io/img_clut.hpp ../img_io/img_gray8.hpp
../img_io/paint.o: ../utils/string_utils.hpp ../gl_fw/Ikfimg.hpp
../img_io/png_io.o: ../img_io/png_io.hpp ../img_io/i_img_io.hpp
../img_io/png_io.o: ../utils/file_io.hpp ../utils/string_utils.hpp
../img_io/png_io.o: ../img_io/i_img.hpp ../img_io/img.hpp ../utils/vtx.hpp
../img_io/png_io.o: ../img_io/img_idx8.hpp ../img_io/i_img.hpp
../img_io/png_io.o: ../img_io/img_clut.hpp ../img_io/img_rgba8.hpp
../img_io/png_io.o: ../img_io/img_gray8.hpp
../img_io/jpeg_io.o: ../utils/file_io.hpp ../utils/string_utils.hpp
../img_io/jpeg_io.o: ../img_io/jpeg_io.hpp ../img_io/i_img_io.hpp
../img_io/jpeg_io.o: ../img_io/i_img.hpp ../img_io/img.hpp ../utils/vtx.hpp
../img_io/jpeg_io.o: ../img_io/img_rgba8.hpp ../img_io/img_idx8.hpp
../img_io/jpeg_io.o: ../img_io/i_img.hpp ../img_io/img_clut.hpp
../img_io/jpeg_io.o: ../img_io/img_gray8.hpp
../img_io/openjpeg_io.o: ../utils/file_io.hpp ../utils/string_utils.hpp
../img_io/openjpeg_io.o: ../img_io/openjpeg_io.hpp ../img_io/i_img_io.hpp
../img_io/openjpeg_io.o: ../img_io/i_img.hpp ../img_io/img.hpp
../img_io/openjpeg_io.o: ../utils/vtx.hpp ../img_io/img_rgba8.hpp
../img_io/openjpeg_io.o: ../img_io/img_idx8.hpp ../img_io/i_img.hpp
../img_io/openjpeg_io.o: ../img_io/img_clut.hpp ../img_io/img_gray8.hpp
../img_io/bmp_io.o: ../img_io/bmp_io.hpp ../img_io/i_img_io.hpp
../img_io/bmp_io.o: ../utils/file_io.hpp ../utils/string_utils.hpp
../img_io/bmp_io.o: ../img_io/i_img.hpp ../img_io/img.hpp ../utils/vtx.hpp
../img_io/bmp_io.o: ../img_io/img_idx8.hpp ../img_io/i_img.hpp
../img_io/bmp_io.o: ../img_io/img_clut.hpp ../img_io/img_rgba8.hpp
../img_io/bmp_io.o: ../img_io/img_gray8.hpp
../img_io/dds_io.o: ../img_io/dds_io.hpp ../img_io/i_img_io.hpp
../img_io/dds_io.o: ../utils/file_io.hpp ../utils/string_utils.hpp
../img_io/dds_io.o: ../img_io/i_img.hpp ../img_io/img.hpp ../utils/vtx.hpp
../img_io/dds_io.o: ../img_io/img_idx8.hpp ../img_io/i_img.hpp
../img_io/dds_io.o: ../img_io/img_clut.hpp ../img_io/img_rgba8.hpp
../img_io/dds_io.o: ../img_io/img_gray8.hpp
../img_io/pdf_in.o: ../img_io/pdf_in.hpp ../img_io/i_img_io.hpp
../img_io/pdf_in.o: ../utils/file_io.hpp ../utils/string_utils.hpp
../img_io/pdf_in.o: ../img_io/i_img.hpp ../img_io/img.hpp ../utils/vtx.hpp
../img_io/pdf_in.o: ../img_io/img_rgba8.hpp ../img_io/img_idx8.hpp
../img_io/pdf_in.o: ../img_io/i_img.hpp ../img_io/img_clut.hpp
../img_io/pdf_in.o: ../img_io/img_gray8.hpp
../img_io/img_files.o: ../img_io/img_files.hpp ../img_io/i_img_io.hpp
../img_io/img_files.o: ../utils/file_io.hpp ../utils/string_utils.hpp
../img_io/img_files.o: ../img_io/i_img.hpp ../img_io/img.hpp ../utils/vtx.hpp
../img_io/img_files.o: ../img_io/jpeg_io.hpp ../img_io/i_img_io.hpp
../img_io/img_files.o: ../img_io/img_rgba8.hpp ../img_io/img_idx8.hpp
../img_io/img_files.o: ../img_io/i_img.hpp ../img_io/img_clut.hpp
../img_io/img_files.o: ../img_io/img_gray8.hpp ../img_io/png_io.hpp
../img_io/img_files.o: ../img_io/bmp_io.hpp ../img_io/dds_io.hpp
../img_io/img_files.o: ../img_io/openjpeg_io.hpp
../img_io/img_utils.o: ../img_io/img_utils.hpp ../img_io/img.hpp
../img_io/img_utils.o: ../img_io/i_img.hpp ../utils/vtx.hpp
../img_io/img_utils.o: ../img_io/img_idx8.hpp ../img_io/i_img.hpp
../img_io/img_utils.o: ../img_io/img_clut.hpp ../img_io/img_gray8.hpp
../img_io/img_utils.o: ../img_io/img_rgba8.hpp
../snd_io/audio_io.o: ../snd_io/audio_io.hpp ../utils/file_io.hpp
../snd_io/audio_io.o: ../utils/string_utils.hpp ../snd_io/i_audio.hpp
../snd_io/audio_io.o: ../snd_io/audio.hpp ../snd_io/pcm.hpp
../snd_io/pcm.o: ../snd_io/pcm.hpp ../snd_io/i_audio.hpp ../utils/file_io.hpp
../snd_io/pcm.o: ../utils/string_utils.hpp ../snd_io/audio.hpp
../snd_io/wav_io.o: ../snd_io/wav_io.hpp ../snd_io/i_snd_io.hpp
../snd_io/wav_io.o: ../utils/file_io.hpp ../utils/string_utils.hpp
../snd_io/wav_io.o: ../snd_io/i_audio.hpp ../snd_io/audio.hpp
../snd_io/wav_io.o: ../snd_io/tag.hpp ../img_io/i_img.hpp ../img_io/img.hpp
../snd_io/wav_io.o: ../utils/vtx.hpp ../snd_io/pcm.hpp
../snd_io/mp3_io.o: ../snd_io/mp3_io.hpp ../snd_io/i_snd_io.hpp
../snd_io/mp3_io.o: ../utils/file_io.hpp ../utils/string_utils.hpp
../snd_io/mp3_io.o: ../snd_io/i_audio.hpp ../snd_io/audio.hpp
../snd_io/mp3_io.o: ../snd_io/tag.hpp ../img_io/i_img.hpp ../img_io/img.hpp
../snd_io/mp3_io.o: ../utils/vtx.hpp ../snd_io/mp3_tag.hpp
../snd_io/mp3_io.o: ../img_io/jpeg_io.hpp ../img_io/i_img_io.hpp
../snd_io/mp3_io.o: ../img_io/i_img.hpp ../img_io/img_rgba8.hpp
../snd_io/mp3_io.o: ../img_io/img_idx8.hpp ../img_io/img_clut.hpp
../snd_io/mp3_io.o: ../img_io/img_gray8.hpp ../img_io/png_io.hpp
../snd_io/mp3_io.o: ../snd_io/pcm.hpp ../img_io/img_utils.hpp
../snd_io/mp3_tag.o: ../snd_io/mp3_tag.hpp ../utils/file_io.hpp
../snd_io/mp3_tag.o: ../utils/string_utils.hpp ../img_io/i_img.hpp
../snd_io/mp3_tag.o: ../img_io/img.hpp ../utils/vtx.hpp ../img_io/jpeg_io.hpp
../snd_io/mp3_tag.o: ../img_io/i_img_io.hpp ../img_io/i_img.hpp
../snd_io/mp3_tag.o: ../img_io/img_rgba8.hpp ../img_io/img_idx8.hpp
../snd_io/mp3_tag.o: ../img_io/img_clut.hpp ../img_io/img_gray8.hpp
../snd_io/mp3_tag.o: ../img_io/png_io.hpp
../snd_io/aac_io.o: ../snd_io/aac_io.hpp ../snd_io/i_snd_io.hpp
../snd_io/aac_io.o: ../utils/file_io.hpp ../utils/string_utils.hpp
../snd_io/aac_io.o: ../snd_io/i_audio.hpp ../snd_io/audio.hpp
../snd_io/aac_io.o: ../snd_io/tag.hpp ../img_io/i_img.hpp ../img_io/img.hpp
../snd_io/aac_io.o: ../utils/vtx.hpp ../snd_io/pcm.hpp
../snd_io/aac_io.o: ../img_io/img_files.hpp ../img_io/i_img_io.hpp
../snd_io/aac_io.o: ../img_io/i_img.hpp ../img_io/img_utils.hpp
../snd_io/aac_io.o: ../img_io/img_idx8.hpp ../img_io/img_clut.hpp
../snd_io/aac_io.o: ../img_io/img_gray8.hpp ../img_io/img_rgba8.hpp
../snd_io/snd_files.o: ../snd_io/snd_files.hpp ../snd_io/i_snd_io.hpp
../snd_io/snd_files.o: ../utils/file_io.hpp ../utils/string_utils.hpp
../snd_io/snd_files.o: ../snd_io/i_audio.hpp ../snd_io/audio.hpp
../snd_io/snd_files.o: ../snd_io/tag.hpp ../img_io/i_img.hpp
../snd_io/snd_files.o: ../img_io/img.hpp ../utils/vtx.hpp
../snd_io/snd_files.o: ../snd_io/wav_io.hpp ../snd_io/mp3_io.hpp
../snd_io/snd_files.o: ../snd_io/mp3_tag.hpp ../img_io/jpeg_io.hpp
../snd_io/snd_files.o: ../img_io/i_img_io.hpp ../img_io/i_img.hpp
../snd_io/snd_files.o: ../img_io/img_rgba8.hpp ../img_io/img_idx8.hpp
../snd_io/snd_files.o: ../img_io/img_clut.hpp ../img_io/img_gray8.hpp
../snd_io/snd_files.o: ../img_io/png_io.hpp ../snd_io/aac_io.hpp
../snd_io/sound.o: ../snd_io/sound.hpp ../snd_io/audio_io.hpp
../snd_io/sound.o: ../utils/file_io.hpp ../utils/string_utils.hpp
../snd_io/sound.o: ../snd_io/i_audio.hpp ../snd_io/audio.hpp
../snd_io/sound.o: ../snd_io/snd_files.hpp ../snd_io/i_snd_io.hpp
../snd_io/sound.o: ../snd_io/tag.hpp ../img_io/i_img.hpp ../img_io/img.hpp
../snd_io/sound.o: ../utils/vtx.hpp ../snd_io/pcm.hpp ../utils/file_info.hpp
../widgets/common_parts.o: ../widgets/common_parts.hpp ../gl_fw/glmobj.hpp
../widgets/common_parts.o: ../gl_fw/gl_info.hpp ../img_io/i_img.hpp
../widgets/common_parts.o: ../img_io/img.hpp ../utils/vtx.hpp
../widgets/common_parts.o: ../img_io/img_rgba8.hpp ../img_io/i_img.hpp
../widgets/common_parts.o: ../img_io/img_idx8.hpp ../img_io/img_clut.hpp
../widgets/common_parts.o: ../img_io/img_gray8.hpp ../widgets/widget.hpp
../widgets/common_parts.o: ../img_io/img.hpp ../img_io/paint.hpp
../widgets/common_parts.o: ../img_io/img_rgba8.hpp ../utils/string_utils.hpp
../widgets/common_parts.o: ../utils/vmath.hpp ../utils/vtx.hpp
../widgets/common_parts.o: ../widgets/widget_utils.hpp
../widgets/common_parts.o: ../widgets/widget_director.hpp
../widgets/common_parts.o: ../img_io/img_files.hpp ../img_io/i_img_io.hpp
../widgets/common_parts.o: ../utils/file_io.hpp
../widgets/widget.o: ../widgets/widget.hpp ../img_io/img.hpp
../widgets/widget.o: ../img_io/paint.hpp ../img_io/img.hpp
../widgets/widget.o: ../img_io/i_img.hpp ../utils/vtx.hpp
../widgets/widget.o: ../img_io/img_rgba8.hpp ../img_io/img_idx8.hpp
../widgets/widget.o: ../img_io/i_img.hpp ../img_io/img_clut.hpp
../widgets/widget.o: ../img_io/img_gray8.hpp ../utils/string_utils.hpp
../widgets/widget.o: ../utils/vmath.hpp ../utils/vtx.hpp
../widgets/widget_director.o: ../gl_fw/gl_info.hpp ../gl_fw/IGLcore.hpp
../widgets/widget_director.o: ../core/device.hpp ../utils/vtx.hpp
../widgets/widget_director.o: ../gl_fw/glfonts.hpp ../gl_fw/Ikfimg.hpp
../widgets/widget_director.o: ../img_io/i_img.hpp ../img_io/img.hpp
../widgets/widget_director.o: ../utils/string_utils.hpp ../utils/file_io.hpp
../widgets/widget_director.o: ../widgets/widget_director.hpp
../widgets/widget_director.o: ../widgets/widget.hpp ../img_io/img.hpp
../widgets/widget_director.o: ../img_io/paint.hpp ../img_io/i_img.hpp
../widgets/widget_director.o: ../img_io/img_rgba8.hpp ../img_io/img_idx8.hpp
../widgets/widget_director.o: ../img_io/img_clut.hpp ../img_io/img_gray8.hpp
../widgets/widget_director.o: ../utils/vmath.hpp ../utils/vtx.hpp
../widgets/widget_director.o: ../widgets/common_parts.hpp ../gl_fw/glmobj.hpp
../widgets/widget_director.o: ../img_io/img_rgba8.hpp ../img_io/img_files.hpp
../widgets/widget_director.o: ../img_io/i_img_io.hpp
../widgets/widget_director.o: ../widgets/widget_utils.hpp
../widgets/widget_director.o: ../widgets/widget_director.hpp
../widgets/widget_director.o: ../widgets/widget_null.hpp
../widgets/widget_director.o: ../widgets/widget_image.hpp
../widgets/widget_director.o: ../widgets/widget_frame.hpp
../widgets/widget_director.o: ../widgets/widget_button.hpp
../widgets/widget_director.o: ../widgets/widget_label.hpp
../widgets/widget_director.o: ../widgets/widget_slider.hpp
../widgets/widget_director.o: ../widgets/widget_check.hpp
../widgets/widget_director.o: ../widgets/widget_radio.hpp
../widgets/widget_director.o: ../widgets/widget_list.hpp
../widgets/widget_director.o: ../Widgets/widget_director.hpp
../widgets/widget_utils.o: ../gl_fw/IGLcore.hpp ../core/device.hpp
../widgets/widget_utils.o: ../utils/vtx.hpp ../gl_fw/gl_info.hpp
../widgets/widget_utils.o: ../gl_fw/glfonts.hpp ../gl_fw/Ikfimg.hpp
../widgets/widget_utils.o: ../img_io/i_img.hpp ../img_io/img.hpp
../widgets/widget_utils.o: ../utils/string_utils.hpp ../utils/file_io.hpp
../widgets/widget_utils.o: ../img_io/paint.hpp ../img_io/i_img.hpp
../widgets/widget_utils.o: ../img_io/img_rgba8.hpp ../img_io/img_idx8.hpp
../widgets/widget_utils.o: ../img_io/img_clut.hpp ../img_io/img_gray8.hpp
../widgets/widget_utils.o: ../img_io/img_utils.hpp
../widgets/widget_utils.o: ../widgets/widget_utils.hpp ../widgets/widget.hpp
../widgets/widget_utils.o: ../img_io/img.hpp ../utils/vmath.hpp
../widgets/widget_utils.o: ../utils/vtx.hpp ../widgets/widget_director.hpp
../widgets/widget_utils.o: ../widgets/common_parts.hpp ../gl_fw/glmobj.hpp
../widgets/widget_utils.o: ../img_io/img_rgba8.hpp ../img_io/img_files.hpp
../widgets/widget_utils.o: ../img_io/i_img_io.hpp
../widgets/widget_utils.o: ../widgets/widget_button.hpp
../widgets/widget_utils.o: ../widgets/widget_label.hpp
../widgets/widget_group.o: ../widgets/widget_group.hpp ../widgets/widget.hpp
../widgets/widget_group.o: ../img_io/img.hpp ../img_io/paint.hpp
../widgets/widget_group.o: ../img_io/img.hpp ../img_io/i_img.hpp
../widgets/widget_group.o: ../utils/vtx.hpp ../img_io/img_rgba8.hpp
../widgets/widget_group.o: ../img_io/img_idx8.hpp ../img_io/i_img.hpp
../widgets/widget_group.o: ../img_io/img_clut.hpp ../img_io/img_gray8.hpp
../widgets/widget_group.o: ../utils/string_utils.hpp ../utils/vmath.hpp
../widgets/widget_group.o: ../utils/vtx.hpp ../widgets/widget_director.hpp
../widgets/widget_group.o: ../widgets/common_parts.hpp ../gl_fw/glmobj.hpp
../widgets/widget_group.o: ../gl_fw/gl_info.hpp ../img_io/img_rgba8.hpp
../widgets/widget_group.o: ../img_io/img_files.hpp ../img_io/i_img_io.hpp
../widgets/widget_group.o: ../utils/file_io.hpp
../widgets/widget_image.o: ../gl_fw/IGLcore.hpp ../core/device.hpp
../widgets/widget_image.o: ../utils/vtx.hpp ../gl_fw/gl_info.hpp
../widgets/widget_image.o: ../gl_fw/glfonts.hpp ../gl_fw/Ikfimg.hpp
../widgets/widget_image.o: ../img_io/i_img.hpp ../img_io/img.hpp
../widgets/widget_image.o: ../utils/string_utils.hpp ../utils/file_io.hpp
../widgets/widget_image.o: ../widgets/widget_image.hpp
../widgets/widget_image.o: ../widgets/widget_director.hpp
../widgets/widget_image.o: ../widgets/widget.hpp ../img_io/img.hpp
../widgets/widget_image.o: ../img_io/paint.hpp ../img_io/i_img.hpp
../widgets/widget_image.o: ../img_io/img_rgba8.hpp ../img_io/img_idx8.hpp
../widgets/widget_image.o: ../img_io/img_clut.hpp ../img_io/img_gray8.hpp
../widgets/widget_image.o: ../utils/vmath.hpp ../utils/vtx.hpp
../widgets/widget_image.o: ../widgets/common_parts.hpp ../gl_fw/glmobj.hpp
../widgets/widget_image.o: ../img_io/img_rgba8.hpp ../img_io/img_files.hpp
../widgets/widget_image.o: ../img_io/i_img_io.hpp ../img_io/img_utils.hpp
../widgets/widget_image.o: ../widgets/widget_utils.hpp
../widgets/widget_frame.o: ../widgets/widget_frame.hpp
../widgets/widget_frame.o: ../widgets/widget_director.hpp
../widgets/widget_frame.o: ../widgets/widget.hpp ../img_io/img.hpp
../widgets/widget_frame.o: ../img_io/paint.hpp ../img_io/img.hpp
../widgets/widget_frame.o: ../img_io/i_img.hpp ../utils/vtx.hpp
../widgets/widget_frame.o: ../img_io/img_rgba8.hpp ../img_io/img_idx8.hpp
../widgets/widget_frame.o: ../img_io/i_img.hpp ../img_io/img_clut.hpp
../widgets/widget_frame.o: ../img_io/img_gray8.hpp ../utils/string_utils.hpp
../widgets/widget_frame.o: ../utils/vmath.hpp ../utils/vtx.hpp
../widgets/widget_frame.o: ../widgets/common_parts.hpp ../gl_fw/glmobj.hpp
../widgets/widget_frame.o: ../gl_fw/gl_info.hpp ../img_io/img_rgba8.hpp
../widgets/widget_frame.o: ../img_io/img_files.hpp ../img_io/i_img_io.hpp
../widgets/widget_frame.o: ../utils/file_io.hpp ../widgets/widget_utils.hpp
../widgets/widget_frame.o: ../widgets/widget_director.hpp
../widgets/widget_button.o: ../gl_fw/IGLcore.hpp ../core/device.hpp
../widgets/widget_button.o: ../utils/vtx.hpp ../gl_fw/gl_info.hpp
../widgets/widget_button.o: ../gl_fw/glfonts.hpp ../gl_fw/Ikfimg.hpp
../widgets/widget_button.o: ../img_io/i_img.hpp ../img_io/img.hpp
../widgets/widget_button.o: ../utils/string_utils.hpp ../utils/file_io.hpp
../widgets/widget_button.o: ../widgets/widget_button.hpp
../widgets/widget_button.o: ../widgets/widget_director.hpp
../widgets/widget_button.o: ../widgets/widget.hpp ../img_io/img.hpp
../widgets/widget_button.o: ../img_io/paint.hpp ../img_io/i_img.hpp
../widgets/widget_button.o: ../img_io/img_rgba8.hpp ../img_io/img_idx8.hpp
../widgets/widget_button.o: ../img_io/img_clut.hpp ../img_io/img_gray8.hpp
../widgets/widget_button.o: ../utils/vmath.hpp ../utils/vtx.hpp
../widgets/widget_button.o: ../widgets/common_parts.hpp ../gl_fw/glmobj.hpp
../widgets/widget_button.o: ../img_io/img_rgba8.hpp ../img_io/img_files.hpp
../widgets/widget_button.o: ../img_io/i_img_io.hpp
../widgets/widget_button.o: ../widgets/widget_utils.hpp
../widgets/widget_label.o: ../gl_fw/IGLcore.hpp ../core/device.hpp
../widgets/widget_label.o: ../utils/vtx.hpp ../gl_fw/gl_info.hpp
../widgets/widget_label.o: ../gl_fw/glfonts.hpp ../gl_fw/Ikfimg.hpp
../widgets/widget_label.o: ../img_io/i_img.hpp ../img_io/img.hpp
../widgets/widget_label.o: ../utils/string_utils.hpp ../utils/file_io.hpp
../widgets/widget_label.o: ../widgets/widget_label.hpp
../widgets/widget_label.o: ../widgets/widget_director.hpp
../widgets/widget_label.o: ../widgets/widget.hpp ../img_io/img.hpp
../widgets/widget_label.o: ../img_io/paint.hpp ../img_io/i_img.hpp
../widgets/widget_label.o: ../img_io/img_rgba8.hpp ../img_io/img_idx8.hpp
../widgets/widget_label.o: ../img_io/img_clut.hpp ../img_io/img_gray8.hpp
../widgets/widget_label.o: ../utils/vmath.hpp ../utils/vtx.hpp
../widgets/widget_label.o: ../widgets/common_parts.hpp ../gl_fw/glmobj.hpp
../widgets/widget_label.o: ../img_io/img_rgba8.hpp ../img_io/img_files.hpp
../widgets/widget_label.o: ../img_io/i_img_io.hpp ../widgets/widget_utils.hpp
../widgets/widget_slider.o: ../gl_fw/IGLcore.hpp ../core/device.hpp
../widgets/widget_slider.o: ../utils/vtx.hpp ../gl_fw/gl_info.hpp
../widgets/widget_slider.o: ../gl_fw/glfonts.hpp ../gl_fw/Ikfimg.hpp
../widgets/widget_slider.o: ../img_io/i_img.hpp ../img_io/img.hpp
../widgets/widget_slider.o: ../utils/string_utils.hpp ../utils/file_io.hpp
../widgets/widget_slider.o: ../widgets/widget_slider.hpp
../widgets/widget_slider.o: ../widgets/widget_director.hpp
../widgets/widget_slider.o: ../widgets/widget.hpp ../img_io/img.hpp
../widgets/widget_slider.o: ../img_io/paint.hpp ../img_io/i_img.hpp
../widgets/widget_slider.o: ../img_io/img_rgba8.hpp ../img_io/img_idx8.hpp
../widgets/widget_slider.o: ../img_io/img_clut.hpp ../img_io/img_gray8.hpp
../widgets/widget_slider.o: ../utils/vmath.hpp ../utils/vtx.hpp
../widgets/widget_slider.o: ../widgets/common_parts.hpp ../gl_fw/glmobj.hpp
../widgets/widget_slider.o: ../img_io/img_rgba8.hpp ../img_io/img_files.hpp
../widgets/widget_slider.o: ../img_io/i_img_io.hpp
../widgets/widget_slider.o: ../widgets/widget_utils.hpp
../widgets/widget_slider.o: ../widgets/widget_director.hpp
../widgets/widget_list.o: ../gl_fw/IGLcore.hpp ../core/device.hpp
../widgets/widget_list.o: ../utils/vtx.hpp ../gl_fw/gl_info.hpp
../widgets/widget_list.o: ../gl_fw/glfonts.hpp ../gl_fw/Ikfimg.hpp
../widgets/widget_list.o: ../img_io/i_img.hpp ../img_io/img.hpp
../widgets/widget_list.o: ../utils/string_utils.hpp ../utils/file_io.hpp
../widgets/widget_list.o: ../widgets/widget_list.hpp
../widgets/widget_list.o: ../Widgets/widget_director.hpp
../widgets/widget_list.o: ../widgets/widget.hpp ../img_io/img.hpp
../widgets/widget_list.o: ../img_io/paint.hpp ../img_io/i_img.hpp
../widgets/widget_list.o: ../img_io/img_rgba8.hpp ../img_io/img_idx8.hpp
../widgets/widget_list.o: ../img_io/img_clut.hpp ../img_io/img_gray8.hpp
../widgets/widget_list.o: ../utils/vmath.hpp ../utils/vtx.hpp
../widgets/widget_list.o: ../widgets/common_parts.hpp ../gl_fw/glmobj.hpp
../widgets/widget_list.o: ../img_io/img_rgba8.hpp ../img_io/img_files.hpp
../widgets/widget_list.o: ../img_io/i_img_io.hpp ../widgets/widget_utils.hpp
../widgets/widget_list.o: ../widgets/widget_director.hpp
../widgets/widget_list.o: ../widgets/widget_null.hpp
../widgets/widget_list.o: ../widgets/widget_director.hpp
../widgets/widget_check.o: ../gl_fw/IGLcore.hpp ../core/device.hpp
../widgets/widget_check.o: ../utils/vtx.hpp ../gl_fw/gl_info.hpp
../widgets/widget_check.o: ../gl_fw/glfonts.hpp ../gl_fw/Ikfimg.hpp
../widgets/widget_check.o: ../img_io/i_img.hpp ../img_io/img.hpp
../widgets/widget_check.o: ../utils/string_utils.hpp ../utils/file_io.hpp
../widgets/widget_check.o: ../widgets/widget_check.hpp
../widgets/widget_check.o: ../widgets/widget_director.hpp
../widgets/widget_check.o: ../widgets/widget.hpp ../img_io/img.hpp
../widgets/widget_check.o: ../img_io/paint.hpp ../img_io/i_img.hpp
../widgets/widget_check.o: ../img_io/img_rgba8.hpp ../img_io/img_idx8.hpp
../widgets/widget_check.o: ../img_io/img_clut.hpp ../img_io/img_gray8.hpp
../widgets/widget_check.o: ../utils/vmath.hpp ../utils/vtx.hpp
../widgets/widget_check.o: ../widgets/common_parts.hpp ../gl_fw/glmobj.hpp
../widgets/widget_check.o: ../img_io/img_rgba8.hpp ../img_io/img_files.hpp
../widgets/widget_check.o: ../img_io/i_img_io.hpp ../widgets/widget_utils.hpp
../widgets/widget_check.o: ../widgets/widget_director.hpp
../widgets/widget_radio.o: ../gl_fw/IGLcore.hpp ../core/device.hpp
../widgets/widget_radio.o: ../utils/vtx.hpp ../gl_fw/gl_info.hpp
../widgets/widget_radio.o: ../gl_fw/glfonts.hpp ../gl_fw/Ikfimg.hpp
../widgets/widget_radio.o: ../img_io/i_img.hpp ../img_io/img.hpp
../widgets/widget_radio.o: ../utils/string_utils.hpp ../utils/file_io.hpp
../widgets/widget_radio.o: ../widgets/widget_radio.hpp
../widgets/widget_radio.o: ../widgets/widget_director.hpp
../widgets/widget_radio.o: ../widgets/widget.hpp ../img_io/img.hpp
../widgets/widget_radio.o: ../img_io/paint.hpp ../img_io/i_img.hpp
../widgets/widget_radio.o: ../img_io/img_rgba8.hpp ../img_io/img_idx8.hpp
../widgets/widget_radio.o: ../img_io/img_clut.hpp ../img_io/img_gray8.hpp
../widgets/widget_radio.o: ../utils/vmath.hpp ../utils/vtx.hpp
../widgets/widget_radio.o: ../widgets/common_parts.hpp ../gl_fw/glmobj.hpp
../widgets/widget_radio.o: ../img_io/img_rgba8.hpp ../img_io/img_files.hpp
../widgets/widget_radio.o: ../img_io/i_img_io.hpp ../widgets/widget_utils.hpp
../widgets/widget_radio.o: ../widgets/widget_director.hpp
../widgets/widget_zero.o: ../widgets/widget_zero.hpp
../widgets/widget_zero.o: ../widgets/widget_director.hpp
../widgets/widget_zero.o: ../widgets/widget.hpp ../img_io/img.hpp
../widgets/widget_zero.o: ../img_io/paint.hpp ../img_io/img.hpp
../widgets/widget_zero.o: ../img_io/i_img.hpp ../utils/vtx.hpp
../widgets/widget_zero.o: ../img_io/img_rgba8.hpp ../img_io/img_idx8.hpp
../widgets/widget_zero.o: ../img_io/i_img.hpp ../img_io/img_clut.hpp
../widgets/widget_zero.o: ../img_io/img_gray8.hpp ../utils/string_utils.hpp
../widgets/widget_zero.o: ../utils/vmath.hpp ../utils/vtx.hpp
../widgets/widget_zero.o: ../widgets/common_parts.hpp ../gl_fw/glmobj.hpp
../widgets/widget_zero.o: ../gl_fw/gl_info.hpp ../img_io/img_rgba8.hpp
../widgets/widget_zero.o: ../img_io/img_files.hpp ../img_io/i_img_io.hpp
../widgets/widget_zero.o: ../utils/file_io.hpp
../widgets/widget_filer.o: ../widgets/widget_filer.hpp ../utils/files.hpp
../widgets/widget_filer.o: ../utils/drive_info.hpp ../utils/file_info.hpp
../widgets/widget_filer.o: ../utils/file_io.hpp ../utils/string_utils.hpp
../widgets/widget_filer.o: ../utils/handle_set.hpp ../utils/preference.hpp
../widgets/widget_filer.o: ../utils/vtx.hpp ../widgets/widget_director.hpp
../widgets/widget_filer.o: ../widgets/widget.hpp ../img_io/img.hpp
../widgets/widget_filer.o: ../img_io/paint.hpp ../img_io/img.hpp
../widgets/widget_filer.o: ../img_io/i_img.hpp ../img_io/img_rgba8.hpp
../widgets/widget_filer.o: ../img_io/img_idx8.hpp ../img_io/i_img.hpp
../widgets/widget_filer.o: ../img_io/img_clut.hpp ../img_io/img_gray8.hpp
../widgets/widget_filer.o: ../utils/vmath.hpp ../utils/vtx.hpp
../widgets/widget_filer.o: ../widgets/common_parts.hpp ../gl_fw/glmobj.hpp
../widgets/widget_filer.o: ../gl_fw/gl_info.hpp ../img_io/img_rgba8.hpp
../widgets/widget_filer.o: ../img_io/img_files.hpp ../img_io/i_img_io.hpp
../widgets/widget_filer.o: ../widgets/widget.hpp ../widgets/widget_null.hpp
../widgets/widget_filer.o: ../widgets/widget_label.hpp
../widgets/widget_filer.o: ../widgets/widget_director.hpp
../widgets/widget_filer.o: ../widgets/widget_frame.hpp
../widgets/widget_terminal.o: ../widgets/widget_terminal.hpp
../widgets/widget_terminal.o: ../widgets/widget_director.hpp
../widgets/widget_terminal.o: ../widgets/widget.hpp ../img_io/img.hpp
../widgets/widget_terminal.o: ../img_io/paint.hpp ../img_io/img.hpp
../widgets/widget_terminal.o: ../img_io/i_img.hpp ../utils/vtx.hpp
../widgets/widget_terminal.o: ../img_io/img_rgba8.hpp ../img_io/img_idx8.hpp
../widgets/widget_terminal.o: ../img_io/i_img.hpp ../img_io/img_clut.hpp
../widgets/widget_terminal.o: ../img_io/img_gray8.hpp
../widgets/widget_terminal.o: ../utils/string_utils.hpp ../utils/vmath.hpp
../widgets/widget_terminal.o: ../utils/vtx.hpp ../widgets/common_parts.hpp
../widgets/widget_terminal.o: ../gl_fw/glmobj.hpp ../gl_fw/gl_info.hpp
../widgets/widget_terminal.o: ../img_io/img_rgba8.hpp ../img_io/img_files.hpp
../widgets/widget_terminal.o: ../img_io/i_img_io.hpp ../utils/file_io.hpp
