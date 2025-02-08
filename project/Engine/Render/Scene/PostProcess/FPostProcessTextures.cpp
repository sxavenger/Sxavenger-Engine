#include "FPostProcessTextures.h"
_DXOBJECT_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/SxavengerSystem.h>

//* lib
#include <Lib/Geometry/VectorComparison.h>

////////////////////////////////////////////////////////////////////////////////////////////
// FProcessTexture class methods
////////////////////////////////////////////////////////////////////////////////////////////

void FProcessTexture::Create(const FTexture* texture) {

	auto device = SxavengerSystem::GetDxDevice()->GetDevice();

	size_   = texture->GetSize();
	format_ = texture->GetFormat();

	{ //!< resourceの生成

		// propの設定
		D3D12_HEAP_PROPERTIES prop = {};
		prop.Type = D3D12_HEAP_TYPE_DEFAULT;

		// descの設定
		D3D12_RESOURCE_DESC desc = {};
		desc.Width            = size_.x;
		desc.Height           = size_.y;
		desc.MipLevels        = 1;
		desc.DepthOrArraySize = 1;
		desc.Format           = format_;
		desc.SampleDesc.Count = 1;
		desc.Dimension        = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		desc.Flags            = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;

		auto hr = device->CreateCommittedResource(
			&prop,
			D3D12_HEAP_FLAG_NONE,
			&desc,
			D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
			nullptr,
			IID_PPV_ARGS(&resource_)
		);
		Assert(SUCCEEDED(hr));

		resource_->SetName(L"FProcessTexture");
	}

	{ //!< UAVの生成

		// handleの取得
		descriptorUAV_ = SxavengerSystem::GetDescriptor(kDescriptor_UAV);

		// descの設定
		D3D12_UNORDERED_ACCESS_VIEW_DESC desc = {};
		desc.Format        = format_;
		desc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;

		// UAVの生成
		device->CreateUnorderedAccessView(
			resource_.Get(),
			nullptr,
			&desc,
			descriptorUAV_.GetCPUHandle()
		);
	}

}

void FProcessTexture::Term() {
	resource_.Reset();
	descriptorUAV_.Delete();
}

void FProcessTexture::CopyFromTexture(const DirectXThreadContext* context, const FTexture* texture) {

	auto commandList = context->GetCommandList();

	D3D12_RESOURCE_BARRIER barriers[2] = {};
	//* src
	barriers[0].Type                   = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barriers[0].Transition.StateBefore = FTexture::GetDefaultState();
	barriers[0].Transition.StateAfter  = D3D12_RESOURCE_STATE_COPY_SOURCE;
	barriers[0].Transition.pResource   = texture->GetResource();

	//* dst
	barriers[1].Type                   = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barriers[1].Transition.StateBefore = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
	barriers[1].Transition.StateAfter  = D3D12_RESOURCE_STATE_COPY_DEST;
	barriers[1].Transition.pResource   = resource_.Get();

	commandList->ResourceBarrier(2, barriers);

	//* copy
	commandList->CopyResource(resource_.Get(), texture->GetResource());

	std::swap(barriers[0].Transition.StateBefore, barriers[0].Transition.StateAfter);
	std::swap(barriers[1].Transition.StateBefore, barriers[1].Transition.StateAfter);

	commandList->ResourceBarrier(2, barriers);
}

void FProcessTexture::CopyToTexture(const DirectXThreadContext* context, const FTexture* texture) {

	auto commandList = context->GetCommandList();

	D3D12_RESOURCE_BARRIER barriers[2] = {};
	//* src
	barriers[0].Type                   = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barriers[0].Transition.StateBefore = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
	barriers[0].Transition.StateAfter  = D3D12_RESOURCE_STATE_COPY_SOURCE;
	barriers[0].Transition.pResource   = resource_.Get();

	//* dst
	barriers[1].Type                   = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barriers[1].Transition.StateBefore = FTexture::GetDefaultState();
	barriers[1].Transition.StateAfter  = D3D12_RESOURCE_STATE_COPY_DEST;
	barriers[1].Transition.pResource   = texture->GetResource();
 
	commandList->ResourceBarrier(2, barriers);

	//* copy
	commandList->CopyResource(texture->GetResource(), resource_.Get());

	std::swap(barriers[0].Transition.StateBefore, barriers[0].Transition.StateAfter);
	std::swap(barriers[1].Transition.StateBefore, barriers[1].Transition.StateAfter);

	commandList->ResourceBarrier(2, barriers);
}

////////////////////////////////////////////////////////////////////////////////////////////
// FPostProcessTextures class methods
////////////////////////////////////////////////////////////////////////////////////////////

void FPostProcessTextures::Create(size_t size, const FTexture* texture) {

	// textureの生成
	textures_.resize(size);

	std::generate(textures_.begin(), textures_.end(), [texture]() {
		auto processTexture = std::make_unique<FProcessTexture>();
		processTexture->Create(texture);
		return processTexture;
	});
}

bool FPostProcessTextures::CheckMatchTexture(const FTexture* texture) {
	if (textures_.empty()) {
		return false;
	}

	if (All(texture->GetSize() == textures_[0]->GetSize())
		&& texture->GetFormat() == textures_[0]->GetFormat()) {
		return true;
	}

	return false;
}

void FPostProcessTextures::NextIndex() {
	index_ = (index_ + 1) % GetSize();
}

FProcessTexture* FPostProcessTextures::GetPrevTexture(size_t prev) const {
	Assert(prev < GetSize(), "prev is over.");
	size_t index = (index_ + GetSize() - prev) % GetSize();
	return textures_[index].get();
}

void FPostProcessTextures::ResetIndex() {
	index_ = 0;
}

void FPostProcessTextures::CopyFromTexture(const DirectXThreadContext* context, const FTexture* texture) {
	Assert(!textures_.empty(), "textures is empty.");
	GetPrevTexture(0)->CopyFromTexture(context, texture);
}

void FPostProcessTextures::CopyToTexture(const DirectXThreadContext* context, const FTexture* texture) {
	Assert(!textures_.empty(), "textures is empty.");
	GetPrevTexture(0)->CopyToTexture(context, texture);
}

size_t FPostProcessTextures::GetSize() const {
	Assert(!textures_.empty(), "textures is empty.");
	return textures_.size();
}

