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
// TransformComponent class
////////////////////////////////////////////////////////////////////////////////////////////
class TransformComponent final
	: public BaseComponent {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	TransformComponent(MonoBehaviour* behaviour) : BaseComponent(behaviour) { CreateBuffer(); }
	~TransformComponent() override = default;

	//* component option *//

	void ShowComponentInspector() override;

	//* buffer option *//

	void CreateBuffer();

	const D3D12_GPU_VIRTUAL_ADDRESS& GetGPUVirtualAddress() const;

	//* transform option *//

	void UpdateMatrix();

	bool HasParent() const;

	const QuaternionTransform& GetTransform() const { return transform_; }
	QuaternionTransform& GetTransform() { return transform_; }

	const Matrix4x4& GetMatrix() const { return mat_; }

	const Vector3f GetPosition() const;

	const Vector3f GetDirection() const;

	//* transformation option *//

	const TransformationMatrix& GetTransformationMatrix() const;

	//* json option *//

	json PerseToJson() const override;

	void InputJson(const json& data) override;

	//=========================================================================================
	// public variables
	//=========================================================================================

	//* transform option *//

	Vector3f& translate = transform_.translate;
	Quaternion& rotate  = transform_.rotate;
	Vector3f& scale     = transform_.scale;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* transform *//

	Matrix4x4 mat_                 = Matrix4x4::Identity();
	QuaternionTransform transform_ = {};

	//* buffer *//

	std::unique_ptr<DxObject::ConstantBuffer<TransformationMatrix>> buffer_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	void TransferGPU();

};
