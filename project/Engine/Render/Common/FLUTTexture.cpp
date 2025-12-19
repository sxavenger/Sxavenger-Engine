#include "FLUTTexture.h"
SXAVENGER_ENGINE_USING
DXOBJECT_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/Utility/StreamLogger.h>
#include <Engine/System/System.h>
#include <Engine/Render/FRenderCore.h>

////////////////////////////////////////////////////////////////////////////////////////////
// FLUTTexture class methods
////////////////////////////////////////////////////////////////////////////////////////////

void FLUTTexture::Create(const std::shared_ptr<AssetTexture>& texture, const Vector2ui& tile) {

	// 引数の保存
	texture_ = texture;

	if (texture_->GetMetadata().IsLightness()) {
		StreamLogger::EngineLog("[FLUTTexture] warning | lut texture is lightness(sRGB) format.");
	}

	parameter_.size = texture_->GetMetadata().size;
	parameter_.tile = tile;

	CreateResource(texture_->GetMetadata().size, tile);
}

void FLUTTexture::Dispatch(const DirectXQueueContext* context) {

	FRenderCore::GetInstance()->GetProcess()->SetPipeline(
		FRenderCoreProcess::ProcessType::ConvertLUTTexture, context
	);

	DxObject::BindBufferDesc desc = {};
	desc.Set32bitConstants("Parameter", 4, static_cast<const void*>(&parameter_));
	desc.SetHandle("gInput",               texture_->GetGPUHandleSRV());
	desc.SetHandle("gOutput",              descriptorUAV_.GetGPUHandle());

	FRenderCore::GetInstance()->GetProcess()->BindComputeBuffer(
		FRenderCoreProcess::ProcessType::ConvertLUTTexture, context, desc
	);

	FRenderCore::GetInstance()->GetProcess()->Dispatch(context, texture_->GetMetadata().size);

	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Type                   = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Transition.pResource   = resource_.Get();
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
	barrier.Transition.StateAfter  = D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE;

	context->GetCommandList()->ResourceBarrier(1, &barrier);

}

void FLUTTexture::CreateResource(const Vector2ui& size, const Vector2ui& tile) {

	auto device = System::GetDxDevice()->GetDevice();
	
	{ //!< resourceの生成

		// propの設定
		D3D12_HEAP_PROPERTIES prop = {};
		prop.Type = D3D12_HEAP_TYPE_DEFAULT;

		// descの設定
		D3D12_RESOURCE_DESC desc = {};
		desc.Width            = tile.x;
		desc.Height           = tile.y;
		desc.DepthOrArraySize = static_cast<UINT16>(size.x / tile.x * size.y / tile.y);
		desc.MipLevels        = 1;
		desc.Format           = DXGI_FORMAT_R10G10B10A2_UNORM;
		desc.SampleDesc.Count = 1;
		desc.Dimension        = D3D12_RESOURCE_DIMENSION_TEXTURE3D;
		desc.Flags            = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;

		auto hr = device->CreateCommittedResource(
			&prop,
			D3D12_HEAP_FLAG_NONE,
			&desc,
			D3D12_RESOURCE_STATE_UNORDERED_ACCESS, //!< すぐ書き込みを行うためUAVで開始
			nullptr,
			IID_PPV_ARGS(&resource_)
		);
		DxObject::Assert(hr, L"lut texture create failed.");

		resource_->SetName(L"FLUTTexture");
	}

	{ //!< SRVの生成

		// descriptorの取得
		descriptorSRV_ = System::GetDescriptor(kDescriptor_SRV);

		// descの設定
		D3D12_SHADER_RESOURCE_VIEW_DESC desc = {};
		desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		desc.Format                  = DXGI_FORMAT_R10G10B10A2_UNORM;
		desc.ViewDimension           = D3D12_SRV_DIMENSION_TEXTURE3D;
		desc.Texture3D.MipLevels     = 1;

		device->CreateShaderResourceView(
			resource_.Get(),
			&desc,
			descriptorSRV_.GetCPUHandle()
		);
	}

	{ //!< UAVの生成

		// descriptorの取得
		descriptorUAV_ = System::GetDescriptor(kDescriptor_UAV);

		// descの設定
		D3D12_UNORDERED_ACCESS_VIEW_DESC desc = {};
		desc.Format          = DXGI_FORMAT_R10G10B10A2_UNORM;
		desc.ViewDimension   = D3D12_UAV_DIMENSION_TEXTURE3D;
		desc.Texture3D.WSize = size.x / tile.x * size.y / tile.y;

		device->CreateUnorderedAccessView(
			resource_.Get(),
			nullptr,
			&desc,
			descriptorUAV_.GetCPUHandle()
		);
	}
}
