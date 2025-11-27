#include "SkyAtmosphereComponent.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/SxavengerSystem.h>

////////////////////////////////////////////////////////////////////////////////////////////
// SkyAtmosphereComponent class methods
////////////////////////////////////////////////////////////////////////////////////////////

SkyAtmosphereComponent::SkyAtmosphereComponent(MonoBehaviour* behaviour) : BaseComponent(behaviour) {
}

void SkyAtmosphereComponent::CreateTransmittance(const DirectXQueueContext* context) {

	auto device = SxavengerSystem::GetDxDevice()->GetDevice();

	auto& [resource, descriptor] = transmittance_;

	{ //!< resourceの生成

		// propの設定
		D3D12_HEAP_PROPERTIES prop = {};
		prop.Type = D3D12_HEAP_TYPE_DEFAULT;

		// descの設定
		D3D12_RESOURCE_DESC desc = {};
		desc.Width            = 256;
		desc.Height           = 64;
		desc.MipLevels        = 1;
		desc.DepthOrArraySize = 1;
		desc.Format           = DXGI_FORMAT_R11G11B10_FLOAT;
		desc.SampleDesc.Count = 1;
		desc.Dimension        = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		desc.Flags            = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;

		auto hr = device->CreateCommittedResource(
			&prop,
			D3D12_HEAP_FLAG_NONE,
			&desc,
			D3D12_RESOURCE_STATE_COMMON,
			nullptr,
			IID_PPV_ARGS(&resource)
		);
		DxObject::Assert(hr, L"texture create failed.");
	}

	{ //!< descriptorの生成
		// handleの取得
		descriptor = SxavengerSystem::GetDescriptor(DxObject::kDescriptor_UAV);

		// descの設定
		D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
		uavDesc.Format        = DXGI_FORMAT_R11G11B10_FLOAT;
		uavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;

		// UAVの生成
		device->CreateUnorderedAccessView(
			resource.Get(),
			nullptr,
			&uavDesc,
			descriptor.GetCPUHandle()
		);
	}

	pipeline1_.CreateBlob(kPackagesShaderDirectory / "Render" / "Atmosphere" / "Transmittance.cs.hlsl");
	pipeline1_.ReflectionPipeline(SxavengerSystem::GetDxDevice());

	pipeline1_.SetPipeline(context->GetDxCommand());

	DxObject::BindBufferDesc desc = {};
	desc.SetHandle("gTransmittance", descriptor.GetGPUHandle());
	desc.Set32bitConstants("Dimension", 2, std::array<UINT, 2>{ 256, 64 }.data());

	pipeline1_.BindComputeBuffer(context->GetDxCommand(), desc);
	pipeline1_.Dispatch(context->GetDxCommand(), { DxObject::RoundUp(256, 16), DxObject::RoundUp(64, 16), 1 });


}
