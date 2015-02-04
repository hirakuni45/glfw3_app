
#ifndef	__EFFEKSEER_RING_RENDERER_H__
#define	__EFFEKSEER_RING_RENDERER_H__

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

class RingRenderer
{
public:

	struct NodeParameter
	{
		Effect*				EffectPointer;
		int32_t				ColorTextureIndex;
		eAlphaBlend			AlphaBlend;
		eTextureFilterType	TextureFilter;
		eTextureWrapType	TextureWrap;
		bool				ZTest;
		bool				ZWrite;
		eBillboardType		Billboard;
		int32_t				VertexCount;
	};

	struct InstanceParameter
	{
		Matrix43	SRTMatrix43;
		float		ViewingAngle;
		Vector2D	OuterLocation;
		Vector2D	InnerLocation;
		float		CenterRatio;
		Color		OuterColor;
		Color		CenterColor;
		Color		InnerColor;
		
		RectF	UV;
	};

public:
	RingRenderer() {}

	virtual ~RingRenderer() {}

	virtual void BeginRendering( const NodeParameter& parameter, int32_t count, void* userData ) {}

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
#endif	// __EFFEKSEER_RING_RENDERER_H__
