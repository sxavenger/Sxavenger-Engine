#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/DirectX/DxObject/DxDimensionBuffer.h>
#include <Engine/System/DirectX/DxObject/DxDescriptor.h>
#include <Engine/System/DirectX/Context/DirectXQueueContext.h>
#include <Engine/Preview/Asset/UAssetTexture.h>

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

	void Create(const std::shared_ptr<UAssetTexture>& texture, const Vector2ui& tile);

	void Dispatch(const DirectXQueueContext* context);

	//* getter *//

	const D3D12_GPU_DESCRIPTOR_HANDLE& GetGPUHandleSRV() const { return descriptorSRV_.GetGPUHandle(); }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* 2d texture *//

	Parameter parameter_;
	std::shared_ptr<UAssetTexture> texture_;

	//* 3d texture *//

	ComPtr<ID3D12Resource> resource_;
	DxObject::Descriptor descriptorSRV_;
	DxObject::Descriptor descriptorUAV_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	void CreateResource(const Vector2ui& size, const Vector2ui& tile);

};
