
#ifndef	__EFFEKSEER_MODEL_RENDERER_H__
#define	__EFFEKSEER_MODEL_RENDERER_H__

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

class ModelRenderer
{
public:

	struct NodeParameter
	{
		Effect*				EffectPointer;
		eAlphaBlend			AlphaBlend;
		eTextureFilterType	TextureFilter;
		eTextureWrapType	TextureWrap;
		bool				ZTest;
		bool				ZWrite;
		bool				Lighting;
		eCullingType		Culling;
		int32_t				ModelIndex;
		int32_t				ColorTextureIndex;
		int32_t				NormalTextureIndex;
		float				Magnification;
		bool				IsRightHand;
	};

	struct InstanceParameter
	{
		Matrix43		SRTMatrix43;
		RectF			UV;
		Color			AllColor;
	};

public:
	ModelRenderer() {}

	virtual ~ModelRenderer() {}

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
#endif	// __EFFEKSEER_MODEL_RENDERER_H__
