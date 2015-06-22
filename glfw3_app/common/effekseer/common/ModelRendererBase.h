
#ifndef	__EFFEKSEERRENDERER_MODEL_RENDERER_BASE_H__
#define	__EFFEKSEERRENDERER_MODEL_RENDERER_BASE_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include "effekseer/Effekseer.h"
#include <assert.h>
#include <string.h>
#include <vector>

#include "CommonUtils.h"
#include "RenderStateBase.h"
#include "VertexBufferBase.h"
#include "IndexBufferBase.h"

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
namespace EffekseerRenderer
{
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
typedef ::Effekseer::ModelRenderer::NodeParameter efkModelNodeParam;
typedef ::Effekseer::ModelRenderer::InstanceParameter efkModelInstanceParam;
typedef ::Effekseer::Vector3D efkVector3D;

template<int MODEL_COUNT>
struct ModelRendererVertexConstantBuffer
{
	Effekseer::Matrix44		CameraMatrix;
	Effekseer::Matrix44		ModelMatrix[MODEL_COUNT];
	float	ModelUV[MODEL_COUNT][4];
	float	ModelColor[MODEL_COUNT][4];

	float	LightDirection[4];
	float	LightColor[4];
	float	LightAmbientColor[4];
};

struct ModelRendererPixelConstantBuffer
{
	float	LightDirection[4];
	float	LightColor[4];
	float	LightAmbientColor[4];
};

class ModelRendererBase
	: public ::Effekseer::ModelRenderer
{
protected:
	std::vector<Effekseer::Matrix44>	m_matrixes;
	std::vector<Effekseer::RectF>		m_uv;
	std::vector<Effekseer::Color>		m_colors;

	void ColorToFloat4(::Effekseer::Color color, float fc[4])
	{
		fc[0] = color.R / 255.0f;
		fc[1] = color.G / 255.0f;
		fc[2] = color.B / 255.0f;
		fc[3] = color.A / 255.0f;
	}
	
	void VectorToFloat4(::Effekseer::Vector3D v, float fc[4])
	{
		fc[0] = v.X;
		fc[1] = v.Y;
		fc[2] = v.Z;
		fc[3] = 1.0f;
	}

	ModelRendererBase();
public:

	virtual ~ModelRendererBase();

	void BeginRendering( const efkModelNodeParam& parameter, int32_t count, void* userData );

	void Rendering( const efkModelNodeParam& parameter, const efkModelInstanceParam& instanceParameter, void* userData );

	template<typename RENDERER, typename SHADER, typename TEXTURE, typename MODEL, bool Instancing, int InstanceCount>
	void EndRendering_(
		RENDERER* renderer, 
		SHADER* shader_lighting_texture_normal,
		SHADER* shader_lighting_normal,
		SHADER* shader_lighting_texture,
		SHADER* shader_lighting,
		SHADER* shader_texture,
		SHADER* shader,
		const efkModelNodeParam& param)
	{
		if (m_matrixes.size() == 0) return;
		if (param.ModelIndex < 0) return;

		MODEL* model = (MODEL*) param.EffectPointer->GetModel(param.ModelIndex);
		if (model == NULL) return;
		
		RenderStateBase::State& state = renderer->GetRenderState()->Push();
		state.DepthTest = param.ZTest;
		state.DepthWrite = param.ZWrite;
		state.AlphaBlend = param.AlphaBlend;
		state.CullingType = param.Culling;

		/*�V�F�[�_�[�I��*/
		SHADER* shader_ = NULL;
		if (param.Lighting)
		{
			if (param.NormalTextureIndex >= 0)
			{
				if (param.ColorTextureIndex >= 0)
				{
					shader_ = shader_lighting_texture_normal;
				}
				else
				{
					shader_ = shader_lighting_normal;
				}
			}
			else
			{
				if (param.ColorTextureIndex >= 0)
				{
					shader_ = shader_lighting_texture;
				}
				else
				{
					shader_ = shader_lighting;
				}
			}
		}
		else
		{
			if (param.ColorTextureIndex >= 0)
			{
				shader_ = shader_texture;
			}
			else
			{
				shader_ = shader;
			}
		}

		renderer->BeginShader(shader_);

		/*�e�N�X�`���I��*/
		TEXTURE textures[2];
		textures[0] = (TEXTURE)NULL;
		textures[1] = (TEXTURE)NULL;

		if (param.ColorTextureIndex >= 0)
		{
			textures[0] = TexturePointerToTexture<TEXTURE>(param.EffectPointer->GetImage(param.ColorTextureIndex));
		}

		if (param.NormalTextureIndex >= 0)
		{
			textures[1] = TexturePointerToTexture<TEXTURE>(param.EffectPointer->GetImage(param.NormalTextureIndex));
		}
		
		renderer->SetTextures(shader_, textures, 2);

		state.TextureFilterTypes[0] = param.TextureFilter;
		state.TextureWrapTypes[0] = param.TextureWrap;
		state.TextureFilterTypes[1] = param.TextureFilter;
		state.TextureWrapTypes[1] = param.TextureWrap;

		renderer->GetRenderState()->Update(false);

		ModelRendererVertexConstantBuffer<InstanceCount>* vcb = (ModelRendererVertexConstantBuffer<InstanceCount>*)shader_->GetVertexConstantBuffer();
		ModelRendererPixelConstantBuffer* pcb = (ModelRendererPixelConstantBuffer*)shader_->GetPixelConstantBuffer();
		
		// �Œ�l�ݒ�
		if (param.Lighting)
		{
			{
				::Effekseer::Vector3D lightDirection = renderer->GetLightDirection();
				::Effekseer::Vector3D::Normal(lightDirection, lightDirection);
				VectorToFloat4(lightDirection, vcb->LightDirection);
				VectorToFloat4(lightDirection, pcb->LightDirection);
			}

			{
				ColorToFloat4(renderer->GetLightColor(), vcb->LightColor);
				ColorToFloat4(renderer->GetLightColor(), pcb->LightColor);
			}

			{
				ColorToFloat4(renderer->GetLightAmbientColor(), vcb->LightAmbientColor);
				ColorToFloat4(renderer->GetLightAmbientColor(), pcb->LightAmbientColor);
			}
		}
		vcb->CameraMatrix = renderer->GetCameraProjectionMatrix();

		/* �o�b�t�@�̐ݒ�̌�Ƀ��C�A�E�g��ݒ肵�Ȃ��Ɩ��� */
		renderer->SetVertexBuffer(model->VertexBuffer, sizeof(Effekseer::Model::VertexWithIndex));
		renderer->SetIndexBuffer(model->IndexBuffer);
		renderer->SetLayout(shader_);

		if(Instancing)
		{
			for( size_t loop = 0; loop < m_matrixes.size(); )
			{
				int32_t modelCount = Effekseer::Min( m_matrixes.size() - loop, model->ModelCount );
				
				for( int32_t num = 0; num < modelCount; num++ )
				{
					vcb->ModelMatrix[num] = m_matrixes[loop+num];

					vcb->ModelUV[num][0] = m_uv[loop+num].X;
					vcb->ModelUV[num][1] = m_uv[loop+num].Y;
					vcb->ModelUV[num][2] = m_uv[loop+num].Width;
					vcb->ModelUV[num][3] = m_uv[loop+num].Height;

					ColorToFloat4(m_colors[loop+num],vcb->ModelColor[num]);
				}

				shader_->SetConstantBuffer();

				renderer->DrawPolygon( model->VertexCount * modelCount, model->IndexCount * modelCount );

				loop += modelCount;
			}
		}
		else
		{
			for( size_t loop = 0; loop < m_matrixes.size(); )
			{
				vcb->ModelMatrix[0] = m_matrixes[loop];
				vcb->ModelUV[0][0] = m_uv[loop].X;
				vcb->ModelUV[0][1] = m_uv[loop].Y;
				vcb->ModelUV[0][2] = m_uv[loop].Width;
				vcb->ModelUV[0][3] = m_uv[loop].Height;
				
				ColorToFloat4( m_colors[loop], vcb->ModelColor[0] );
				shader_->SetConstantBuffer();
				renderer->DrawPolygon( model->VertexCount, model->IndexCount );

				loop += 1;
			}
		}

		renderer->EndShader(shader_);

		renderer->GetRenderState()->Pop();
	}
};
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
}
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif	// __EFFEKSEERRENDERER_MODEL_RENDERER_H__
