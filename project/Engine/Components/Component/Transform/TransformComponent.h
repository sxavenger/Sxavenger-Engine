#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* component
#include "../BaseComponent.h"

//* engine
#include <Engine/Foundation.h>
#include <Engine/System/DirectX/DxObject/DxDimensionBuffer.h>

//* lib
#include <Lib/Math/Matrix4x4.h>
#include <Lib/Transform/Transformation.h>
#include <Lib/Transform/TransformationMatrix.h>
#include <Lib/Transform/TransformQuaternion.h>

//* c++
#include <memory>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// TransformComponent class
////////////////////////////////////////////////////////////////////////////////////////////
//! @brief 3D空間の変換情報component.
class TransformComponent final
	: public BaseComponent {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	TransformComponent(EntityBehaviour* behaviour);
	~TransformComponent() override = default;

	//* component option *//

	void ShowComponentInspector() override;

	//* buffer option *//

	const D3D12_GPU_VIRTUAL_ADDRESS GetGPUVirtualAddress() const;

	//* transform option *//

	//! @brief 行列を更新
	void UpdateMatrix();

	//! @brief 親が存在するか確認
	bool HasParent() const;

	const TransformQuaternion& GetTransform() const { return transform_; }
	TransformQuaternion& GetTransform() { return transform_; }

	const Transformation& GetTransformation() const { return transformation_; }

	const TransformationMatrix& GetTransformationMatrix() const;

	const Matrix4x4& GetMatrix() const;

	const Vector3f GetPosition() const;

	const Vector3f GetDirection() const;

	//* operator [accessor] *//

	const TransformQuaternion* operator->() const { return &transform_; }
	TransformQuaternion* operator->() { return &transform_; }

	const TransformQuaternion& operator*() const { return transform_; }
	TransformQuaternion& operator*() { return transform_; }

	//* json option *//

	json ParseToJson() const override;

	void InputJson(const json& data) override;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* transformation *//

	TransformQuaternion transform_ = {};
	Transformation transformation_ = {};

	//* buffer *//

	std::unique_ptr<DxObject::ConstantBuffer<TransformationMatrix>> buffer_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	void TransferGPU();

};

SXAVENGER_ENGINE_NAMESPACE_END
