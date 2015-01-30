
#ifndef	__EFFEKSEER_BASE_PRE_H__
#define	__EFFEKSEER_BASE_PRE_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include <stdio.h>
#include <string.h>

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#ifdef _WIN32
#define	EFK_STDCALL	__stdcall
#else
#define	EFK_STDCALL
#endif

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#ifdef _WIN32
typedef signed char			int8_t;
typedef unsigned char		uint8_t;
typedef short				int16_t;
typedef unsigned short		uint16_t;
typedef int					int32_t;
typedef unsigned int		uint32_t;
typedef __int64				int64_t;
typedef unsigned __int64	uint64_t;
#else
#include <stdint.h>
#endif

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#include <pthread.h>
#include <sys/time.h>
#endif

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
typedef uint16_t			EFK_CHAR;

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
namespace Effekseer
{
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
struct Vector2D;
struct Vector3D;
struct Matrix43;
struct Matrix44;
struct RectF;

class Manager;
class Effect;

class ParticleRenderer;
class SpriteRenderer;
class RibbonRenderer;
class RingRenderer;
class ModelRenderer;
class TrackRenderer;

class Setting;
class EffectLoader;
class TextureLoader;

class SoundPlayer;
class SoundLoader;

class ModelLoader;

class Model;

typedef	int	Handle;

/**
	@brief	�������m�ۊ֐�
*/
typedef void* ( EFK_STDCALL *MallocFunc ) ( unsigned int size );

/**
	@brief	�������j���֐�
*/
typedef	void ( EFK_STDCALL *FreeFunc ) ( void* p, unsigned int size );

/**
	@brief	�����_���֐�
*/
typedef	int ( EFK_STDCALL *RandFunc ) (void);

/**
	@brief	�G�t�F�N�g�̃C���X�^���X�j�����̃R�[���o�b�N�C�x���g
	@param	manager	[in]	�������Ă���}�l�[�W���[
	@param	handle	[in]	�G�t�F�N�g�̃C���X�^���X�̃n���h��
	@param	isRemovingManager	[in]	�}�l�[�W���[��j�������Ƃ��ɃG�t�F�N�g�̃C���X�^���X��j�����Ă��邩
*/
typedef	void ( EFK_STDCALL *EffectInstanceRemovingCallback ) ( Manager* manager, Handle handle, bool isRemovingManager );

#define ES_SAFE_ADDREF(val)						if ( (val) != NULL ) { (val)->AddRef(); }
#define ES_SAFE_RELEASE(val)					if ( (val) != NULL ) { (val)->Release(); (val) = NULL; }
#define ES_SAFE_DELETE(val)						if ( (val) != NULL ) { delete (val); (val) = NULL; }
#define ES_SAFE_DELETE_ARRAY(val)				if ( (val) != NULL ) { delete [] (val); (val) = NULL; }

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
/**
	@brief	�A���t�@�u�����h
*/
enum eAlphaBlend
{
	/// <summary>
	/// �s����
	/// </summary>
	ALPHA_BLEND_OPACITY = 0,
	/// <summary>
	/// ����
	/// </summary>
	ALPHA_BLEND_BLEND = 1,
	/// <summary>
	/// ���Z
	/// </summary>
	ALPHA_BLEND_ADD = 2,
	/// <summary>
	/// ���Z
	/// </summary>
	ALPHA_BLEND_SUB = 3,
	/// <summary>
	/// ��Z
	/// </summary>
	ALPHA_BLEND_MUL = 4,

	ALPHA_BLEND_DWORD = 0x7fffffff,
};

enum eTextureFilterType
{
	TEXTURE_FILTER_NEAREST = 0,
	TEXTURE_FILTER_LINEAR = 1,
	TEXTURE_FILTER_DWORD = 0x7fffffff,
};

enum eTextureWrapType
{
	TEXTURE_WRAP_REPEAT = 0,
	TEXTURE_WRAP_CLAMP = 1,

	TEXTURE_WRAP_DWORD = 0x7fffffff,
};

enum eCullingType
{
	CULLING_FRONT = 0,
	CULLING_BACK = 1,
	CULLING_DOUBLE = 2,

	CULLING_DWORD = 0x7fffffff,
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
enum eBillboardType
{
	BillboardType_Billboard = 0,
	BillboardType_YAxisFixed = 1,
	BillboardType_Fixed = 2,
	BillboardType_RotatedBillboard = 3,

