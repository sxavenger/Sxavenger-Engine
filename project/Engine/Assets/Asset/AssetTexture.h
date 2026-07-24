#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* asset
#include "BaseAsset.h"

//* engine
#include <Engine/Foundation.h>
#include <Engine/System/DirectX/DxObject/DxResource.h>
#include <Engine/System/DirectX/DxObject/DxDescriptor.h>
#include <Engine/System/DirectX/Context/DirectXQueueContext.h>

//* lib
#include <Lib/Math/Vector2.h>

//* directx12
#include <d3dx12.h>
#include <DirectXTex.h>

//* c++
#include <array>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// AssetTexture class
////////////////////////////////////////////////////////////////////////////////////////////
class AssetTexture final
	: public BaseAsset {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// Metadata structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct Metadata {
	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		void Assign(const DirectX::TexMetadata& metadata);

		DxObject::ColorEncoding GetColorEncoding() const {
			return DirectX::IsSRGB(format) ? DxObject::ColorEncoding::Lightness : DxObject::ColorEncoding::Intensity;
		}

		bool IsCubemap() const { return (miscflags[0] & DirectX::TEX_MISC_TEXTURECUBE) != 0; }

		//=========================================================================================
		// public variables
		//=========================================================================================

		Vector2ui size;
		uint32_t depth;

		uint32_t miplevels;
		DXGI_FORMAT format;

		std::array<uint32_t, 2> miscflags;

	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* constructor / destructor *//

	AssetTexture(const Uuid& id) : BaseAsset(id) {}

	~AssetTexture() override = default;

	//* setup option *//

	void Setup(const DirectXQueueContext* context, const DirectX::ScratchImage& image);

	//* texture option *//

	void Transition(const DirectXQueueContext* context);

	const DxObject::Descriptor& GetDescriptorSRV() const;

	const D3D12_GPU_DESCRIPTOR_HANDLE& GetGPUHandleSRV() const;

	const Metadata& GetMetadata() const { return metadata_; }
	// hack: metadata読み込みの完了を待つ必要がある

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* DirectX12 *// 

	DxObject::Resource   resource_;
	DxObject::Descriptor descriptorSRV_;

	//* parameter *//

	Metadata metadata_ = {};

	//=========================================================================================
	// private methods
	//=========================================================================================

	//* texture helper methods *//

	static DxObject::Resource CreateTextureResource(const DirectX::TexMetadata& metadata);

	static NODISCARD ComPtr<ID3D12Resource> UploadTextureData(const DirectXQueueContext* context, ID3D12Resource* texture, const DirectX::ScratchImage& image);

};

SXAVENGER_ENGINE_NAMESPACE_END
