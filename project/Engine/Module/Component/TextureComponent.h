#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/DirectX/DxObject/DxDimensionBuffer.h>

//* lib
#include <Lib/Geometry/Vector4.h>
#include <Lib/Adapter/Json/IJsonSerializer.h>
#include <Lib/Transform/Transform.h>

//* c++
#include <memory>

////////////////////////////////////////////////////////////////////////////////////////////
// TextureComponent class
////////////////////////////////////////////////////////////////////////////////////////////
class TextureComponent {
	//* textureのmaterial関係のcomponent
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	TextureComponent()          = default;
	virtual ~TextureComponent() = default;

	void CreateBuffer();

	void Transfer();

	//* getter *//

	const D3D12_GPU_VIRTUAL_ADDRESS& GetGPUVirtualAddress() const;

	const Transform2d& GetTransform2d() const { return transform2d_; }
	Transform2d& GetTransform2d() { return transform2d_; }

	const Color4f& GetColor() const { return color_; }
	Color4f& GetColor() { return color_; }

protected:

	////////////////////////////////////////////////////////////////////////////////////////////
	// GPUBuffer strcutre
	////////////////////////////////////////////////////////////////////////////////////////////
	struct GPUBuffer {

		//* member *//

		Matrix4x4 mat;
		Color4f   color;

		//* method *//

		void Init();

		void Transfer(const Transform2d& transform, const Color4f& _color);

	};

protected:

	//=========================================================================================
	// protected variables
	//=========================================================================================

	Transform2d transform2d_;
	Color4f     color_ = { 1.0f, 1.0f, 1.0f, 1.0f };

	std::unique_ptr<DxObject::DimensionBuffer<GPUBuffer>> buffer_;

	// todo: componentのinstance化
	// todo: textureを持たせる
};