	BillboardType_DWORD = 0x7fffffff,
};

enum eCoordinateSystem
{
	COORDINATE_SYSTEM_LH,
	COORDINATE_SYSTEM_RH,
	COORDINATE_SYSTEM_DWORD = 0x7fffffff,
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
/**
	@brief	�ő�l�擾
*/
template <typename T,typename U>
T Max( T t, U u )
{
	if( t > (T)u )
	{
		return t;
	}
	return u;
}

/**
	@brief	�ŏ��l�擾
*/
template <typename T,typename U>
T Min( T t, U u )
{
	if( t < (T)u )
	{
		return t;
	}
	return u;
}

/**
	@brief	�͈͓��l�擾
*/
template <typename T,typename U,typename V>
T Clamp( T t, U max_, V min_ )
{
	if( t > (T)max_ )
	{
		t = (T)max_;
	}

	if( t < (T)min_ )
	{
		t = (T)min_;
	}

	return t;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
inline float NormalizeAngle(float angle)
{
    int32_t ofs = (*(int32_t*)&angle & 0x80000000) | 0x3F000000; 
    return (angle - ((int)(angle * 0.159154943f + *(float*)&ofs) * 6.283185307f)); 
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
inline void SinCos(float x, float& s, float& c)
{
	x = NormalizeAngle(x);
	float x2 = x * x;
	float x4 = x * x * x * x;
	float x6 = x * x * x * x * x * x;
	float x8 = x * x * x * x * x * x * x * x;
	float x10 = x * x * x * x * x * x * x * x * x * x;
	s = x * (1.0f - x2 / 6.0f + x4 / 120.0f - x6 / 5040.0f + x8 / 362880.0f - x10 / 39916800.0f);
	c = 1.0f - x2 / 2.0f + x4 / 24.0f - x6 / 720.0f + x8 / 40320.0f - x10 / 3628800.0f;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
/**
	@brief	�����R�[�h��ϊ�����B(UTF16 -> UTF8)
	@param	dst	[out]	�o�͔z��̐擪�|�C���^
	@param	dst_size	[in]	�o�͔z��̒���
	@param	src			[in]	���͔z��̐擪�|�C���^
	@return	������
*/
inline int32_t ConvertUtf16ToUtf8( int8_t* dst, int32_t dst_size, const int16_t* src )
{
	int32_t cnt = 0;
	const int16_t* wp = src;
	int8_t* cp = dst;

	if (dst_size == 0) return 0;
	
	dst_size -= 3;

	for (cnt = 0; cnt < dst_size; )
	{
		int16_t wc = *wp++;
		if (wc == 0)
		{
			break;
		}
		if ((wc & ~0x7f) == 0)
		{
			*cp++ = wc & 0x7f;
			cnt += 1;
		} else if ((wc & ~0x7ff) == 0)
		{
			*cp++ = ((wc >>  6) & 0x1f) | 0xc0;
			*cp++ = ((wc)       & 0x3f) | 0x80;
			cnt += 2;
		} else {
			*cp++ = ((wc >> 12) &  0xf) | 0xe0;
			*cp++ = ((wc >>  6) & 0x3f) | 0x80;
			*cp++ = ((wc)       & 0x3f) | 0x80;
			cnt += 3;
		}
	}
	*cp = '\0';
	return cnt;
}

/**
	@brief	�����R�[�h��ϊ�����B(UTF8 -> UTF16)
	@param	dst	[out]	�o�͔z��̐擪�|�C���^
	@param	dst_size	[in]	�o�͔z��̒���
	@param	src			[in]	���͔z��̐擪�|�C���^
	@return	������
*/
inline int32_t ConvertUtf8ToUtf16( int16_t* dst, int32_t dst_size, const int8_t* src )
{
	int32_t i, code;
	int8_t c0, c1, c2;

	if (dst_size == 0) return 0;
	
	dst_size -= 1;

	for (i = 0; i < dst_size; i++)
	{
		int16_t wc;
		
		c0 = *src++;
		if (c0 == '\0')
		{
			break;
		}
		// UTF8����UTF16�ɕϊ�
		code = (uint8_t)c0 >> 4;
		if (code <= 7)
		{
			// 8bit����
			wc = c0;
		} 
		else if (code >= 12 && code <= 13)
		{
			// 16bit����
			c1 = *src++;
			wc = ((c0 & 0x1F) << 6) | (c1 & 0x3F);
		} 
		else if (code == 14)
		{
			// 24bit����
			c1 = *src++;
			c2 = *src++;
			wc = ((c0 & 0x0F) << 12) | ((c1 & 0x3F) << 6) | (c2 & 0x3F);
		} 
		else
		{
			continue;
		}
		dst[i] = wc;
	}
	dst[i] = 0;
	return i;
}


//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
}
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif	// __EFFEKSEER_BASE_PRE_H__
#ifndef	__EFFEKSEER_VECTOR2D_H__
#define	__EFFEKSEER_VECTOR2D_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
namespace Effekseer { 
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
/**
	@brief	3�����x�N�g��
*/
struct Vector2D
{
public:
	/**
		@brief	X
	*/
	float	X;

	/**
		@brief	Y
	*/
	float	Y;

	/**
		@brief	�R���X�g���N�^
	*/
	Vector2D();

	/**
		@brief	�R���X�g���N�^
	*/
	Vector2D( float x, float y );

	Vector2D& operator+=( const Vector2D& value );
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
 } 
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif	// __EFFEKSEER_VECTOR3D_H__

#ifndef	__EFFEKSEER_VECTOR3D_H__
#define	__EFFEKSEER_VECTOR3D_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
namespace Effekseer { 
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
/**
	@brief	3�����x�N�g��
*/
struct Vector3D
{
public:
	/**
		@brief	X
	*/
	float	X;

	/**
		@brief	Y
	*/
	float	Y;

	/**
		@brief	Z
	*/
	float	Z;

	/**
		@brief	�R���X�g���N�^
	*/
	Vector3D();

	/**
		@brief	�R���X�g���N�^
	*/
	Vector3D( float x, float y, float z );

	Vector3D operator + ( const Vector3D& o ) const;

	Vector3D operator - ( const Vector3D& o ) const;

	Vector3D operator * ( const float& o ) const;

	Vector3D operator / ( const float& o ) const;

	Vector3D& operator += ( const Vector3D& o );

	Vector3D& operator -= ( const Vector3D& o );

	Vector3D& operator *= ( const float& o );

	Vector3D& operator /= ( const float& o );

	/**
		@brief	���Z
	*/
	static void Add( Vector3D* pOut, const Vector3D* pIn1, const Vector3D* pIn2 );

	/**
		@brief	���Z
	*/
	static Vector3D& Sub( Vector3D& o, const Vector3D& in1, const Vector3D& in2 );

	/**
		@brief	����
	*/
	static float Length( const Vector3D& in );

	/**
		@brief	�����̓��
	*/
	static float LengthSq( const Vector3D& in );

	/**
		@brief	����
	*/
	static float Dot( const Vector3D& in1, const Vector3D& in2 );

	/**
		@brief	�P�ʃx�N�g��
	*/
	static void Normal( Vector3D& o, const Vector3D& in );

	/**
		@brief	�O��
		@note
		�E��n�̏ꍇ�A�E��̐e�w��in1�A�l�����w��in2�Ƃ����Ƃ��A���w�̕�����Ԃ��B<BR>
		����n�̏ꍇ�A����̐e�w��in1�A�l�����w��in2�Ƃ����Ƃ��A���w�̕�����Ԃ��B<BR>
	*/
	static Vector3D& Cross( Vector3D& o, const Vector3D& in1, const Vector3D& in2 );

	static Vector3D& Transform( Vector3D& o, const Vector3D& in, const Matrix43& mat );

	static Vector3D& Transform( Vector3D& o, const Vector3D& in, const Matrix44& mat );
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
 } 
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif	// __EFFEKSEER_VECTOR3D_H__

#ifndef	__EFFEKSEER_COLOR_H__
#define	__EFFEKSEER_COLOR_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
namespace Effekseer
{
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
enum ColorMode
{
	COLOR_MODE_RGBA,
	COLOR_MODE_HSVA,
	COLOR_MODE_DWORD = 0x7FFFFFFF
};

/**
	@brief	�F
*/
#pragma pack(push,1)
struct Color
{
	/**
		@brief	��
	*/
	uint8_t		R;

	/**
		@brief	��
	*/
	uint8_t		G;

	/**
		@brief	��
	*/
	uint8_t		B;
	
	/**
		@brief	�����x
	*/
	uint8_t		A;

	/**
		@brief	�R���X�g���N�^
	*/
	Color();

	/**
		@brief	�R���X�g���N�^
	*/
	Color( uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255 );

	/**
		@brief	��Z
	*/
	static void Mul( Color& o, const Color& in1, const Color& in2 );
};
#pragma pack(pop)
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
}
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif	// __EFFEKSEER_COLOR_H__

#ifndef	__EFFEKSEER_RECTF_H__
#define	__EFFEKSEER_RECTF_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
namespace Effekseer { 
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
/**
	@brief	�l�p�`
*/
struct RectF
{
private:

public:
	float	X;

	float	Y;

	float	Width;

	float	Height;

	RectF();

	RectF( float x, float y, float width, float height );

	Vector2D Position() const;

	Vector2D Size() const;
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
 } 
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif	// __EFFEKSEER_RECTF_H__

#ifndef	__EFFEKSEER_MATRIX43_H__
#define	__EFFEKSEER_MATRIX43_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
namespace Effekseer { 
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------

/**
	@brief	4x3�s��
	@note
	�E��n(��]:�����v���)<BR>
	V[x,y,z,1] * M �̌`�Ńx�N�g���Ƃ̏�Z���\�ł���B<BR>
	[0,0][0,1][0,2]<BR>
	[1,0][1,1][1,2]<BR>
	[2,0][2,1][2,2]<BR>
	[3,0][3,1][3,2]<BR>
*/
#pragma pack(push,1)
struct Matrix43
{
private:

public:
	/**
		@brief	�s��̒l
	*/
	float	Value[4][3];
	
	/**
		@brief	�P�ʍs�񉻂��s���B
	*/
	void Indentity();

	/**
		@brief	�g��s�񉻂��s���B
		@param	x	[in]	X�����g�嗦
		@param	y	[in]	Y�����g�嗦
		@param	z	[in]	Z�����g�嗦
	*/
	void Scaling( float x, float y, float z );

	/**
		@brief	�����v���������X����]�s�񉻂��s���B
		@param	angle	[in]	�p�x(���W�A��)
	*/
	void RotationX( float angle );

	/**
		@brief	�����v���������Y����]�s�񉻂��s���B
		@param	angle	[in]	�p�x(���W�A��)
	*/
	void RotationY( float angle );

	/**
		@brief	�����v���������Z����]�s�񉻂��s���B
		@param	angle	[in]	�p�x(���W�A��)
	*/
	void RotationZ( float angle );
	
	/**
		@brief	�����v���������XYZ����]�s�񉻂��s���B
		@param	rx	[in]	�p�x(���W�A��)
		@param	ry	[in]	�p�x(���W�A��)
		@param	rz	[in]	�p�x(���W�A��)
	*/
	void RotationXYZ( float rx, float ry, float rz );
	
	/**
		@brief	�����v���������ZXY����]�s�񉻂��s���B
		@param	rz	[in]	�p�x(���W�A��)
		@param	rx	[in]	�p�x(���W�A��)
		@param	ry	[in]	�p�x(���W�A��)
	*/
	void RotationZXY( float rz, float rx, float ry );

	/**
		@brief	�C�ӎ��ɑ΂��锽���v���������]�s�񉻂��s���B
		@param	axis	[in]	��]��
		@param	angle	[in]	�p�x(���W�A��)
	*/
	void RotationAxis( const Vector3D& axis, float angle );

	/**
		@brief	�C�ӎ��ɑ΂��锽���v���������]�s�񉻂��s���B
		@param	axis	[in]	��]��
		@param	s	[in]	�T�C��
		@param	c	[in]	�R�T�C��
	*/
	void RotationAxis( const Vector3D& axis, float s, float c );

	/**
		@brief	�ړ��s�񉻂��s���B
		@param	x	[in]	X�����ړ�
		@param	y	[in]	Y�����ړ�
		@param	z	[in]	Z�����ړ�
	*/
	void Translation( float x, float y, float z );

	/**
		@brief	�s����A�g��A��]�A�ړ��̍s��ƃx�N�g���ɕ�������B
		@param	s	[out]	�g��s��
		@param	r	[out]	��]�s��
		@param	t	[out]	�ʒu
	*/
	void GetSRT( Vector3D& s, Matrix43& r, Vector3D& t ) const; 

	/**
		@brief	�s��̊g��A��]�A�ړ���ݒ肷��B
		@param	s	[in]	�g��s��
		@param	r	[in]	��]�s��
		@param	t	[in]	�ʒu
	*/
	void SetSRT( const Vector3D& s, const Matrix43& r, const Vector3D& t );

	/**
		@brief	�s�񓯎m�̏�Z���s���B
		@param	out	[out]	����
		@param	in1	[in]	��Z�̍���
		@param	in2	[in]	��Z�̉E��
	*/
	static void Multiple( Matrix43& out, const Matrix43& in1, const Matrix43& in2 );
};

#pragma pack(pop)
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
 } 
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif	// __EFFEKSEER_MATRIX43_H__

#ifndef	__EFFEKSEER_MATRIX44_H__
#define	__EFFEKSEER_MATRIX44_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
namespace Effekseer { 
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------

/**
	@brief	�s��
	@note
	�E��n<BR>
	����n<BR>
	V[x,y,z,1] * M �̌`<BR>
	[0,0][0,1][0,2][0,3]
	[1,0][1,1][1,2][1,3]
	[2,0][2,1][2,2][2,3]
	[3,0][3,1][3,2][3,3]
*/
#pragma pack(push,1)
struct Matrix44
{
private:

public:

	/**
		@brief	�R���X�g���N�^
	*/
	Matrix44();

	/**
		@brief	�s��̒l
	*/
	float	Values[4][4];

	/**
		@brief	�P�ʍs��
	*/
	Matrix44& Indentity();

	/**
		@brief	�J�����s��(�E��n)
	*/
	Matrix44& LookAtRH( const Vector3D& eye, const Vector3D& at, const Vector3D& up );

	/**
		@brief	�J�����s��(����n)
	*/
	Matrix44& LookAtLH( const Vector3D& eye, const Vector3D& at, const Vector3D& up );

	/**
		@brief	�ˉe�s��(�E��n)
	*/
	Matrix44& PerspectiveFovRH( float ovY, float aspect, float zn, float zf );

	/**
		@brief	OpenGL�p�ˉe�s��(�E��n)
	*/
	Matrix44& PerspectiveFovRH_OpenGL( float ovY, float aspect, float zn, float zf );

	/**
		@brief	�ˉe�s��(����n)
	*/
	Matrix44& PerspectiveFovLH( float ovY, float aspect, float zn, float zf );
	
	/**
	 @brief	OpenGL�p�ˉe�s��(����n)
	 */
	Matrix44& PerspectiveFovLH_OpenGL( float ovY, float aspect, float zn, float zf );
	
	/**
		@brief	���ˉe�s��(�E��n)
	*/
	Matrix44& OrthographicRH( float width, float height, float zn, float zf );

	/**
		@brief	���ˉe�s��(����n)
	*/
	Matrix44& OrthographicLH( float width, float height, float zn, float zf );

	/**
		@brief	�g��s��
	*/
	void Scaling( float x, float y, float z );

	/**
		@brief	X����]�s��(�E��)
	*/
	void RotationX( float angle );

	/**
		@brief	Y����]�s��(�E��)
	*/
	void RotationY( float angle );

	/**
		@brief	Z����]�s��(�E��)
	*/
	void RotationZ( float angle );

	/**
		@brief	�ړ��s��
	*/
	void Translation( float x, float y, float z );

	/**
		@brief	�C�ӎ������v��]�s��
	*/
	void RotationAxis( const Vector3D& axis, float angle );

	/**
		@brief	�N�I�[�^�j�I������s��ɕϊ�
	*/
	void Quaternion( float x, float y, float z, float w );

	/**
		@brief	��Z
	*/
	static Matrix44& Mul( Matrix44& o, const Matrix44& in1, const Matrix44& in2 );

	/**
		@brief	�t�s��
	*/
	static Matrix44& Inverse( Matrix44& o, const Matrix44& in );
};

#pragma pack(pop)
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
 } 
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif	// __EFFEKSEER_MATRIX44_H__

#ifndef	__EFFEKSEER_FILE_H__
#define	__EFFEKSEER_FILE_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
namespace Effekseer { 
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
/**
	@brief	�t�@�C���ǂݍ��݃N���X
*/
class FileReader
{
private:

public:
	FileReader() {}

	virtual ~FileReader() {}

	virtual size_t Read( void* buffer, size_t size ) = 0;

	virtual void Seek(int position) = 0;

	virtual int GetPosition() = 0;

	virtual size_t GetLength() = 0;
};

/**
	@brief	�t�@�C���������݃N���X
*/
class FileWriter
{
private:

public:
	FileWriter() {}

	virtual ~FileWriter() {}

	virtual size_t Write( const void* buffer, size_t size ) = 0;

	virtual void Flush() = 0;

	virtual void Seek(int position) = 0;

	virtual int GetPosition() = 0;

	virtual size_t GetLength() = 0;
};

/**
	@brief	�t�@�C���A�N�Z�X�p�̃t�@�N�g���N���X
*/
class FileInterface
{
private:

public:
	virtual FileReader* OpenRead( const EFK_CHAR* path ) = 0;

	virtual FileWriter* OpenWrite( const EFK_CHAR* path ) = 0;
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
 } 
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif	// __EFFEKSEER_FILE_H__

#ifndef	__EFFEKSEER_DEFAULT_FILE_H__
#define	__EFFEKSEER_DEFAULT_FILE_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
namespace Effekseer { 
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
/**
	@brief	�W���̃t�@�C���ǂݍ��݃N���X
*/

class DefaultFileReader : public FileReader
{
private:
	FILE* m_filePtr;

public:
	DefaultFileReader( FILE* filePtr );

	~DefaultFileReader();

	size_t Read( void* buffer, size_t size );

	void Seek( int position );

	int GetPosition();

	size_t GetLength();
};

class DefaultFileWriter : public FileWriter
{
private:
	FILE* m_filePtr;

public:
	DefaultFileWriter( FILE* filePtr );

	~DefaultFileWriter();

	size_t Write( const void* buffer, size_t size );

	void Flush();

	void Seek( int position );

	int GetPosition();

	size_t GetLength();
};

class DefaultFileInterface : public FileInterface
{
private:

public:
	FileReader* OpenRead( const EFK_CHAR* path );

	FileWriter* OpenWrite( const EFK_CHAR* path );
};


//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
 } 
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif	// __EFFEKSEER_DEFAULT_FILE_H__

#ifndef	__EFFEKSEER_EFFECT_H__
#define	__EFFEKSEER_EFFECT_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------

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

#ifndef	__EFFEKSEER_MANAGER_H__
#define	__EFFEKSEER_MANAGER_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------

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

#ifndef	__EFFEKSEER_SPRITE_RENDERER_H__
#define	__EFFEKSEER_SPRITE_RENDERER_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
namespace Effekseer
{
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------

class SpriteRenderer
{
public:

	struct NodeParameter
	{
		Effect*				EffectPointer;
		int32_t				ColorTextureIndex;
		eAlphaBlend			AlphaBlend;
		eTextureFilterType	TextureFilter;
		eTextureWrapType	TextureWrap;
		bool				ZTest;
		bool				ZWrite;
		eBillboardType		Billboard;
	};

	struct InstanceParameter
	{
		Matrix43		SRTMatrix43;
		Color		AllColor;

		// �����A�E���A����A�E��
		Color		Colors[4];

		Vector2D	Positions[4];

		RectF	UV;
	};

public:
	SpriteRenderer() {}

	virtual ~SpriteRenderer() {}

	virtual void BeginRendering( const NodeParameter& parameter, int32_t count, void* userData ) {}

	virtual void Rendering( const NodeParameter& parameter, const InstanceParameter& instanceParameter, void* userData ) {}

	virtual void EndRendering( const NodeParameter& parameter, void* userData ) {}
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
}
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif	// __EFFEKSEER_SPRITE_RENDERER_H__

#ifndef	__EFFEKSEER_RIBBON_RENDERER_H__
#define	__EFFEKSEER_RIBBON_RENDERER_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
namespace Effekseer
{
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------

class RibbonRenderer
{
public:

	struct NodeParameter
	{
		Effect*				EffectPointer;
		int32_t				ColorTextureIndex;
		eAlphaBlend			AlphaBlend;
		eTextureFilterType	TextureFilter;
		eTextureWrapType	TextureWrap;
		bool				ZTest;
		bool				ZWrite;
		bool				ViewpointDependent;
	};

	struct InstanceParameter
	{
		int32_t			InstanceCount;
		int32_t			InstanceIndex;
		Matrix43		SRTMatrix43;
		Color		AllColor;

		// ���A�E
		Color		Colors[2];

		float	Positions[2];
	};

public:
	RibbonRenderer() {}

	virtual ~RibbonRenderer() {}

	virtual void BeginRendering( const NodeParameter& parameter, int32_t count, void* userData ) {}

	virtual void Rendering( const NodeParameter& parameter, const InstanceParameter& instanceParameter, void* userData ) {}

	virtual void EndRendering( const NodeParameter& parameter, void* userData ) {}
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
}
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif	// __EFFEKSEER_RIBBON_RENDERER_H__

#ifndef	__EFFEKSEER_RING_RENDERER_H__
#define	__EFFEKSEER_RING_RENDERER_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
namespace Effekseer
{
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------

class RingRenderer
{
public:

	struct NodeParameter
	{
		Effect*				EffectPointer;
		int32_t				ColorTextureIndex;
		eAlphaBlend			AlphaBlend;
		eTextureFilterType	TextureFilter;
		eTextureWrapType	TextureWrap;
		bool				ZTest;
		bool				ZWrite;
		eBillboardType		Billboard;
		int32_t				VertexCount;
	};

	struct InstanceParameter
	{
		Matrix43	SRTMatrix43;
		float		ViewingAngle;
		Vector2D	OuterLocation;
		Vector2D	InnerLocation;
		float		CenterRatio;
		Color		OuterColor;
		Color		CenterColor;
		Color		InnerColor;
		
		RectF	UV;
	};

public:
	RingRenderer() {}

	virtual ~RingRenderer() {}

	virtual void BeginRendering( const NodeParameter& parameter, int32_t count, void* userData ) {}

	virtual void Rendering( const NodeParameter& parameter, const InstanceParameter& instanceParameter, void* userData ) {}

	virtual void EndRendering( const NodeParameter& parameter, void* userData ) {}
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
}
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif	// __EFFEKSEER_RING_RENDERER_H__

#ifndef	__EFFEKSEER_MODEL_RENDERER_H__
#define	__EFFEKSEER_MODEL_RENDERER_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
namespace Effekseer
{
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------

class ModelRenderer
{
public:

	struct NodeParameter
	{
		Effect*				EffectPointer;
		eAlphaBlend			AlphaBlend;
		eTextureFilterType	TextureFilter;
		eTextureWrapType	TextureWrap;
		bool				ZTest;
		bool				ZWrite;
		bool				Lighting;
		eCullingType		Culling;
		int32_t				ModelIndex;
		int32_t				ColorTextureIndex;
		int32_t				NormalTextureIndex;
		float				Magnification;
		bool				IsRightHand;
	};

	struct InstanceParameter
	{
		Matrix43		SRTMatrix43;
		RectF			UV;
		Color			AllColor;
	};

public:
	ModelRenderer() {}

	virtual ~ModelRenderer() {}

	virtual void BeginRendering( const NodeParameter& parameter, int32_t count, void* userData ) {}

	virtual void Rendering( const NodeParameter& parameter, const InstanceParameter& instanceParameter, void* userData ) {}

	virtual void EndRendering( const NodeParameter& parameter, void* userData ) {}
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
}
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif	// __EFFEKSEER_MODEL_RENDERER_H__

#ifndef	__EFFEKSEER_TRACK_RENDERER_H__
#define	__EFFEKSEER_TRACK_RENDERER_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
namespace Effekseer
{
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------

class TrackRenderer
{
public:

	struct NodeParameter
	{
		Effect*				EffectPointer;
		int32_t				ColorTextureIndex;
		eAlphaBlend			AlphaBlend;
		eTextureFilterType	TextureFilter;
		eTextureWrapType	TextureWrap;
		bool				ZTest;
		bool				ZWrite;
	};

	struct InstanceGroupParameter
	{
		
	};

	struct InstanceParameter
	{
		int32_t			InstanceCount;
		int32_t			InstanceIndex;
		Matrix43		SRTMatrix43;

		Color	ColorLeft;
		Color	ColorCenter;
		Color	ColorRight;

		Color	ColorLeftMiddle;
		Color	ColorCenterMiddle;
		Color	ColorRightMiddle;

		float	SizeFor;
		float	SizeMiddle;
		float	SizeBack;
	};

public:
	TrackRenderer() {}

	virtual ~TrackRenderer() {}

	virtual void BeginRendering( const NodeParameter& parameter, int32_t count, void* userData ) {}

	virtual void Rendering( const NodeParameter& parameter, const InstanceParameter& instanceParameter, void* userData ) {}

	virtual void EndRendering( const NodeParameter& parameter, void* userData ) {}
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
}
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif	// __EFFEKSEER_TRACK_RENDERER_H__

#ifndef	__EFFEKSEER_EFFECTLOADER_H__
#define	__EFFEKSEER_EFFECTLOADER_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
namespace Effekseer { 
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
/**
	@brief	�G�t�F�N�g�t�@�C���ǂݍ��ݔj���֐��w��N���X
*/
class EffectLoader
{
public:
	/**
		@brief	�R���X�g���N�^
	*/
	EffectLoader() {}

	/**
		@brief	�f�X�g���N�^
	*/
	virtual ~EffectLoader() {}

	/**
		@brief	�G�t�F�N�g�t�@�C����ǂݍ��ށB
		@param	path	[in]	�ǂݍ��݌��p�X
		@param	data	[out]	�f�[�^�z��̐擪�̃|�C���^���o�͂����
		@param	size	[out]	�f�[�^�z��̒������o�͂����
		@return	����
		@note
		�G�t�F�N�g�t�@�C����ǂݍ��ށB
		::Effekseer::Effect::Create���s���Ɏg�p�����B
	*/
	virtual bool Load( const EFK_CHAR* path, void*& data, int32_t& size ) = 0;

	/**
		@brief	�G�t�F�N�g�t�@�C����j������B
		@param	data	[in]	�f�[�^�z��̐擪�̃|�C���^
		@param	size	[int]	�f�[�^�z��̒���
		@note
		�G�t�F�N�g�t�@�C����j������B
		::Effekseer::Effect::Create���s�I�����Ɏg�p�����B
	*/
	virtual void Unload( void* data, int32_t size ) = 0;
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
 } 
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif	// __EFFEKSEER_EFFECTLOADER_H__

#ifndef	__EFFEKSEER_TEXTURELOADER_H__
#define	__EFFEKSEER_TEXTURELOADER_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
namespace Effekseer { 
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
/**
	@brief	�e�N�X�`���ǂݍ��ݔj���֐��w��N���X
*/
class TextureLoader
{
public:
	/**
		@brief	�R���X�g���N�^
	*/
	TextureLoader() {}

	/**
		@brief	�f�X�g���N�^
	*/
	virtual ~TextureLoader() {}

	/**
		@brief	�e�N�X�`����ǂݍ��ށB
		@param	path	[in]	�ǂݍ��݌��p�X
		@return	�e�N�X�`���̃|�C���^
		@note
		�e�N�X�`����ǂݍ��ށB
		::Effekseer::Effect::Create���s���Ɏg�p�����B
	*/
	virtual void* Load( const EFK_CHAR* path ) { return NULL; }

	/**
		@brief	�e�N�X�`����j������B
		@param	data	[in]	�e�N�X�`��
		@note
		�e�N�X�`����j������B
		::Effekseer::Effect�̃C���X�^���X���j�����ꂽ���Ɏg�p�����B
	*/
	virtual void Unload( void* data ) {}
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
 } 
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif	// __EFFEKSEER_TEXTURELOADER_H__

#ifndef	__EFFEKSEER_MODELLOADER_H__
#define	__EFFEKSEER_MODELLOADER_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
namespace Effekseer { 
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
/**
	@brief	���f���ǂݍ��ݔj���֐��w��N���X
*/
class ModelLoader
{
public:
	/**
		@brief	�R���X�g���N�^
	*/
	ModelLoader() {}

	/**
		@brief	�f�X�g���N�^
	*/
	virtual ~ModelLoader() {}

	/**
		@brief	���f����ǂݍ��ށB
		@param	path	[in]	�ǂݍ��݌��p�X
		@return	���f���̃|�C���^
		@note
		���f����ǂݍ��ށB
		::Effekseer::Effect::Create���s���Ɏg�p�����B
	*/
	virtual void* Load( const EFK_CHAR* path ) { return NULL; }

	/**
		@brief	���f����j������B
		@param	data	[in]	���f��
		@note
		���f����j������B
		::Effekseer::Effect�̃C���X�^���X���j�����ꂽ���Ɏg�p�����B
	*/
	virtual void Unload( void* data ) {}
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
 } 
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif	// __EFFEKSEER_MODELLOADER_H__

#ifndef	__EFFEKSEER_MODEL_H__
#define	__EFFEKSEER_MODEL_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
namespace Effekseer { 
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
/**
	@brief	���f���N���X
*/
class Model
{
public:
	struct Vertex
	{
		Vector3D Position;
		Vector3D Normal;
		Vector3D Binormal;
		Vector3D Tangent;
		Vector2D UV;
	};

	struct VertexWithIndex
	{
		Vector3D Position;
		Vector3D Normal;
		Vector3D Binormal;
		Vector3D Tangent;
		Vector2D UV;
		uint8_t Index[4];
	};

	struct Face
	{
		int32_t	Indexes[3];
	};

	struct Emitter
	{
		Vector3D	Position;
		Vector3D	Normal;
		Vector3D	Binormal;
		Vector3D	Tangent;
	};

private:
	uint8_t*	m_data;
	int32_t		m_size;

	int32_t		m_version;

	int32_t		m_vertexCount;
	Vertex*		m_vertexes;

	int32_t		m_faceCount;
	Face*		m_faces;

	int32_t		m_modelCount;

public:
	/**
		@brief	�R���X�g���N�^
	*/
	Model( void* data, int32_t size ) 
		: m_data	( NULL )
		, m_size	( size )
		, m_version	( 0 )
		, m_vertexCount	( 0 )
		, m_vertexes	( NULL )
		, m_faceCount	( 0 )
		, m_faces		( NULL )
	{
		m_data = new uint8_t[m_size];
		memcpy( m_data, data, m_size );

		uint8_t* p = (uint8_t*)m_data;
	
		memcpy( &m_version, p, sizeof(int32_t) );
		p += sizeof(int32_t);

		memcpy( &m_modelCount, p, sizeof(int32_t) );
		p += sizeof(int32_t);

		memcpy( &m_vertexCount, p, sizeof(int32_t) );
		p += sizeof(int32_t);

		m_vertexes = (Vertex*)p;
		p += ( sizeof(Vertex) * m_vertexCount );

		memcpy( &m_faceCount, p, sizeof(int32_t) );
		p += sizeof(int32_t);

		m_faces = (Face*)p;
		p += ( sizeof(Face) * m_faceCount );
	}

	Vertex* GetVertexes() const { return m_vertexes; }
	int32_t GetVertexCount() { return m_vertexCount; }

	Face* GetFaces() const { return m_faces; }
	int32_t GetFaceCount() { return m_faceCount; }

	int32_t GetModelCount() { return m_modelCount; }

	/**
		@brief	�f�X�g���N�^
	*/
	virtual ~Model()
	{
		ES_SAFE_DELETE_ARRAY( m_data );
	}

	Emitter GetEmitter( Manager* manager, eCoordinateSystem coordinate, float magnification )
	{
		RandFunc randFunc = manager->GetRandFunc();
		int32_t randMax = manager->GetRandMax();

		int32_t faceInd = (int32_t)( (GetFaceCount() - 1) * ( (float)randFunc() / (float)randMax ) );
		faceInd = Clamp( faceInd, GetFaceCount() - 1, 0 );
		Face& face = GetFaces()[faceInd];
		Vertex& v0 = GetVertexes()[face.Indexes[0]];
		Vertex& v1 = GetVertexes()[face.Indexes[1]];
		Vertex& v2 = GetVertexes()[face.Indexes[2]];

		float p1 = ( (float)randFunc() / (float)randMax );
		float p2 = ( (float)randFunc() / (float)randMax );

		/* �ʓ��Ɏ��߂� */
		if( p1 + p2 > 1.0f )
		{
			p1 = 1.0f - p1;
			p2 = 1.0f - p2;			
		}

		float p0 = 1.0f - p1 - p2;
		
		Emitter emitter;
		emitter.Position = (v0.Position * p0 + v1.Position * p1 + v2.Position * p2) * magnification;
		emitter.Normal = v0.Normal * p0 + v1.Normal * p1 + v2.Normal * p2;
		emitter.Binormal = v0.Binormal * p0 + v1.Binormal * p1 + v2.Binormal * p2;
		emitter.Tangent = v0.Tangent * p0 + v1.Tangent * p1 + v2.Tangent * p2;

		if( coordinate == COORDINATE_SYSTEM_LH )
		{
			emitter.Position.Z = - emitter.Position.Z;
			emitter.Normal.Z = - emitter.Normal.Z;
			emitter.Binormal.Z = - emitter.Binormal.Z;
			emitter.Tangent.Z = - emitter.Tangent.Z;
		}

		return emitter;
	}

	Emitter GetEmitterFromVertex( Manager* manager, eCoordinateSystem coordinate, float magnification )
	{
		RandFunc randFunc = manager->GetRandFunc();
		int32_t randMax = manager->GetRandMax();

		int32_t vertexInd = (int32_t)( (GetVertexCount() - 1) * ( (float)randFunc() / (float)randMax ) );
		vertexInd = Clamp( vertexInd, GetVertexCount() - 1, 0 );
		Vertex& v = GetVertexes()[vertexInd];
		
		Emitter emitter;
		emitter.Position = v.Position * magnification;
		emitter.Normal = v.Normal;
		emitter.Binormal = v.Binormal;
		emitter.Tangent = v.Tangent;

		if( coordinate == COORDINATE_SYSTEM_LH )
		{
			emitter.Position.Z = - emitter.Position.Z;
			emitter.Normal.Z = - emitter.Normal.Z;
			emitter.Binormal.Z = - emitter.Binormal.Z;
			emitter.Tangent.Z = - emitter.Tangent.Z;
		}

		return emitter;
	}

	Emitter GetEmitterFromVertex( int32_t index, eCoordinateSystem coordinate, float magnification )
	{
		int32_t vertexInd = index % GetVertexCount();
		Vertex& v = GetVertexes()[vertexInd];
		
		Emitter emitter;
		emitter.Position = v.Position * magnification;
		emitter.Normal = v.Normal;
		emitter.Binormal = v.Binormal;
		emitter.Tangent = v.Tangent;

		if( coordinate == COORDINATE_SYSTEM_LH )
		{
			emitter.Position.Z = - emitter.Position.Z;
			emitter.Normal.Z = - emitter.Normal.Z;
			emitter.Binormal.Z = - emitter.Binormal.Z;
			emitter.Tangent.Z = - emitter.Tangent.Z;
		}

		return emitter;
	}

	Emitter GetEmitterFromFace( Manager* manager, eCoordinateSystem coordinate, float magnification )
	{
		RandFunc randFunc = manager->GetRandFunc();
		int32_t randMax = manager->GetRandMax();

		int32_t faceInd = (int32_t)( (GetFaceCount() - 1) * ( (float)randFunc() / (float)randMax ) );
		faceInd = Clamp( faceInd, GetFaceCount() - 1, 0 );
		Face& face = GetFaces()[faceInd];
		Vertex& v0 = GetVertexes()[face.Indexes[0]];
		Vertex& v1 = GetVertexes()[face.Indexes[1]];
		Vertex& v2 = GetVertexes()[face.Indexes[2]];

		float p0 = 1.0f / 3.0f;
		float p1 = 1.0f / 3.0f;
		float p2 = 1.0f / 3.0f;

		Emitter emitter;
		emitter.Position = (v0.Position * p0 + v1.Position * p1 + v2.Position * p2) * magnification;
		emitter.Normal = v0.Normal * p0 + v1.Normal * p1 + v2.Normal * p2;
		emitter.Binormal = v0.Binormal * p0 + v1.Binormal * p1 + v2.Binormal * p2;
		emitter.Tangent = v0.Tangent * p0 + v1.Tangent * p1 + v2.Tangent * p2;

		if( coordinate == COORDINATE_SYSTEM_LH )
		{
			emitter.Position.Z = - emitter.Position.Z;
			emitter.Normal.Z = - emitter.Normal.Z;
			emitter.Binormal.Z = - emitter.Binormal.Z;
			emitter.Tangent.Z = - emitter.Tangent.Z;
		}

		return emitter;
	}

	Emitter GetEmitterFromFace( int32_t index, eCoordinateSystem coordinate, float magnification )
	{
		int32_t faceInd = index % (GetFaceCount() - 1);
		Face& face = GetFaces()[faceInd];
		Vertex& v0 = GetVertexes()[face.Indexes[0]];
		Vertex& v1 = GetVertexes()[face.Indexes[1]];
		Vertex& v2 = GetVertexes()[face.Indexes[2]];

		float p0 = 1.0f / 3.0f;
		float p1 = 1.0f / 3.0f;
		float p2 = 1.0f / 3.0f;

		Emitter emitter;
		emitter.Position = (v0.Position * p0 + v1.Position * p1 + v2.Position * p2) * magnification;
		emitter.Normal = v0.Normal * p0 + v1.Normal * p1 + v2.Normal * p2;
		emitter.Binormal = v0.Binormal * p0 + v1.Binormal * p1 + v2.Binormal * p2;
		emitter.Tangent = v0.Tangent * p0 + v1.Tangent * p1 + v2.Tangent * p2;

		if( coordinate == COORDINATE_SYSTEM_LH )
		{
			emitter.Position.Z = - emitter.Position.Z;
			emitter.Normal.Z = - emitter.Normal.Z;
			emitter.Binormal.Z = - emitter.Binormal.Z;
			emitter.Tangent.Z = - emitter.Tangent.Z;
		}

		return emitter;
	}
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
 } 
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif	// __EFFEKSEER_MODEL_H__

#ifndef	__EFFEKSEER_SOUND_PLAYER_H__
#define	__EFFEKSEER_SOUND_PLAYER_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
namespace Effekseer
{
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------

typedef void* SoundHandle;
typedef void* SoundTag;

class SoundPlayer
{
public:
	struct InstanceParameter
	{
		void*		Data;
		float		Volume;
		float		Pan;
		float		Pitch;
		bool		Mode3D;
		Vector3D	Position;
		float		Distance;
	};

public:
	SoundPlayer() {}

	virtual ~SoundPlayer() {}

	virtual SoundHandle Play( SoundTag tag, const InstanceParameter& parameter ) = 0;
	
	virtual void Stop( SoundHandle handle, SoundTag tag ) = 0;

	virtual void Pause( SoundHandle handle, SoundTag tag, bool pause ) = 0;

	virtual bool CheckPlaying( SoundHandle handle, SoundTag tag ) = 0;

	virtual void StopTag( SoundTag tag ) = 0;

	virtual void PauseTag( SoundTag tag, bool pause ) = 0;

	virtual bool CheckPlayingTag( SoundTag tag ) = 0;

	virtual void StopAll() = 0;
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
}
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif	// __EFFEKSEER_SOUND_PLAYER_H__

#ifndef	__EFFEKSEER_SOUNDLOADER_H__
#define	__EFFEKSEER_SOUNDLOADER_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
namespace Effekseer {
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
/**
	@brief	�T�E���h�ǂݍ��ݔj���֐��w��N���X
*/
class SoundLoader
{
public:
	/**
		@brief	�R���X�g���N�^
	*/
	SoundLoader() {}

	/**
		@brief	�f�X�g���N�^
	*/
	virtual ~SoundLoader() {}

	/**
		@brief	�T�E���h��ǂݍ��ށB
		@param	path	[in]	�ǂݍ��݌��p�X
		@return	�T�E���h�̃|�C���^
		@note
		�T�E���h��ǂݍ��ށB
		::Effekseer::Effect::Create���s���Ɏg�p�����B
	*/
	virtual void* Load( const EFK_CHAR* path ) { return NULL; }

	/**
		@brief	�T�E���h��j������B
		@param	data	[in]	�T�E���h
		@note
		�T�E���h��j������B
		::Effekseer::Effect�̃C���X�^���X���j�����ꂽ���Ɏg�p�����B
	*/
	virtual void Unload( void* source ) {}
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
 } 
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif	// __EFFEKSEER_SOUNDLOADER_H__

#ifndef	__EFFEKSEER_LOADER_H__
#define	__EFFEKSEER_LOADER_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
namespace Effekseer { 
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
/**
	@brief	�ݒ�N���X
	@note
	EffectLoader���A�t�@�C���ǂݍ��݂Ɋւ���ݒ肷�邱�Ƃ��ł���B
	Manager�̑���ɃG�t�F�N�g�ǂݍ��ݎ��Ɏg�p���邱�ƂŁAManager�Ƃ͓Ɨ�����Effect�C���X�^���X�𐶐����邱�Ƃ��ł���B
*/
	class Setting
	{
	private:
		int32_t		m_ref;

		/* ���W�n */
		eCoordinateSystem		m_coordinateSystem;

		EffectLoader*	m_effectLoader;
		TextureLoader*	m_textureLoader;
		SoundLoader*	m_soundLoader;
		ModelLoader*	m_modelLoader;

		/**
			@brief	�R���X�g���N�^
			*/
		Setting();

		/**
			@brief	�f�X�g���N�^
			*/
		 ~Setting();
	public:

		/**
			@brief	�ݒ�C���X�^���X�𐶐�����B
		*/
		static Setting* Create();

		/**
			@brief	�Q�ƃJ�E���^�����Z����B
			@return	�Q�ƃJ�E���^
		*/
		int32_t AddRef();

		/**
			@brief	�Q�ƃJ�E���^�����Z����B
			@return	�Q�ƃJ�E���^
		*/
		int32_t Release();

		/**
		@brief	���W�n���擾����B
		@return	���W�n
		*/
		eCoordinateSystem GetCoordinateSystem() const;

		/**
		@brief	���W�n��ݒ肷��B
		@param	coordinateSystem	[in]	���W�n
		@note
		���W�n��ݒ肷��B
		�G�t�F�N�g�t�@�C����ǂݍ��ޑO�ɐݒ肷��K�v������B
		*/
		void SetCoordinateSystem(eCoordinateSystem coordinateSystem);

		/**
			@brief	�G�t�F�N�g���[�_�[���擾����B
			@return	�G�t�F�N�g���[�_�[
			*/
		EffectLoader* GetEffectLoader();

		/**
			@brief	�G�t�F�N�g���[�_�[��ݒ肷��B
			@param	loader	[in]		���[�_�[
			*/
		void SetEffectLoader(EffectLoader* loader);

		/**
			@brief	�e�N�X�`�����[�_�[���擾����B
			@return	�e�N�X�`�����[�_�[
			*/
		TextureLoader* GetTextureLoader();

		/**
			@brief	�e�N�X�`�����[�_�[��ݒ肷��B
			@param	loader	[in]		���[�_�[
			*/
		void SetTextureLoader(TextureLoader* loader);

		/**
			@brief	���f�����[�_�[���擾����B
			@return	���f�����[�_�[
			*/
		ModelLoader* GetModelLoader();

		/**
			@brief	���f�����[�_�[��ݒ肷��B
			@param	loader	[in]		���[�_�[
			*/
		void SetModelLoader(ModelLoader* loader);

		/**
			@brief	�T�E���h���[�_�[���擾����B
			@return	�T�E���h���[�_�[
			*/
		SoundLoader* GetSoundLoader();

		/**
			@brief	�T�E���h���[�_�[��ݒ肷��B
			@param	loader	[in]		���[�_�[
			*/
		void SetSoundLoader(SoundLoader* loader);
	};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
 } 
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif	// __EFFEKSEER_LOADER_H__

#ifndef	__EFFEKSEER_SERVER_H__
#define	__EFFEKSEER_SERVER_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
namespace Effekseer {
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
class Server
{
public:

	Server() {}
	virtual ~Server() {}

	static Server* Create();

	/**
		@brief	�T�[�o�[���J�n����B
	*/
	virtual bool Start( uint16_t port ) = 0;

	virtual void Stop() = 0;

	/**
		@brief	�G�t�F�N�g�������[�h�̑ΏۂƂ��ēo�^����B
		@param	key	[in]	�����p�L�[
		@param	effect	[in]	�����[�h����Ώۂ̃G�t�F�N�g
	*/
	virtual void Regist( const EFK_CHAR* key, Effect* effect ) = 0;

	/**
		@brief	�G�t�F�N�g�������[�h�̑Ώۂ���O���B
		@param	effect	[in]	�����[�h����O���G�t�F�N�g
	*/
	virtual void Unregist( Effect* effect ) = 0;

	/**
		@brief	�T�[�o�[���X�V���A�G�t�F�N�g�̃����[�h���s���B
	*/
	virtual void Update() = 0;

	/**
		@brief	�f�ރp�X��ݒ肷��B
	*/
	virtual void SetMaterialPath( const EFK_CHAR* materialPath ) = 0;
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
 } 
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif	// __EFFEKSEER_SERVER_H__

#ifndef	__EFFEKSEER_CLIENT_H__
#define	__EFFEKSEER_CLIENT_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
namespace Effekseer {
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
class Client
{
public:
	Client() {}
	virtual ~Client() {}

	static Client* Create();

	virtual bool Start( char* host, uint16_t port ) = 0;
	virtual void Stop()= 0;

	virtual void Reload( const EFK_CHAR* key, void* data, int32_t size ) = 0;
	virtual void Reload( Manager* manager, const EFK_CHAR* path, const EFK_CHAR* key ) = 0;
	virtual bool IsConnected() = 0;
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
 } 
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif	// __EFFEKSEER_CLIENT_H__

#ifndef	__EFFEKSEER_CRITICALSESSION_H__
#define	__EFFEKSEER_CRITICALSESSION_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#include <pthread.h>
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
/**
	@brief	�N���e�B�J���Z�N�V����
*/
class CriticalSection
{
private:
#ifdef _WIN32
	mutable CRITICAL_SECTION m_criticalSection;
#else
	mutable pthread_mutex_t m_mutex;
#endif

public:

	CriticalSection();

	~CriticalSection();

	void Enter() const;

	void Leave() const;
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
}
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif	//	__EFFEKSEER_CRITICALSESSION_H__

#ifndef	__EFFEKSEER_THREAD_H__
#define	__EFFEKSEER_THREAD_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------

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
