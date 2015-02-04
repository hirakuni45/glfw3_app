
#ifndef	__EFFEKSEER_PARTICLE_RENDERER_H__
#define	__EFFEKSEER_PARTICLE_RENDERER_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include "effekseer/core/Base.h"
#include "effekseer/core/Vector2D.h"
#include "effekseer/core/Vector3D.h"
#include "effekseer/core/Matrix43.h"
#include "effekseer/core/Color.h"

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
namespace Effekseer
{
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
class ParticleRenderer
{
public:

	struct NodeParameter
	{
		Effect*				EffectPointer;
		int32_t				TextureIndex;
		eAlphaBlend			AlphaBlend;
		eTextureFilterType	TextureFilter;
		eTextureWrapType	TextureWrap;
	};

	struct InstanceParameter
	{
		Vector3D	Position;
		float		Size;
		Color		ParticleColor;
	};

public:
	ParticleRenderer() {}

	virtual ~ParticleRenderer() {}

	virtual void BeginRendering( const NodeParameter& parameter, void* userData ) {}

	virtual void Rendering( const NodeParameter& parameter, const InstanceParameter& instanceParameter, void* userData ) {}

	virtual void EndRendering( const NodeParameter& parameter, void* userData ) {}
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
}
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif	// __EFFEKSEER_PARTICLE_RENDERER_H__
