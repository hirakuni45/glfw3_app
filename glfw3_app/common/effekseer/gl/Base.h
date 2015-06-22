
#ifndef	__EFFEKSEERRENDERER_GL_BASE_H__
#define	__EFFEKSEERRENDERER_GL_BASE_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include "Base_Pre.h"

#include "effekseer/Effekseer.h"

#include <stdio.h>
#include <math.h>
#include <float.h>
#include <assert.h>
#include <string.h>

#include <map>
#include <vector>
#include <set>
#include <list>
#include <string>
#include <queue>

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
namespace EffekseerRendererGL
{
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
class RendererImplemented;

class RenderStateBase;

class DeviceObject;
class Texture;
class TargetTexture;
class DepthTexture;
class VertexBuffer;
class IndexBuffer;
class Shader;

class SpriteRenderer;
class RibbonRenderer;

class TextureLoader;

#if _WIN32
#include <gl/GLU.h>
#pragma comment(lib, "glu32.lib")
#ifdef _DEBUG
#define GLCheckError()		{ int __code = glGetError(); if(__code != GL_NO_ERROR) { printf("GLError filename = %s , line = %d, error = %s\n", __FILE__, __LINE__, (const char*)gluErrorString(__code) ); }  }
#else
#define GLCheckError()
#endif
#else
#define GLCheckError()
#endif

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
}
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif	// __EFFEKSEERRENDERER_GL_BASE_H__
