
#ifndef	__EFFEKSEER_MATRIX44_H__
#define	__EFFEKSEER_MATRIX44_H__

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
