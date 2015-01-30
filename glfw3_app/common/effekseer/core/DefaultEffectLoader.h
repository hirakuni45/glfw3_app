
#ifndef	__EFFEKSEER_DEFAULTEFFECTLOADER_H__
#define	__EFFEKSEER_DEFAULTEFFECTLOADER_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include "Effekseer.Base.h"
#include "Effekseer.EffectLoader.h"
#include "Effekseer.DefaultFile.h"

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
namespace Effekseer { 
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
/**
	@brief	�W���̃G�t�F�N�g�t�@�C���ǂݍ��ݔj���֐��w��N���X
*/
class DefaultEffectLoader
	: public EffectLoader
{
	DefaultFileInterface m_defaultFileInterface;
	FileInterface* m_fileInterface;
public:

	DefaultEffectLoader( FileInterface* fileInterface = NULL );

	virtual ~DefaultEffectLoader();

	bool Load( const EFK_CHAR* path, void*& data, int32_t& size );

	void Unload( void* data, int32_t size );
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
 } 
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif	// __EFFEKSEER_DEFAULTEFFECTLOADER_H__
