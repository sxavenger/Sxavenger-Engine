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

////////////////////////////////////////////////////////////////////////////////////////
// BaseTransform2dComponent class
////////////////////////////////////////////////////////////////////////////////////////
class BaseTransform2dComponent {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	BaseTransform2dComponent()  = default;
	~BaseTransform2dComponent() = default;

	void CreateBuffer();

	void Transfer();

	//* prent option *//

	void SetParent(const BaseTransform2dComponent* component) { parent_ = component; }

	bool HasParent() const { return parent_ != nullptr; }

	//* getter *//

	const D3D12_GPU_VIRTUAL_ADDRESS& GetGPUVirtualAddress() const;

	const Matrix4x4& GetMatrix() const { return mat_; }

protected:

	//=========================================================================================
	// protected variables
	//=========================================================================================

	//* parent *//

	const BaseTransform2dComponent* parent_ = nullptr;

	//* buffer matrix *//

	Matrix4x4 mat_ = Matrix4x4::Identity();
	std::unique_ptr<DxObject::DimensionBuffer<Matrix4x4>> buffer_;

};

////////////////////////////////////////////////////////////////////////////////////////
// Transform2dComponent class
////////////////////////////////////////////////////////////////////////////////////////
class Transform2dComponent
	: public BaseTransform2dComponent, public IJsonSerializer {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	Transform2dComponent()  = default;
	~Transform2dComponent() = default;

	void UpdateMatrix();

	//* json serializer *//

	json OutputJson() override;

	void InputJson(const json& data) override;

	//* accessor *//

	const Transform2d& GetTransform2d() const { return transform2d_; }
	Transform2d& GetTransform2d() { return transform2d_; }

	float SetDepth(float depth) { return depth_ = depth; }

protected:

	//=========================================================================================
	// protected variables
	//=========================================================================================

	Transform2d transform2d_ = {};
	float depth_           = 0.0f; //!< 描画順として使用

};
