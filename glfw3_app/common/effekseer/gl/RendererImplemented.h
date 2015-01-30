
#ifndef	__EFFEKSEERRENDERER_GL_RENDERER_IMPLEMENTED_H__
#define	__EFFEKSEERRENDERER_GL_RENDERER_IMPLEMENTED_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include "EffekseerRendererGL.Base.h"
#include "EffekseerRendererGL.Renderer.h"
#include "../../EffekseerRendererCommon/EffekseerRenderer.RenderStateBase.h"

#if defined(_M_IX86) || defined(__x86__)
#define EFK_SSE2
#include <emmintrin.h>
#endif

/* Visual Studio 2008 */
#if _MSC_VER == 1500
#include <xmmintrin.h>
#endif

//----------------------------------------------------------------------------------
// Lib
//----------------------------------------------------------------------------------

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
namespace EffekseerRendererGL
{

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
struct Vertex
{
	::Effekseer::Vector3D	Pos;
	::Effekseer::Color		Col;
	float					UV[2];

	void SetColor( const ::Effekseer::Color& color )
	{
		Col = color;
	}
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
inline void TransformVertexes( Vertex* vertexes, int32_t count, const ::Effekseer::Matrix43& mat )
{
	#ifdef EFK_SSE2
		__m128 r0 = _mm_loadu_ps( mat.Value[0] );
		__m128 r1 = _mm_loadu_ps( mat.Value[1] );
		__m128 r2 = _mm_loadu_ps( mat.Value[2] );
		__m128 r3 = _mm_loadu_ps( mat.Value[3] );

		float tmp_out[4];
		::Effekseer::Vector3D* inout_prev;

		// �P���[�v��
		{
			::Effekseer::Vector3D* inout_cur = &vertexes[0].Pos;
			__m128 v = _mm_loadu_ps( (const float*)inout_cur );

			__m128 x = _mm_shuffle_ps( v, v, _MM_SHUFFLE(0,0,0,0) );
			__m128 a0 = _mm_mul_ps( r0, x );
			__m128 y = _mm_shuffle_ps( v, v, _MM_SHUFFLE(1,1,1,1) );
			__m128 a1 = _mm_mul_ps( r1, y );
			__m128 z = _mm_shuffle_ps( v, v, _MM_SHUFFLE(2,2,2,2) );
			__m128 a2 = _mm_mul_ps( r2, z );

			__m128 a01 = _mm_add_ps( a0, a1 );
			__m128 a23 = _mm_add_ps( a2, r3 );
			__m128 a = _mm_add_ps( a01, a23 );

			// ����̌��ʂ��X�g�A���Ă���
			_mm_storeu_ps( tmp_out, a );
			inout_prev = inout_cur;
		}

		for( int i = 1; i < count; i++ )
		{
			::Effekseer::Vector3D* inout_cur = &vertexes[i].Pos;
			__m128 v = _mm_loadu_ps( (const float*)inout_cur );

			__m128 x = _mm_shuffle_ps( v, v, _MM_SHUFFLE(0,0,0,0) );
			__m128 a0 = _mm_mul_ps( r0, x );
			__m128 y = _mm_shuffle_ps( v, v, _MM_SHUFFLE(1,1,1,1) );
			__m128 a1 = _mm_mul_ps( r1, y );
			__m128 z = _mm_shuffle_ps( v, v, _MM_SHUFFLE(2,2,2,2) );
			__m128 a2 = _mm_mul_ps( r2, z );

			__m128 a01 = _mm_add_ps( a0, a1 );
			__m128 a23 = _mm_add_ps( a2, r3 );
			__m128 a = _mm_add_ps( a01, a23 );

			// ���O�̃��[�v�̌��ʂ��������݂܂�
			inout_prev->X = tmp_out[0];
			inout_prev->Y = tmp_out[1];
			inout_prev->Z = tmp_out[2];

			// ����̌��ʂ��X�g�A���Ă���
			_mm_storeu_ps( tmp_out, a );
			inout_prev = inout_cur;
		}

		// �Ō�̃��[�v�̌��ʂ���������
		{
			inout_prev->X = tmp_out[0];
			inout_prev->Y = tmp_out[1];
			inout_prev->Z = tmp_out[2];
		}
	#else
		for( int i = 0; i < count; i++ )
		{
			::Effekseer::Vector3D::Transform(
			vertexes[i].Pos,
			vertexes[i].Pos,
			mat );
		}
	#endif
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
struct RenderStateSet
{
	GLboolean	blend;
	GLboolean	cullFace;
	GLboolean	depthTest;
	GLboolean	depthWrite;
	GLboolean	texture;
	GLint		blendSrc;
	GLint		blendDst;
	GLint		blendEquation;
};

/**
	@brief	�`��N���X
	@note
	�c�[�������̕`��@�\�B
*/
class RendererImplemented
	: public Renderer
{
friend class DeviceObject;

private:
	/* �Q�ƃJ�E���^ */
	int	m_reference;

	VertexBuffer*		m_vertexBuffer;
	IndexBuffer*		m_indexBuffer;
	int32_t				m_squareMaxCount;
	
	::Effekseer::Vector3D	m_lightDirection;
	::Effekseer::Color		m_lightColor;
	::Effekseer::Color		m_lightAmbient;

	::Effekseer::Matrix44	m_proj;
	::Effekseer::Matrix44	m_camera;
	::Effekseer::Matrix44	m_cameraProj;

	::EffekseerRenderer::RenderStateBase*		m_renderState;

	std::set<DeviceObject*>	m_deviceObjects;

	// �X�e�[�g�ۑ��p
	RenderStateSet m_originalState;

	bool	m_restorationOfStates;

	/* ���ݐݒ肳��Ă���e�N�X�`�� */
	std::vector<GLuint>	m_currentTextures;

public:
	/**
		@brief	�R���X�g���N�^
	*/
	RendererImplemented( int32_t squareMaxCount );

	/**
		@brief	�f�X�g���N�^
	*/
	~RendererImplemented();

	void OnLostDevice();
	void OnResetDevice();

	/**
		@brief	������
	*/
	bool Initialize();

	/**
		@brief	�Q�ƃJ�E���^�����Z����B
		@return	���s��̎Q�ƃJ�E���^�̒l
	*/
	int AddRef();

	/**
		@brief	�Q�ƃJ�E���^�����Z����B
		@return	���s��̎Q�ƃJ�E���^�̒l
	*/
	int Release();

	void Destory();

	void SetRestorationOfStatesFlag(bool flag);

	/**
		@brief	�`��J�n
	*/
	bool BeginRendering();

	/**
		@brief	�`��I��
	*/
	bool EndRendering();

	/**
		@brief	���_�o�b�t�@�擾
	*/
	VertexBuffer* GetVertexBuffer();

	/**
		@brief	�C���f�b�N�X�o�b�t�@�擾
	*/
	IndexBuffer* GetIndexBuffer();

	/**
		@brief	�ő�`��X�v���C�g��
	*/
	int32_t GetSquareMaxCount() const;

	::EffekseerRenderer::RenderStateBase* GetRenderState();
	
	/**
		@brief	���C�g�̕������擾����B
	*/
	const ::Effekseer::Vector3D& GetLightDirection() const;

	/**
		@brief	���C�g�̕�����ݒ肷��B
	*/
	void SetLightDirection( ::Effekseer::Vector3D& direction );

	/**
		@brief	���C�g�̐F���擾����B
	*/
	const ::Effekseer::Color& GetLightColor() const;

	/**
		@brief	���C�g�̐F��ݒ肷��B
	*/
	void SetLightColor( ::Effekseer::Color& color );

	/**
		@brief	���C�g�̊����̐F���擾����B
	*/
	const ::Effekseer::Color& GetLightAmbientColor() const;

	/**
		@brief	���C�g�̊����̐F��ݒ肷��B
	*/
	void SetLightAmbientColor( ::Effekseer::Color& color );

	/**
		@brief	���e�s����擾����B
	*/
	const ::Effekseer::Matrix44& GetProjectionMatrix() const;

	/**
		@brief	���e�s���ݒ肷��B
	*/
	void SetProjectionMatrix( const ::Effekseer::Matrix44& mat );

	/**
		@brief	�J�����s����擾����B
	*/
	const ::Effekseer::Matrix44& GetCameraMatrix() const;

	/**
		@brief	�J�����s���ݒ肷��B
	*/
	void SetCameraMatrix( const ::Effekseer::Matrix44& mat );

	/**
		@brief	�J�����v���W�F�N�V�����s����擾����B
	*/
	::Effekseer::Matrix44& GetCameraProjectionMatrix();

	/**
		@brief	�X�v���C�g�����_���[�𐶐�����B
	*/
	::Effekseer::SpriteRenderer* CreateSpriteRenderer();

	/**
		@brief	���{�������_���[�𐶐�����B
	*/
	::Effekseer::RibbonRenderer* CreateRibbonRenderer();
	
	/**
		@brief	�����O�����_���[�𐶐�����B
	*/
	::Effekseer::RingRenderer* CreateRingRenderer();
	
	/**
		@brief	���f�������_���[�𐶐�����B
	*/
	::Effekseer::ModelRenderer* CreateModelRenderer();

	/**
		@brief	�O�Ճ����_���[�𐶐�����B
	*/
	::Effekseer::TrackRenderer* CreateTrackRenderer();

	/**
		@brief	�e�N�X�`���Ǎ��N���X�𐶐�����B
	*/
	::Effekseer::TextureLoader* CreateTextureLoader( ::Effekseer::FileInterface* fileInterface = NULL );
	
	/**
		@brief	���f���Ǎ��N���X�𐶐�����B
	*/
	::Effekseer::ModelLoader* CreateModelLoader( ::Effekseer::FileInterface* fileInterface = NULL );

	void SetVertexBuffer( VertexBuffer* vertexBuffer, int32_t size );
	void SetVertexBuffer(GLuint vertexBuffer, int32_t size);
	void SetIndexBuffer( IndexBuffer* indexBuffer );
	void SetIndexBuffer(GLuint indexBuffer);

	void SetLayout(Shader* shader);
	void DrawSprites( int32_t spriteCount, int32_t vertexOffset );
	void DrawPolygon( int32_t vertexCount, int32_t indexCount);
	void BeginShader(Shader* shader);
	void EndShader(Shader* shader);

	void SetTextures(Shader* shader, GLuint* textures, int32_t count);

	void ResetRenderState();

	std::vector<GLuint>& GetCurrentTextures() { return m_currentTextures; }
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
}
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif	// __EFFEKSEERRENDERER_GL_RENDERER_IMPLEMENTED_H__