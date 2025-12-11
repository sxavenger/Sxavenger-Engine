#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* uasset
#include "BaseAsset.h"

//* engine
#include <Engine/System/DirectX/DxObject/DxDescriptor.h>
#include <Engine/System/DirectX/Context/DirectXQueueContext.h>

//* directx12
#include <d3dx12.h>
#include <DirectXTex.h>

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
		// todo: resource自体の情報を持たせる.

		bool IsLightness() const { return DirectX::IsSRGB(format); }

		//=========================================================================================
		// public variables
		//=========================================================================================

		Vector2ui size;
		uint32_t depth;
		uint32_t miplevels;
		DXGI_FORMAT format;
		bool isCubemap;

	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	AssetTexture(const Uuid& id) : BaseAsset(id) {}
	~AssetTexture() override { Reset(); }

	void Setup(const DirectXQueueContext* context, const DirectX::ScratchImage& image);

	void Update(const DirectXQueueContext* context);

	void Reset();

	//* inspector option *//

	void ShowInspector() override;

	//* getter *//
	// multi-threadで使用するために, 読み込み時の仮Textureを返すように作成.

	const DxObject::Descriptor& GetDescriptorSRV() const;

	const D3D12_GPU_DESCRIPTOR_HANDLE& GetGPUHandleSRV() const;

	const Metadata& GetMetadata() const { return metadata_; }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* directx12 *// 

	ComPtr<ID3D12Resource> resource_;
	DxObject::Descriptor   descriptorSRV_;

	bool isTransition_ = false;

	//* parameter *//

	Metadata metadata_ = {};

	//=========================================================================================
	// private methods
	//=========================================================================================

	//* texture helper methods *//

	ComPtr<ID3D12Resource> CreateTextureResource(const DirectX::TexMetadata& metadata) const;
	NODISCARD ComPtr<ID3D12Resource> UploadTextureData(const DirectXQueueContext* context, ID3D12Resource* texture, const DirectX::ScratchImage& image) const;

};
