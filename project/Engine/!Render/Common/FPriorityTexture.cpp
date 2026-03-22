#include "FPriorityTexture.h"
SXAVENGER_ENGINE_USING
DXOBJECT_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/System.h>

////////////////////////////////////////////////////////////////////////////////////////////
// FPriorityTexture class methods
////////////////////////////////////////////////////////////////////////////////////////////

void FPriorityTexture::Create(const Vector2ui& size) {

	auto device = System::GetDxDevice()->GetDevice();

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
		desc.Format           = DxObject::kDefaultDepthFormat; //!< todo: UI用のformatを使用
		desc.SampleDesc.Count = 1;
		desc.Dimension        = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		desc.Flags            = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

		// 深度値のクリア設定
		D3D12_CLEAR_VALUE clear = {};
		clear.DepthStencil.Depth = 1.0f;
		clear.Format             = desc.Format;

		// resourceの生成
		auto hr = device->CreateCommittedResource(
			&prop,
			D3D12_HEAP_FLAG_NONE,
			&desc,
			D3D12_RESOURCE_STATE_DEPTH_WRITE,
			&clear,
			IID_PPV_ARGS(&resource_)
		);
		DxObject::Assert(hr, L"depth stencil texture create failed.");

		resource_->SetName(L"FPriorityTexture | Depth");
	}

	{ //!< DSVの生成
		// handleの取得
		descriptorDSV_ = System::GetDescriptor(kDescriptor_DSV);

		// descの設定
		D3D12_DEPTH_STENCIL_VIEW_DESC desc = {};
		desc.Format        = DxObject::kDefaultDepthFormat; // Depth format
		desc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
		
		device->CreateDepthStencilView(
			resource_.Get(),
			&desc,
			descriptorDSV_.GetCPUHandle()
		);
	}

}

void FPriorityTexture::Term() {
	resource_.Reset();
	descriptorDSV_ = {};
}

void FPriorityTexture::ClearDepth(const DirectXQueueContext* context) const {
	// 深度値のクリア
	context->GetCommandList()->ClearDepthStencilView(
		descriptorDSV_.GetCPUHandle(),
		D3D12_CLEAR_FLAG_DEPTH,
		1.0f,
		0,
		0,
		nullptr
	);
}
