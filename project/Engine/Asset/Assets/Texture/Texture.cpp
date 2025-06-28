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

void Texture::Load(const DirectXThreadContext* context, const std::filesystem::path& filepath, const Option& option) {

	DirectX::ScratchImage image = LoadTexture(filepath, option);
	const auto& metadata        = image.GetMetadata();

	// deviceの取得
	auto device = SxavengerSystem::GetDxDevice()->GetDevice();

	// resourceの生成
	resource_ = CreateTextureResource(device, metadata);
	auto intermediate = UploadTextureData(resource_.Get(), image, SxavengerSystem::GetDxDevice()->GetDevice(), context->GetCommandList());

	// SRVの生成
	{
		D3D12_SHADER_RESOURCE_VIEW_DESC desc = {};
		desc.Format                  = metadata.format;
		desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

		if (metadata.IsCubemap()) { //!< mipImageがcubeMapの場合
			desc.ViewDimension         = D3D12_SRV_DIMENSION_TEXTURECUBE;
			desc.TextureCube.MipLevels = UINT_MAX;

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

	context->ExecuteAllAllocators();
	Logger::EngineThreadLog("texture loaded. filepath: " + filepath.generic_string());

}

void Texture::Term() {
	descriptorSRV_.Delete();
}

Texture::Encoding Texture::GetFormatEncoding(DXGI_FORMAT format) {
	return DirectX::IsSRGB(format) ? Encoding::Lightness : Encoding::Intensity;
}

DirectX::ScratchImage Texture::LoadFromDDSFile(const std::filesystem::path& filepath, const Option& option) {

	DirectX::ScratchImage image = {};

	// ddsファイルの読み込み
	auto hr = DirectX::LoadFromDDSFile(
		filepath.generic_wstring().c_str(),
		DirectX::DDS_FLAGS_NONE,
		nullptr,
		image
	);
	Exception::Assert(SUCCEEDED(hr), "texture load failed. filepath: " + filepath.generic_string());

	// encodingの設定と一致しているか確認
	if (option.encoding != GetFormatEncoding(image.GetMetadata().format)) {
		Logger::EngineThreadLog("[Texture]: warning | encoding is mismatched. filepath: " + filepath.generic_string());
	}

	if (DirectX::IsCompressed(image.GetMetadata().format)) { //!< 圧縮formatかどうか調べる
		return image;
	}

	if (!option.isGenerateMipmap) {
		return image; //!< mipmapを生成しない場合はここで終了
	}

	// mipmapの生成
	DirectX::ScratchImage mipimage = {};

	DirectX::TEX_FILTER_FLAGS flags = DirectX::TEX_FILTER_DEFAULT;

	if (option.encoding == Encoding::Lightness) {
		flags = DirectX::TEX_FILTER_SRGB;
	}

	hr = DirectX::GenerateMipMaps(
		image.GetImages(),
		image.GetImageCount(),
		image.GetMetadata(),
		flags,
		0,
		mipimage
	);

	return mipimage;
	
}

DirectX::ScratchImage Texture::LoadFromHDRFile(const std::filesystem::path& filepath, const Option& option) {

	DirectX::ScratchImage image = {};

	// hdrファイルの読み込み
	auto hr = DirectX::LoadFromHDRFile(
		filepath.generic_wstring().c_str(),
		nullptr,
		image
	);
	Exception::Assert(SUCCEEDED(hr), "texture load failed. filepath: " + filepath.generic_string());

	// encodingの設定と一致しているか確認
	if (option.encoding != GetFormatEncoding(image.GetMetadata().format)) {
		Logger::EngineThreadLog("[Texture]: warning | encoding is mismatched. filepath: " + filepath.generic_string());
	}

	if (!option.isGenerateMipmap) {
		return image; //!< mipmapを生成しない場合はここで終了
	}

	// mipmapの生成
	DirectX::ScratchImage mipimage = {};

	DirectX::TEX_FILTER_FLAGS flags = DirectX::TEX_FILTER_DEFAULT;

	if (option.encoding == Encoding::Lightness) {
		flags = DirectX::TEX_FILTER_SRGB;
	}

	hr = DirectX::GenerateMipMaps(
		image.GetImages(),
		image.GetImageCount(),
		image.GetMetadata(),
		flags,
		0,
		mipimage
	);
	Exception::Assert(SUCCEEDED(hr), "mipmaps create failed.");

	return mipimage;

}

DirectX::ScratchImage Texture::LoadFromTGAFile(const std::filesystem::path& filepath, const Option& option) {

	DirectX::ScratchImage image = {};

	// tgaファイルの読み込み
	auto hr = DirectX::LoadFromTGAFile(
		filepath.generic_wstring().c_str(),
		nullptr,
		image
	);
	Exception::Assert(SUCCEEDED(hr), "texture load failed. filepath: " + filepath.generic_string());

	// encodingの設定と一致しているか確認
	if (option.encoding != GetFormatEncoding(image.GetMetadata().format)) {
		Logger::EngineThreadLog("[Texture]: warning | encoding is mismatched. filepath: " + filepath.generic_string());
	}

	if (!option.isGenerateMipmap) {
		return image; //!< mipmapを生成しない場合はここで終了
	}

	// mipmapの生成
	DirectX::ScratchImage mipimage = {};

	DirectX::TEX_FILTER_FLAGS flags = DirectX::TEX_FILTER_DEFAULT;

	if (option.encoding == Encoding::Lightness) {
		flags = DirectX::TEX_FILTER_SRGB;
	}

	hr = DirectX::GenerateMipMaps(
		image.GetImages(),
		image.GetImageCount(),
		image.GetMetadata(),
		flags,
		0,
		mipimage
	);
	Exception::Assert(SUCCEEDED(hr), "mipmaps create failed.");

	return mipimage;
}

DirectX::ScratchImage Texture::LoadFromWICFile(const std::filesystem::path& filepath, const Option& option) {

	DirectX::ScratchImage image = {};

	DirectX::WIC_FLAGS flags = DirectX::WIC_FLAGS_NONE;

	if (option.encoding == Encoding::Lightness) {
		flags = DirectX::WIC_FLAGS_FORCE_SRGB | DirectX::WIC_FLAGS_DEFAULT_SRGB;

	} else if (option.encoding == Encoding::Intensity) {
		flags = DirectX::WIC_FLAGS_FORCE_RGB;
	}

	// wicファイルの読み込み
	auto hr = DirectX::LoadFromWICFile(
		filepath.generic_wstring().c_str(),
		flags,
		nullptr,
		image
	);
	Exception::Assert(SUCCEEDED(hr), "texture load failed. filepath: " + filepath.generic_string());

	// encodingの設定と一致しているか確認
	if (option.encoding != GetFormatEncoding(image.GetMetadata().format)) {
		Logger::EngineThreadLog("[Texture]: warning | encoding is mismatched. filepath: " + filepath.generic_string());
	}

	if (!option.isGenerateMipmap) {
		return image; //!< mipmapを生成しない場合はここで終了
	}

	// mipmapの生成
	DirectX::ScratchImage mipimage = {};

	DirectX::TEX_FILTER_FLAGS mipFlags = DirectX::TEX_FILTER_DEFAULT;

	if (option.encoding == Encoding::Lightness) {
		mipFlags = DirectX::TEX_FILTER_SRGB;
	}

	hr = DirectX::GenerateMipMaps(
		image.GetImages(),
		image.GetImageCount(),
		image.GetMetadata(),
		mipFlags,
		0,
		mipimage
	);
	Exception::Assert(SUCCEEDED(hr), "mipmaps create failed.");

	return mipimage;

}

DirectX::ScratchImage Texture::LoadTexture(const std::filesystem::path& filepath, const Option& option) {

	const std::filesystem::path& extension = filepath.extension();

	if (extension == ".dds") { //!< filenameが".dds"で終わっている場合
		return LoadFromDDSFile(filepath, option);

	} else if (extension == ".hdr") { //!< filenameが".hdr"で終わっている場合
		return LoadFromHDRFile(filepath, option);

	} else if (extension == ".tga") { //!< filenameが".tga"で終わっている場合
		return LoadFromTGAFile(filepath, option);

	} else {
		return LoadFromWICFile(filepath, option);
	}
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

	auto hr = device->CreateCommittedResource(
		&prop,
		D3D12_HEAP_FLAG_NONE,
		&desc,
		D3D12_RESOURCE_STATE_COPY_DEST,
		nullptr,
		IID_PPV_ARGS(&resource)
	);
	Exception::Assert(SUCCEEDED(hr), "texture resource create failed.");

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
	// todo: copy queueで行わないようにする.
	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Type                   = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags                  = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource   = texture;
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
	barrier.Transition.StateAfter  = D3D12_RESOURCE_STATE_COMMON;

	commandList->ResourceBarrier(1, &barrier);

	intermediateResource->SetName(L"upload texture buffer");
	return intermediateResource;
}
