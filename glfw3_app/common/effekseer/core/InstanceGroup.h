
#ifndef	__EFFEKSEER_INSTANCEGROUP_H__
#define	__EFFEKSEER_INSTANCEGROUP_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include "Effekseer.Base.h"
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
	@brief	�C���X�^���X�O���[�v
	@note
	�C���X�^���X�R���e�i���ł���ɃC���X�^���X���O���[�v������N���X
*/
class InstanceGroup
{
friend class InstanceContainer;

private:
	ManagerImplemented*		m_manager;
	EffectNode*			m_effectNode;
	InstanceContainer*	m_container;
	InstanceGlobal*		m_global;
	int32_t				m_time;

	// �C���X�^���X�̎���
	std::list<Instance*> m_instances;
	std::list<Instance*> m_removingInstances;

	InstanceGroup( Manager* manager, EffectNode* effectNode, InstanceContainer* container, InstanceGlobal* global );

	~InstanceGroup();

	void RemoveInvalidInstances();

public:

	/** 
		@brief	�`��ɕK�v�ȃp�����[�^
	*/
	union
	{
		EffectNodeTrack::InstanceGroupValues		track;
	} rendererValues;

	/**
		@brief	�C���X�^���X�̐���
	*/
	Instance* CreateInstance();

	Instance* GetFirst();

	int GetInstanceCount() const;

	int GetRemovingInstanceCount() const;

	void Update( float deltaFrame, bool shown );

	void SetBaseMatrix( const Matrix43& mat );

	void RemoveForcibly();

	void KillAllInstances();

	int32_t GetTime() const { return m_time; }

	/**
		@brief	�O���[�v�𐶐������C���X�^���X����̎Q�Ƃ��c���Ă��邩?
	*/
	bool IsReferencedFromInstance;

	/**
		@brief	�C���X�^���X���痘�p����A�����X�g�̎��̃I�u�W�F�N�g�ւ̃|�C���^
	*/
	InstanceGroup*	NextUsedByInstance;

	/**
		@brief	�R���e�i���痘�p����A�����X�g�̎��̃I�u�W�F�N�g�ւ̃|�C���^
	*/
	InstanceGroup*	NextUsedByContainer;

};
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
}
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif	// __EFFEKSEER_INSTANCEGROUP_H__
