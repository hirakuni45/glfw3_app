
#ifndef	__EFFEKSEER_INSTANCE_H__
#define	__EFFEKSEER_INSTANCE_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include "Effekseer.Base.h"

#include "Effekseer.Vector3D.h"
#include "Effekseer.Matrix43.h"
#include "Effekseer.RectF.h"
#include "Effekseer.Color.h"

#include "Effekseer.EffectNodeSprite.h"
#include "Effekseer.EffectNodeRibbon.h"
#include "Effekseer.EffectNodeRing.h"
#include "Effekseer.EffectNodeModel.h"
#include "Effekseer.EffectNodeTrack.h"

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
namespace Effekseer
{
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------

/**
	@brief	�G�t�F�N�g�̎���
*/
class Instance
{
	friend class Manager;
	friend class InstanceContainer;
public:

	// �}�l�[�W��
	Manager*	m_pManager;

	// �p�����[�^�[
	EffectNode* m_pEffectNode;

	// �R���e�i
	InstanceContainer*	m_pContainer;

	// �O���[�v�̘A�����X�g�̐擪
	InstanceGroup*	m_headGroups;

	// �e
	Instance*	m_pParent;

	// �ʒu
	Vector3D	m_LocalPosition;

	union 
	{
		struct
		{
		
		} fixed;

		struct
		{
			vector3d location;
			vector3d velocity;
			vector3d acceleration;
		} random;

		struct
		{
			vector3d	start;
			vector3d	end;
		} easing;

		struct
		{
			vector3d	offset;
		} fcruve;

	} translation_values;	

	// �␳�ʒu
	vector3d	m_globalRevisionLocation;

	union 
	{
		struct
		{
		
		} none;

		struct
		{
			vector3d	velocity;
		} gravity;

	} translation_abs_values;

	// ��]
	Vector3D	m_LocalAngle;

	union 
	{
		struct
		{
		
		} fixed;

		struct
		{
			vector3d rotation;
			vector3d velocity;
			vector3d acceleration;
		} random;

		struct
		{
			vector3d start;
			vector3d end;
		} easing;
		
		struct
		{
			float rotation;

			union
			{
				struct
				{
					float rotation;
					float velocity;
					float acceleration;
				} random;

				struct
				{
					float start;
					float end;
				} easing;
			};
		} axis;

		struct
		{
			vector3d offset;
		} fcruve;

	} rotation_values;

	// �g��k��
	Vector3D	m_LocalScaling;

	union 
	{
		struct
		{
		
		} fixed;

		struct
		{
			vector3d  scale;
			vector3d  velocity;
			vector3d  acceleration;
		} random;

		struct
		{
			vector3d  start;
			vector3d  end;
		} easing;
		
		struct
		{
			float  scale;
			float  velocity;
			float  acceleration;
		} single_random;

		struct
		{
			float  start;
			float  end;
		} single_easing;

		struct
		{
			vector3d offset;
		} fcruve;

	} scaling_values;

	// �`��
	union
	{
		EffectNodeSprite::InstanceValues	sprite;
		EffectNodeRibbon::InstanceValues	ribbon;
		EffectNodeRing::InstanceValues		ring;
		EffectNodeModel::InstanceValues		model;
		EffectNodeTrack::InstanceValues		track;
	} rendererValues;
	
	// ��
	union
	{
		int		delay;
	} soundValues;

	// ���
	eInstanceState	m_State;

	// ��������
	float		m_LivedTime;

	// ��������Ă���̎���
	float		m_LivingTime;

	// �����ʒu
	Matrix43		m_generation_location;

	// �ϊ��p�s��
	Matrix43		m_GlobalMatrix43;

	// �e�̕ϊ��p�s��
	Matrix43		m_ParentMatrix43;

	/* ���Ԃ�i�߂邩�ǂ���? */
	bool			m_stepTime;

	/* �X�V�ԍ� */
	uint32_t		m_sequenceNumber;

	// �R���X�g���N�^
	Instance( Manager* pManager, EffectNode* pEffectNode, InstanceContainer* pContainer );

	// �f�X�g���N�^
	virtual ~Instance();
public:

	/**
		@brief	��Ԃ̎擾
	*/
	eInstanceState GetState() const;

	/**
		@brief	�s��̎擾
	*/
	const Matrix43& GetGlobalMatrix43() const;

	/**
		@brief	������
	*/
	void Initialize( Instance* parent, int32_t instanceNumber );

	/**
		@brief	�X�V
	*/
	void Update( float deltaFrame, bool shown );

	/**
		@brief	�s��̍X�V
	*/
	void CalculateMatrix( float deltaFrame );

	/**
		@brief	�`��
	*/
	void Draw();

	/**
		@brief	�j��
	*/
	void Kill();

	/**
		@brief	UV�̈ʒu�擾
	*/
	RectF GetUV() const;
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
}
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif	// __EFFEKSEER_INSTANCE_H__
