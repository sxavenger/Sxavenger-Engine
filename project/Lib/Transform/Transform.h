#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* lib
#include <Lib/Geometry/Vector2.h>
#include <Lib/Geometry/Vector3.h>
#include <Lib/Geometry/Quaternion.h>
#include <Lib/Geometry/Matrix4x4.h>
#include <Lib/Geometry/GeometryMath.h>
#include <Lib/Adapter/Json/IJsonSerializer.h>

////////////////////////////////////////////////////////////////////////////////////////////
// EulerTransform structure
////////////////////////////////////////////////////////////////////////////////////////////
struct EulerTransform {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	EulerTransform()  = default;
	~EulerTransform() = default;

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
struct QuaternionTransform
	: public IJsonSerializer {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	QuaternionTransform()  = default;
	~QuaternionTransform() = default;

	void SetImGuiCommand(float granularityTranslate = 0.01f, float granularityRotate = 0.01f, float granularityScale = 0.01f);

	json PerseToJson() const override;

	void InputJson(const json& data) override;

	//* geometry option *//

	Matrix4x4 ToMatrix() const;

	Vector3f GetForward() const;

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

	TransformationMatrix()  = default;
	~TransformationMatrix() = default;

	void Init();

	void Transfer(const Matrix4x4& _mat);

	//=========================================================================================
	// public variables
	//=========================================================================================

	Matrix4x4 mat                 = Matrix4x4::Identity();
	Matrix4x4 matInverseTranspose = Matrix4x4::Identity(); //!< 非均一スケール用
};

////////////////////////////////////////////////////////////////////////////////////////////
// Transform2d structure
////////////////////////////////////////////////////////////////////////////////////////////
struct Transform2d
	: public IJsonSerializer {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	Transform2d()  = default;
	~Transform2d() = default;

	void SetImGuiCommand(float granularityTranslate = 0.01f, float granularityScale = 0.01f);

	Matrix4x4 ToMatrix() const;

	json PerseToJson() const override;

	void InputJson(const json& data) override;

	//=========================================================================================
	// public variables
	//=========================================================================================

	Vector2f scale     = kUnit2<float>;
	float    rotate    = 0.0f;
	Vector2f translate = kOrigin2<float>;

};
