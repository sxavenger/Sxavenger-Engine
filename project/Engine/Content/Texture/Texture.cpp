#include "Texture.h"
_DXOBJECT_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/SxavengerSystem.h>
#include <Engine/System/Utility/Logger.h>

////////////////////////////////////////////////////////////////////////////////////////////
// BaseTexture base class methods
////////////////////////////////////////////////////////////////////////////////////////////

void BaseTexture::Term() {
	descriptorSRV_.Delete();
}

////////////////////////////////////////////////////////////////////////////////////////////
// Texture class methods
////////////////////////////////////////////////////////////////////////////////////////////

void Texture::Load(const std::string& filepath, DirectXThreadContext* context) {

	// device, commandListの取得
	auto device      = SxavengerSystem::GetDxDevice()->GetDevice();
	auto commandList = context->GetCommandList();

	DirectX::ScratchImage mipImage      = LoadTexture(filepath);
	const DirectX::TexMetadata metadata = mipImage.GetMetadata();

	size_   = { static_cast<uint32_t>(metadata.width), static_cast<uint32_t>(metadata.height) };
	format_ = metadata.format;

	resource_            = CreateTextureResource(device, metadata);
	intermediateResouce_ = UploadTextureData(resource_.Get(), mipImage, device, commandList);

	// SRV - shaderResourceViewの生成
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
}

void Texture::Term() {
}

DirectX::ScratchImage Texture::LoadTexture(const std::string& filepath) {
	DirectX::ScratchImage image = {};
	std::wstring filePathW = ToWString(filepath); //!< wstringに変換

	HRESULT hr;

	if (filePathW.ends_with(L".dds")) { //!< .ddsでpathが終わっている場合
		// DDSファイルとして読み込み
		hr = DirectX::LoadFromDDSFile(
			filePathW.c_str(),
			DirectX::DDS_FLAGS_NONE,
			nullptr,
			image
		);

	} else {
		hr = DirectX::LoadFromWICFile(
			filePathW.c_str(),
			DirectX::WIC_FLAGS_FORCE_SRGB,
			nullptr,
			image
		);
	}

	Assert(SUCCEEDED(hr), "Texture not found. filepath: " + filepath);

	// MipMapsの生成
	DirectX::ScratchImage mipImage = {};

	if (DirectX::IsCompressed(image.GetMetadata().format)) { //!< 圧縮formatかどうか調べる
		// 圧縮formatならそのまま使う
		mipImage = std::move(image);

	} else {
		auto metadata = image.GetMetadata();

		if (metadata.width == 1 && metadata.height == 1) {
			//!< 1x1ならMipMapの生成をスキップ
			mipImage = std::move(image);

		} else {
			hr = DirectX::GenerateMipMaps(
				image.GetImages(),
				image.GetImageCount(),
				image.GetMetadata(),
				DirectX::TEX_FILTER_SRGB,
				0,
				mipImage
			);
			Assert(SUCCEEDED(hr));
		}
	}

	return mipImage;
}

ComPtr<ID3D12Resource> Texture::CreateTextureResource(ID3D12Device* device, const DirectX::TexMetadata& metadata) {
	// デスクの設定
	D3D12_RESOURCE_DESC desc = {};
	desc.Width            = UINT(metadata.width);
	desc.Height           = UINT(metadata.height);
	desc.MipLevels        = UINT16(metadata.mipLevels);
	desc.DepthOrArraySize = UINT16(metadata.arraySize);
	desc.Format           = metadata.format;
	desc.SampleDesc.Count = 1;
	desc.Dimension        = D3D12_RESOURCE_DIMENSION(metadata.dimension);

	// ヒーププロパティの設定
	D3D12_HEAP_PROPERTIES prop = {};
	prop.Type = D3D12_HEAP_TYPE_DEFAULT;

	// Resourceの生成
	ComPtr<ID3D12Resource> result;

	auto hr = device->CreateCommittedResource(
		&prop,
		D3D12_HEAP_FLAG_NONE,
		&desc,
		D3D12_RESOURCE_STATE_COPY_DEST,
		nullptr,
		IID_PPV_ARGS(&result)
	);
	Assert(SUCCEEDED(hr));

	return result;
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

	return intermediateResource;
}

////////////////////////////////////////////////////////////////////////////////////////////
// RenderTexture class methods
////////////////////////////////////////////////////////////////////////////////////////////

