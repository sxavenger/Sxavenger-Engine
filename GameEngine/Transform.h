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

// imgui
#include <imgui.h>

////////////////////////////////////////////////////////////////////////////////////////////
// EulerTransform structure
////////////////////////////////////////////////////////////////////////////////////////////
struct EulerTransform {
	Vector3f scale;
	Vector3f rotate;
	Vector3f translate;

	EulerTransform()
		: scale(unitVector), rotate(origin), translate(origin) {
	}

	void SetImGuiCommand(float granularity = 0.01f) {
		ImGui::DragFloat3("scale", &scale.x, granularity);
		ImGui::DragFloat3("rotate", &rotate.x, granularity);
		ImGui::DragFloat3("translate", &translate.x, granularity);
	}

	Matrix4x4 ToMatrix() const {
		return Matrix::MakeAffine(scale, rotate, translate);
	}
};

////////////////////////////////////////////////////////////////////////////////////////////
// QuaternionTransform structure
////////////////////////////////////////////////////////////////////////////////////////////
struct QuaternionTransform {
	Vector3f   scale;
	Quaternion rotate;
	Vector3f   translate;

	QuaternionTransform()
		: scale(unitVector), rotate({}), translate(origin) {
	}

	// todo: SetImGuiCommand()

	Matrix4x4 ToMatrix() const {
		return Matrix::MakeAffine(scale, rotate, translate);
	}

};

////////////////////////////////////////////////////////////////////////////////////////////
// TransformationMatrix structure
////////////////////////////////////////////////////////////////////////////////////////////
struct TransformationMatrix {
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
class BaseTransformBuffer {
	//!< このクラス単体では使わない
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	BaseTransformBuffer() = default;
	virtual ~BaseTransformBuffer() {}

	void Init();

	//* Getter *//

	const D3D12_GPU_VIRTUAL_ADDRESS GetGPUVirtualAddress() const {
		return buffer_->GetGPUVirtualAddress();
	}

	const Matrix4x4& GetWorldMatrix() const {
		return (*buffer_)[0].worldMatrix;
	}

	//* Setter *//

	void SetParent(const BaseTransformBuffer* parent) { parent_ = parent; }

protected:

	//=========================================================================================
	// protected variables
	//=========================================================================================

	//* GPUbuffer *//
	std::unique_ptr<DxObject::BufferResource<TransformationMatrix>> buffer_;

	//* parent *//
	const BaseTransformBuffer* parent_ = nullptr;

};

////////////////////////////////////////////////////////////////////////////////////////////
// EulerTransformBuffer class
////////////////////////////////////////////////////////////////////////////////////////////
class EulerTransformBuffer
	: public BaseTransformBuffer {
	//!< EulerTransform structureがなくなった場合, 名前を変更
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	EulerTransformBuffer() = default;
	~EulerTransformBuffer() override { Term(); }

	void Init();

	void Term();

	void UpdateMatrix();

	void SetImGuiCommand();

	//=========================================================================================
	// public variables
	//=========================================================================================

	//* transforms *//

	EulerTransform transform;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

};

////////////////////////////////////////////////////////////////////////////////////////////
// QuaternionTransformBuffer class
////////////////////////////////////////////////////////////////////////////////////////////
class QuaternionTransformBuffer
	: public BaseTransformBuffer {
	//!< QuaternionTransform structureがなくなった場合, 名前を変更
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	QuaternionTransformBuffer() = default;
	~QuaternionTransformBuffer() override { Term(); }

	void Init();

	void Term();

	void UpdateMatrix();

	//=========================================================================================
	// public variables
	//=========================================================================================

	//* transforms *//

	QuaternionTransform transform;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

};