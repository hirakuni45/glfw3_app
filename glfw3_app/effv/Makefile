# -*- tab-width : 4 -*-
#=======================================================================
#   @file
#   @brief  glfw_app
#   @author 平松邦仁 (hira@rvf-rc45.net)
#	@copyright	Copyright (C) 2019, 2023 Kunihito Hiramatsu @n
#				Released under the MIT license @n
#				https://github.com/hirakuni45/glfw_app/blob/master/LICENSE
#=======================================================================
TARGET		=	effv

# 'debug' or 'release'
BUILD		=	release

VPATH		=	../common

CSOURCES	=	minizip/ioapi.c \
				minizip/unzip.c

PSOURCES	=	main.cpp \
				effv_main.cpp \
				core/glcore.cpp \
				core/device.cpp \
				widgets/widget_director.cpp \
				widgets/widget_filer.cpp \
				effekseer/core/CriticalSection.cpp \
				effekseer/core/Setting.cpp \
				effekseer/core/DefaultEffectLoader.cpp \
				effekseer/core/DefaultFile.cpp \
				effekseer/core/RectF.cpp \
				effekseer/core/Vector2D.cpp \
				effekseer/core/Vector3D.cpp \
				effekseer/core/Matrix44.cpp \
				effekseer/core/Matrix43.cpp \
				effekseer/core/FCurves.cpp \
				effekseer/core/Color.cpp \
				effekseer/core/Manager.cpp \
				effekseer/core/Instance.cpp \
				effekseer/core/InstanceGlobal.cpp \
				effekseer/core/InstanceGroup.cpp \
				effekseer/core/InstanceContainer.cpp \
				effekseer/core/Effect.cpp \
				effekseer/core/EffectNode.cpp \
				effekseer/core/EffectNodeRoot.cpp \
				effekseer/core/EffectNodeRing.cpp \
				effekseer/core/EffectNodeRibbon.cpp \
				effekseer/core/EffectNodeTrack.cpp \
				effekseer/core/EffectNodeSprite.cpp \
				effekseer/core/EffectNodeModel.cpp \
				effekseer/common/IndexBufferBase.cpp \
				effekseer/common/ModelRendererBase.cpp \
				effekseer/common/PngTextureLoader.cpp \
				effekseer/common/Renderer.cpp \
				effekseer/common/RenderStateBase.cpp \
				effekseer/common/RibbonRendererBase.cpp \
				effekseer/common/RingRendererBase.cpp \
				effekseer/common/SpriteRendererBase.cpp \
				effekseer/common/TrackRendererBase.cpp \
				effekseer/common/VertexBufferBase.cpp \
				effekseer/gl/DeviceObject.cpp \
				effekseer/gl/GLExtension.cpp \
				effekseer/gl/IndexBuffer.cpp \
				effekseer/gl/ModelLoader.cpp \
				effekseer/gl/ModelRenderer.cpp \
				effekseer/gl/Renderer.cpp \
				effekseer/gl/RenderState.cpp \
				effekseer/gl/RibbonRenderer.cpp \
				effekseer/gl/RingRenderer.cpp \
				effekseer/gl/Shader.cpp \
				effekseer/gl/SpriteRenderer.cpp \
				effekseer/gl/TextureLoader.cpp \
				effekseer/gl/TrackRenderer.cpp \
				effekseer/gl/VertexBuffer.cpp

# C++ version
CPP_VER		=	-std=c++17

# C++ include path for application
PINC_APP	=	. ../common
# C include path for application
CINC_APP	=	. ../common

# User include path
INC_USR		=
# User(optional) link library
LIBS_USR	=
# User library path 
LIB_DIR_USR	=
# cmpiler flags (-Dxxx)
CFLAGS		=
PFLAGS		=

-include $(VPATH)/makefile

-include $(DEPENDS)
