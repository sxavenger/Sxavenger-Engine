#pragma once
//* ConstantBuffer用のTransformクラス

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
// Geometry
#include <Vector3.h>
#include <Vector4.h>
#include <Quaternion.h>
#include <Matrix4x4.h>

// DxObject
#include <DxBufferResource.h>

// c++
#include <memory>

////////////////////////////////////////////////////////////////////////////////////////////
// TransformationMatrix structure
////////////////////////////////////////////////////////////////////////////////////////////
struct TransformationMat { //!< TransformationMatrix structureと名前がかぶるため仮でこの名前
	//!< member書き換え不可
	Matrix4x4 worldMatrix;
	Matrix4x4 worldInverceTranspose; //! 非均一スケール用

	void Init() {
		worldMatrix           = Matrix4x4::Identity();
		worldInverceTranspose = Matrix4x4::Identity();
	}
};

////////////////////////////////////////////////////////////////////////////////////////////
// BaseTransform base class
////////////////////////////////////////////////////////////////////////////////////////////
class BaseTransform {
	//!< このクラス単体では使わない
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	BaseTransform() = default;
	virtual ~BaseTransform() {}

	void Init();

	//* Getter *//

	const D3D12_GPU_VIRTUAL_ADDRESS GetGPUVirtualAddress() const {
		return buffer_->GetGPUVirtualAddress();
	}

	const Matrix4x4& GetWorldMatrix() const {
		return (*buffer_)[0].worldMatrix;
	}

	//* Setter *//

	void SetParent(const BaseTransform* parent) { parent_ = parent; }

protected:

	//=========================================================================================
	// protected variables
	//=========================================================================================

	//* GPUbuffer *//
	std::unique_ptr<DxObject::BufferResource<TransformationMat>> buffer_;

	//* parent *//
	const BaseTransform* parent_ = nullptr;

};

////////////////////////////////////////////////////////////////////////////////////////////
// TransformEuler class
////////////////////////////////////////////////////////////////////////////////////////////
class TransformEuler
	: public BaseTransform {
	//!< EulerTransform structureがなくなった場合, 名前を変更
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	TransformEuler() = default;
	~TransformEuler() override { Term(); }

	void Init();

	void Term();

	void UpdateMatrix();

	//=========================================================================================
	// public variables
	//=========================================================================================

	//* transforms *//

	Vector3f scale       = unitVector;
	Vector3f rotation    = origin;
	Vector3f translation = origin;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

};

////////////////////////////////////////////////////////////////////////////////////////////
// TransformQuaternion class
////////////////////////////////////////////////////////////////////////////////////////////
class TransformQuaternion
	: public BaseTransform {
	//!< QuaternionTransform structureがなくなった場合, 名前を変更
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	TransformQuaternion() = default;
	~TransformQuaternion() override { Term(); }

	void Init();

	void Term();

	void UpdateMatrix();

	//=========================================================================================
	// public variables
	//=========================================================================================

	//* transforms *//

	Vector3f   scale       = unitVector;
	Quaternion quaternion  = {};
	Vector3f   translation = origin;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

};