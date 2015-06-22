
//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include "RenderState.h"

#include "Renderer.h"
#include "RendererImplemented.h"

#include "GLExtension.h"

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
namespace EffekseerRendererGL
{
//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
RenderState::RenderState( RendererImplemented* renderer )
	: m_renderer	( renderer )
{
#ifdef	__USE_SAMPLERS
	glGenSamplers( 4, m_samplers );
#endif
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
RenderState::~RenderState()
{
#ifdef	__USE_SAMPLERS
	glDeleteSamplers( 4, m_samplers );
#endif
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
void RenderState::Update( bool forced )
{
	GLCheckError();

	if( m_active.DepthTest != m_next.DepthTest || forced )
	{
		if( m_next.DepthTest )
		{
			glEnable( GL_DEPTH_TEST );
		}
		else
		{
			glDisable( GL_DEPTH_TEST );
		}
	}

	GLCheckError();

	if( m_active.DepthWrite != m_next.DepthWrite || forced )
	{
		glDepthMask( m_next.DepthWrite );
	}

	GLCheckError();

	if( m_active.CullingType != m_next.CullingType || forced )
	{
		if( m_next.CullingType == Effekseer::CULLING_FRONT )
		{
			glEnable( GL_CULL_FACE );
			glCullFace( GL_FRONT );
		}
		else if( m_next.CullingType == Effekseer::CULLING_BACK )
		{
			glEnable( GL_CULL_FACE );
			glCullFace( GL_BACK );
		}
		else if( m_next.CullingType == Effekseer::CULLING_DOUBLE )
		{
			glDisable( GL_CULL_FACE );
			glCullFace( GL_FRONT_AND_BACK );
		}
	}

	GLCheckError();

	if( m_active.AlphaBlend != m_next.AlphaBlend || forced )
	{
		if(  m_next.AlphaBlend == ::Effekseer::ALPHA_BLEND_OPACITY )
		{
			glDisable( GL_BLEND );
		}
		else
		{
			glEnable( GL_BLEND );

			if( m_next.AlphaBlend == ::Effekseer::ALPHA_BLEND_SUB )
			{
				GLExt::glBlendEquationSeparate(GL_FUNC_REVERSE_SUBTRACT, GL_FUNC_ADD);
				GLExt::glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE, GL_ONE, GL_ONE);
			}
			else
			{
				GLExt::glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
				if( m_next.AlphaBlend == ::Effekseer::ALPHA_BLEND_BLEND )
				{
					GLExt::glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE);
				}
				else if( m_next.AlphaBlend == ::Effekseer::ALPHA_BLEND_ADD )
				{
					GLExt::glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE, GL_ONE, GL_ONE);
				}
				else if( m_next.AlphaBlend == ::Effekseer::ALPHA_BLEND_MUL )
				{
					GLExt::glBlendFuncSeparate(GL_ZERO, GL_SRC_COLOR, GL_ONE, GL_ONE);
				}
			}
		}
	}

	GLCheckError();
	
	static const GLint glfilterMin[] = { GL_NEAREST, GL_LINEAR_MIPMAP_LINEAR };
	static const GLint glfilterMag[] = { GL_NEAREST, GL_LINEAR };
	static const GLint glwrap[] = { GL_REPEAT, GL_CLAMP_TO_EDGE };

#ifdef	__USE_SAMPLERS
	for( int32_t i = 0; i < 4; i++ )
	{
		if( m_active.TextureFilterTypes[i] != m_next.TextureFilterTypes[i] || forced )
		{
			glActiveTexture( GL_TEXTURE0 + i );

			int32_t filter_ = (int32_t)m_next.TextureFilterTypes[i];

			glSamplerParameteri( m_samplers[i], GL_TEXTURE_MAG_FILTER, glfilter[filter_] );
			glSamplerParameteri( m_samplers[i], GL_TEXTURE_MIN_FILTER, glfilter[filter_] );
			//glSamplerParameteri( m_samplers[i],  GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			//glSamplerParameteri( m_samplers[i],  GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);

			glBindSampler(i, m_samplers[i]);
		}

		if( m_active.TextureWrapTypes[i] != m_next.TextureWrapTypes[i] || forced )
		{
			glActiveTexture( GL_TEXTURE0 + i );

			int32_t wrap_ = (int32_t)m_next.TextureWrapTypes[i];
			glSamplerParameteri( m_samplers[i], GL_TEXTURE_WRAP_S, glwrap[wrap_] );
			glSamplerParameteri( m_samplers[i], GL_TEXTURE_WRAP_T, glwrap[wrap_] );

			glBindSampler( i, m_samplers[i] );
		}
	}
#else

	GLCheckError();
	for (int32_t i = 0; i < m_renderer->GetCurrentTextures().size(); i++)
	{
		/* �e�N�X�`�����ݒ肳��Ă��Ȃ��ꍇ�̓X�L�b�v */
		if (m_renderer->GetCurrentTextures()[i] == 0) continue;

		if( m_active.TextureFilterTypes[i] != m_next.TextureFilterTypes[i] || forced )
		{
			GLExt::glActiveTexture( GL_TEXTURE0 + i );
			GLCheckError();

			int32_t filter_ = (int32_t)m_next.TextureFilterTypes[i];

			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, glfilterMag[filter_] );
			GLCheckError();

			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, glfilterMin[filter_] );
			GLCheckError();
		}

		if( m_active.TextureWrapTypes[i] != m_next.TextureWrapTypes[i] || forced )
		{
			GLExt::glActiveTexture( GL_TEXTURE0 + i );
			GLCheckError();

			int32_t wrap_ = (int32_t)m_next.TextureWrapTypes[i];

			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, glwrap[wrap_] );
			GLCheckError();

			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, glwrap[wrap_] );
			GLCheckError();
		}
	}
	GLCheckError();

#endif

	GLExt::glActiveTexture( GL_TEXTURE0 );
	
	m_active = m_next;

	GLCheckError();
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
}
//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
