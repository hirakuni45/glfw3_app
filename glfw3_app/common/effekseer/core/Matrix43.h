
#ifndef	__EFFEKSEER_MATRIX43_H__
#define	__EFFEKSEER_MATRIX43_H__

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
