#include "Texture.h"
_DXOBJECT_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/SxavengerSystem.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Metadata structure methods
////////////////////////////////////////////////////////////////////////////////////////////

void Texture::Metadata::Assign(const DirectX::TexMetadata& metadata) {
	size      = { static_cast<uint32_t>(metadata.width), static_cast<uint32_t>(metadata.height) };
	miplevels = static_cast<uint32_t>(metadata.mipLevels);
	format    = metadata.format;
	isCubemap = metadata.IsCubemap();
}

////////////////////////////////////////////////////////////////////////////////////////////
// Texture class methods
////////////////////////////////////////////////////////////////////////////////////////////

void Texture::Load(const DirectXThreadContext* context, const std::filesystem::path& filepath) {

	DirectX::ScratchImage image = LoadTexture(filepath);
	const auto& metadata        = image.GetMetadata();

	// deviceの取得
	auto device = SxavengerSystem::GetDxDevice()->GetDevice();

	// resourceの生成
	resource_     = CreateTextureResource(device, metadata);
	intermediate_ = UploadTextureData(resource_.Get(), image, SxavengerSystem::GetDxDevice()->GetDevice(), context->GetCommandList());

	// SRVの生成
	{
		D3D12_SHADER_RESOURCE_VIEW_DESC desc = {};
		desc.Format                  = metadata.format;
		desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

		if (metadata.IsCubemap()) { //!< mipImageがcubeMapの場合
			desc.ViewDimension                   = D3D12_SRV_DIMENSION_TEXTURECUBE;
			desc.TextureCube.MostDetailedMip     = 0;
			desc.TextureCube.MipLevels           = UINT_MAX;
			desc.TextureCube.ResourceMinLODClamp = 0.0f;

		} else {
			// それ以外はTexture2D扱い
			desc.ViewDimension       = D3D12_SRV_DIMENSION_TEXTURE2D;
			desc.Texture2D.MipLevels = UINT(metadata.mipLevels);
		}

		// SRVを生成するDescriptorHeapの場所を決める
		descriptorSRV_ = SxavengerSystem::GetDescriptor(kDescriptor_SRV);

		// SRVの生成
		device->CreateShaderResourceView(
			resource_.Get(),
			&desc,
			descriptorSRV_.GetCPUHandle()
		);
	}

	// metadataの保存
	metadata_.Assign(metadata);

}

void Texture::Term() {
	descriptorSRV_.Delete();
}

DirectX::ScratchImage Texture::LoadTexture(const std::filesystem::path& filepath) {

	DirectX::ScratchImage image = {};

	HRESULT hr = {};

	if (filepath.extension() == ".dds") { //!< filenameが".dds"で終わっている場合
		hr = DirectX::LoadFromDDSFile(
			filepath.generic_wstring().c_str(),
			DirectX::DDS_FLAGS_NONE,
			nullptr,
			image
		);

	} else if (filepath.extension() == ".hdr") { //!< filenameが".hdr"で終わっている場合
		hr = DirectX::LoadFromHDRFile(
			filepath.generic_wstring().c_str(),
			nullptr,
			image
		);

	} else {
		hr = DirectX::LoadFromWICFile(
			filepath.generic_wstring().c_str(),
			DirectX::WIC_FLAGS_FORCE_SRGB,
			nullptr,
			image
		);
	}

	Assert(SUCCEEDED(hr), "Texture not found. filepath: " + filepath.generic_string());

	if (DirectX::IsCompressed(image.GetMetadata().format)) { //!< 圧縮formatかどうか調べる
		return image;
	}

	const auto& metadata = image.GetMetadata();

	if (metadata.mipLevels > 1) { //!< MipMapがある場合
		DirectX::ScratchImage mipImage = {};

		// MipMapの生成
		hr = DirectX::GenerateMipMaps(
			image.GetImages(),
			image.GetImageCount(),
			image.GetMetadata(),
			DirectX::TEX_FILTER_SRGB,
			0,
			mipImage
		);

		if (SUCCEEDED(hr)) {
			return mipImage;
		}
	}

	return image;
}

ComPtr<ID3D12Resource> Texture::CreateTextureResource(ID3D12Device* device, const DirectX::TexMetadata& metadata) {

	// propの設定
	D3D12_HEAP_PROPERTIES prop = {};
	prop.Type = D3D12_HEAP_TYPE_DEFAULT;

	// descの設定
	D3D12_RESOURCE_DESC desc = {};
	desc.Dimension        = static_cast<D3D12_RESOURCE_DIMENSION>(metadata.dimension);
	desc.Width            = static_cast<UINT>(metadata.width);
	desc.Height           = static_cast<UINT>(metadata.height);
	desc.MipLevels        = static_cast<UINT16>(metadata.mipLevels);
	desc.DepthOrArraySize = static_cast<UINT16>(metadata.arraySize);
	desc.Format           = metadata.format;
	desc.SampleDesc.Count = 1;

	// resourceの生成
	ComPtr<ID3D12Resource> resource = nullptr;

	device->CreateCommittedResource(
		&prop,
		D3D12_HEAP_FLAG_NONE,
		&desc,
		D3D12_RESOURCE_STATE_COPY_DEST,
		nullptr,
		IID_PPV_ARGS(&resource)
	);

	resource->SetName(L"texture");
	return resource;
}

ComPtr<ID3D12Resource> Texture::UploadTextureData(ID3D12Resource* texture, const DirectX::ScratchImage& mipImages, ID3D12Device* device, ID3D12GraphicsCommandList* commandList) {

	std::vector<D3D12_SUBRESOURCE_DATA> subresource;
	DirectX::PrepareUpload(device, mipImages.GetImages(), mipImages.GetImageCount(), mipImages.GetMetadata(), subresource);

	uint64_t intermediateSize = GetRequiredIntermediateSize(texture, 0, UINT(subresource.size()));
	ComPtr<ID3D12Resource> intermediateResource = CreateBufferResource(device, intermediateSize);
	UpdateSubresources(commandList, texture, intermediateResource.Get(), 0, 0, UINT(subresource.size()), subresource.data());

	// 転送後は利用できるように D3D12_RESOUCE_STATE_COPY_DESC -> D3D12_RESOUCE_STATE_GENETIC_READ へ変更
	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Type                   = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags                  = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource   = texture;
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
	barrier.Transition.StateAfter  = D3D12_RESOURCE_STATE_GENERIC_READ;

	commandList->ResourceBarrier(1, &barrier);

	intermediateResource->SetName(L"upload texture buffer");
	return intermediateResource;
}
