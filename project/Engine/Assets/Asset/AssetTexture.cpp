#include "AssetTexture.h"
SXAVENGER_ENGINE_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/System.h>

////////////////////////////////////////////////////////////////////////////////////////////
// [Texture] Metadata structure methods
////////////////////////////////////////////////////////////////////////////////////////////

void Asset::Texture::Metadata::Assign(const DirectX::TexMetadata& metadata) {
	size         = { static_cast<uint32_t>(metadata.width), static_cast<uint32_t>(metadata.height), static_cast<uint32_t>(metadata.depth) };
	miplevels    = static_cast<uint32_t>(metadata.mipLevels);
	format       = metadata.format;
	miscflags[0] = metadata.miscFlags;
	miscflags[1] = metadata.miscFlags2;
}

////////////////////////////////////////////////////////////////////////////////////////////
// Texture class methods
////////////////////////////////////////////////////////////////////////////////////////////

void Asset::Texture::Setup(const DirectXQueueContext* context, const DirectX::ScratchImage& image) {
	context->RequestQueue(DirectXQueueContext::RenderQueue::Copy); //!< CopyQueue以上を使用

	// metadataの取得
	const DirectX::TexMetadata& metadata = image.GetMetadata();

	// deviceの取得
	auto device = System::GetDxDevice()->GetDevice();

	// resourceの生成
	resource_         = CreateTextureResource(metadata);
	auto intermediate = UploadTextureData(context, resource_.Get(), image);

	{ //!< SRVの生成

		descriptorSRV_ = System::GetDescriptor(DxObject::kDescriptor_SRV);

		D3D12_SHADER_RESOURCE_VIEW_DESC desc = {};
		desc.Format                  = metadata.format;
		desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

		if (metadata.IsCubemap()) { //!< mipImageがcubeMapの場合
			desc.ViewDimension         = D3D12_SRV_DIMENSION_TEXTURECUBE;
			desc.TextureCube.MipLevels = UINT_MAX;

		} else {
			//!< それ以外はTexture2D扱い
			desc.ViewDimension       = D3D12_SRV_DIMENSION_TEXTURE2D;
			desc.Texture2D.MipLevels = UINT(metadata.mipLevels);
		}

		//!< SRVの生成
		device->CreateShaderResourceView(
			resource_.Get(),
			&desc,
			descriptorSRV_.GetCPUHandle()
		);
	}

	// metadataの保存
	metadata_.Assign(metadata);

	// textureをuploadさせる.
	context->ExecuteAllAllocators();

	BaseAsset::SetComplete();
	StreamLogger::EngineThreadLog(std::format("[AssetTexture]: texture setup complete. uuid: {}", BaseAsset::SerializeId()));
}

void Asset::Texture::Transition(const DirectXQueueContext* context) {
	if (!BaseAsset::IsComplete()) {
		return;
	}

	context->RequestQueue(DirectXQueueContext::RenderQueue::Direct); //!< DirectQueue以上を使用
	resource_.Transition(context->GetDxCommand(), D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE);
}

const DxObject::Descriptor& Asset::Texture::GetDescriptorSRV() const {
	BaseAsset::WaitComplete(); // TODO: 仮Textureの設定
	return descriptorSRV_;
}

const D3D12_GPU_DESCRIPTOR_HANDLE& Asset::Texture::GetGPUHandleSRV() const {
	return GetDescriptorSRV().GetGPUHandle();
}

DxObject::Resource Asset::Texture::CreateTextureResource(const DirectX::TexMetadata& metadata) {
	DxObject::Resource resource;

	resource = DxObject::Resource::CreateTexture(
		System::GetDxDevice(),
		static_cast<D3D12_RESOURCE_DIMENSION>(metadata.dimension),
		Vector3ui{ static_cast<uint32_t>(metadata.width), static_cast<uint32_t>(metadata.height), static_cast<uint32_t>(metadata.depth) },
		static_cast<UINT16>(metadata.mipLevels),
		metadata.format,
		D3D12_RESOURCE_FLAG_NONE,
		D3D12_RESOURCE_STATE_COMMON,
		std::nullopt
	);

	resource.SetName(L"Asset | Texture");
	return resource;
}

ComPtr<ID3D12Resource> Asset::Texture::UploadTextureData(const DirectXQueueContext* context, ID3D12Resource* texture, const DirectX::ScratchImage& image) {

	auto device = System::GetDxDevice()->GetDevice();
	auto commandList = context->GetCommandList();

	std::vector<D3D12_SUBRESOURCE_DATA> subresource;
	DirectX::PrepareUpload(device, image.GetImages(), image.GetImageCount(), image.GetMetadata(), subresource);

	uint64_t intermediateSize = GetRequiredIntermediateSize(texture, 0, UINT(subresource.size()));
	ComPtr<ID3D12Resource> intermediateResource = DxObject::CreateBufferResource(device, D3D12_HEAP_TYPE_UPLOAD, intermediateSize, D3D12_RESOURCE_FLAG_NONE, D3D12_RESOURCE_STATE_COPY_SOURCE);

	UpdateSubresources(commandList, texture, intermediateResource.Get(), 0, 0, UINT(subresource.size()), subresource.data());

	intermediateResource->SetName(L"Asset | intermediate upload resource");
	return intermediateResource;

}
