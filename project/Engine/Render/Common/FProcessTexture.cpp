#include "FProcessTexture.h"
SXAVENGER_ENGINE_USING
DXOBJECT_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* core
#include <Engine/Render/FRenderCore.h>

//* engine
#include <Engine/System/Utility/StreamLogger.h>
#include <Engine/System/System.h>

////////////////////////////////////////////////////////////////////////////////////////////
// FProcessTexture class methods
////////////////////////////////////////////////////////////////////////////////////////////

void FProcessTexture::Create(const Vector2ui& size, DXGI_FORMAT format) {

	// deviceの取り出し
	auto device = System::GetDxDevice()->GetDevice();

	{ //!< resourceの生成

		// propの設定
		D3D12_HEAP_PROPERTIES prop = {};
		prop.Type = D3D12_HEAP_TYPE_DEFAULT;

		// descの設定
		D3D12_RESOURCE_DESC desc = {};
		desc.Width            = size.x;
		desc.Height           = size.y;
		desc.MipLevels        = kMipLevels;
		desc.DepthOrArraySize = 1;
		desc.Format           = format;
		desc.SampleDesc.Count = 1;
		desc.Dimension        = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		desc.Flags            = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;

		auto hr = device->CreateCommittedResource(
			&prop,
			D3D12_HEAP_FLAG_NONE,
			&desc,
			kDefaultState,
			nullptr,
			IID_PPV_ARGS(&resource_)
		);
		DxObject::Assert(hr, L"unordered texture create failed.");

		resource_->SetName(L"FProcessTexture");
	}

	{ //!< SRVの生成

		// handleの取得
		descriptorSRV_ = System::GetDescriptor(kDescriptor_SRV);

		// descの設定
		D3D12_SHADER_RESOURCE_VIEW_DESC desc = {};
		desc.Format                    = format;
		desc.ViewDimension             = D3D12_SRV_DIMENSION_TEXTURE2D;
		desc.Shader4ComponentMapping   = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		desc.Texture2D.MipLevels       = kMipLevels;

		// SRVの生成
		device->CreateShaderResourceView(
			resource_.Get(),
			&desc,
			descriptorSRV_.GetCPUHandle()
		);
	}

	for (uint32_t i = 0; i < kMipLevels; ++i) { //!< UAVの生成

		// handleの取得
		descriptorsUAV_[i] = System::GetDescriptor(kDescriptor_UAV);

		// descの設定
		D3D12_UNORDERED_ACCESS_VIEW_DESC desc = {};
		desc.Format             = format;
		desc.ViewDimension      = D3D12_UAV_DIMENSION_TEXTURE2D;
		desc.Texture2D.MipSlice = i;

		// UAVの生成
		device->CreateUnorderedAccessView(
			resource_.Get(),
			nullptr,
			&desc,
			descriptorsUAV_[i].GetCPUHandle()
		);
	}

	// 引数の保存
	size_   = size;
}

void FProcessTexture::Term() {
	resource_.Reset();
	descriptorSRV_.Delete();
	for (auto& descriptorUAV : descriptorsUAV_) {
		descriptorUAV.Delete();
	}
}

D3D12_RESOURCE_BARRIER FProcessTexture::TransitionBeginUnordered() const {
	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Type                   = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Transition.pResource   = resource_.Get();
	barrier.Transition.StateBefore = kDefaultState;
	barrier.Transition.StateAfter  = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

	return barrier;
}

void FProcessTexture::TransitionBeginUnordered(const DirectXQueueContext* context) const {
	D3D12_RESOURCE_BARRIER barrier = TransitionBeginUnordered();
	context->GetCommandList()->ResourceBarrier(1, &barrier);
}

D3D12_RESOURCE_BARRIER FProcessTexture::TransitionEndUnordered() const {
	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Type                   = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Transition.pResource   = resource_.Get();
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
	barrier.Transition.StateAfter  = kDefaultState;
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

	return barrier;
}

