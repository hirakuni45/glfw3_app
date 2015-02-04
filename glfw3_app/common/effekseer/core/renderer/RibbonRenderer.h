
#ifndef	__EFFEKSEER_RIBBON_RENDERER_H__
#define	__EFFEKSEER_RIBBON_RENDERER_H__

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

class RibbonRenderer
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
		bool				ViewpointDependent;
	};

	struct InstanceParameter
	{
		int32_t			InstanceCount;
		int32_t			InstanceIndex;
		Matrix43		SRTMatrix43;
		Color		AllColor;

		// ç∂ÅAâE
		Color		Colors[2];

		float	Positions[2];
	};

public:
	RibbonRenderer() {}

	virtual ~RibbonRenderer() {}

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
#endif	// __EFFEKSEER_RIBBON_RENDERER_H__
