#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* lib
#include <Lib/Geometry/Vector3.h>
#include <Lib/Geometry/Quaternion.h>
#include <Lib/Geometry/Matrix4x4.h>
#include <Lib/Geometry/MathLib.h>

////////////////////////////////////////////////////////////////////////////////////////////
// EulerTransform structure
////////////////////////////////////////////////////////////////////////////////////////////
struct EulerTransform {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	EulerTransform() = default;

	void SetImGuiCommand(float granularityTranslate = 0.01f, float granularityRotate = 1.0f, float granularityScale = 0.01f);

	Matrix4x4 ToMatrix() const;

	//=========================================================================================
	// public variables
	//=========================================================================================

	Vector3f scale     = kUnit3<float>;
	Vector3f rotate    = kOrigin3<float>;
	Vector3f translate = kOrigin3<float>;
};

////////////////////////////////////////////////////////////////////////////////////////////
// QuaternionTransform structure
////////////////////////////////////////////////////////////////////////////////////////////
struct QuaternionTransform {

	//=========================================================================================
	// public methods
	//=========================================================================================

	QuaternionTransform() = default;

	void SetImGuiCommand(float granularityTranslate = 0.01f, float granularityRotate = 0.01f, float granularityScale = 0.01f);

	Matrix4x4 ToMatrix() const;

	//=========================================================================================
	// public variables
	//=========================================================================================

	Vector3f scale     = kUnit3<float>;
	Quaternion rotate  = Quaternion::Identity();
	Vector3f translate = kOrigin3<float>;
};

////////////////////////////////////////////////////////////////////////////////////////////
// TransformationMatrix structure
////////////////////////////////////////////////////////////////////////////////////////////
struct TransformationMatrix {

	//=========================================================================================
	// public methods
	//=========================================================================================

	TransformationMatrix() = default;

	void Init();

	void Transfer(const Matrix4x4& _mat);

	//=========================================================================================
	// public variables
	//=========================================================================================

	Matrix4x4 mat                 = Matrix4x4::Identity();
	Matrix4x4 matInverseTranspose = Matrix4x4::Identity(); //!< 非均一スケール用
};