void FProcessTexture::TransitionEndUnordered(const DirectXQueueContext* context) const {
	D3D12_RESOURCE_BARRIER barrier = TransitionEndUnordered();
	context->GetCommandList()->ResourceBarrier(1, &barrier);
}

D3D12_RESOURCE_BARRIER FProcessTexture::TransitionBeginState(D3D12_RESOURCE_STATES state) const {
	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Type                   = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Transition.pResource   = resource_.Get();
	barrier.Transition.StateBefore = kDefaultState;
	barrier.Transition.StateAfter  = state;
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

	return barrier;
}

void FProcessTexture::TransitionBeginState(const DirectXQueueContext* context, D3D12_RESOURCE_STATES state) const {
	D3D12_RESOURCE_BARRIER barrier = TransitionBeginState(state);
	context->GetCommandList()->ResourceBarrier(1, &barrier);
}

D3D12_RESOURCE_BARRIER FProcessTexture::TransitionEndState(D3D12_RESOURCE_STATES state) const {
	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Type                   = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Transition.pResource   = resource_.Get();
	barrier.Transition.StateBefore = state;
	barrier.Transition.StateAfter  = kDefaultState;
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

	return barrier;
}

void FProcessTexture::TransitionEndState(const DirectXQueueContext* context, D3D12_RESOURCE_STATES state) const {
	D3D12_RESOURCE_BARRIER barrier = TransitionEndState(state);
	context->GetCommandList()->ResourceBarrier(1, &barrier);
}

void FProcessTexture::BarrierUAV(const DirectXQueueContext* context) const {
	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Type                   = D3D12_RESOURCE_BARRIER_TYPE_UAV;
	barrier.UAV.pResource          = resource_.Get();

	context->GetCommandList()->ResourceBarrier(1, &barrier);
}

void FProcessTexture::GenerateMipmap(const DirectXQueueContext* context) {

	auto core = FRenderCore::GetInstance()->GetTransition();
	core->SetPipeline(FRenderCoreTransition::Transition::MipmapTransition, context);

	for (uint32_t i = 1; i < kMipLevels; ++i) {

		Vector2ui dimension = {
			std::max(size_.x >> i, 1u),
			std::max(size_.y >> i, 1u)
		};

		BindBufferDesc desc = {};
		desc.Set32bitConstants("Dimension", 2, &dimension);
		desc.SetHandle("gInput",  descriptorsUAV_[i - 1].GetGPUHandle());
		desc.SetHandle("gOutput", descriptorsUAV_[i].GetGPUHandle());

		core->BindComputeBuffer(FRenderCoreTransition::Transition::MipmapTransition, context, desc);
		core->Dispatch(context, dimension);

		BarrierUAV(context);
	}
}

void FProcessTexture::CopySource(const DirectXQueueContext* context, FBaseTexture* texture) {

	// commandListの取得
	auto commandList = context->GetCommandList();

	{ //!< textureのcopy

		std::array<D3D12_RESOURCE_BARRIER, 2> barriers = {};
		barriers[0] = texture->TransitionBeginState(D3D12_RESOURCE_STATE_COPY_DEST);
		barriers[1] = this->TransitionBeginState(D3D12_RESOURCE_STATE_COPY_SOURCE);
		commandList->ResourceBarrier(2, barriers.data());

		// mip0のみcopy
		D3D12_TEXTURE_COPY_LOCATION src = {};
		src.pResource        = this->GetResource();
		src.Type             = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
		src.SubresourceIndex = 0;

		D3D12_TEXTURE_COPY_LOCATION dst = {};
		dst.pResource        = texture->GetResource();
		dst.Type             = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
		dst.SubresourceIndex = 0;

		commandList->CopyTextureRegion(&dst, 0, 0, 0, &src, nullptr);

		barriers[0] = texture->TransitionEndState(D3D12_RESOURCE_STATE_COPY_DEST);
		barriers[1] = this->TransitionEndState(D3D12_RESOURCE_STATE_COPY_SOURCE);
		commandList->ResourceBarrier(2, barriers.data());
	}

}

