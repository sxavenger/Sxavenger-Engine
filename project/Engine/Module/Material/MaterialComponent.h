#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/DirectX/DxObject/DxDimensionBuffer.h>

//* lib
#include <Lib/Transform/Transform.h>

//* lib
#include <Lib/Geometry/Vector4.h>
#include <Lib/Geometry/Matrix4x4.h>
#include <Lib/Adapter/Json/IJsonSerializer.h>

//* c++
#include <memory>

////////////////////////////////////////////////////////////////////////////////////////////
// MaterialComponent class
////////////////////////////////////////////////////////////////////////////////////////////
class MaterialComponent //!< CONSIDER: TransformComponent
	: public IJsonSerializer {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	MaterialComponent()  = default;
	~MaterialComponent() = default;

	void CreateBuffer();

	void Transfer();

	void SetImGuiCommand();

	virtual json OutputJson() override;

	virtual void InputJson(const json& data) override;

	//* getter *//

	const UVTransform& GetUVTransform() const { return uvTransform_; }
	UVTransform& GetUVTransform() { return uvTransform_; }

	const D3D12_GPU_VIRTUAL_ADDRESS& GetTransformGPUVirtualAddress() const;

	const Color4f& GetColor() const { return color_; }
	Color4f& GetColor() { return color_; }

	const D3D12_GPU_VIRTUAL_ADDRESS& GetColorGPUVirtualAddress() const;

protected:

	//=========================================================================================
	// protected variables
	//=========================================================================================

	UVTransform uvTransform_;
	std::unique_ptr<DxObject::DimensionBuffer<Matrix4x4>> transformBuffer_;

	Color4f color_ = kWhite4;
	std::unique_ptr<DxObject::DimensionBuffer<Color4f>> colorBuffer_;

};


