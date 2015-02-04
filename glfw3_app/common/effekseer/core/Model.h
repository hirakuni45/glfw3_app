
#ifndef	__EFFEKSEER_MODEL_H__
#define	__EFFEKSEER_MODEL_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include "Base.h"
#include "Vector2D.h"
#include "Vector3D.h"
#include "Manager.h"

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
namespace Effekseer { 
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
/**
	@brief	���f���N���X
*/
class Model
{
public:
	struct Vertex
	{
		Vector3D Position;
		Vector3D Normal;
		Vector3D Binormal;
		Vector3D Tangent;
		Vector2D UV;
	};

	struct VertexWithIndex
	{
		Vector3D Position;
		Vector3D Normal;
		Vector3D Binormal;
		Vector3D Tangent;
		Vector2D UV;
		uint8_t Index[4];
	};

	struct Face
	{
		int32_t	Indexes[3];
	};

	struct Emitter
	{
		Vector3D	Position;
		Vector3D	Normal;
		Vector3D	Binormal;
		Vector3D	Tangent;
	};

private:
	uint8_t*	m_data;
	int32_t		m_size;

	int32_t		m_version;

	int32_t		m_vertexCount;
	Vertex*		m_vertexes;

	int32_t		m_faceCount;
	Face*		m_faces;

	int32_t		m_modelCount;

public:
	/**
		@brief	�R���X�g���N�^
	*/
	Model( void* data, int32_t size ) 
		: m_data	( NULL )
		, m_size	( size )
		, m_version	( 0 )
		, m_vertexCount	( 0 )
		, m_vertexes	( NULL )
		, m_faceCount	( 0 )
		, m_faces		( NULL )
	{
		m_data = new uint8_t[m_size];
		memcpy( m_data, data, m_size );

		uint8_t* p = (uint8_t*)m_data;
	
		memcpy( &m_version, p, sizeof(int32_t) );
		p += sizeof(int32_t);

		memcpy( &m_modelCount, p, sizeof(int32_t) );
		p += sizeof(int32_t);

		memcpy( &m_vertexCount, p, sizeof(int32_t) );
		p += sizeof(int32_t);

		m_vertexes = (Vertex*)p;
		p += ( sizeof(Vertex) * m_vertexCount );

		memcpy( &m_faceCount, p, sizeof(int32_t) );
		p += sizeof(int32_t);

		m_faces = (Face*)p;
		p += ( sizeof(Face) * m_faceCount );
	}

	Vertex* GetVertexes() const { return m_vertexes; }
	int32_t GetVertexCount() { return m_vertexCount; }

	Face* GetFaces() const { return m_faces; }
	int32_t GetFaceCount() { return m_faceCount; }

	int32_t GetModelCount() { return m_modelCount; }

	/**
		@brief	�f�X�g���N�^
	*/
	virtual ~Model()
	{
		ES_SAFE_DELETE_ARRAY( m_data );
	}

	Emitter GetEmitter( Manager* manager, eCoordinateSystem coordinate, float magnification )
	{
		RandFunc randFunc = manager->GetRandFunc();
		int32_t randMax = manager->GetRandMax();

		int32_t faceInd = (int32_t)( (GetFaceCount() - 1) * ( (float)randFunc() / (float)randMax ) );
		faceInd = Clamp( faceInd, GetFaceCount() - 1, 0 );
		Face& face = GetFaces()[faceInd];
		Vertex& v0 = GetVertexes()[face.Indexes[0]];
		Vertex& v1 = GetVertexes()[face.Indexes[1]];
		Vertex& v2 = GetVertexes()[face.Indexes[2]];

		float p1 = ( (float)randFunc() / (float)randMax );
		float p2 = ( (float)randFunc() / (float)randMax );

		/* �ʓ��Ɏ��߂� */
		if( p1 + p2 > 1.0f )
		{
			p1 = 1.0f - p1;
			p2 = 1.0f - p2;			
		}

		float p0 = 1.0f - p1 - p2;
		
		Emitter emitter;
		emitter.Position = (v0.Position * p0 + v1.Position * p1 + v2.Position * p2) * magnification;
		emitter.Normal = v0.Normal * p0 + v1.Normal * p1 + v2.Normal * p2;
		emitter.Binormal = v0.Binormal * p0 + v1.Binormal * p1 + v2.Binormal * p2;
		emitter.Tangent = v0.Tangent * p0 + v1.Tangent * p1 + v2.Tangent * p2;

		if( coordinate == COORDINATE_SYSTEM_LH )
		{
			emitter.Position.Z = - emitter.Position.Z;
			emitter.Normal.Z = - emitter.Normal.Z;
			emitter.Binormal.Z = - emitter.Binormal.Z;
			emitter.Tangent.Z = - emitter.Tangent.Z;
		}

		return emitter;
	}

