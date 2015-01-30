

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#include "Effekseer.Effect.h"
#include "Effekseer.EffectImplemented.h"

#include "Effekseer.EffectNode.h"
#include "Effekseer.Instance.h"
#include "Effekseer.InstanceContainer.h"
#include "Effekseer.InstanceGlobal.h"
#include "Effekseer.InstanceGroup.h"
#include "Effekseer.Manager.h"
#include "Effekseer.ManagerImplemented.h"

#include "Effekseer.DefaultEffectLoader.h"
#include "Effekseer.TextureLoader.h"

#include "Effekseer.Setting.h"

#include "Renderer/Effekseer.SpriteRenderer.h"
#include "Renderer/Effekseer.RibbonRenderer.h"
#include "Renderer/Effekseer.RingRenderer.h"
#include "Renderer/Effekseer.ModelRenderer.h"
#include "Renderer/Effekseer.TrackRenderer.h"

#include "Effekseer.SoundLoader.h"
#include "Sound/Effekseer.SoundPlayer.h"

#include "Effekseer.ModelLoader.h"

#ifdef _WIN32
#include <windows.h>
#else
#include <sys/time.h>
#endif

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
namespace Effekseer
{

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
static int64_t GetTime(void)
{
#ifdef _WIN32
	int64_t count, freq;
	if (QueryPerformanceCounter((LARGE_INTEGER*)&count))
	{
		if (QueryPerformanceFrequency((LARGE_INTEGER*)&freq))
		{
			return count * 1000000 / freq;
		}
	}
	return 0;
#else
	struct timeval tv;
    gettimeofday(&tv, NULL);
    return (int64_t)tv.tv_sec * 1000000 + (int64_t)tv.tv_usec;
#endif
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
Manager* Manager::Create( int instance_max, bool autoFlip )
{
	return new ManagerImplemented( instance_max, autoFlip );
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
Handle ManagerImplemented::AddDrawSet( Effect* effect, InstanceContainer* pInstanceContainer, InstanceGlobal* pGlobalPointer )
{
	Handle Temp = m_NextHandle;

	if( ++m_NextHandle < 0 )
	{
		// �I�[�o�[�t���[�����烊�Z�b�g����
		m_NextHandle = 0;
	}

	DrawSet drawset( effect, pInstanceContainer, pGlobalPointer );

	ES_SAFE_ADDREF( effect );

	m_DrawSets[Temp] = drawset;

	return Temp;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void ManagerImplemented::GCDrawSet( bool isRemovingManager )
{
	// �C���X�^���X�O���[�v���̂̍폜����
	{
		std::map<Handle,DrawSet>::iterator it = m_RemovingDrawSets[1].begin();
		while( it != m_RemovingDrawSets[1].end() )
		{
			DrawSet& drawset = (*it).second;

			// �S�j������
			drawset.InstanceContainerPointer->RemoveForcibly( true );
			drawset.InstanceContainerPointer->~InstanceContainer();
			InstanceContainer::operator delete( drawset.InstanceContainerPointer, this );
			ES_SAFE_RELEASE( drawset.ParameterPointer );
			ES_SAFE_DELETE( drawset.GlobalPointer );
			m_RemovingDrawSets[1].erase( it++ );
		}
		m_RemovingDrawSets[1].clear();
	}

	{
		std::map<Handle,DrawSet>::iterator it = m_RemovingDrawSets[0].begin();
		while( it != m_RemovingDrawSets[0].end() )
		{
			DrawSet& drawset = (*it).second;
			m_RemovingDrawSets[1][ (*it).first ] = (*it).second;
			m_RemovingDrawSets[0].erase( it++ );
		}
		m_RemovingDrawSets[0].clear();
	}

	{
		std::map<Handle,DrawSet>::iterator it = m_DrawSets.begin();
		while( it != m_DrawSets.end() )
		{
			DrawSet& draw_set = (*it).second;

			// �폜�t���O�������Ă��鎞
			bool isRemoving = draw_set.IsRemoving;

			// �������݂��Ȃ���
			if( !isRemoving && draw_set.GlobalPointer->GetInstanceCount() == 0 )
			{
				isRemoving = true;
			}

			// ���[�g�̂ݑ��݂��A���ɐV�����������錩���݂��Ȃ��Ƃ�
			if( !isRemoving && draw_set.GlobalPointer->GetInstanceCount() == 1 )
			{
				InstanceContainer* pRootContainer = draw_set.InstanceContainerPointer;
				InstanceGroup* group = pRootContainer->GetFirstGroup();

				if( group )
				{
					Instance* pRootInstance = group->GetFirst();

					if( pRootInstance && pRootInstance->GetState() == INSTANCE_STATE_ACTIVE )
					{
						int maxcreate_count = 0;
						for( int i = 0; i < pRootInstance->m_pEffectNode->GetChildrenCount(); i++ )
						{
							float last_generation_time = 
								pRootInstance->m_pEffectNode->GetChild(i)->CommonValues.GenerationTime *
								(pRootInstance->m_pEffectNode->GetChild(i)->CommonValues.MaxGeneration - 1) +
								pRootInstance->m_pEffectNode->GetChild(i)->CommonValues.GenerationTimeOffset +
								1.0f;

							if( pRootInstance->m_LivingTime >= last_generation_time )
							{
								maxcreate_count++;
							}
							else
							{
								break;
							}
						}
					
						if( maxcreate_count == pRootInstance->m_pEffectNode->GetChildrenCount() )
						{
							// �����Đ����łȂ��Ƃ�
							if (!GetSoundPlayer() || !GetSoundPlayer()->CheckPlayingTag(draw_set.GlobalPointer))
							{
								isRemoving = true;
							}
						}
					}
				}
			}

			if( isRemoving )
			{
				// ��������
				StopEffect( (*it).first );

				if( (*it).second.RemovingCallback != NULL )
				{
					(*it).second.RemovingCallback( this, (*it).first, isRemovingManager );
				}

				m_RemovingDrawSets[0][ (*it).first ] = (*it).second;
				m_DrawSets.erase( it++ );
			}
			else
			{
				++it;
			}			
		}
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
InstanceContainer* ManagerImplemented::CreateInstanceContainer( EffectNode* pEffectNode, InstanceGlobal* pGlobal, bool isRoot, Instance* pParent )
{
	InstanceContainer* pContainer = new(this) InstanceContainer(
		this,
		pEffectNode,
		pGlobal,
		pEffectNode->GetChildrenCount() );
	
	for( int i = 0; i < pEffectNode->GetChildrenCount(); i++ )
	{
		pContainer->SetChild( i, CreateInstanceContainer( pEffectNode->GetChild(i), pGlobal ) );
	}

	if( isRoot )
	{
		InstanceGroup* group = pContainer->CreateGroup();
		Instance* instance = group->CreateInstance();
		instance->Initialize( NULL, 0 );

		/* �C���X�^���X�����������킯�ł͂Ȃ�����false�ɕύX */
		group->IsReferencedFromInstance = false;
	}

	return pContainer;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void* EFK_STDCALL ManagerImplemented::Malloc( unsigned int size )
{
	return (void*)new char*[size];
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void EFK_STDCALL ManagerImplemented::Free( void* p, unsigned int size )
{
	char* pData = (char*)p;
	delete [] pData;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
int EFK_STDCALL ManagerImplemented::Rand()
{
	return rand();
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void ManagerImplemented::ExecuteEvents()
{	
	std::map<Handle,DrawSet>::iterator it = m_DrawSets.begin();
	std::map<Handle,DrawSet>::iterator it_end = m_DrawSets.end();

	while( it != it_end )
	{
		if( (*it).second.GoingToStop )
		{
			InstanceContainer* pContainer = (*it).second.InstanceContainerPointer;
			pContainer->KillAllInstances( true );
			(*it).second.IsRemoving = true;
			if (GetSoundPlayer() != NULL)
			{
				GetSoundPlayer()->StopTag((*it).second.GlobalPointer);
			}
		}

		if( (*it).second.GoingToStopRoot )
		{
			InstanceContainer* pContainer = (*it).second.InstanceContainerPointer;
			pContainer->KillAllInstances( false );
		}

		++it;
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
ManagerImplemented::ManagerImplemented( int instance_max, bool autoFlip )
	: m_reference	( 1 )
	, m_autoFlip	( autoFlip )
	, m_NextHandle	( 0 )
	, m_instance_max	( instance_max )
	, m_setting			( NULL )
	, m_sequenceNumber	( 0 )

	, m_MallocFunc(NULL)
	, m_FreeFunc(NULL)
	, m_randFunc(NULL)
	, m_randMax(0)

	, m_spriteRenderer(NULL)
	, m_ribbonRenderer(NULL)
	, m_ringRenderer(NULL)
	, m_modelRenderer(NULL)
	, m_trackRenderer(NULL)

	, m_soundPlayer(NULL)
{
	m_setting = Setting::Create();

	SetMallocFunc( Malloc );
	SetFreeFunc( Free );
	SetRandFunc( Rand );
	SetRandMax( RAND_MAX );

	m_renderingDrawSets.reserve( 64 );

	m_reserved_instances_buffer = new uint8_t[ sizeof(Instance) * m_instance_max ];

	for( int i = 0; i < m_instance_max; i++ )
	{
		Instance* instances = (Instance*)m_reserved_instances_buffer;
		m_reserved_instances.push( &instances[i] );
	}

	m_setting->SetEffectLoader(new DefaultEffectLoader());
	EffekseerPrintDebug("*** Create : Manager\n");
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
ManagerImplemented::~ManagerImplemented()
{
	StopAllEffects();

	ExecuteEvents();

	for( int i = 0; i < 5; i++ )
	{
		GCDrawSet( true );
	}

	assert( m_reserved_instances.size() == m_instance_max ); 
	ES_SAFE_DELETE_ARRAY( m_reserved_instances_buffer );


	ES_SAFE_DELETE(m_spriteRenderer);
	ES_SAFE_DELETE(m_ribbonRenderer);
	ES_SAFE_DELETE(m_modelRenderer);
	ES_SAFE_DELETE(m_trackRenderer);
	ES_SAFE_DELETE(m_ringRenderer);

	ES_SAFE_DELETE(m_soundPlayer);

	ES_SAFE_RELEASE( m_setting );
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void ManagerImplemented::PushInstance( Instance* instance )
{
	m_reserved_instances.push( instance );
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
Instance* ManagerImplemented::PopInstance()
{
	if( m_reserved_instances.empty() )
	{
		return NULL;
	}

	Instance* ret = m_reserved_instances.front();
	m_reserved_instances.pop();
	return ret;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
int ManagerImplemented::AddRef()
{
	m_reference++;
	return m_reference;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
int ManagerImplemented::Release()
{
	m_reference--;
	int count = m_reference;
	if ( count == 0 )
	{
		delete this;
	}
	return count;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void ManagerImplemented::Destroy()
{
	StopAllEffects();

	ExecuteEvents();

	for( int i = 0; i < 5; i++ )
	{
		GCDrawSet( true );
	}

	Release();
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
uint32_t ManagerImplemented::GetSequenceNumber() const
{
	return m_sequenceNumber;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
MallocFunc ManagerImplemented::GetMallocFunc() const
{
	return m_MallocFunc;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void ManagerImplemented::SetMallocFunc(MallocFunc func)
{
	m_MallocFunc = func;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
FreeFunc ManagerImplemented::GetFreeFunc() const
{
	return m_FreeFunc;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void ManagerImplemented::SetFreeFunc(FreeFunc func)
{
	m_FreeFunc = func;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
RandFunc ManagerImplemented::GetRandFunc() const
{
	return m_randFunc;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void ManagerImplemented::SetRandFunc(RandFunc func)
{
	m_randFunc = func;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
int ManagerImplemented::GetRandMax() const
{
	return m_randMax;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void ManagerImplemented::SetRandMax(int max_)
{
	m_randMax = max_;
}


//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
eCoordinateSystem ManagerImplemented::GetCoordinateSystem() const
{
	return m_setting->GetCoordinateSystem();
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void ManagerImplemented::SetCoordinateSystem( eCoordinateSystem coordinateSystem )
{
	m_setting->SetCoordinateSystem(coordinateSystem);
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
SpriteRenderer* ManagerImplemented::GetSpriteRenderer()
{
	return m_spriteRenderer;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void ManagerImplemented::SetSpriteRenderer(SpriteRenderer* renderer)
{
	ES_SAFE_DELETE(m_spriteRenderer);
	m_spriteRenderer = renderer;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
RibbonRenderer* ManagerImplemented::GetRibbonRenderer()
{
	return m_ribbonRenderer;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void ManagerImplemented::SetRibbonRenderer(RibbonRenderer* renderer)
{
	ES_SAFE_DELETE(m_ribbonRenderer);
	m_ribbonRenderer = renderer;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
RingRenderer* ManagerImplemented::GetRingRenderer()
{
	return m_ringRenderer;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void ManagerImplemented::SetRingRenderer(RingRenderer* renderer)
{
	ES_SAFE_DELETE(m_ringRenderer);
	m_ringRenderer = renderer;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
ModelRenderer* ManagerImplemented::GetModelRenderer()
{
	return m_modelRenderer;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void ManagerImplemented::SetModelRenderer(ModelRenderer* renderer)
{
	ES_SAFE_DELETE(m_modelRenderer);
	m_modelRenderer = renderer;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
TrackRenderer* ManagerImplemented::GetTrackRenderer()
{
	return m_trackRenderer;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void ManagerImplemented::SetTrackRenderer(TrackRenderer* renderer)
{
	ES_SAFE_DELETE(m_trackRenderer);
	m_trackRenderer = renderer;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
SoundPlayer* ManagerImplemented::GetSoundPlayer()
{
	return m_soundPlayer;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void ManagerImplemented::SetSoundPlayer(SoundPlayer* soundPlayer)
{
	ES_SAFE_DELETE(m_soundPlayer);
	m_soundPlayer = soundPlayer;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
Setting* ManagerImplemented::GetSetting()
{
	return m_setting;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void ManagerImplemented::SetSetting(Setting* setting)
{
	ES_SAFE_RELEASE(m_setting);
	m_setting = setting;
	ES_SAFE_ADDREF(m_setting);
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
EffectLoader* ManagerImplemented::GetEffectLoader()
{
	return m_setting->GetEffectLoader();
}
	
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void ManagerImplemented::SetEffectLoader( EffectLoader* effectLoader )
{
	m_setting->SetEffectLoader(effectLoader);
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
TextureLoader* ManagerImplemented::GetTextureLoader()
{
	return m_setting->GetTextureLoader();
}
	
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void ManagerImplemented::SetTextureLoader( TextureLoader* textureLoader )
{
	m_setting->SetTextureLoader(textureLoader);
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
SoundLoader* ManagerImplemented::GetSoundLoader()
{
	return m_setting->GetSoundLoader();
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void ManagerImplemented::SetSoundLoader( SoundLoader* soundLoader )
{
	m_setting->SetSoundLoader(soundLoader);
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
ModelLoader* ManagerImplemented::GetModelLoader()
{
	return m_setting->GetModelLoader();
}
	
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void ManagerImplemented::SetModelLoader( ModelLoader* modelLoader )
{
	m_setting->SetModelLoader(modelLoader);
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void ManagerImplemented::StopEffect( Handle handle )
{
	if( m_DrawSets.count( handle ) > 0 )
	{
		DrawSet& drawSet = m_DrawSets[handle];
		drawSet.GoingToStop = true;
		drawSet.IsRemoving = true;
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void ManagerImplemented::StopAllEffects()
{
	std::map<Handle,DrawSet>::iterator it = m_DrawSets.begin();
	while( it != m_DrawSets.end() )
	{
		(*it).second.GoingToStop = true;
		(*it).second.IsRemoving = true;
		++it;		
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void ManagerImplemented::StopRoot( Handle handle )
{
	if( m_DrawSets.count( handle ) > 0 )
	{
		m_DrawSets[handle].GoingToStopRoot = true;
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void ManagerImplemented::StopRoot( Effect* effect )
{
	std::map<Handle,DrawSet>::iterator it = m_DrawSets.begin();
	std::map<Handle,DrawSet>::iterator it_end = m_DrawSets.end();

	while( it != it_end )
	{
		if( (*it).second.ParameterPointer == effect )
		{
			(*it).second.GoingToStopRoot = true;
		}
		++it;
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
bool ManagerImplemented::Exists( Handle handle )
{
	if( m_DrawSets.count( handle ) > 0 )
	{
		if( m_DrawSets[handle].IsRemoving ) return false;
		return true;
	}
	return false;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
int32_t ManagerImplemented::GetInstanceCount( Handle handle )
{
	if( m_DrawSets.count( handle ) > 0 )
	{
		return m_DrawSets[handle].GlobalPointer->GetInstanceCount();
	}
	return 0;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
Matrix43 ManagerImplemented::GetMatrix( Handle handle )
{
	if( m_DrawSets.count( handle ) > 0 )
	{
		DrawSet& drawSet = m_DrawSets[handle];

		InstanceContainer* pContainer = drawSet.InstanceContainerPointer;
		
		InstanceGroup* pGroup = pContainer->GetFirstGroup();

		if( pGroup != NULL )
		{
			Instance* instance = pGroup->GetFirst();
			if( instance != NULL )
			{
				return instance->m_GlobalMatrix43;
			}
		}

		return Matrix43();
	}

	return Matrix43();
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void ManagerImplemented::SetMatrix( Handle handle, const Matrix43& mat )
{
	if( m_DrawSets.count( handle ) > 0 )
	{
		DrawSet& drawSet = m_DrawSets[handle];

		InstanceContainer* pContainer = drawSet.InstanceContainerPointer;
		
		InstanceGroup* pGroup = pContainer->GetFirstGroup();

		if( pGroup != NULL )
		{
			Instance* instance = pGroup->GetFirst();
			if( instance != NULL )
			{
				instance->m_GlobalMatrix43 = mat;

				drawSet.GlobalMatrix = instance->m_GlobalMatrix43;
			}
		}
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
Vector3D ManagerImplemented::GetLocation( Handle handle )
{
	Vector3D location;

	if( m_DrawSets.count( handle ) > 0 )
	{
		DrawSet& drawSet = m_DrawSets[handle];

		InstanceContainer* pContainer = drawSet.InstanceContainerPointer;
		
		Instance* pInstance = pContainer->GetFirstGroup()->GetFirst();

		location.X = pInstance->m_GlobalMatrix43.Value[3][0];
		location.Y = pInstance->m_GlobalMatrix43.Value[3][1];
		location.Z = pInstance->m_GlobalMatrix43.Value[3][2];
	}

	return location;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void ManagerImplemented::SetLocation( Handle handle, float x, float y, float z )
{
	if( m_DrawSets.count( handle ) > 0 )
	{
		DrawSet& drawSet = m_DrawSets[handle];

		InstanceContainer* pContainer = drawSet.InstanceContainerPointer;
		
		Instance* pInstance = pContainer->GetFirstGroup()->GetFirst();

		pInstance->m_GlobalMatrix43.Value[3][0] = x;
		pInstance->m_GlobalMatrix43.Value[3][1] = y;
		pInstance->m_GlobalMatrix43.Value[3][2] = z;

		drawSet.GlobalMatrix = pInstance->m_GlobalMatrix43;
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void ManagerImplemented::SetLocation( Handle handle, const Vector3D& location )
{
	SetLocation( handle, location.X, location.Y, location.Z );
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void ManagerImplemented::AddLocation( Handle handle, const Vector3D& location )
{
	if( m_DrawSets.count( handle ) > 0 )
	{
		DrawSet& drawSet = m_DrawSets[handle];

		InstanceContainer* pContainer = drawSet.InstanceContainerPointer;
		
		Instance* pInstance = pContainer->GetFirstGroup()->GetFirst();

		pInstance->m_GlobalMatrix43.Value[3][0] += location.X;
		pInstance->m_GlobalMatrix43.Value[3][1] += location.Y;
		pInstance->m_GlobalMatrix43.Value[3][2] += location.Z;

		drawSet.GlobalMatrix = pInstance->m_GlobalMatrix43;
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void ManagerImplemented::SetRotation( Handle handle, float x, float y, float z )
{
	if( m_DrawSets.count( handle ) > 0 )
	{
		DrawSet& drawSet = m_DrawSets[handle];

		InstanceContainer* pContainer = drawSet.InstanceContainerPointer;

		if( pContainer == NULL ) return;
		if( pContainer->GetFirstGroup() == NULL ) return;
		if( pContainer->GetFirstGroup()->GetFirst() == NULL ) return;

		Instance* pInstance = pContainer->GetFirstGroup()->GetFirst();
		
		Matrix43 MatRotX,MatRotY,MatRotZ;

		MatRotX.RotationX( x );
		MatRotY.RotationY( y );
		MatRotZ.RotationZ( z );
		
		Matrix43 r;
		Vector3D s, t;

		pInstance->m_GlobalMatrix43.GetSRT( s, r, t );

		r.Indentity();
		Matrix43::Multiple( r, r, MatRotZ );
		Matrix43::Multiple( r, r, MatRotX );
		Matrix43::Multiple( r, r, MatRotY );

		pInstance->m_GlobalMatrix43.SetSRT( s, r, t );
		
		drawSet.GlobalMatrix = pInstance->m_GlobalMatrix43;
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void ManagerImplemented::SetRotation( Handle handle, const Vector3D& axis, float angle )
{
	if( m_DrawSets.count( handle ) > 0 )
	{
		DrawSet& drawSet = m_DrawSets[handle];

		InstanceContainer* pContainer = drawSet.InstanceContainerPointer;
		
		if( pContainer == NULL ) return;
		if( pContainer->GetFirstGroup() == NULL ) return;
		if( pContainer->GetFirstGroup()->GetFirst() == NULL ) return;

		Instance* pInstance = pContainer->GetFirstGroup()->GetFirst();

		Matrix43 r;
		Vector3D s, t;

		pInstance->m_GlobalMatrix43.GetSRT( s, r, t );

		r.RotationAxis( axis, angle );

		pInstance->m_GlobalMatrix43.SetSRT( s, r, t );
		
		drawSet.GlobalMatrix = pInstance->m_GlobalMatrix43;
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void ManagerImplemented::SetScale( Handle handle, float x, float y, float z )
{
	if( m_DrawSets.count( handle ) > 0 )
	{
		DrawSet& drawSet = m_DrawSets[handle];

		InstanceContainer* pContainer = drawSet.InstanceContainerPointer;
		
		if( pContainer == NULL ) return;
		if( pContainer->GetFirstGroup() == NULL ) return;
		if( pContainer->GetFirstGroup()->GetFirst() == NULL ) return;

		Instance* pInstance = pContainer->GetFirstGroup()->GetFirst();

		Matrix43 r;
		Vector3D s, t;

		pInstance->m_GlobalMatrix43.GetSRT( s, r, t );

		s.X = x;
		s.Y = y;
		s.Z = z;

		pInstance->m_GlobalMatrix43.SetSRT( s, r, t );

		drawSet.GlobalMatrix = pInstance->m_GlobalMatrix43;
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
Matrix43 ManagerImplemented::GetBaseMatrix( Handle handle )
{
	if( m_DrawSets.count( handle ) > 0 )
	{
		return m_DrawSets[handle].BaseMatrix;
	}

	return Matrix43();
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void ManagerImplemented::SetBaseMatrix( Handle handle, const Matrix43& mat )
{
	if( m_DrawSets.count( handle ) > 0 )
	{
		m_DrawSets[handle].BaseMatrix = mat;
		m_DrawSets[handle].DoUseBaseMatrix = true;
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void ManagerImplemented::SetRemovingCallback( Handle handle, EffectInstanceRemovingCallback callback )
{
	if( m_DrawSets.count( handle ) > 0 )
	{
		m_DrawSets[handle].RemovingCallback = callback;
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void ManagerImplemented::SetShown( Handle handle, bool shown )
{
	if( m_DrawSets.count( handle ) > 0 )
	{
		m_DrawSets[handle].IsShown = shown;
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void ManagerImplemented::SetPaused( Handle handle, bool paused )
{
	if( m_DrawSets.count( handle ) > 0 )
	{
		m_DrawSets[handle].IsPaused = paused;
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void ManagerImplemented::SetSpeed( Handle handle, float speed )
{
	if( m_DrawSets.count( handle ) > 0 )
	{
		m_DrawSets[handle].Speed = speed;
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void ManagerImplemented::SetAutoDrawing( Handle handle, bool autoDraw )
{
	if( m_DrawSets.count( handle ) > 0 )
	{
		m_DrawSets[handle].IsAutoDrawing = autoDraw;
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void ManagerImplemented::Flip()
{
	if( !m_autoFlip )
	{
		m_renderingSession.Enter();
	}

	ExecuteEvents();

	// DrawSet�폜����
	GCDrawSet( false );

	m_renderingDrawSets.clear();

	{
		std::map<Handle,DrawSet>::iterator it = m_DrawSets.begin();
		std::map<Handle,DrawSet>::iterator it_end = m_DrawSets.end();
		
		while( it != it_end )
		{
			m_renderingDrawSets.push_back( (*it).second );
			++it;
		}
	}

	if( !m_autoFlip )
	{
		m_renderingSession.Leave();
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void ManagerImplemented::Update( float deltaFrame )
{
	BeginUpdate();

	// �J�n���Ԃ��L�^
	int64_t beginTime = ::Effekseer::GetTime();

	for( size_t i = 0; i < m_renderingDrawSets.size(); i++ )
	{
		DrawSet& drawSet = m_renderingDrawSets[i];
		
		UpdateHandle( drawSet, deltaFrame );
	}

	// �o�ߎ��Ԃ��v�Z
	m_updateTime = (int)(Effekseer::GetTime() - beginTime);

	EndUpdate();
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void ManagerImplemented::BeginUpdate()
{
	m_renderingSession.Enter();

	if( m_autoFlip )
	{
		Flip();
	}

	m_sequenceNumber++;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void ManagerImplemented::EndUpdate()
{
	m_renderingSession.Leave();
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void ManagerImplemented::UpdateHandle( Handle handle, float deltaFrame )
{
	std::map<Handle,DrawSet>::iterator it = m_DrawSets.find( handle );
	if( it != m_DrawSets.end() )
	{
		DrawSet& drawSet = it->second;

		UpdateHandle( drawSet, deltaFrame );
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void ManagerImplemented::UpdateHandle( DrawSet& drawSet, float deltaFrame )
{
	if( !drawSet.IsPaused )
	{
		float df = deltaFrame * drawSet.Speed;

		drawSet.InstanceContainerPointer->Update( true, df, drawSet.IsShown );

		if( drawSet.DoUseBaseMatrix )
		{
			drawSet.InstanceContainerPointer->SetBaseMatrix( true, drawSet.BaseMatrix );
		}

		drawSet.GlobalPointer->AddUpdatedFrame( df );
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void ManagerImplemented::Draw()
{
	m_renderingSession.Enter();

	// �J�n���Ԃ��L�^
	int64_t beginTime = ::Effekseer::GetTime();

	for( size_t i = 0; i < m_renderingDrawSets.size(); i++ )
	{
		DrawSet& drawSet = m_renderingDrawSets[i];

		if( drawSet.IsShown && drawSet.IsAutoDrawing )
		{
			drawSet.InstanceContainerPointer->Draw( true );
		}
	}

	// �o�ߎ��Ԃ��v�Z
	m_drawTime = (int)(Effekseer::GetTime() - beginTime);

	m_renderingSession.Leave();
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
Handle ManagerImplemented::Play( Effect* effect, float x, float y, float z )
{
	if( effect == NULL ) return -1;
	
	// ���[�g����
	InstanceGlobal* pGlobal = new InstanceGlobal();
	InstanceContainer* pContainer = CreateInstanceContainer( ((EffectImplemented*)effect)->GetRoot(), pGlobal, true, NULL );
	
	pGlobal->SetRootContainer(  pContainer );

	Instance* pInstance = pContainer->GetFirstGroup()->GetFirst();

	pInstance->m_GlobalMatrix43.Value[3][0] = x;
	pInstance->m_GlobalMatrix43.Value[3][1] = y;
	pInstance->m_GlobalMatrix43.Value[3][2] = z;

	Handle handle = AddDrawSet( effect, pContainer, pGlobal );

	m_DrawSets[handle].GlobalMatrix = pInstance->m_GlobalMatrix43;

	return handle;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void ManagerImplemented::DrawHandle( Handle handle )
{
	m_renderingSession.Enter();
	
	std::map<Handle,DrawSet>::iterator it = m_DrawSets.find( handle );
	if( it != m_DrawSets.end() )
	{
		DrawSet& drawSet = it->second;

		if( drawSet.IsShown )
		{
			drawSet.InstanceContainerPointer->Draw( true );
		}
	}

	m_renderingSession.Leave();
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void ManagerImplemented::BeginReloadEffect( Effect* effect )
{
	m_renderingSession.Enter();

	std::map<Handle,DrawSet>::iterator it = m_DrawSets.begin();
	std::map<Handle,DrawSet>::iterator it_end = m_DrawSets.end();

	for(; it != it_end; ++it )
	{
		if( (*it).second.ParameterPointer != effect ) continue;
	
		/* �C���X�^���X�폜 */
		(*it).second.InstanceContainerPointer->RemoveForcibly( true );
		(*it).second.InstanceContainerPointer->~InstanceContainer();
		InstanceContainer::operator delete( (*it).second.InstanceContainerPointer, this );
		(*it).second.InstanceContainerPointer = NULL;


	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void ManagerImplemented::EndReloadEffect( Effect* effect )
{
	std::map<Handle,DrawSet>::iterator it = m_DrawSets.begin();
	std::map<Handle,DrawSet>::iterator it_end = m_DrawSets.end();

	for(; it != it_end; ++it )
	{
		if( (*it).second.ParameterPointer != effect ) continue;

		/* �C���X�^���X���� */
		(*it).second.InstanceContainerPointer = CreateInstanceContainer( ((EffectImplemented*)effect)->GetRoot(), (*it).second.GlobalPointer, true, NULL );
		(*it).second.GlobalPointer->SetRootContainer(  (*it).second.InstanceContainerPointer );

		/* �s��ݒ� */
		(*it).second.InstanceContainerPointer->GetFirstGroup()->GetFirst()->m_GlobalMatrix43 = 
			(*it).second.GlobalMatrix;
		
		/* �X�L�b�v */
		for( float f = 0; f < (*it).second.GlobalPointer->GetUpdatedFrame() - 1; f+= 1.0f )
		{
			(*it).second.InstanceContainerPointer->Update( true, 1.0f, false );
		}

		(*it).second.InstanceContainerPointer->Update( true, 1.0f, (*it).second.IsShown );
	}

	m_renderingSession.Leave();
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
