
#ifndef	__EFFEKSEER_MANAGER_H__
#define	__EFFEKSEER_MANAGER_H__

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
	@brief �G�t�F�N�g�Ǘ��N���X
*/
class Manager
{
protected:
	Manager() {}
    ~Manager() {}

public:
	/**
		@brief �}�l�[�W���[�𐶐�����B
		@param	instance_max	[in]	�ő�C���X�^���X��
		@param	autoFlip		[in]	�����ŃX���b�h�Ԃ̃f�[�^�����ւ��邩�ǂ����A���w�肷��Btrue�̏ꍇ�AUpdate���ɓ���ւ��B
		@return	�}�l�[�W���[
	*/
	static Manager* Create( int instance_max, bool autoFlip = true );

	/**
		@brief �}�l�[�W���[��j������B
		@note
		���̃}�l�[�W���[���琶�����ꂽ�G�t�F�N�g�͑S�ċ����I�ɔj�������B
	*/
	virtual void Destroy() = 0;

	/**
		@brief	�������m�ۊ֐����擾����B
	*/
	virtual MallocFunc GetMallocFunc() const = 0;

	/**
		@brief	�������m�ۊ֐���ݒ肷��B
	*/
	virtual void SetMallocFunc( MallocFunc func ) = 0;

	/**
		@brief	�������j���֐����擾����B
	*/
	virtual FreeFunc GetFreeFunc() const = 0;

	/**
		@brief	�������j���֐���ݒ肷��B
	*/
	virtual void SetFreeFunc( FreeFunc func ) = 0;

	/**
		@brief	�����_���֐����擾����B
	*/
	virtual RandFunc GetRandFunc() const = 0;

	/**
		@brief	�����_���֐���ݒ肷��B
	*/
	virtual void SetRandFunc( RandFunc func ) = 0;

	/**
		@brief	�����_���ő�l���擾����B
	*/
	virtual int GetRandMax() const = 0;

	/**
		@brief	�����_���֐���ݒ肷��B
	*/
	virtual void SetRandMax( int max_ ) = 0;

	/**
		@brief	���W�n���擾����B
		@return	���W�n
	*/
	virtual eCoordinateSystem GetCoordinateSystem() const = 0;

	/**
		@brief	���W�n��ݒ肷��B
		@param	coordinateSystem	[in]	���W�n
		@note
		���W�n��ݒ肷��B
		�G�t�F�N�g�t�@�C����ǂݍ��ޑO�ɐݒ肷��K�v������B
	*/
	virtual void SetCoordinateSystem( eCoordinateSystem coordinateSystem ) = 0;

	/**
		@brief	�X�v���C�g�`��@�\���擾����B
	*/
	virtual SpriteRenderer* GetSpriteRenderer() = 0;

	/**
		@brief	�X�v���C�g�`��@�\��ݒ肷��B
	*/
	virtual void SetSpriteRenderer( SpriteRenderer* renderer ) = 0;

	/**
		@brief	�X�g���C�v�`��@�\���擾����B
	*/
	virtual RibbonRenderer* GetRibbonRenderer() = 0;

	/**
		@brief	�X�g���C�v�`��@�\��ݒ肷��B
	*/
	virtual void SetRibbonRenderer( RibbonRenderer* renderer ) = 0;

	/**
		@brief	�����O�`��@�\���擾����B
	*/
	virtual RingRenderer* GetRingRenderer() = 0;

	/**
		@brief	�����O�`��@�\��ݒ肷��B
	*/
	virtual void SetRingRenderer( RingRenderer* renderer ) = 0;

	/**
		@brief	���f���`��@�\���擾����B
	*/
	virtual ModelRenderer* GetModelRenderer() = 0;

	/**
		@brief	���f���`��@�\��ݒ肷��B
	*/
	virtual void SetModelRenderer( ModelRenderer* renderer ) = 0;

	/**
		@brief	�O�Օ`��@�\���擾����B
	*/
	virtual TrackRenderer* GetTrackRenderer() = 0;

	/**
		@brief	�O�Օ`��@�\��ݒ肷��B
	*/
	virtual void SetTrackRenderer( TrackRenderer* renderer ) = 0;

	/**
		@brief	�ݒ�N���X���擾����B
	*/
	virtual Setting* GetSetting() = 0;

	/**
		@brief	�ݒ�N���X��ݒ肷��B
		@param	setting	[in]	�ݒ�
	*/
	virtual void SetSetting(Setting* setting) = 0;

	/**
		@brief	�G�t�F�N�g�Ǎ��N���X���擾����B
	*/
	virtual EffectLoader* GetEffectLoader() = 0;

	/**
		@brief	�G�t�F�N�g�Ǎ��N���X��ݒ肷��B
	*/
	virtual void SetEffectLoader( EffectLoader* effectLoader ) = 0;

	/**
		@brief	�e�N�X�`���Ǎ��N���X���擾����B
	*/
	virtual TextureLoader* GetTextureLoader() = 0;

	/**
		@brief	�e�N�X�`���Ǎ��N���X��ݒ肷��B
	*/
	virtual void SetTextureLoader( TextureLoader* textureLoader ) = 0;
	
