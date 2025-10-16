#include "UAssetTexture.h"
_DXOBJECT_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/UI/SxImGui.h>
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
	context->RequestQueue(DirectXQueueContext::RenderQueue::Copy); //!< CopyQueue以上を使用

	// metadataの取得
	const DirectX::TexMetadata& metadata = image.GetMetadata();

	// deviceの取得
	auto device = SxavengerSystem::GetDxDevice()->GetDevice();

	// resourceの生成
	resource_         = CreateTextureResource(metadata);
	auto intermediate = UploadTextureData(context, resource_.Get(), image);

	{ //!< SRVの生成
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

	isTransition_ = false;

	// textureをuploadさせる.
	context->ExecuteAllAllocators();

	UBaseAsset::Complete();
	Logger::EngineThreadLog(std::format("[UAssetTexture]: texture setup complete. uuid: {}", UBaseAsset::GetId().Serialize()));
}

void UAssetTexture::Update(const DirectXQueueContext* context) {
	if (!UBaseAsset::IsComplete() || isTransition_) {
		return;
	}
	context->RequestQueue(DirectXQueueContext::RenderQueue::Direct); //!< DirectQueue以上を使用

	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Type                   = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Transition.pResource   = resource_.Get();
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
	barrier.Transition.StateAfter  = D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE;

	context->GetCommandList()->ResourceBarrier(1, &barrier);

	isTransition_ = true;
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

const D3D12_GPU_DESCRIPTOR_HANDLE& UAssetTexture::GetGPUHandleSRV() const {
	return GetDescriptorSRV().GetGPUHandle();
}

void UAssetTexture::ShowInspector() {
	UBaseAsset::ShowInspector();

	if (!UBaseAsset::IsComplete()) { //!< loadが完了していない場合
		ImGui::Text("loading...");
		return;
	}

	const D3D12_RESOURCE_DESC desc = resource_->GetDesc();

	if (ImGui::CollapsingHeader("Texture", ImGuiTreeNodeFlags_DefaultOpen)) {
		if (desc.Dimension == D3D12_RESOURCE_DIMENSION_TEXTURE2D) {
			SxImGui::Image(descriptorSRV_.GetGPUHandle().ptr, ImVec2{ static_cast<float>(metadata_.size.x), static_cast<float>(metadata_.size.y) });

		} else {
			ImGui::Text("texture dimension type is not D3D12_RESOURCE_DIMENSION_TEXTURE2D");
		}

	}

	if (ImGui::CollapsingHeader("Desc", ImGuiTreeNodeFlags_DefaultOpen)) {
		ImGui::Text("dimension: %s", magic_enum::enum_name(desc.Dimension).data());
		ImGui::Text("width:     %u", desc.Width);
		ImGui::Text("height:    %u", desc.Height);
		ImGui::Text("depth:     %u", desc.DepthOrArraySize);
		ImGui::Text("miplevels: %u", desc.MipLevels);
		ImGui::Text("format:    %s", magic_enum::enum_name(desc.Format).data());
	}

	if (ImGui::CollapsingHeader("Descriptor", ImGuiTreeNodeFlags_DefaultOpen)) {
		ImGui::Text("index:  %u",   descriptorSRV_.GetIndex());
		ImGui::Text("handle: 0x%p", descriptorSRV_.GetGPUHandle().ptr);
	}
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
	DxObject::Assert(hr, L"texture resource create failed.");

	resource->SetName(L"UAsset | Texture");
	return resource;
}

ComPtr<ID3D12Resource> UAssetTexture::UploadTextureData(const DirectXQueueContext* context, ID3D12Resource* texture, const DirectX::ScratchImage& image) const {

	auto device      = SxavengerSystem::GetDxDevice()->GetDevice();
	auto commandList = context->GetCommandList();

	std::vector<D3D12_SUBRESOURCE_DATA> subresource;
	DirectX::PrepareUpload(device, image.GetImages(), image.GetImageCount(), image.GetMetadata(), subresource);

	uint64_t intermediateSize = GetRequiredIntermediateSize(texture, 0, UINT(subresource.size()));
	ComPtr<ID3D12Resource> intermediateResource = CreateBufferResource(device, D3D12_HEAP_TYPE_UPLOAD, intermediateSize, D3D12_RESOURCE_FLAG_NONE, D3D12_RESOURCE_STATE_COPY_SOURCE);

	UpdateSubresources(commandList, texture, intermediateResource.Get(), 0, 0, UINT(subresource.size()), subresource.data());

	intermediateResource->SetName(L"UAsset | intermediate upload resource");
	return intermediateResource;
	
}
