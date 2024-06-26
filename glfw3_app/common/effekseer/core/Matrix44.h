
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
	@brief	s๑
	@note
	E่n<BR>
	ถ่n<BR>
	V[x,y,z,1] * M ฬ`<BR>
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
		@brief	RXgN^
	*/
	Matrix44();

	/**
		@brief	s๑ฬl
	*/
	float	Values[4][4];

	/**
		@brief	Pสs๑ป
	*/
	Matrix44& Indentity();

	/**
		@brief	Js๑ป(E่n)
	*/
	Matrix44& LookAtRH( const Vector3D& eye, const Vector3D& at, const Vector3D& up );

	/**
		@brief	Js๑ป(ถ่n)
	*/
	Matrix44& LookAtLH( const Vector3D& eye, const Vector3D& at, const Vector3D& up );

	/**
		@brief	หes๑ป(E่n)
	*/
	Matrix44& PerspectiveFovRH( float ovY, float aspect, float zn, float zf );

	/**
		@brief	OpenGLpหes๑ป(E่n)
	*/
	Matrix44& PerspectiveFovRH_OpenGL( float ovY, float aspect, float zn, float zf );

	/**
		@brief	หes๑ป(ถ่n)
	*/
	Matrix44& PerspectiveFovLH( float ovY, float aspect, float zn, float zf );
	
	/**
	 @brief	OpenGLpหes๑ป(ถ่n)
	 */
	Matrix44& PerspectiveFovLH_OpenGL( float ovY, float aspect, float zn, float zf );
	
	/**
		@brief	ณหes๑ป(E่n)
	*/
	Matrix44& OrthographicRH( float width, float height, float zn, float zf );

	/**
		@brief	ณหes๑ป(ถ่n)
	*/
	Matrix44& OrthographicLH( float width, float height, float zn, float zf );

	/**
		@brief	gๅs๑ป
	*/
	void Scaling( float x, float y, float z );

	/**
		@brief	Xฒ๑]s๑(E่)
	*/
	void RotationX( float angle );

	/**
		@brief	Yฒ๑]s๑(E่)
	*/
	void RotationY( float angle );

	/**
		@brief	Zฒ๑]s๑(E่)
	*/
	void RotationZ( float angle );

	/**
		@brief	ฺฎs๑
	*/
	void Translation( float x, float y, float z );

	/**
		@brief	Cำฒฝv๑]s๑
	*/
	void RotationAxis( const Vector3D& axis, float angle );

	/**
		@brief	NI[^jIฉ็s๑ษฯท
	*/
	void Quaternion( float x, float y, float z, float w );

	/**
		@brief	ๆZ
	*/
	static Matrix44& Mul( Matrix44& o, const Matrix44& in1, const Matrix44& in2 );

	/**
		@brief	ts๑
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
