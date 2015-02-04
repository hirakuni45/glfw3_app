

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#include "Manager.h"
#include "Effect.h"
#include "EffectNode.h"
#include "Vector3D.h"
#include "Instance.h"
#include "InstanceGroup.h"
#include "EffectNodeRibbon.h"

#include "Setting.h"

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
namespace Effekseer
{
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void EffectNodeRibbon::LoadRendererParameter(unsigned char*& pos, Setting* setting)
{
	int32_t type = 0;
	memcpy( &type, pos, sizeof(int) );
	pos += sizeof(int);
	assert( type == GetType() );
	EffekseerPrintDebug("Renderer : Ribbon\n");

	int32_t size = 0;

	if( m_effect->GetVersion() >= 3)
	{
		AlphaBlend = Texture.AlphaBlend;
	}
	else
	{
		memcpy( &AlphaBlend, pos, sizeof(int) );
		pos += sizeof(int);
	}

	memcpy( &ViewpointDependent, pos, sizeof(int) );
	pos += sizeof(int);
	
	memcpy( &RibbonAllColor.type, pos, sizeof(int) );
	pos += sizeof(int);
	EffekseerPrintDebug("RibbonColorAllType : %d\n", RibbonAllColor.type );

	if( RibbonAllColor.type == RibbonAllColorParameter::Fixed )
	{
		memcpy( &RibbonAllColor.fixed, pos, sizeof(RibbonAllColor.fixed) );
		pos += sizeof(RibbonAllColor.fixed);
	}
	else if( RibbonAllColor.type == RibbonAllColorParameter::Random )
	{
		RibbonAllColor.random.all.load( m_effect->GetVersion(), pos );
	}
	else if( RibbonAllColor.type == RibbonAllColorParameter::Easing )
	{
		RibbonAllColor.easing.all.load( m_effect->GetVersion(), pos );
	}

	memcpy( &RibbonColor.type, pos, sizeof(int) );
	pos += sizeof(int);
	EffekseerPrintDebug("RibbonColorType : %d\n", RibbonColor.type );

	if( RibbonColor.type == RibbonColor.Default )
	{
	}
	else if( RibbonColor.type == RibbonColor.Fixed )
	{
		memcpy( &RibbonColor.fixed, pos, sizeof(RibbonColor.fixed) );
		pos += sizeof(RibbonColor.fixed);
	}

	memcpy( &RibbonPosition.type, pos, sizeof(int) );
	pos += sizeof(int);
	EffekseerPrintDebug("RibbonPosition : %d\n", RibbonPosition.type );

	if( RibbonPosition.type == RibbonPosition.Default )
	{
		if( m_effect->GetVersion() >= 8 )
		{
			memcpy( &RibbonPosition.fixed, pos, sizeof(RibbonPosition.fixed) );
			pos += sizeof(RibbonPosition.fixed);
			RibbonPosition.type = RibbonPosition.Fixed;
		}
	}
	else if( RibbonPosition.type == RibbonPosition.Fixed )
	{
		memcpy( &RibbonPosition.fixed, pos, sizeof(RibbonPosition.fixed) );
		pos += sizeof(RibbonPosition.fixed);
	}

	if( m_effect->GetVersion() >= 3)
	{
		RibbonTexture = Texture.ColorTextureIndex;
	}
	else
	{
		memcpy( &RibbonTexture, pos, sizeof(int) );
		pos += sizeof(int);
	}

	// �E��n����n�ϊ�
	if( setting->GetCoordinateSystem() == COORDINATE_SYSTEM_LH )
	{
	}

	/* �ʒu�g�又�� */
	if( m_effect->GetVersion() >= 8 )
	{
		if( RibbonPosition.type == RibbonPosition.Default )
		{
		}
		else if( RibbonPosition.type == RibbonPosition.Fixed )
		{
			RibbonPosition.fixed.l *= m_effect->GetMaginification();
			RibbonPosition.fixed.r *= m_effect->GetMaginification();
		}
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void EffectNodeRibbon::BeginRendering(int32_t count, Manager* manager)
{
	RibbonRenderer* renderer = manager->GetRibbonRenderer();
	if( renderer != NULL )
	{
		m_nodeParameter.AlphaBlend = AlphaBlend;
		m_nodeParameter.TextureFilter = Texture.FilterType;
		m_nodeParameter.TextureWrap = Texture.WrapType;
		m_nodeParameter.ZTest = Texture.ZTest;
		m_nodeParameter.ZWrite = Texture.ZWrite;
		m_nodeParameter.ViewpointDependent = ViewpointDependent != 0;
		m_nodeParameter.ColorTextureIndex = RibbonTexture;
		m_nodeParameter.EffectPointer = GetEffect();

		renderer->BeginRendering( m_nodeParameter, count, m_userData );
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void EffectNodeRibbon::BeginRenderingGroup(InstanceGroup* group, Manager* manager)
{
	RibbonRenderer* renderer = manager->GetRibbonRenderer();
	if( renderer != NULL )
	{
		m_instanceParameter.InstanceCount = group->GetInstanceCount();
		m_instanceParameter.InstanceIndex = 0;
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void EffectNodeRibbon::Rendering(const Instance& instance, Manager* manager)
{
	const InstanceValues& instValues = instance.rendererValues.ribbon;
	RibbonRenderer* renderer = manager->GetRibbonRenderer();
	if( renderer != NULL )
	{
		instValues._color.setValueToArg( m_instanceParameter.AllColor );
		m_instanceParameter.SRTMatrix43 = instance.GetGlobalMatrix43();

		color color_l = instValues._color;
		color color_r = instValues._color;

		if( RibbonColor.type == RibbonColorParameter::Default )
		{

		}
		else if( RibbonColor.type == RibbonColorParameter::Fixed )
		{
			color_l = color::mul( color_l, RibbonColor.fixed.l );
			color_r = color::mul( color_r, RibbonColor.fixed.r );
		}

		float fadeAlpha = GetFadeAlpha( instance );
		if( fadeAlpha != 1.0f )
		{
			color_l.a = (uint8_t)(color_l.a * fadeAlpha);
			color_r.a = (uint8_t)(color_r.a * fadeAlpha);
		}

		color_l.setValueToArg( m_instanceParameter.Colors[0] );
		color_r.setValueToArg( m_instanceParameter.Colors[1] );


		
		if( RibbonPosition.type == RibbonPositionParameter::Default )
		{
			m_instanceParameter.Positions[0] = -0.5f;
			m_instanceParameter.Positions[1] = 0.5f;
		}
		else if( RibbonPosition.type == RibbonPositionParameter::Fixed )
		{
			m_instanceParameter.Positions[0] = RibbonPosition.fixed.l;
			m_instanceParameter.Positions[1] = RibbonPosition.fixed.r;
		}

		renderer->Rendering( m_nodeParameter, m_instanceParameter, m_userData );

		m_instanceParameter.InstanceIndex++;
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void EffectNodeRibbon::EndRendering(Manager* manager)
{
	RibbonRenderer* renderer = manager->GetRibbonRenderer();
	if( renderer != NULL )
	{
		renderer->EndRendering( m_nodeParameter, m_userData );
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void EffectNodeRibbon::InitializeRenderedInstance(Instance& instance, Manager* manager)
{
	InstanceValues& instValues = instance.rendererValues.ribbon;

	if( RibbonAllColor.type == RibbonAllColorParameter::Fixed )
	{
		instValues._color = RibbonAllColor.fixed.all;
	}
	else if( RibbonAllColor.type == RibbonAllColorParameter::Random )
	{
		instValues._color = RibbonAllColor.random.all.getValue(*(manager));
	}
	else if( RibbonAllColor.type == RibbonAllColorParameter::Easing )
	{
		instValues.allColorValues.easing.start = RibbonAllColor.easing.all.getStartValue(*(manager));
		instValues.allColorValues.easing.end = RibbonAllColor.easing.all.getEndValue(*(manager));
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void EffectNodeRibbon::UpdateRenderedInstance(Instance& instance, Manager* manager)
{
	InstanceValues& instValues = instance.rendererValues.ribbon;

	if( RibbonAllColor.type == RibbonAllColorParameter::Easing )
	{
		float t = instance.m_LivingTime / instance.m_LivedTime;

		RibbonAllColor.easing.all.setValueToArg(
			instValues._color, 
			instValues.allColorValues.easing.start,
			instValues.allColorValues.easing.end,
			t );
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