	/**
		@brief	�T�E���h�Đ��@�\���擾����B
	*/
	virtual SoundPlayer* GetSoundPlayer() = 0;

	/**
		@brief	�T�E���h�Đ��@�\��ݒ肷��B
	*/
	virtual void SetSoundPlayer( SoundPlayer* soundPlayer ) = 0;
	
	/**
		@brief	�T�E���h�Ǎ��N���X���擾����
	*/
	virtual SoundLoader* GetSoundLoader() = 0;
	
	/**
		@brief	�T�E���h�Ǎ��N���X��ݒ肷��B
	*/
	virtual void SetSoundLoader( SoundLoader* soundLoader ) = 0;

	/**
		@brief	���f���Ǎ��N���X���擾����B
	*/
	virtual ModelLoader* GetModelLoader() = 0;

	/**
		@brief	���f���Ǎ��N���X��ݒ肷��B
	*/
	virtual void SetModelLoader( ModelLoader* modelLoader ) = 0;

	/**
		@brief	�G�t�F�N�g���~����B
		@param	handle	[in]	�C���X�^���X�̃n���h��
	*/
	virtual void StopEffect( Handle handle ) = 0;

	/**
		@brief	�S�ẴG�t�F�N�g���~����B
	*/
	virtual void StopAllEffects() = 0;

	/**
		@brief	�G�t�F�N�g�̃��[�g�������~����B
		@param	handle	[in]	�C���X�^���X�̃n���h��
	*/
	virtual void StopRoot( Handle handle ) = 0;

	/**
		@brief	�G�t�F�N�g�̃��[�g�������~����B
		@param	effect	[in]	�G�t�F�N�g
	*/
	virtual void StopRoot( Effect* effect ) = 0;

	/**
		@brief	�G�t�F�N�g�̃C���X�^���X�����݂��Ă��邩�擾����B
		@param	handle	[in]	�C���X�^���X�̃n���h��
		@return	���݂��Ă邩?
	*/
	virtual bool Exists( Handle handle ) = 0;

	/**
		@brief	�G�t�F�N�g�Ɏg�p����Ă���C���X�^���X�����擾����B
		@param	handle	[in]	�C���X�^���X�̃n���h��
		@return	�C���X�^���X��
		@note
		Root�����Ɋ܂܂��B�܂�ARoot�폜�����Ă��Ȃ�����A
		Manager�Ɏc���Ă���C���X�^���X��+�G�t�F�N�g�Ɏg�p����Ă���C���X�^���X���͑��݂��Ă���Root�̐�����
		�ŏ��Ɋm�ۂ����������������݂���B
	*/
	virtual int32_t GetInstanceCount( Handle handle ) = 0;

	/**
		@brief	�G�t�F�N�g�̃C���X�^���X�ɐݒ肳��Ă���s����擾����B
		@param	handle	[in]	�C���X�^���X�̃n���h��
		@return	�s��
	*/
	virtual Matrix43 GetMatrix( Handle handle ) = 0;

	/**
		@brief	�G�t�F�N�g�̃C���X�^���X�ɕϊ��s���ݒ肷��B
		@param	handle	[in]	�C���X�^���X�̃n���h��
		@param	mat		[in]	�ϊ��s��
	*/
	virtual void SetMatrix( Handle handle, const Matrix43& mat ) = 0;

	/**
		@brief	�G�t�F�N�g�̃C���X�^���X�̈ʒu���擾����B
		@param	handle	[in]	�C���X�^���X�̃n���h��
		@return	�ʒu
	*/
	virtual Vector3D GetLocation( Handle handle ) = 0;

	/**
		@brief	�G�t�F�N�g�̃C���X�^���X�̈ʒu���w�肷��B
		@param	x	[in]	X���W
		@param	y	[in]	Y���W
		@param	z	[in]	Z���W
	*/
	virtual void SetLocation( Handle handle, float x, float y, float z ) = 0;

	/**
		@brief	�G�t�F�N�g�̃C���X�^���X�̈ʒu���w�肷��B
		@param	location	[in]	�ʒu
	*/
	virtual void SetLocation( Handle handle, const Vector3D& location ) = 0;

	/**
		@brief	�G�t�F�N�g�̃C���X�^���X�̈ʒu�ɉ��Z����B
		@param	location	[in]	���Z����l
	*/
	virtual void AddLocation( Handle handle, const Vector3D& location ) = 0;

	/**
		@brief	�G�t�F�N�g�̃C���X�^���X�̉�]�p�x���w�肷��B(���W�A��)
	*/
	virtual void SetRotation( Handle handle, float x, float y, float z ) = 0;

	/**
		@brief	�G�t�F�N�g�̃C���X�^���X�̔C�ӎ�����̔����v����̉�]�p�x���w�肷��B
		@param	handle	[in]	�C���X�^���X�̃n���h��
		@param	axis	[in]	��
		@param	angle	[in]	�p�x(���W�A��)
	*/
	virtual void SetRotation( Handle handle, const Vector3D& axis, float angle ) = 0;

