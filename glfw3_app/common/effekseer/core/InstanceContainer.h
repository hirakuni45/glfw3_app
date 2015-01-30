
#ifndef	__EFFEKSEER_INSTANCECONTAINER_H__
#define	__EFFEKSEER_INSTANCECONTAINER_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include "Effekseer.Base.h"

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
namespace Effekseer
{
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------

/**
	@brief	
	@note

*/
class InstanceContainer
{
	friend class ManagerImplemented;

private:

	// �}�l�[�W��
	Manager*	m_pManager;

	// �p�����[�^�[
	EffectNode* m_pEffectNode;

	// �O���[�o��
	InstanceGlobal*	m_pGlobal;

	// �q�̃R���e�i
	InstanceContainer**	m_Children;

	// �C���X�^���X�̎q�̐�
	int	m_ChildrenCount;

	// �O���[�v�̘A�����X�g�̐擪
	InstanceGroup*	m_headGroups;

	// �O���[�v�̘A�����X�g�̍Ō�
	InstanceGroup*	m_tailGroups;

	// placement new
	static void* operator new( size_t size, Manager* pManager );

	// placement delete
	static void operator delete( void* p, Manager* pManager );

	// default delete
	static void operator delete( void* p ){}

	// �R���X�g���N�^
	InstanceContainer( Manager* pManager, EffectNode* pEffectNode, InstanceGlobal* pGlobal, int ChildrenCount );

	// �f�X�g���N�^
	virtual ~InstanceContainer();

	// �w�肵���ԍ��ɃR���e�i��ݒ�
	void SetChild( int num, InstanceContainer* pContainter );

	// �����ȃO���[�v�̔j��
	void RemoveInvalidGroups();

public:
	// �w�肵���ԍ��̃R���e�i���擾
	InstanceContainer* GetChild( int num );

	/**
		@brief	�O���[�v�̍쐬
	*/
	InstanceGroup* CreateGroup();

	/**
		@brief	�O���[�v�̐擪�擾
	*/
	InstanceGroup* GetFirstGroup() const;

	void Update( bool recursive, float deltaFrame, bool shown );

	void SetBaseMatrix( bool recursive, const Matrix43& mat );

	void RemoveForcibly( bool recursive );

	void Draw( bool recursive );

	void KillAllInstances(  bool recursive );

	InstanceGlobal* GetRootInstance();
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
}
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif	// __EFFEKSEER_INSTANCECONTAINER_H__
