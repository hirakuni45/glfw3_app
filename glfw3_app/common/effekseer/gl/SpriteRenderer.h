
#ifndef	__EFFEKSEERRENDERER_GL_SPRITE_RENDERER_H__
#define	__EFFEKSEERRENDERER_GL_SPRITE_RENDERER_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include "RendererImplemented.h"
#include "effekseer/common/SpriteRendererBase.h"

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
namespace EffekseerRendererGL
{
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
typedef ::Effekseer::SpriteRenderer::NodeParameter efkSpriteNodeParam;
typedef ::Effekseer::SpriteRenderer::InstanceParameter efkSpriteInstanceParam;
typedef ::Effekseer::Vector3D efkVector3D;

class SpriteRenderer
	: public ::EffekseerRenderer::SpriteRendererBase
{
private:
	RendererImplemented*	m_renderer;
	Shader*		m_shader;
	Shader*		m_shader_no_texture;

	SpriteRenderer(RendererImplemented* renderer, Shader* shader, Shader* shader_no_texture);

public:

	virtual ~SpriteRenderer();

	static SpriteRenderer* Create( RendererImplemented* renderer );

public:
	void BeginRendering( const efkSpriteNodeParam& parameter, int32_t count, void* userData );

	void Rendering( const efkSpriteNodeParam& parameter, const efkSpriteInstanceParam& instanceParameter, void* userData );

	void EndRendering( const efkSpriteNodeParam& parameter, void* userData );
};
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
}
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif	// __EFFEKSEERRENDERER_GL_SPRITE_RENDERER_H__