	Emitter GetEmitterFromVertex( Manager* manager, eCoordinateSystem coordinate, float magnification )
	{
		RandFunc randFunc = manager->GetRandFunc();
		int32_t randMax = manager->GetRandMax();

		int32_t vertexInd = (int32_t)( (GetVertexCount() - 1) * ( (float)randFunc() / (float)randMax ) );
		vertexInd = Clamp( vertexInd, GetVertexCount() - 1, 0 );
		Vertex& v = GetVertexes()[vertexInd];
		
		Emitter emitter;
		emitter.Position = v.Position * magnification;
		emitter.Normal = v.Normal;
		emitter.Binormal = v.Binormal;
		emitter.Tangent = v.Tangent;

		if( coordinate == COORDINATE_SYSTEM_LH )
		{
			emitter.Position.Z = - emitter.Position.Z;
			emitter.Normal.Z = - emitter.Normal.Z;
			emitter.Binormal.Z = - emitter.Binormal.Z;
			emitter.Tangent.Z = - emitter.Tangent.Z;
		}

		return emitter;
	}

	Emitter GetEmitterFromVertex( int32_t index, eCoordinateSystem coordinate, float magnification )
	{
		int32_t vertexInd = index % GetVertexCount();
		Vertex& v = GetVertexes()[vertexInd];
		
		Emitter emitter;
		emitter.Position = v.Position * magnification;
		emitter.Normal = v.Normal;
		emitter.Binormal = v.Binormal;
		emitter.Tangent = v.Tangent;

		if( coordinate == COORDINATE_SYSTEM_LH )
		{
			emitter.Position.Z = - emitter.Position.Z;
			emitter.Normal.Z = - emitter.Normal.Z;
			emitter.Binormal.Z = - emitter.Binormal.Z;
			emitter.Tangent.Z = - emitter.Tangent.Z;
		}

		return emitter;
	}

	Emitter GetEmitterFromFace( Manager* manager, eCoordinateSystem coordinate, float magnification )
	{
		RandFunc randFunc = manager->GetRandFunc();
		int32_t randMax = manager->GetRandMax();

		int32_t faceInd = (int32_t)( (GetFaceCount() - 1) * ( (float)randFunc() / (float)randMax ) );
		faceInd = Clamp( faceInd, GetFaceCount() - 1, 0 );
		Face& face = GetFaces()[faceInd];
		Vertex& v0 = GetVertexes()[face.Indexes[0]];
		Vertex& v1 = GetVertexes()[face.Indexes[1]];
		Vertex& v2 = GetVertexes()[face.Indexes[2]];

		float p0 = 1.0f / 3.0f;
		float p1 = 1.0f / 3.0f;
		float p2 = 1.0f / 3.0f;

		Emitter emitter;
		emitter.Position = (v0.Position * p0 + v1.Position * p1 + v2.Position * p2) * magnification;
		emitter.Normal = v0.Normal * p0 + v1.Normal * p1 + v2.Normal * p2;
		emitter.Binormal = v0.Binormal * p0 + v1.Binormal * p1 + v2.Binormal * p2;
		emitter.Tangent = v0.Tangent * p0 + v1.Tangent * p1 + v2.Tangent * p2;

		if( coordinate == COORDINATE_SYSTEM_LH )
		{
			emitter.Position.Z = - emitter.Position.Z;
			emitter.Normal.Z = - emitter.Normal.Z;
			emitter.Binormal.Z = - emitter.Binormal.Z;
			emitter.Tangent.Z = - emitter.Tangent.Z;
		}

		return emitter;
	}

	Emitter GetEmitterFromFace( int32_t index, eCoordinateSystem coordinate, float magnification )
	{
		int32_t faceInd = index % (GetFaceCount() - 1);
		Face& face = GetFaces()[faceInd];
		Vertex& v0 = GetVertexes()[face.Indexes[0]];
		Vertex& v1 = GetVertexes()[face.Indexes[1]];
		Vertex& v2 = GetVertexes()[face.Indexes[2]];

		float p0 = 1.0f / 3.0f;
		float p1 = 1.0f / 3.0f;
		float p2 = 1.0f / 3.0f;

		Emitter emitter;
		emitter.Position = (v0.Position * p0 + v1.Position * p1 + v2.Position * p2) * magnification;
		emitter.Normal = v0.Normal * p0 + v1.Normal * p1 + v2.Normal * p2;
		emitter.Binormal = v0.Binormal * p0 + v1.Binormal * p1 + v2.Binormal * p2;
		emitter.Tangent = v0.Tangent * p0 + v1.Tangent * p1 + v2.Tangent * p2;

		if( coordinate == COORDINATE_SYSTEM_LH )
		{
			emitter.Position.Z = - emitter.Position.Z;
			emitter.Normal.Z = - emitter.Normal.Z;
			emitter.Binormal.Z = - emitter.Binormal.Z;
			emitter.Tangent.Z = - emitter.Tangent.Z;
		}

		return emitter;
	}
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
 } 
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif	// __EFFEKSEER_MODEL_H__
