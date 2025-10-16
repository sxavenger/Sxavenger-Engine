#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* component
#include "../BaseComponent.h"

//* engine
#include <Engine/System/DirectX/DxObject/DxConstantBuffer.h>

//* lib
#include <Lib/Geometry/Matrix4x4.h>
#include <Lib/Transform/Transform.h>

//* c++
#include <memory>

////////////////////////////////////////////////////////////////////////////////////////////
// RectTransformComponent class
////////////////////////////////////////////////////////////////////////////////////////////
class RectTransformComponent final
	: public BaseComponent {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	RectTransformComponent(MonoBehaviour* behaviour) : BaseComponent(behaviour) { CreateBuffer(); }
	~RectTransformComponent() override = default;

	//* component option *//

	void ShowComponentInspector() override;

	//* buffer option *//

	void CreateBuffer();

	const D3D12_GPU_VIRTUAL_ADDRESS& GetGPUVirtualAddress() const;

	//* transform option *//

	void UpdateMatrix();

	bool HasParent() const;

	RectTransform& GetTransform() { return transform_; }
	const RectTransform& GetTransform() const { return transform_; }

	const Matrix4x4& GetMatrix() const { return mat_; }

	const Vector2f GetPosition() const;

	//* json option *//

	json PerseToJson() const override;

	void InputJson(const json& data) override;

	//=========================================================================================
	// public variables
	//=========================================================================================

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* transform *//

	Matrix4x4 mat_           = Matrix4x4::Identity();
	RectTransform transform_ = {};

	//* buffer *//

	std::unique_ptr<DxObject::ConstantBuffer<Matrix4x4>> buffer_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	void TransferGPU();

};
