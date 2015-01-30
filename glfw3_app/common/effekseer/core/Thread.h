
#ifndef	__EFFEKSEER_THREAD_H__
#define	__EFFEKSEER_THREAD_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include "Effekseer.Base.h"
#include "Effekseer.CriticalSection.h"

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
namespace Effekseer { 
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
	
class Thread
{
private:
#ifdef _WIN32
	static DWORD EFK_STDCALL ThreadProc( void* arguments );
#else
	static void* ThreadProc( void* arguments );
#endif

private:
#ifdef _WIN32
	HANDLE m_thread;
#else
	pthread_t m_thread;
	bool m_running;
#endif

	void* m_data;
	void (*m_mainProc)( void* );
	CriticalSection m_cs;

public:

	Thread();
	~Thread();


	/**
		@brief �X���b�h�𐶐�����B
		@param threadFunc	[in] �X���b�h�֐�
		@param pData		[in] �X���b�h�Ɉ����n���f�[�^�|�C���^
		@return	����
	*/
	bool Create( void (*threadFunc)( void* ), void* data );

	/**
		@brief �X���b�h�I�����m�F����B
	*/
	bool IsExitThread() const;

	/**
		@brief �X���b�h�I����҂B
	*/
	bool Wait() const;
};
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
 } 
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif	// __EFFEKSEER_VECTOR3D_H__
