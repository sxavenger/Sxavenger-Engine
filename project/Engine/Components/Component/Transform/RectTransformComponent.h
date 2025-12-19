#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* component
#include "../BaseComponent.h"

//* engine
#include <Engine/Foundation.h>
#include <Engine/System/DirectX/DxObject/DxConstantBuffer.h>

//* lib
#include <Lib/Geometry/Matrix4x4.h>
#include <Lib/Transform/Transform.h>

//* c++
#include <memory>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// RectTransformComponent class
////////////////////////////////////////////////////////////////////////////////////////////
//! @brief 2D空間の変換情報component.
class RectTransformComponent final
	: public BaseComponent {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	RectTransformComponent(MonoBehaviour* behaviour);
	~RectTransformComponent() override = default;

	//* component option *//

	void ShowComponentInspector() override;

	//* buffer option *//

	const D3D12_GPU_VIRTUAL_ADDRESS& GetGPUVirtualAddress() const;

	//* transform option *//

	//! @brief 行列を更新
	void UpdateMatrix();

	//! @brief 親が存在するか確認
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

SXAVENGER_ENGINE_NAMESPACE_END
