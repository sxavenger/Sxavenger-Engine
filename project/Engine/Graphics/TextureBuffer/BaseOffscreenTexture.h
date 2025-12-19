#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/Foundation.h>
#include <Engine/System/Utility/ComPtr.h>
#include <Engine/System/DirectX/DxObject/DxDescriptor.h>

//* lib
#include <Lib/Geometry/Vector2.h>

//* c++
#include <concepts>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// BaseOffscreenTexture class
////////////////////////////////////////////////////////////////////////////////////////////
//! @brief offscreen用texture基底クラス.
class BaseOffscreenTexture {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	~BaseOffscreenTexture();

	//* getter *//

	ID3D12Resource* GetResource() const { return resource_.Get(); }

	const DxObject::Descriptor& GetDescriptorSRV() const { return descriptorSRV_; }

	const D3D12_GPU_DESCRIPTOR_HANDLE& GetGPUHandleSRV() const { return descriptorSRV_.GetGPUHandle(); }

	const Vector2ui& GetSize() const { return size_; }

	const DXGI_FORMAT GetFormat() const { return format_; }

protected:

	//=========================================================================================
	// protected variables
	//=========================================================================================

	//* DirectX12 *//

	ComPtr<ID3D12Resource> resource_;

	//* descriptor *//

	DxObject::Descriptor descriptorSRV_;

	//* parameter *//

	Vector2ui   size_;
	DXGI_FORMAT format_;

};

////////////////////////////////////////////////////////////////////////////////////////////
// concept
////////////////////////////////////////////////////////////////////////////////////////////
template <class T>
concept BaseOffscreenTextureConcept = std::derived_from<BaseOffscreenTexture, T>;

SXAVENGER_ENGINE_NAMESPACE_END
