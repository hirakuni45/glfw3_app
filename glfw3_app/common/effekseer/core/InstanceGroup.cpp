

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#include "Effekseer.ManagerImplemented.h"

#include "Effekseer.InstanceGroup.h"
#include "Effekseer.Instance.h"
#include "Effekseer.InstanceContainer.h"
#include "Effekseer.InstanceGlobal.h"

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
namespace Effekseer
{
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
InstanceGroup::InstanceGroup( Manager* manager, EffectNode* effectNode, InstanceContainer* container, InstanceGlobal* global )
	: m_manager		( (ManagerImplemented*)manager )
	, m_effectNode	( effectNode )
	, m_container	( container )
	, m_global		( global )
	, m_time		( 0 )
	, IsReferencedFromInstance	( true )
	, NextUsedByInstance	( NULL )
	, NextUsedByContainer	( NULL )
{

}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
InstanceGroup::~InstanceGroup()
{
	RemoveForcibly();
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void InstanceGroup::RemoveInvalidInstances()
{
	std::list<Instance*>::iterator it = m_removingInstances.begin();

	while( it != m_removingInstances.end() )
	{
		if( (*it)->m_State == INSTANCE_STATE_ACTIVE )
		{
			assert(0);
		}
		else if( (*it)->m_State == INSTANCE_STATE_REMOVING )
		{
			// �폜������
			(*it)->m_State = INSTANCE_STATE_REMOVED;
			it++;
		}
		else if( (*it)->m_State == INSTANCE_STATE_REMOVED )
		{
			// �폜����
			if( (*it)->m_pEffectNode->GetType() == EFFECT_NODE_TYPE_ROOT )
			{
				delete (*it);
			}
			else
			{
				(*it)->~Instance();
				m_manager->PushInstance( (*it) );
			}

			it = m_removingInstances.erase( it );
			m_global->DecInstanceCount();
		}
	}
}
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
Instance* InstanceGroup::CreateInstance()
{
	Instance* instance = NULL;

	if( m_effectNode->GetType() == EFFECT_NODE_TYPE_ROOT )
	{
		instance = new Instance( m_manager, m_effectNode, m_container );
	}
	else
	{
		Instance* buf = m_manager->PopInstance();
		if( buf == NULL ) return NULL;
		instance = new(buf)Instance( m_manager, m_effectNode, m_container );
	}
	
	m_instances.push_back( instance );
	m_global->IncInstanceCount();
	return instance;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
Instance* InstanceGroup::GetFirst()
{
	if( m_instances.size() > 0 )
	{
		return m_instances.front();
	}
	return NULL;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
int InstanceGroup::GetInstanceCount() const
{
	return (int32_t)m_instances.size();
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
int InstanceGroup::GetRemovingInstanceCount() const
{
	return (int32_t)m_removingInstances.size();
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void InstanceGroup::Update( float deltaFrame, bool shown )
{
	RemoveInvalidInstances();

	std::list<Instance*>::iterator it = m_instances.begin();

	while( it != m_instances.end() )
	{
		if( (*it)->m_State == INSTANCE_STATE_ACTIVE )
		{
			// �X�V����
			(*it)->Update( deltaFrame, shown );

			// �j���`�F�b�N
			if( (*it)->m_State != INSTANCE_STATE_ACTIVE )
			{
				m_removingInstances.push_back( (*it) );
				it = m_instances.erase( it );
			}
			else
			{
				it++;
			}
		}
	}

	m_time++;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void InstanceGroup::SetBaseMatrix( const Matrix43& mat )
{
	std::list<Instance*>::iterator it = m_instances.begin();

	while( it != m_instances.end() )
	{
		if( (*it)->m_State == INSTANCE_STATE_ACTIVE )
		{
			Matrix43::Multiple( (*it)->m_GlobalMatrix43, (*it)->m_GlobalMatrix43, mat );
		}

		it++;
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void InstanceGroup::RemoveForcibly()
{
	KillAllInstances();

	RemoveInvalidInstances();
	RemoveInvalidInstances();
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void InstanceGroup::KillAllInstances()
{
	std::list<Instance*>::iterator it = m_instances.begin();
	std::list<Instance*>::iterator it_end = m_instances.end();

	while( it != it_end )
	{
		if( (*it)->GetState() == INSTANCE_STATE_ACTIVE )
		{
			(*it)->Kill();
			m_removingInstances.push_back( (*it) );
		}

		it++;
	}

	m_instances.clear();
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------