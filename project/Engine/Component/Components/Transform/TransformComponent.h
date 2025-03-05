#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* component
#include "../../BaseComponent.h"

//* engine
#include <Engine/System/DirectX/DxObject/DxDimensionBuffer.h>

//* lib
#include <Lib/Geometry/Matrix4x4.h>
#include <Lib/Transform/Transform.h>

//* c++
#include <memory>

////////////////////////////////////////////////////////////////////////////////////////////
// TransformComponent class
////////////////////////////////////////////////////////////////////////////////////////////
class TransformComponent
	: public BaseComponent {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	TransformComponent(MonoBehaviour* behaviour) : BaseComponent(behaviour) {}
	~TransformComponent() override = default;

	//* buffer option *//

	void CreateBuffer();

	const D3D12_GPU_VIRTUAL_ADDRESS& GetGPUVirtualAddress() const;

	//* transform option *//

	void UpdateMatrix();

	const QuaternionTransform& GetTransform() const { return transform_; }
	QuaternionTransform& GetTransform() { return transform_; }

	const Matrix4x4& GetMatrix() const { return mat_; }

	const Vector3f GetPosition() const;

	const TransformationMatrix& GetTransformationMatrix() const;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* transform *//

	Matrix4x4 mat_                 = Matrix4x4::Identity();
	QuaternionTransform transform_ = {};

	//* buffer *//

	std::unique_ptr<DxObject::DimensionBuffer<TransformationMatrix>> buffer_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	void TransferGPU();

};
