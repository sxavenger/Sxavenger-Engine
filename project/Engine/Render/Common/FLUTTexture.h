#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/DirectX/DxObject/DxDimensionBuffer.h>
#include <Engine/System/DirectX/DxObject/DxDescriptor.h>
#include <Engine/System/DirectX/DirectXContext.h>
#include <Engine/Asset/Observer/AssetObserver.h>
#include <Engine/Asset/Assets/Texture/AssetTexture.h>

//* lib
#include <Lib/Geometry/Vector2.h>

//* c++
#include <memory>

////////////////////////////////////////////////////////////////////////////////////////////
// FLUTTexture class
////////////////////////////////////////////////////////////////////////////////////////////
class FLUTTexture {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// Parameter structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct Parameter {
		Vector2ui size;
		Vector2ui tile;
	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	void Create(const AssetObserver<AssetTexture>& texture, const Vector2ui& tile);

	void Dispatch(const DirectXThreadContext* context);

	//* getter *//

	const D3D12_GPU_DESCRIPTOR_HANDLE& GetGPUHandleSRV() const { return descriptorSRV_.GetGPUHandle(); }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* 2d texture *//

	std::unique_ptr<DxObject::DimensionBuffer<Parameter>> parameter_;
	std::shared_ptr<AssetTexture> texture_;

	//* 3d texture *//

	ComPtr<ID3D12Resource> resource_;
	DxObject::Descriptor descriptorSRV_;
	DxObject::Descriptor descriptorUAV_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	void CreateResource(const Vector2ui& size, const Vector2ui& tile);

	void CreateBuffer(const Vector2ui& size, const Vector2ui& tile);

};