void RenderTexture::Create(const Vector2ui& size, const Color4f& clearColor, DXGI_FORMAT format) {

	clearColor_ = clearColor;
	format_     = format;
	size_       = size;

	// deviceの取り出し
	auto device = SxavengerSystem::GetDxDevice()->GetDevice();

	// resourceの生成
	{
		D3D12_RESOURCE_DESC desc = {};
		desc.Dimension        = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		desc.Width            = size.x;
		desc.Height           = size.y;
		desc.MipLevels        = 1;
		desc.Format           = format;
		desc.Flags            = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
		desc.SampleDesc.Count = 1;
		desc.DepthOrArraySize = 1;

		D3D12_HEAP_PROPERTIES prop = {};
		prop.Type = D3D12_HEAP_TYPE_DEFAULT;

		D3D12_CLEAR_VALUE clearValue = {};
		clearValue.Format   = format;
		clearValue.Color[0] = clearColor_.r;
		clearValue.Color[1] = clearColor_.g;
		clearValue.Color[2] = clearColor_.b;
		clearValue.Color[3] = clearColor_.a;

		device->CreateCommittedResource(
			&prop,
			D3D12_HEAP_FLAG_ALLOW_ALL_BUFFERS_AND_TEXTURES,
			&desc,
			D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
			&clearValue,
			IID_PPV_ARGS(&resource_)
		);
	}

	// SRV - shaderResourceViewの生成
	{
		D3D12_SHADER_RESOURCE_VIEW_DESC desc = {};
		desc.Format                  = format;
		desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		desc.ViewDimension           = D3D12_SRV_DIMENSION_TEXTURE2D;
		desc.Texture2D.MipLevels     = 1;

		// SRVを生成するDescriptorHeapの場所を決める
		descriptorSRV_ = SxavengerSystem::GetDescriptor(kDescriptor_SRV);

		// SRVの生成
		device->CreateShaderResourceView(
			resource_.Get(),
			&desc,
			descriptorSRV_.GetCPUHandle()
		);
	}

	// RTV - RenderTargetViewの生成
	{
		descriptorRTV_ = SxavengerSystem::GetDescriptor(kDescriptor_RTV);

		device->CreateRenderTargetView(
			resource_.Get(),
			nullptr,
			descriptorRTV_.GetCPUHandle()
		);
	}
}

void RenderTexture::Term() {
	descriptorRTV_.Delete();
}

////////////////////////////////////////////////////////////////////////////////////////////
// DummyTexture class methods
////////////////////////////////////////////////////////////////////////////////////////////

void DummyTexture::Create(const Vector2ui& size, DXGI_FORMAT format) {

	// 引数の保存
	format_ = format;
	size_   = size;

	// deviceの取り出し
	auto device = SxavengerSystem::GetDxDevice()->GetDevice();

	// resourceの生成
	{
		D3D12_HEAP_PROPERTIES prop = {};
		prop.Type = D3D12_HEAP_TYPE_DEFAULT;

		D3D12_RESOURCE_DESC desc = {};
		desc.Dimension        = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		desc.Width            = size.x;
		desc.Height           = size.y;
		desc.DepthOrArraySize = 1;
		desc.SampleDesc.Count = 1;
		desc.MipLevels        = 1;
		desc.Format           = format;
		desc.Flags            = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
		

		device->CreateCommittedResource(
			&prop,
			D3D12_HEAP_FLAG_NONE,
			&desc,
			D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
			nullptr,
			IID_PPV_ARGS(&resource_)
		);
	}

	// SRV - shaderResourceViewの生成
	{
		D3D12_SHADER_RESOURCE_VIEW_DESC desc = {};
		desc.Format                  = format;
		desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		desc.ViewDimension           = D3D12_SRV_DIMENSION_TEXTURE2D;
		desc.Texture2D.MipLevels     = 1;

		// SRVを生成するDescriptorHeapの場所を決める
		descriptorSRV_ = SxavengerSystem::GetDescriptor(kDescriptor_SRV);

		// SRVの生成
		device->CreateShaderResourceView(
			resource_.Get(),
			&desc,
			descriptorSRV_.GetCPUHandle()
		);
	}

	// UAV - unorderedAccessViewの生成
	{
		D3D12_UNORDERED_ACCESS_VIEW_DESC desc = {};
		desc.ViewDimension      = D3D12_UAV_DIMENSION_TEXTURE2D;
		desc.Format             = format;

		// UAVDescriptorの取得
		descriptorUAV_ = SxavengerSystem::GetDescriptor(kDescriptor_UAV);
		
		device->CreateUnorderedAccessView(
			resource_.Get(),
			nullptr,
			&desc,
			descriptorUAV_.GetCPUHandle()
		);
	}
}

void DummyTexture::Term() {
	descriptorUAV_.Delete();
}
