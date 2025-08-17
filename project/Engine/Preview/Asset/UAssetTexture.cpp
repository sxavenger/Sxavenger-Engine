#include "UAssetTexture.h"
_DXOBJECT_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/SxavengerSystem.h>
#include <Engine/Content/SxavengerContent.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Metadata structure methods
////////////////////////////////////////////////////////////////////////////////////////////

void UAssetTexture::Metadata::Assign(const DirectX::TexMetadata& metadata) {
	size      = { static_cast<uint32_t>(metadata.width), static_cast<uint32_t>(metadata.height) };
	miplevels = static_cast<uint32_t>(metadata.mipLevels);
	format    = metadata.format;
	isCubemap = metadata.IsCubemap();
}

////////////////////////////////////////////////////////////////////////////////////////////
// UAssetTexture class methods
////////////////////////////////////////////////////////////////////////////////////////////

void UAssetTexture::Setup(const DirectXQueueContext* context, const DirectX::ScratchImage& image) {
	context->RequestQueue(DirectXQueueContext::RenderQueue::Compute); //!< ComputeQueue以上を使用
	// FIXME: CopyQueueを使用するようにする

	// metadataの取得
	const DirectX::TexMetadata& metadata = image.GetMetadata();

	// deviceの取得
	auto device = SxavengerSystem::GetDxDevice()->GetDevice();

	// resourceの生成
	resource_         = CreateTextureResource(metadata);
	auto intermediate = UploadTextureData(context, resource_.Get(), image);

	
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

	// textureをuploadさせる.
	context->ExecuteAllAllocators();

	UBaseAsset::Complete();
	Logger::EngineThreadLog(std::format("[UAssetTexture]: texture setup complete. uuid: {}", UBaseAsset::GetId().Serialize()));
}

void UAssetTexture::Reset() {
	descriptorSRV_.Delete();
	resource_.Reset();
	metadata_ = {};

	status_ = Status::None; //!< 状態を初期化
}

const DxObject::Descriptor& UAssetTexture::GetDescriptorSRV() const {
	if (!UBaseAsset::IsComplete()) {
		return SxavengerContent::GetDescriptorSRV("checkerboard");
	}

	return descriptorSRV_;
}

ComPtr<ID3D12Resource> UAssetTexture::CreateTextureResource(const DirectX::TexMetadata& metadata) const {
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

	auto hr = SxavengerSystem::GetDxDevice()->GetDevice()->CreateCommittedResource(
		&prop,
		D3D12_HEAP_FLAG_NONE,
		&desc,
		D3D12_RESOURCE_STATE_COPY_DEST,
		nullptr,
		IID_PPV_ARGS(&resource)
	);
	Exception::Assert(SUCCEEDED(hr), "texture resource create failed.");

	resource->SetName(L"UAsset | Texture");
	return resource;
}

ComPtr<ID3D12Resource> UAssetTexture::UploadTextureData(const DirectXQueueContext* context, ID3D12Resource* texture, const DirectX::ScratchImage& image) const {

	auto device      = SxavengerSystem::GetDxDevice()->GetDevice();
	auto commandList = context->GetCommandList();

	std::vector<D3D12_SUBRESOURCE_DATA> subresource;
	DirectX::PrepareUpload(device, image.GetImages(), image.GetImageCount(), image.GetMetadata(), subresource);

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

	intermediateResource->SetName(L"UAsset | intermediate upload resource");
	return intermediateResource;
	
}
