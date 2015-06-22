
#ifndef	__EFFEKSEER_VECTOR3D_H__
#define	__EFFEKSEER_VECTOR3D_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include "Base.h"

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
