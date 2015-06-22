
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#include "Effekseer.Thread.h"

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
namespace Effekseer { 

#ifdef _WIN32

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
DWORD EFK_STDCALL Thread::ThreadProc( void* arguments )
{
	Thread* thread = (Thread*)(arguments);

	thread->m_mainProc( thread->m_data );

	thread->m_cs.Enter();

	thread->m_mainProc	= NULL;
	thread->m_data		= NULL;

	::CloseHandle( thread->m_thread );
	thread->m_thread		= NULL;

	thread->m_cs.Leave();

	return 0;
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
Thread::Thread()
	: m_data		( NULL )
	, m_thread		( NULL )
	, m_mainProc	( NULL )
{
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
Thread::~Thread()
{
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
bool Thread::Create( void (*threadFunc)( void* ), void* data )
{
	m_cs.Enter();

	if ( m_thread == NULL )
	{
		m_data		= data;
		m_mainProc	= threadFunc;
		m_thread	= ::CreateThread( NULL, 0, ThreadProc, this, CREATE_SUSPENDED, NULL );

		// �X���b�h�J�n
		::SetThreadPriority( m_thread, THREAD_PRIORITY_NORMAL );
		::ResumeThread( m_thread );

		m_cs.Leave();
		return true;
	}

	m_cs.Leave();
	return false;
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
bool Thread::IsExitThread() const
{
	m_cs.Enter();
	bool ret = m_thread == NULL;
	m_cs.Leave();
	return ret;
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
bool Thread::Wait() const
{
	::WaitForSingleObject( m_thread, INFINITE );

	return true;
}

#else

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
void* Thread::ThreadProc( void* arguments )
{
	Thread* thread = (Thread*)(arguments);

	thread->m_mainProc( thread->m_data );

	thread->m_cs.Enter();

	thread->m_mainProc	= NULL;
	thread->m_data		= NULL;

	pthread_detach( thread->m_thread );

	thread->m_running	= false;

	thread->m_cs.Leave();

	return 0;
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
Thread::Thread()
	: m_running		( false )
	, m_thread		()
	, m_data		( NULL )
	, m_mainProc	( NULL )
{
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
Thread::~Thread()
{
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
bool Thread::Create( void (*threadFunc)( void* ), void* data )
{
	m_cs.Enter();

	if ( !m_running )
	{
		m_data		= data;
		m_mainProc	= threadFunc;

		pthread_attr_t attr;
		pthread_attr_init( &attr );

		// �X���b�h�J�n
		m_running = true;
		pthread_create( &m_thread, &attr, ThreadProc, this );
		
		m_cs.Leave();
		return true;
	}

	m_cs.Leave();
	return false;
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
bool Thread::IsExitThread() const
{
	m_cs.Enter();
	bool ret = !m_running;
	m_cs.Leave();
	return ret;
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
bool Thread::Wait() const
{
	pthread_join( m_thread, NULL );

	return true;
}
#endif

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
}
