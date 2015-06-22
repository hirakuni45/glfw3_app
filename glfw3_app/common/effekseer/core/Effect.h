
#ifndef	__EFFEKSEER_EFFECT_H__
#define	__EFFEKSEER_EFFECT_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include "Base.h"

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
namespace Effekseer
{
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------

/**
	@brief	�G�t�F�N�g�p�����[�^�[�N���X
	@note
	�G�t�F�N�g�ɐݒ肳�ꂽ�p�����[�^�[�B
*/
class Effect
{
protected:
	Effect() {}
    ~Effect() {}

public:

	/**
		@brief	�G�t�F�N�g�𐶐�����B
		@param	manager			[in]	�Ǘ��N���X
		@param	data			[in]	�f�[�^�z��̐擪�̃|�C���^
		@param	size			[in]	�f�[�^�z��̒���
		@param	magnification	[in]	�ǂݍ��ݎ��̊g�嗦
		@param	materialPath	[in]	�f�ރ��[�h���̊�p�X
		@return	�G�t�F�N�g�B���s�����ꍇ��NULL��Ԃ��B
	*/
	static Effect* Create( Manager* manager, void* data, int32_t size, float magnification = 1.0f, const EFK_CHAR* materialPath = NULL );

	/**
		@brief	�G�t�F�N�g�𐶐�����B
		@param	manager			[in]	�Ǘ��N���X
		@param	path			[in]	�Ǎ����̃p�X
		@param	magnification	[in]	�ǂݍ��ݎ��̊g�嗦
		@param	materialPath	[in]	�f�ރ��[�h���̊�p�X
		@return	�G�t�F�N�g�B���s�����ꍇ��NULL��Ԃ��B
	*/
	static Effect* Create( Manager* manager, const EFK_CHAR* path, float magnification = 1.0f, const EFK_CHAR* materialPath = NULL );

		/**
		@brief	�G�t�F�N�g�𐶐�����B
		@param	setting			[in]	�ݒ�N���X
		@param	data			[in]	�f�[�^�z��̐擪�̃|�C���^
		@param	size			[in]	�f�[�^�z��̒���
		@param	magnification	[in]	�ǂݍ��ݎ��̊g�嗦
		@param	materialPath	[in]	�f�ރ��[�h���̊�p�X
		@return	�G�t�F�N�g�B���s�����ꍇ��NULL��Ԃ��B
	*/
	static Effect* Create( Setting*	setting, void* data, int32_t size, float magnification = 1.0f, const EFK_CHAR* materialPath = NULL );

	/**
		@brief	�G�t�F�N�g�𐶐�����B
		@param	setting			[in]	�ݒ�N���X
		@param	path			[in]	�Ǎ����̃p�X
		@param	magnification	[in]	�ǂݍ��ݎ��̊g�嗦
		@param	materialPath	[in]	�f�ރ��[�h���̊�p�X
		@return	�G�t�F�N�g�B���s�����ꍇ��NULL��Ԃ��B
	*/
	static Effect* Create( Setting*	setting, const EFK_CHAR* path, float magnification = 1.0f, const EFK_CHAR* materialPath = NULL );

	/**
	@brief	�W���̃G�t�F�N�g�Ǎ��C���X�^���X�𐶐�����B
	*/
	static ::Effekseer::EffectLoader* CreateEffectLoader(::Effekseer::FileInterface* fileInterface = NULL);

	/**
		@brief	�Q�ƃJ�E���^�����Z����B
		@return	���s��̎Q�ƃJ�E���^�̒l
	*/
	virtual int AddRef() = 0;

	/**
		@brief	�Q�ƃJ�E���^�����Z����B
		@return	���s��̎Q�ƃJ�E���^�̒l
	*/
	virtual int Release() = 0;

	/**
	@brief	�ݒ���擾����B
	@return	�ݒ�
	*/
	virtual Setting* GetSetting() const = 0;

	/* �g�嗦���擾����B */
	virtual float GetMaginification() const = 0;
	
	/**
		@brief	�G�t�F�N�g�f�[�^�̃o�[�W�����擾
	*/
	virtual int GetVersion() const = 0;

	/**
		@brief	�i�[����Ă���摜�̃|�C���^���擾����B
		@param	n	[in]	�摜�̃C���f�b�N�X
		@return	�摜�̃|�C���^
	*/
	virtual void* GetImage( int n ) const = 0;

	/**
		@brief	�i�[����Ă��鉹�g�`�̃|�C���^���擾����B
	*/
	virtual void* GetWave( int n ) const = 0;

	/**
		@brief	�i�[����Ă��郂�f���̃|�C���^���擾����B
	*/
	virtual void* GetModel( int n ) const = 0;

	/**
		@brief	�G�t�F�N�g�̃����[�h���s���B
	*/
	virtual bool Reload( void* data, int32_t size, const EFK_CHAR* materialPath = NULL ) = 0;

	/**
		@brief	�G�t�F�N�g�̃����[�h���s���B
	*/
	virtual bool Reload( const EFK_CHAR* path, const EFK_CHAR* materialPath = NULL ) = 0;

	/**
		@brief	�G�t�F�N�g�̃����[�h���s���B
		@param	managers	[in]	�}�l�[�W���[�̔z��
		@param	managersCount	[in]	�}�l�[�W���[�̌�
		@param	data	[in]	�G�t�F�N�g�̃f�[�^
		@param	size	[in]	�G�t�F�N�g�̃f�[�^�T�C�Y
		@param	materialPath	[in]	���\�[�X�̓ǂݍ��݌�
		@return	����
		@note
		Setting��p���ăG�t�F�N�g�𐶐������Ƃ��ɁAManager���w�肷�邱�ƂőΏۂ�Manager���̃G�t�F�N�g�̃����[�h���s���B
	*/
	virtual bool Reload( Manager* managers, int32_t managersCount, void* data, int32_t size, const EFK_CHAR* materialPath = NULL ) = 0;

	/**
	@brief	�G�t�F�N�g�̃����[�h���s���B
	@param	managers	[in]	�}�l�[�W���[�̔z��
	@param	managersCount	[in]	�}�l�[�W���[�̌�
	@param	path	[in]	�G�t�F�N�g�̓ǂݍ��݌�
	@param	materialPath	[in]	���\�[�X�̓ǂݍ��݌�
	@return	����
	@note
	Setting��p���ăG�t�F�N�g�𐶐������Ƃ��ɁAManager���w�肷�邱�ƂőΏۂ�Manager���̃G�t�F�N�g�̃����[�h���s���B
	*/
	virtual bool Reload( Manager* managers, int32_t managersCount,const EFK_CHAR* path, const EFK_CHAR* materialPath = NULL ) = 0;

	/**
		@brief	�摜�����\�[�X�̍ēǂݍ��݂��s���B
	*/
	virtual void ReloadResources( const EFK_CHAR* materialPath = NULL ) = 0;

	/**
		@brief	�摜�����\�[�X�̔j�����s���B
	*/
	virtual void UnloadResources() = 0;
};
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
}
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif	// __EFFEKSEER_EFFECT_H__
