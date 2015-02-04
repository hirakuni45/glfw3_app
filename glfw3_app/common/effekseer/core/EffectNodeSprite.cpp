

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#include "Manager.h"
#include "Effect.h"
#include "EffectNode.h"
#include "Vector3D.h"
#include "Instance.h"
#include "EffectNodeSprite.h"

#include "effekseer/core/renderer/SpriteRenderer.h"

#include "Setting.h"

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
namespace Effekseer
{
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
	void EffectNodeSprite::LoadRendererParameter(unsigned char*& pos, Setting* setting)
{
	int32_t type = 0;
	memcpy(&type, pos, sizeof(int));
	pos += sizeof(int);
	assert(type == GetType());
	EffekseerPrintDebug("Renderer : Sprite\n");

	int32_t size = 0;

	memcpy(&RenderingOrder, pos, sizeof(int));
	pos += sizeof(int);

	if (m_effect->GetVersion() >= 3)
	{
		AlphaBlend = Texture.AlphaBlend;
	}
	else
	{
		memcpy(&AlphaBlend, pos, sizeof(int));
		pos += sizeof(int);
	}

	memcpy(&Billboard, pos, sizeof(int));
	pos += sizeof(int);

	SpriteAllColor.load(pos, m_effect->GetVersion());
	EffekseerPrintDebug("SpriteColorAllType : %d\n", SpriteAllColor.type);

	memcpy(&SpriteColor.type, pos, sizeof(int));
	pos += sizeof(int);
	EffekseerPrintDebug("SpriteColorType : %d\n", SpriteColor.type);

	if (SpriteColor.type == SpriteColor.Default)
	{
	}
	else if (SpriteColor.type == SpriteColor.Fixed)
	{
		memcpy(&SpriteColor.fixed, pos, sizeof(SpriteColor.fixed));
		pos += sizeof(SpriteColor.fixed);
	}

	memcpy(&SpritePosition.type, pos, sizeof(int));
	pos += sizeof(int);
	EffekseerPrintDebug("SpritePosition : %d\n", SpritePosition.type);

	if (SpritePosition.type == SpritePosition.Default)
	{
		if (m_effect->GetVersion() >= 8)
		{
			memcpy(&SpritePosition.fixed, pos, sizeof(SpritePosition.fixed));
			pos += sizeof(SpritePosition.fixed);
			SpritePosition.type = SpritePosition.Fixed;
		}
	}
	else if (SpritePosition.type == SpritePosition.Fixed)
	{
		memcpy(&SpritePosition.fixed, pos, sizeof(SpritePosition.fixed));
		pos += sizeof(SpritePosition.fixed);
	}

	if (m_effect->GetVersion() >= 3)
	{
		SpriteTexture = Texture.ColorTextureIndex;
	}
	else
	{
		memcpy(&SpriteTexture, pos, sizeof(int));
		pos += sizeof(int);
	}

	// �E��n����n�ϊ�
	if (setting->GetCoordinateSystem() == COORDINATE_SYSTEM_LH)
	{
	}

	/* �ʒu�g�又�� */
	if (m_effect->GetVersion() >= 8)
	{
		if (SpritePosition.type == SpritePosition.Default)
		{
		}
		else if (SpritePosition.type == SpritePosition.Fixed)
		{
			SpritePosition.fixed.ll *= m_effect->GetMaginification();
			SpritePosition.fixed.lr *= m_effect->GetMaginification();
			SpritePosition.fixed.ul *= m_effect->GetMaginification();
			SpritePosition.fixed.ur *= m_effect->GetMaginification();
		}
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void EffectNodeSprite::BeginRendering(int32_t count, Manager* manager)
{
	SpriteRenderer* renderer = manager->GetSpriteRenderer();
	if( renderer != NULL )
	{
		SpriteRenderer::NodeParameter nodeParameter;
		nodeParameter.AlphaBlend = AlphaBlend;
		nodeParameter.TextureFilter = Texture.FilterType;
		nodeParameter.TextureWrap = Texture.WrapType;
		nodeParameter.ZTest = Texture.ZTest;
		nodeParameter.ZWrite = Texture.ZWrite;
		nodeParameter.Billboard = Billboard;
		nodeParameter.ColorTextureIndex = SpriteTexture;
		nodeParameter.EffectPointer = GetEffect();
		renderer->BeginRendering( nodeParameter, count, m_userData );
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void EffectNodeSprite::Rendering(const Instance& instance, Manager* manager)
{
	const InstanceValues& instValues = instance.rendererValues.sprite;
	SpriteRenderer* renderer = manager->GetSpriteRenderer();
	if( renderer != NULL )
	{
		SpriteRenderer::NodeParameter nodeParameter;
		nodeParameter.AlphaBlend = AlphaBlend;
		nodeParameter.TextureFilter = Texture.FilterType;
		nodeParameter.TextureWrap = Texture.WrapType;
		nodeParameter.ZTest = Texture.ZTest;
		nodeParameter.ZWrite = Texture.ZWrite;
		nodeParameter.Billboard = Billboard;
		nodeParameter.ColorTextureIndex = SpriteTexture;
		nodeParameter.EffectPointer = GetEffect();

		SpriteRenderer::InstanceParameter instanceParameter;
		instValues._color.setValueToArg( instanceParameter.AllColor );
		instanceParameter.SRTMatrix43 = instance.GetGlobalMatrix43();

		color color_ll = instValues._color;
		color color_lr = instValues._color;
		color color_ul = instValues._color;
		color color_ur = instValues._color;

		if( SpriteColor.type == SpriteColorParameter::Default )
		{
		}
		else if( SpriteColor.type == SpriteColorParameter::Fixed )
		{
			color_ll = color::mul( color_ll, SpriteColor.fixed.ll );
			color_lr = color::mul( color_lr, SpriteColor.fixed.lr );
			color_ul = color::mul( color_ul, SpriteColor.fixed.ul );
			color_ur = color::mul( color_ur, SpriteColor.fixed.ur );
		}

		float fadeAlpha = GetFadeAlpha( instance );
		if( fadeAlpha != 1.0f )
		{
			color_ll.a = (uint8_t)(color_ll.a * fadeAlpha);
			color_lr.a = (uint8_t)(color_lr.a * fadeAlpha);
			color_ul.a = (uint8_t)(color_ul.a * fadeAlpha);
			color_ur.a = (uint8_t)(color_ur.a * fadeAlpha);
		}

		color_ll.setValueToArg( instanceParameter.Colors[0] );
		color_lr.setValueToArg( instanceParameter.Colors[1] );
		color_ul.setValueToArg( instanceParameter.Colors[2] );
		color_ur.setValueToArg( instanceParameter.Colors[3] );
		

		if( SpritePosition.type == SpritePosition.Default )
		{
			instanceParameter.Positions[0].X = -0.5f;
			instanceParameter.Positions[0].Y = -0.5f;
			instanceParameter.Positions[1].X = 0.5f;
			instanceParameter.Positions[1].Y = -0.5f;
			instanceParameter.Positions[2].X = -0.5f;
			instanceParameter.Positions[2].Y = 0.5f;
			instanceParameter.Positions[3].X = 0.5f;
			instanceParameter.Positions[3].Y = 0.5f;
		}
		else if( SpritePosition.type == SpritePosition.Fixed )
		{
			SpritePosition.fixed.ll.setValueToArg( instanceParameter.Positions[0] );
			SpritePosition.fixed.lr.setValueToArg( instanceParameter.Positions[1] );
			SpritePosition.fixed.ul.setValueToArg( instanceParameter.Positions[2] );
			SpritePosition.fixed.ur.setValueToArg( instanceParameter.Positions[3] );
		}

		instanceParameter.UV = instance.GetUV();
		renderer->Rendering( nodeParameter, instanceParameter, m_userData );
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void EffectNodeSprite::EndRendering(Manager* manager)
{
	SpriteRenderer* renderer = manager->GetSpriteRenderer();
	if( renderer != NULL )
	{
		SpriteRenderer::NodeParameter nodeParameter;
		nodeParameter.AlphaBlend = AlphaBlend;
		nodeParameter.TextureFilter = Texture.FilterType;
		nodeParameter.TextureWrap = Texture.WrapType;
		nodeParameter.ZTest = Texture.ZTest;
		nodeParameter.ZWrite = Texture.ZWrite;
		nodeParameter.Billboard = Billboard;
		nodeParameter.ColorTextureIndex = SpriteTexture;
		nodeParameter.EffectPointer = GetEffect();
		renderer->EndRendering( nodeParameter, m_userData );
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void EffectNodeSprite::InitializeRenderedInstance(Instance& instance, Manager* manager)
{
	InstanceValues& instValues = instance.rendererValues.sprite;

	if( SpriteAllColor.type == StandardColorParameter::Fixed )
	{
		instValues._color = SpriteAllColor.fixed.all;
	}
	else if( SpriteAllColor.type == StandardColorParameter::Random )
	{
		instValues._color = SpriteAllColor.random.all.getValue(*(manager));
	}
	else if( SpriteAllColor.type == StandardColorParameter::Easing )
	{
		instValues.allColorValues.easing.start = SpriteAllColor.easing.all.getStartValue(*(manager));
		instValues.allColorValues.easing.end = SpriteAllColor.easing.all.getEndValue(*(manager));

		float t = instance.m_LivingTime / instance.m_LivedTime;

		SpriteAllColor.easing.all.setValueToArg(
			instValues._color, 
			instValues.allColorValues.easing.start,
			instValues.allColorValues.easing.end,
			t );
	}
	else if( SpriteAllColor.type == StandardColorParameter::FCurve_RGBA )
	{
		instValues.allColorValues.fcurve_rgba.offset[0] = SpriteAllColor.fcurve_rgba.FCurve->R.GetOffset(*(manager));
		instValues.allColorValues.fcurve_rgba.offset[1] = SpriteAllColor.fcurve_rgba.FCurve->G.GetOffset(*(manager));
		instValues.allColorValues.fcurve_rgba.offset[2] = SpriteAllColor.fcurve_rgba.FCurve->B.GetOffset(*(manager));
		instValues.allColorValues.fcurve_rgba.offset[3] = SpriteAllColor.fcurve_rgba.FCurve->A.GetOffset(*(manager));
		
		instValues._color.r = (uint8_t)Clamp( (instValues.allColorValues.fcurve_rgba.offset[0] + SpriteAllColor.fcurve_rgba.FCurve->R.GetValue( (int32_t)instance.m_LivingTime )), 255, 0);
		instValues._color.g = (uint8_t)Clamp( (instValues.allColorValues.fcurve_rgba.offset[1] + SpriteAllColor.fcurve_rgba.FCurve->G.GetValue( (int32_t)instance.m_LivingTime )), 255, 0);
		instValues._color.b = (uint8_t)Clamp( (instValues.allColorValues.fcurve_rgba.offset[2] + SpriteAllColor.fcurve_rgba.FCurve->B.GetValue( (int32_t)instance.m_LivingTime )), 255, 0);
		instValues._color.a = (uint8_t)Clamp( (instValues.allColorValues.fcurve_rgba.offset[3] + SpriteAllColor.fcurve_rgba.FCurve->A.GetValue( (int32_t)instance.m_LivingTime )), 255, 0);
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void EffectNodeSprite::UpdateRenderedInstance(Instance& instance, Manager* manager)
{
	InstanceValues& instValues = instance.rendererValues.sprite;

	if( SpriteAllColor.type == StandardColorParameter::Easing )
	{
		float t = instance.m_LivingTime / instance.m_LivedTime;

		SpriteAllColor.easing.all.setValueToArg(
			instValues._color, 
			instValues.allColorValues.easing.start,
			instValues.allColorValues.easing.end,
			t );
	}
	else if( SpriteAllColor.type == StandardColorParameter::FCurve_RGBA )
	{
		instValues._color.r = (uint8_t)Clamp( (instValues.allColorValues.fcurve_rgba.offset[0] + SpriteAllColor.fcurve_rgba.FCurve->R.GetValue( (int32_t)instance.m_LivingTime )), 255, 0);
		instValues._color.g = (uint8_t)Clamp( (instValues.allColorValues.fcurve_rgba.offset[1] + SpriteAllColor.fcurve_rgba.FCurve->G.GetValue( (int32_t)instance.m_LivingTime )), 255, 0);
		instValues._color.b = (uint8_t)Clamp( (instValues.allColorValues.fcurve_rgba.offset[2] + SpriteAllColor.fcurve_rgba.FCurve->B.GetValue( (int32_t)instance.m_LivingTime )), 255, 0);
		instValues._color.a = (uint8_t)Clamp( (instValues.allColorValues.fcurve_rgba.offset[3] + SpriteAllColor.fcurve_rgba.FCurve->A.GetValue( (int32_t)instance.m_LivingTime )), 255, 0);
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
