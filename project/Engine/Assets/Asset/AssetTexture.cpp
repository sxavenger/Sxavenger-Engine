#include "AssetTexture.h"
SXAVENGER_ENGINE_USING
DXOBJECT_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/UI/SxImGui.h>
#include <Engine/System/System.h>
#include <Engine/Graphics/Graphics.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Metadata structure methods
////////////////////////////////////////////////////////////////////////////////////////////

void AssetTexture::Metadata::Assign(const DirectX::TexMetadata& metadata) {
	size      = { static_cast<uint32_t>(metadata.width), static_cast<uint32_t>(metadata.height) };
	miplevels = static_cast<uint32_t>(metadata.mipLevels);
	format    = metadata.format;
	isCubemap = metadata.IsCubemap();
}

////////////////////////////////////////////////////////////////////////////////////////////
// AssetTexture class methods
////////////////////////////////////////////////////////////////////////////////////////////

void AssetTexture::Setup(const DirectXQueueContext* context, const DirectX::ScratchImage& image) {
	context->RequestQueue(DirectXQueueContext::RenderQueue::Copy); //!< CopyQueue以上を使用

	// metadataの取得
	const DirectX::TexMetadata& metadata = image.GetMetadata();

	// deviceの取得
	auto device = System::GetDxDevice()->GetDevice();

	// resourceの生成
	resource_         = CreateTextureResource(metadata);
	auto intermediate = UploadTextureData(context, resource_.Get(), image);

	{ //!< SRVの生成

		descriptorSRV_ = System::GetDescriptor(kDescriptor_SRV);

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

	BaseAsset::Complete();
	StreamLogger::EngineThreadLog(std::format("[AssetTexture]: texture setup complete. uuid: {}", BaseAsset::GetId().Serialize()));
}

void AssetTexture::Update(const DirectXQueueContext* context) {
	if (!BaseAsset::IsComplete()) {
		return;
	}

	context->RequestQueue(DirectXQueueContext::RenderQueue::Direct); //!< DirectQueue以上を使用

	resource_.Transition(context->GetDxCommand(), D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE);
}

void AssetTexture::Reset() {
	descriptorSRV_.Delete();
	resource_.Reset();
	metadata_ = {};

	status_ = Status::None; //!< 状態を初期化
}

const DxObject::Descriptor& AssetTexture::GetDescriptorSRV() const {
	if (!BaseAsset::IsComplete()) {
		return Graphics::GetDescriptorSRV("checkerboard");
	}

	return descriptorSRV_;
}

const D3D12_GPU_DESCRIPTOR_HANDLE& AssetTexture::GetGPUHandleSRV() const {
	return GetDescriptorSRV().GetGPUHandle();
}

void AssetTexture::ShowInspector() {
	BaseAsset::ShowInspector();

	if (!BaseAsset::IsComplete()) { //!< loadが完了していない場合
		ImGui::Text("loading...");
		return;
	}

	const D3D12_RESOURCE_DESC desc = resource_.GetDesc();

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

DxObject::Resource AssetTexture::CreateTextureResource(const DirectX::TexMetadata& metadata) {

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

ComPtr<ID3D12Resource> AssetTexture::UploadTextureData(const DirectXQueueContext* context, ID3D12Resource* texture, const DirectX::ScratchImage& image) {

	auto device      = System::GetDxDevice()->GetDevice();
	auto commandList = context->GetCommandList();

	std::vector<D3D12_SUBRESOURCE_DATA> subresource;
	DirectX::PrepareUpload(device, image.GetImages(), image.GetImageCount(), image.GetMetadata(), subresource);

	uint64_t intermediateSize = GetRequiredIntermediateSize(texture, 0, UINT(subresource.size()));
	ComPtr<ID3D12Resource> intermediateResource = CreateBufferResource(device, D3D12_HEAP_TYPE_UPLOAD, intermediateSize, D3D12_RESOURCE_FLAG_NONE, D3D12_RESOURCE_STATE_COPY_SOURCE);

	UpdateSubresources(commandList, texture, intermediateResource.Get(), 0, 0, UINT(subresource.size()), subresource.data());

	intermediateResource->SetName(L"Asset | intermediate upload resource");
	return intermediateResource;
	
}
