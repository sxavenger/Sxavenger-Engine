#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/DirectX/DxObject/DxDimensionBuffer.h>

//* lib
#include <Lib/Geometry/Matrix4x4.h>
#include <Lib/Transform/Transform.h>

//* c++
#include <memory>

////////////////////////////////////////////////////////////////////////////////////////////
// BaseTransform2dComponent class
////////////////////////////////////////////////////////////////////////////////////////////
class BaseTransform2dComponent {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// Transformation2d structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct Transformation2d {

		//* member *//

		Matrix4x4 mat;

		//* method *//

		void Init() {
			mat = Matrix4x4::Identity();
		}

		void Transfer(const Matrix4x4& _mat) {
			mat = _mat;
		}

	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	BaseTransform2dComponent()          = default;
	virtual ~BaseTransform2dComponent() = default;

	void CreateBuffer();

	void Transfer();

	//* parent option *//

	void SetParent(const BaseTransform2dComponent* parent) { parent_ = parent; }

	bool HasParent() const { return parent_ != nullptr; }

	//* getter *//

	const D3D12_GPU_VIRTUAL_ADDRESS& GetGPUVirtualAddress() const;

protected:

	//=========================================================================================
	// protected variables
	//=========================================================================================

	Matrix4x4 mat_ = Matrix4x4::Identity();

	const BaseTransform2dComponent* parent_ = nullptr;

	std::unique_ptr<DxObject::DimensionBuffer<Transformation2d>> buffer_;

};

////////////////////////////////////////////////////////////////////////////////////////////
// Transform2dComponent class
////////////////////////////////////////////////////////////////////////////////////////////
class Transform2dComponent
	: public BaseTransform2dComponent {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	Transform2dComponent()          = default;
	virtual ~Transform2dComponent() = default;

	void UpdateMatrix();

	//* getter *//

	const Transform2d& GetTransform2d() const { return transform2d_; }
	Transform2d& GetTransform2d() { return transform2d_; }

protected:

	//=========================================================================================
	// protected variables
	//=========================================================================================

	Transform2d transform2d_;

};
