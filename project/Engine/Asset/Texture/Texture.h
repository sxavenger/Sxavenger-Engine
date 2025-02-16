#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/Utility/ComPtr.h>
#include <Engine/System/DirectX/DxObject/DxDescriptor.h>
#include <Engine/System/DirectX/DirectXContext.h>

//* DirectX12
#include <d3dx12.h>
#include <DirectXTex.h>

//* c++
#include <filesystem>

////////////////////////////////////////////////////////////////////////////////////////////
// Texture class
////////////////////////////////////////////////////////////////////////////////////////////
class Texture {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	Texture()  = default;
	~Texture() { Term(); }

	void Load(const DirectXThreadContext* context, const std::filesystem::path& filepath);

	void Term();

	//* getter *//

	const D3D12_GPU_DESCRIPTOR_HANDLE& GetGPUHandleSRV() const { return descriptorSRV_.GetGPUHandle(); }

	const DxObject::Descriptor& GetDescriptorSRV() const { return descriptorSRV_; }

	const Vector2ui& GetSize() const { return size_; }

	const DXGI_FORMAT GetFormat() const { return format_; }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* DirectX12 *// 

	ComPtr<ID3D12Resource> resource_;
	DxObject::Descriptor   descriptorSRV_;

	ComPtr<ID3D12Resource> intermediate_; // FIXME: 中管リソースを不要に.

	//* parameter *//

	Vector2ui   size_;
	DXGI_FORMAT format_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	static DirectX::ScratchImage LoadTexture(const std::filesystem::path& filepath);

	static ComPtr<ID3D12Resource> CreateTextureResource(ID3D12Device* device, const DirectX::TexMetadata& metadata);

	static _NODISCARD ComPtr<ID3D12Resource> UploadTextureData(ID3D12Resource* texture, const DirectX::ScratchImage& mipImages, ID3D12Device* device, ID3D12GraphicsCommandList* commandList);

};