	/**
		@brief	�G�t�F�N�g�̃C���X�^���X�̊g�嗦���w�肷��B
		@param	handle	[in]	�C���X�^���X�̃n���h��
		@param	x		[in]	X�����g�嗦
		@param	y		[in]	Y�����g�嗦
		@param	z		[in]	Z�����g�嗦
	*/
	virtual void SetScale( Handle handle, float x, float y, float z ) = 0;

	/**
		@brief	�G�t�F�N�g�̃x�[�X�s����擾����B
		@param	handle	[in]	�C���X�^���X�̃n���h��
		@return	�x�[�X�s��
	*/
	virtual Matrix43 GetBaseMatrix( Handle handle ) = 0;

	/**
		@brief	�G�t�F�N�g�̃x�[�X�s���ݒ肷��B
		@param	handle	[in]	�C���X�^���X�̃n���h��
		@param	mat		[in]	�ݒ肷��s��
		@note
		�G�t�F�N�g�S�̂̕\���ʒu���w�肷��s���ݒ肷��B
	*/
	virtual void SetBaseMatrix( Handle handle, const Matrix43& mat ) = 0;

	/**
		@brief	�G�t�F�N�g�̃C���X�^���X�ɔp�����̃R�[���o�b�N��ݒ肷��B
		@param	handle	[in]	�C���X�^���X�̃n���h��
		@param	callback	[in]	�R�[���o�b�N
	*/
	virtual void SetRemovingCallback( Handle handle, EffectInstanceRemovingCallback callback ) = 0;

	/**
		@brief	�G�t�F�N�g�̃C���X�^���X��Draw���ɕ`�悷�邩�ݒ肷��B
		@param	handle	[in]	�C���X�^���X�̃n���h��
		@param	shown	[in]	�`�悷�邩?
	*/
	virtual void SetShown( Handle handle, bool shown ) = 0;

	/**
		@brief	�G�t�F�N�g�̃C���X�^���X��Update���ɍX�V���邩�ݒ肷��B
		@param	handle	[in]	�C���X�^���X�̃n���h��
		@param	paused	[in]	�X�V���邩?
	*/
	virtual void SetPaused( Handle handle, bool paused ) = 0;

	/**
		@brief	�G�t�F�N�g�̃C���X�^���X���Đ��X�s�[�h��ݒ肷��B
		@param	handle	[in]	�C���X�^���X�̃n���h��
		@param	speed	[in]	�X�s�[�h
	*/
	virtual void SetSpeed( Handle handle, float speed ) = 0;

	/**
		@brief	�G�t�F�N�g��Draw�ŕ`�悳��邩�ݒ肷��B
				autoDraw��false�̏ꍇ�ADrawHandle�ŕ`�悷��K�v������B
		@param	autoDraw	[in]	�����`��t���O
	*/
	virtual void SetAutoDrawing( Handle handle, bool autoDraw ) = 0;

	/**
		@brief	���܂ł�Play���̏�����Update���s���ɓK�p����悤�ɂ���B
	*/
	virtual void Flip() = 0;

	/**
		@brief	�X�V�������s���B
		@param	deltaFrame	[in]	�X�V����t���[����(60fps�)	
	*/
	virtual void Update( float deltaFrame = 1.0f ) = 0;

	/**
		@brief	�X�V�������J�n����B
		@note
		Update�����s����ۂ́A���s����K�v�͂Ȃ��B
	*/
	virtual void BeginUpdate() = 0;

	/**
		@brief	�X�V�������I������B
		@note
		Update�����s����ۂ́A���s����K�v�͂Ȃ��B
	*/
	virtual void EndUpdate() = 0;

	/**
		@brief	�n���h���P�ʂ̍X�V���s���B
		@param	handle		[in]	�n���h��
		@param	deltaFrame	[in]	�X�V����t���[����(60fps�)
		@note
		�X�V����O��BeginUpdate�A�X�V���I��������EndUpdate�����s����K�v������B
	*/
	virtual void UpdateHandle( Handle handle, float deltaFrame = 1.0f ) = 0;

	/**
		@brief	�`�揈�����s���B
	*/
	virtual void Draw() = 0;
	
	/**
		@brief	�n���h���P�ʂ̕`�揈�����s���B
	*/
	virtual void DrawHandle( Handle handle ) = 0;

	/**
		@brief	�Đ�����B
		@param	effect	[in]	�G�t�F�N�g
		@param	x	[in]	X���W
		@param	y	[in]	Y���W
		@param	z	[in]	Z���W
		@return	�G�t�F�N�g�̃C���X�^���X�̃n���h��
	*/
	virtual Handle Play( Effect* effect, float x, float y, float z ) = 0;
	
	/**
		@brief	Update�������Ԃ��擾�B
	*/
	virtual int GetUpdateTime() const = 0;
	
	/**
		@brief	Draw�������Ԃ��擾�B
	*/
	virtual int GetDrawTime() const = 0;

	/**
		@brief	�c��̊m�ۂ����C���X�^���X�����擾����B
	*/
	virtual int32_t GetRestInstancesCount() const = 0;
};
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
}
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif	// __EFFEKSEER_MANAGER_H__
