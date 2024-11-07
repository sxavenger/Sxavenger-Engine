#pragma once
//* ConstantBuffer用のTransformクラス

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
// Geometry
#include <Lib/Geometry/Vector3.h>
#include <Lib/Geometry/Quaternion.h>
#include <Lib/Geometry/Matrix4x4.h>
#include <Lib/Geometry/MathLib.h>

// DxObject
#include <Engine/System/DxObject/DxBufferResource.h>

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
		: scale(kUnit3), rotate(kOrigin3), translate(kOrigin3) {
	}

	void SetImGuiCommand(float granularity = 0.01f) {
		ImGui::DragFloat3("scale", &scale.x, granularity);

		Vector3f deg = rotate * kRadToDeg;
		if (ImGui::DragFloat3("rotate", &deg.x, 1.0f, 0.0f, 0.0f, "%.0f deg")) {
			rotate = deg * kDegToRad;
		}

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
		: scale(kUnit3), rotate(Quaternion::Identity()), translate(kOrigin3) {
	}

	// todo: SetImGuiCommand()
	void SetImGuiCommand(float granularity = 0.01f) {
		ImGui::DragFloat3("scale", &scale.x, granularity);

		Vector3f drag = {};
		if (ImGui::DragFloat3("rotation drager", &drag.x, 0.01f, -1.0f, 1.0f, "", ImGuiSliderFlags_NoInput)) {
			rotate *= ToQuaternion(drag);
		}

		ImGui::DragFloat3("translate", &translate.x, granularity);
	}

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

	void Transfer(const Matrix4x4& world) {
		worldMatrix           = world;
		worldInverceTranspose = world.Inverse().Transpose();
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

	void Create();

	void Transfer();

	//* Getter *//

	const D3D12_GPU_VIRTUAL_ADDRESS GetGPUVirtualAddress() const;

	const Matrix4x4& GetWorldMatrix() const { return mat_; }

	const Vector3f GetWorldPosition() const;

	//* Setter *//

	void SetParent(const BaseTransformBuffer* parent) { parent_ = parent; }

	//=========================================================================================
	// public variables
	//=========================================================================================

	Matrix4x4 mat_ = Matrix4x4::Identity();

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
	~EulerTransformBuffer() override = default;

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
	~QuaternionTransformBuffer() override = default;

	void UpdateMatrix();

	void SetImGuiCommand();

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

////////////////////////////////////////////////////////////////////////////////////////////
// UVTransform structure
////////////////////////////////////////////////////////////////////////////////////////////
struct UVTransform {

	//=========================================================================================
	// member
	//=========================================================================================

	Vector2f scale;
	float    rotate;
	Vector2f translate;

	//=========================================================================================
	// methods
	//=========================================================================================

	UVTransform()
		: scale({ 1.0f, 1.0f }), rotate(0.0f), translate({ 0.0f, 0.0f }) {
	}

	Matrix4x4 ToMatrix() const {
		return Matrix::MakeAffine({ scale.x, scale.y, 1.0f }, { 0.0f, 0.0f, rotate }, { translate.x, translate.y, 0.0f });
	}

	void SetImGuiCommand() {
		ImGui::DragFloat2("scale", &scale.x, 0.01f);
		ImGui::SliderAngle("rotate", &rotate);
		ImGui::DragFloat2("translate", &translate.x, 0.01f);
	}
};

////////////////////////////////////////////////////////////////////////////////////////////
// UVTransformationMatrix structure
////////////////////////////////////////////////////////////////////////////////////////////
struct UVTransformationMatrix {
	Matrix4x4 mat;
};

////////////////////////////////////////////////////////////////////////////////////////////
// UVTransformBuffer class
////////////////////////////////////////////////////////////////////////////////////////////
class UVTransformBuffer {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	void Create();

	void Transfer();

	void SetImGuiCommand();

	//* getter *//

	const D3D12_GPU_VIRTUAL_ADDRESS GetVirtualAddress() const { return buffer_->GetGPUVirtualAddress(); }

	//=========================================================================================
	// public variables
	//=========================================================================================

	UVTransform transform;

private:

	//=========================================================================================
	// public methods
	//=========================================================================================

	std::unique_ptr<DxObject::BufferResource<UVTransformationMatrix>> buffer_;

};