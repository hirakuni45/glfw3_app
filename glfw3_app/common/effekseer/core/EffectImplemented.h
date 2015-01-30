
#ifndef	__EFFEKSEER_EFFECT_IMPLEMENTED_H__
#define	__EFFEKSEER_EFFECT_IMPLEMENTED_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include "Base.h"
#include "Effect.h"

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
namespace Effekseer
{
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------

/**
	@brief	�G�t�F�N�g�p�����[�^�[
	@note
	�G�t�F�N�g�ɐݒ肳�ꂽ�p�����[�^�[�B
*/

class EffectImplemented
	: public Effect
{
	friend class ManagerImplemented;
private:
	ManagerImplemented* m_pManager;

	Setting*	m_setting;

	int	m_reference;

	int	m_version;

	int	m_ImageCount;
	EFK_CHAR**		m_ImagePaths;
	void**			m_pImages;
	
	int	m_WaveCount;
	EFK_CHAR**		m_WavePaths;
	void**			m_pWaves;

	int32_t	m_modelCount;
	EFK_CHAR**		m_modelPaths;
	void**			m_pModels;

	std::basic_string<EFK_CHAR>		m_materialPath;

	/* �g�嗦 */
	float	m_maginification;

	float	m_maginificationExternal;

	// �q�m�[�h
	EffectNode* m_pRoot;

public:
	/**
		@brief	����
	*/
	static Effect* Create( Manager* pManager, void* pData, int size, float magnification, const EFK_CHAR* materialPath = NULL );

	/**
		@brief	����
	*/
	static Effect* Create( Setting* setting, void* pData, int size, float magnification, const EFK_CHAR* materialPath = NULL );

	// �R���X�g���N�^
	EffectImplemented( Manager* pManager, void* pData, int size );

	// �R���X�g���N�^
	EffectImplemented( Setting* setting, void* pData, int size );

	// �f�X�g���N�^
	virtual ~EffectImplemented();

	// Root�̎擾
	EffectNode* GetRoot() const;

	/* �g�嗦�̎擾 */
	float GetMaginification() const;

	/**
		@brief	�ǂݍ��ށB
	*/
	void Load( void* pData, int size, float mag, const EFK_CHAR* materialPath );

	/**
		@breif	�����ǂݍ��܂�Ă��Ȃ���Ԃɖ߂�
	*/
	void Reset();

	/**
		@brief	�Q�ƃJ�E���^���Z
	*/
	int AddRef();

	/**
		@brief	�Q�ƃJ�E���^���Z
	*/
	int Release();

private:
	/**
		@brief	�}�l�[�W���[�擾
	*/
	Manager* GetManager() const;

public:
	/**
	@brief	�ݒ�擾
	*/
	Setting* GetSetting() const;
	
	/**
		@brief	�G�t�F�N�g�f�[�^�̃o�[�W�����擾
	*/
	int GetVersion() const;

	/**
		@brief	�i�[����Ă���摜�̃|�C���^���擾����B
	*/
	void* GetImage( int n ) const;
	
	/**
		@brief	�i�[����Ă��鉹�g�`�̃|�C���^���擾����B
	*/
	void* GetWave( int n ) const;

	/**
		@brief	�i�[����Ă��郂�f���̃|�C���^���擾����B
	*/
	void* GetModel( int n ) const;

	/**
		@brief	�G�t�F�N�g�̃����[�h���s���B
	*/
	bool Reload( void* data, int32_t size, const EFK_CHAR* materialPath = NULL );

	/**
		@brief	�G�t�F�N�g�̃����[�h���s���B
	*/
	bool Reload( const EFK_CHAR* path, const EFK_CHAR* materialPath = NULL );

	/**
		@brief	�G�t�F�N�g�̃����[�h���s���B
	*/
	bool Reload( Manager* managers, int32_t managersCount, void* data, int32_t size, const EFK_CHAR* materialPath = NULL );

	/**
		@brief	�G�t�F�N�g�̃����[�h���s���B
	*/
	bool Reload( Manager* managers, int32_t managersCount, const EFK_CHAR* path, const EFK_CHAR* materialPath = NULL );

	/**
		@brief	�摜�����\�[�X�̍ēǂݍ��݂��s���B
	*/
	void ReloadResources( const EFK_CHAR* materialPath );

	/**
		@brief	�摜�����\�[�X�̔j�����s���B
	*/
	void UnloadResources();
};
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
}
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif	// __EFFEKSEER_EFFECT_IMPLEMENTED_H__