void FProcessTexture::CopyDest(const DirectXQueueContext* context, FBaseTexture* texture) {

	// commandListの取得
	auto commandList = context->GetCommandList();
	
	{ //!< textureのcopy

		std::array<D3D12_RESOURCE_BARRIER, 2> barriers = {};
		barriers[0] = texture->TransitionBeginState(D3D12_RESOURCE_STATE_COPY_SOURCE);
		barriers[1] = this->TransitionBeginState(D3D12_RESOURCE_STATE_COPY_DEST);
		commandList->ResourceBarrier(2, barriers.data());

		// mip0のみcopy
		D3D12_TEXTURE_COPY_LOCATION src = {};
		src.pResource        = texture->GetResource();
		src.Type             = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
		src.SubresourceIndex = 0;

		D3D12_TEXTURE_COPY_LOCATION dst = {};
		dst.pResource        = this->GetResource();
		dst.Type             = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
		dst.SubresourceIndex = 0;

		commandList->CopyTextureRegion(&dst, 0, 0, 0, &src, nullptr);

		barriers[0] = texture->TransitionEndState(D3D12_RESOURCE_STATE_COPY_SOURCE);
		barriers[1] = this->TransitionEndState(D3D12_RESOURCE_STATE_COPY_DEST);
		commandList->ResourceBarrier(2, barriers.data());
	}

}

const D3D12_GPU_DESCRIPTOR_HANDLE& FProcessTexture::GetGPUHandleUAV(uint32_t mipLevel) const {
	StreamLogger::AssertA(mipLevel < kMipLevels, "FProcessTexture::GetGPUHandleUAV: mipLevel is out of range.");
	return descriptorsUAV_.at(mipLevel).GetGPUHandle();
}

////////////////////////////////////////////////////////////////////////////////////////////
// FProcessTextureCollection class methods
////////////////////////////////////////////////////////////////////////////////////////////

void FProcessTextureCollection::Create(uint32_t count, const Vector2ui& size, DXGI_FORMAT format) {
	textures_.resize(count);

	std::generate(textures_.begin(), textures_.end(), [&]() {
		std::unique_ptr<FProcessTexture> texture = std::make_unique<FProcessTexture>();
		texture->Create(size, format);
		return texture;
	});
}

void FProcessTextureCollection::BeginProcess(const DirectXQueueContext* context, FBaseTexture* texture) {

	// commandListの取得
	auto commandList = context->GetCommandList();

	// indexの初期化
	currentIndex_ = 0;

	
	{ //!< textureのcopy

		std::array<D3D12_RESOURCE_BARRIER, 2> barriers = {};
		barriers[0] = texture->TransitionBeginState(D3D12_RESOURCE_STATE_COPY_SOURCE);
		barriers[1] = textures_[currentIndex_]->TransitionBeginState(D3D12_RESOURCE_STATE_COPY_DEST);
		commandList->ResourceBarrier(2, barriers.data());

		// mip0のみcopy
		D3D12_TEXTURE_COPY_LOCATION src = {};
		src.pResource        = texture->GetResource();
		src.Type             = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
		src.SubresourceIndex = 0;

		D3D12_TEXTURE_COPY_LOCATION dst = {};
		dst.pResource        = textures_[currentIndex_]->GetResource();
		dst.Type             = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
		dst.SubresourceIndex = 0;

		commandList->CopyTextureRegion(&dst, 0, 0, 0, &src, nullptr);

		barriers[0] = texture->TransitionEndState(D3D12_RESOURCE_STATE_COPY_SOURCE);
		barriers[1] = textures_[currentIndex_]->TransitionEndState(D3D12_RESOURCE_STATE_COPY_DEST);
		commandList->ResourceBarrier(2, barriers.data());
	}
}

