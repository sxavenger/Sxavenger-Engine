#include "FProcessTexture.h"
_DXOBJECT_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/SxavengerSystem.h>

////////////////////////////////////////////////////////////////////////////////////////////
// FProcessTexture class methods
////////////////////////////////////////////////////////////////////////////////////////////

void FProcessTexture::Create(const Vector2ui& size, DXGI_FORMAT format) {

	auto device = SxavengerSystem::GetDxDevice()->GetDevice();

	format_ = format;

	{ //!< resourceの生成

		// propの設定
		D3D12_HEAP_PROPERTIES prop = {};
		prop.Type = D3D12_HEAP_TYPE_DEFAULT;

		// descの設定
		D3D12_RESOURCE_DESC desc = {};
		desc.Width            = size.x;
		desc.Height           = size.y;
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
			kDefaultState_,
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

////////////////////////////////////////////////////////////////////////////////////////////
// FProcessTextures class methods
////////////////////////////////////////////////////////////////////////////////////////////

void FProcessTextures::Create(uint32_t count, const Vector2ui& size, DXGI_FORMAT format) {
	textures_.resize(count);
	for (auto& texture : textures_) {
		texture = std::make_unique<FProcessTexture>();
		texture->Create(size, format);
	}
}

void FProcessTextures::BeginProcess(const DirectXThreadContext* context, FTexture* texture) {
	Assert(textures_.size() != NULL, "process texture is null.");

	index_ = 0; //!< indexの初期化

	auto commandList = context->GetCommandList();

	
	{ //!< textureのcopy

		std::array<D3D12_RESOURCE_BARRIER, 2> barriers = {};
		barriers[0] = texture->TransitionBeginState(D3D12_RESOURCE_STATE_COPY_SOURCE);
		barriers[1] = textures_[index_]->TransitionBeginState(D3D12_RESOURCE_STATE_COPY_DEST);
		commandList->ResourceBarrier(2, barriers.data());

		commandList->CopyResource(textures_[index_]->GetResource(), texture->GetResource());

		barriers[0] = texture->TransitionEndState(D3D12_RESOURCE_STATE_COPY_SOURCE);
		barriers[1] = textures_[index_]->TransitionEndState(D3D12_RESOURCE_STATE_COPY_DEST);
		commandList->ResourceBarrier(2, barriers.data());
	}
	
	
	{ //!< process textureのunordered設定

		UINT size = static_cast<UINT>(textures_.size());
		std::vector<D3D12_RESOURCE_BARRIER> barriers(size);

		for (UINT i = 0; i < size; ++i) {
			barriers[i] = textures_[i]->TransitionBeginUnordered();
		}

		commandList->ResourceBarrier(size, barriers.data());
	}
}

void FProcessTextures::EndProcess(const DirectXThreadContext* context, FTexture* texture) {

	auto commandList = context->GetCommandList();

	
	{ //!< process textureのunordered設定

		UINT size = static_cast<UINT>(textures_.size());
		std::vector<D3D12_RESOURCE_BARRIER> barriers(size);

		for (UINT i = 0; i < size; ++i) {
			barriers[i] = textures_[i]->TransitionEndUnordered();
		}

		commandList->ResourceBarrier(size, barriers.data());
	}

	
	{ //!< textureのcopy

		std::array<D3D12_RESOURCE_BARRIER, 2> barriers = {};
		barriers[0] = texture->TransitionBeginState(D3D12_RESOURCE_STATE_COPY_DEST);
		barriers[1] = textures_[index_]->TransitionBeginState(D3D12_RESOURCE_STATE_COPY_SOURCE);
		commandList->ResourceBarrier(2, barriers.data());

		commandList->CopyResource(texture->GetResource(), textures_[index_]->GetResource());

		barriers[0] = texture->TransitionEndState(D3D12_RESOURCE_STATE_COPY_DEST);
		barriers[1] = textures_[index_]->TransitionEndState(D3D12_RESOURCE_STATE_COPY_SOURCE);
		commandList->ResourceBarrier(2, barriers.data());
	}
}

void FProcessTextures::NextProcess() {
	Assert(textures_.size() != NULL, "process texture is null.");
	index_ = ++index_ % textures_.size();
}

FProcessTexture* FProcessTextures::GetPrevTexture(uint32_t prev) const {
	Assert(prev <= textures_.size(), "process texture array size is not exist prev.");
	return textures_[(index_ + textures_.size() - 1) % textures_.size()].get();
}

FProcessTexture* FProcessTextures::GetIndexTexture() const {
	return textures_[index_].get();
}
