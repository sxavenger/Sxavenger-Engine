#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/DirectX/DxObject/DxDimensionBuffer.h>

//* lib
#include <Lib/Adapter/Json/IJsonSerializer.h>
#include <Lib/Transform/Transform.h>

//* c++
#include <memory>

////////////////////////////////////////////////////////////////////////////////////////////
// BaseTransformComponent class
////////////////////////////////////////////////////////////////////////////////////////////
class BaseTransformComponent {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	BaseTransformComponent()  = default;
	~BaseTransformComponent() = default;

	void CreateBuffer();

	void Transfer();

	//* prent *//

	void SetParent(const BaseTransformComponent* component) { parent_ = component; }

	bool HasParent() const { return parent_ != nullptr; }

	//* getter *//

	const D3D12_GPU_VIRTUAL_ADDRESS& GetGPUVirtualAddress() const;

	const Vector3f GetPosition() const;

	const Matrix4x4& GetMatrix() const { return mat_; }
	Matrix4x4& GetMatrix() { return mat_; }

protected:

	//=========================================================================================
	// protected variables
	//=========================================================================================

	Matrix4x4 mat_ = Matrix4x4::Identity();

	const BaseTransformComponent* parent_ = nullptr;

	std::unique_ptr<DxObject::DimensionBuffer<TransformationMatrix>> buffer_;

};

////////////////////////////////////////////////////////////////////////////////////////////
// TransformComponent class
////////////////////////////////////////////////////////////////////////////////////////////
class TransformComponent
	: public BaseTransformComponent, public IJsonSerializer {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	TransformComponent()  = default;
	~TransformComponent() = default;

	void UpdateMatrix();

	void SetImGuiCommand();

	virtual json OutputJson() override;

	virtual void InputJson(const json& data) override;

	//* getter *//

	const QuaternionTransform& GetTransform() const { return transform_; }
	QuaternionTransform& GetTransform() { return transform_; }

protected:

	//=========================================================================================
	// protected variables
	//=========================================================================================

	QuaternionTransform transform_;

};

