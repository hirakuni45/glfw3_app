
#pragma once

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include "RendererImplemented.h"
#include "effekseer/common/VertexBufferBase.h"
#include "DeviceObject.h"

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
namespace EffekseerRendererGL
{
//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
class VertexBuffer
	: public DeviceObject
	, public ::EffekseerRenderer::VertexBufferBase
{
private:
	GLuint					m_buffer;

	uint32_t				m_vertexRingStart;
	uint32_t				m_vertexRingOffset;
	bool					m_ringBufferLock;

	VertexBuffer( RendererImplemented* renderer, int size, bool isDynamic );
public:
	virtual ~VertexBuffer();

	static VertexBuffer* Create( RendererImplemented* renderer, int size, bool isDynamic );

	GLuint GetInterface() { return m_buffer; }

public:	// �f�o�C�X�����p
	virtual void OnLostDevice();
	virtual void OnResetDevice();

public:
	void Lock();
	bool RingBufferLock( int32_t size, int32_t& offset, void*& data );
	void Unlock();
};

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
}
//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