void FProcessTextureCollection::BeginProcess(const DirectXQueueContext* context, const std::vector<FBaseTexture*>& textures) {
	StreamLogger::AssertA(textures.size() <= textures_.size(), "FProcessTextureCollection::BeginProcess: textures is empty.");

	// commandListの取得
	auto commandList = context->GetCommandList();

	// indexの初期化
	currentIndex_ = static_cast<uint32_t>(textures.size() - 1);

	for (size_t i = 0; i < textures.size(); ++i) { //!< textureのcopy

		std::array<D3D12_RESOURCE_BARRIER, 2> barriers = {};
		barriers[0] = textures[i]->TransitionBeginState(D3D12_RESOURCE_STATE_COPY_SOURCE);
		barriers[1] = textures_[i]->TransitionBeginState(D3D12_RESOURCE_STATE_COPY_DEST);
		commandList->ResourceBarrier(2, barriers.data());

		// mip0のみcopy
		D3D12_TEXTURE_COPY_LOCATION src = {};
		src.pResource        = textures[i]->GetResource();
		src.Type             = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
		src.SubresourceIndex = 0;

		D3D12_TEXTURE_COPY_LOCATION dst = {};
		dst.pResource        = textures_[i]->GetResource();
		dst.Type             = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
		dst.SubresourceIndex = 0;

		commandList->CopyTextureRegion(&dst, 0, 0, 0, &src, nullptr);

		barriers[0] = textures[i]->TransitionEndState(D3D12_RESOURCE_STATE_COPY_SOURCE);
		barriers[1] = textures_[i]->TransitionEndState(D3D12_RESOURCE_STATE_COPY_DEST);
		commandList->ResourceBarrier(2, barriers.data());
	}

}

void FProcessTextureCollection::EndProcess(const DirectXQueueContext* context, FBaseTexture* texture) {
	auto commandList = context->GetCommandList();

	FProcessTexture* current = GetCurrentTexture();

	{ //!< textureのcopy

		std::array<D3D12_RESOURCE_BARRIER, 2> barriers = {};
		barriers[0] = texture->TransitionBeginState(D3D12_RESOURCE_STATE_COPY_DEST);
		barriers[1] = current->TransitionBeginState(D3D12_RESOURCE_STATE_COPY_SOURCE);
		commandList->ResourceBarrier(2, barriers.data());

		// mip0のみcopy
		D3D12_TEXTURE_COPY_LOCATION src = {};
		src.pResource        = current->GetResource();
		src.Type             = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
		src.SubresourceIndex = 0;

		D3D12_TEXTURE_COPY_LOCATION dst = {};
		dst.pResource        = texture->GetResource();
		dst.Type             = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
		dst.SubresourceIndex = 0;

		commandList->CopyTextureRegion(&dst, 0, 0, 0, &src, nullptr);

		barriers[0] = texture->TransitionEndState(D3D12_RESOURCE_STATE_COPY_DEST);
		barriers[1] = current->TransitionEndState(D3D12_RESOURCE_STATE_COPY_SOURCE);
		commandList->ResourceBarrier(2, barriers.data());
	}
}

void FProcessTextureCollection::NextProcess(uint32_t count) {
	//uint32_t prev = currentIndex_;
	uint32_t next = (currentIndex_ + count) % textures_.size();

	currentIndex_ = next;
}

void FProcessTextureCollection::BarrierUAV(const DirectXQueueContext* context) const {
	textures_[currentIndex_]->BarrierUAV(context);
}

FProcessTexture* FProcessTextureCollection::GetPrevTexture(uint32_t prev) const {
	StreamLogger::AssertA(prev <= textures_.size(), "process texture array size is not exist prev.");
	return textures_[(currentIndex_ + textures_.size() - prev) % textures_.size()].get();
}

FProcessTexture* FProcessTextureCollection::GetCurrentTexture() const {
	return textures_[currentIndex_].get();
}

void FProcessTextureCollection::ReqiureCount(uint32_t count) {
	StreamLogger::AssertA(count <= textures_.size(), "process texture array size is not exist.");
}
