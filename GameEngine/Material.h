#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* Geometry
#include <Vector2.h>
#include <Vector4.h>
#include <Matrix4x4.h>

//* DxObject
#include <DxBufferResource.h>

//* c++
#include <memory>

//* imgui
#include <imgui.h>

////////////////////////////////////////////////////////////////////////////////////////////
// UVTransform structure
////////////////////////////////////////////////////////////////////////////////////////////
struct UVTransform {
	Vector2f scale;
	float    rotate;
	Vector2f translate;

	UVTransform()
		: scale({1.0f, 1.0f}), rotate(0.0f), translate({0.0f, 0.0f}) { }

	Matrix4x4 ToMatrix() const {
		return Matrix::MakeAffine({scale.x, scale.y, 1.0f}, {0.0f, 0.0f, rotate}, {translate.x, translate.y, 0.0f});
	}

	void SetImGuiCommand() {
		ImGui::DragFloat2("scale",     &scale.x, 0.01f);
		ImGui::SliderAngle("rotate",   &rotate);
		ImGui::DragFloat2("translate", &translate.x, 0.01f);
	}
};

////////////////////////////////////////////////////////////////////////////////////////////
// ObjectMaterial class
////////////////////////////////////////////////////////////////////////////////////////////
class ObjectMaterialBuffer {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	ObjectMaterialBuffer()  = default;
	~ObjectMaterialBuffer() = default;

	void Init();

	void Transfer();

	const D3D12_GPU_VIRTUAL_ADDRESS GetGPUVirtualAddress() const {
		return buffer_->GetGPUVirtualAddress();
	}

	//=========================================================================================
	// public variables
	//=========================================================================================

	Color4f     color = { 1.0f, 1.0f, 1.0f, 1.0f };
	UVTransform uvTransform;

private:

	////////////////////////////////////////////////////////////////////////////////////////////
	// ObjectMaterialGPU structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct ObjectMaterial {
		Color4f   color;
		Matrix4x4 uvTransform;

		void Init() {
			color       = {1.0f, 1.0f, 1.0f, 1.0f};
			uvTransform = Matrix4x4::Identity();
		}
	};

	//=========================================================================================
	// private variables
	//=========================================================================================

	std::unique_ptr<DxObject::BufferResource<ObjectMaterial>> buffer_;

};