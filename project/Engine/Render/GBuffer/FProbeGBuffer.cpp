#include "FProbeGBuffer.h"
SXAVENGER_ENGINE_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* GBuffer
#include "FMainGBuffer.h"

//* render
#include "../Core/FRenderCoreProbe.h"

//* engine
#include <Engine/System/Utility/Convert.h>
#include <Engine/System/System.h>

////////////////////////////////////////////////////////////////////////////////////////////
// FProbeTexture class
////////////////////////////////////////////////////////////////////////////////////////////

FProbeTexture::~FProbeTexture() {
	resource_.Reset();
	descriptorSRV_.Delete();
	descriptorUAV_.Delete();
}

void FProbeTexture::Create(const Vector3ui& probeCount, const Vector2ui& resolution, DXGI_FORMAT format) {

	auto device = System::GetDxDevice()->GetDevice();

	// sizeの計算
	size_ = {
		resolution.x * probeCount.x,
		resolution.y* probeCount.y,
		probeCount.z
	};

	{ //!< resourceの生成

		D3D12_HEAP_PROPERTIES prop = {};
		prop.Type = D3D12_HEAP_TYPE_DEFAULT;

		D3D12_RESOURCE_DESC desc = {};
		desc.Format           = format;
		desc.Dimension        = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		desc.Width            = size_.x;
		desc.Height           = size_.y;
		desc.DepthOrArraySize = static_cast<UINT16>(size_.z);
		desc.MipLevels        = 1;
		desc.SampleDesc.Count = 1;
		desc.Flags            = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;

		auto hr = device->CreateCommittedResource(
			&prop,
			D3D12_HEAP_FLAG_NONE,
			&desc,
			D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE,
			nullptr,
			IID_PPV_ARGS(&resource_)
		);

		DxObject::Assert(hr, L"FProbeTexture create failed.");
	}

	{ //!< UAVの生成

		D3D12_UNORDERED_ACCESS_VIEW_DESC desc = {};
		desc.Format                    = format;
		desc.ViewDimension             = D3D12_UAV_DIMENSION_TEXTURE2DARRAY;
		desc.Texture2DArray.ArraySize  = static_cast<UINT>(probeCount.z);

		descriptorUAV_ = System::GetDxDescriptorHeaps()->GetDescriptor(DxObject::kDescriptor_UAV);

		device->CreateUnorderedAccessView(
			resource_.Get(),
			nullptr,
			&desc,
			descriptorUAV_.GetCPUHandle()
		);

	}

	{ //!< SRVの生成

		D3D12_SHADER_RESOURCE_VIEW_DESC desc = {};
		desc.Format                    = format;
		desc.ViewDimension             = D3D12_SRV_DIMENSION_TEXTURE2DARRAY;
		desc.Texture2DArray.ArraySize  = static_cast<UINT>(probeCount.z);
		desc.Texture2DArray.MipLevels  = 1;
		desc.Shader4ComponentMapping   = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

		descriptorSRV_ = System::GetDxDescriptorHeaps()->GetDescriptor(DxObject::kDescriptor_SRV);

		device->CreateShaderResourceView(
			resource_.Get(),
			&desc,
			descriptorSRV_.GetCPUHandle()
		);

	}

}

//=========================================================================================
// static variable
//=========================================================================================

const std::array<DXGI_FORMAT, FProbeGBuffer::kProbeCount> FProbeGBuffer::kFormats = {
	DXGI_FORMAT_R16G16B16A16_FLOAT,    //!< Sample
	DXGI_FORMAT_R16G16_FLOAT,          //!< Reservoir
	DXGI_FORMAT_R16G16_UINT,           //!< Moment
	DXGI_FORMAT_R16G16B16A16_FLOAT,    //!< Irradiance
};

////////////////////////////////////////////////////////////////////////////////////////////
// FProbeGBuffer class methods
////////////////////////////////////////////////////////////////////////////////////////////

void FProbeGBuffer::Init() {

	static const FRenderCoreProbe::Config config = {};

	for (size_t i = 0; i < kProbeCount; ++i) {

		probe_[i] = std::make_unique<FProbeTexture>();
		probe_[i]->Create(config.probeCount, config.resolution, kFormats[i]);

		// nameの設定
		std::string name = "FProbeGBuffer | ";
		name += magic_enum::enum_name(static_cast<FProbeGBuffer::Probe>(i));

		probe_[i]->SetName(ToWString(name).c_str());
	}

}

void FProbeGBuffer::TransitionBeginUnordered(const DirectXQueueContext* context) {
	std::array<D3D12_RESOURCE_BARRIER, kProbeCount> barriers = {};

	for (size_t i = 0; i < kProbeCount; ++i) {
		barriers[i].Type                   = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barriers[i].Transition.pResource   = probe_[i]->GetResource();
		barriers[i].Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
		barriers[i].Transition.StateBefore = D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE;
		barriers[i].Transition.StateAfter  = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
	}

	context->GetCommandList()->ResourceBarrier(static_cast<UINT>(barriers.size()), barriers.data());
}

void FProbeGBuffer::TransitionEndUnordered(const DirectXQueueContext* context) {
	std::array<D3D12_RESOURCE_BARRIER, kProbeCount> barriers = {};

	for (size_t i = 0; i < kProbeCount; ++i) {
		barriers[i].Type                   = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barriers[i].Transition.pResource   = probe_[i]->GetResource();
		barriers[i].Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
		barriers[i].Transition.StateBefore = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
		barriers[i].Transition.StateAfter  = D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE;
	}

	context->GetCommandList()->ResourceBarrier(static_cast<UINT>(barriers.size()), barriers.data());

}
