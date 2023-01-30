
TARGET		=	seeda_peeping_app

ICON_RC		=

# 'debug' or 'release'
BUILD		=	release

VPATH		=	../common

# 				./emu/nofrendo.c

CSOURCES	=	minizip/ioapi.c \
				minizip/unzip.c

PSOURCES	=	main.cpp \
				core/glcore.cpp \
				core/device.cpp \
				widgets/widget_director.cpp \
				widgets/widget_filer.cpp

# C++ version
CPP_VER		=	-std=c++17

# C++ include path for application
PINC_APP	=	. ../common
# C include path for application
CINC_APP	=	. ../common

# User include path
INC_USR		=
# User(optional) link library
ifeq ($(OS),Windows_NT)
LIBS_USR	=	ncftp
else
LIBS_USR	=	ncftp
endif

# User library path 
LIB_DIR_USR	=	./libncftp
# cmpiler flags (-Dxxx)
CFLAGS		=
PFLAGS		=

-include $(VPATH)/makefile

-include $(DEPENDS)
