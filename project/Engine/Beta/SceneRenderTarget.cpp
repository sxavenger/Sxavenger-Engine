#include "SceneRenderTarget.h"
_DXOBJECT_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/Sxavenger.h>

////////////////////////////////////////////////////////////////////////////////////////////
// SceneRenderTarget class methods
////////////////////////////////////////////////////////////////////////////////////////////

void SceneRenderTarget::Create(const Vector2ui& size) {

	// 引数の保存
	size_ = size;

	CreateGBuffer();
	CreateDepthStencil();
}

void SceneRenderTarget::Term() {
}

void SceneRenderTarget::CreateGBuffer() {

	// formatの定義
	formats_[GBUFFER_ALBEDO]   = DxObject::kScreenFormat;
	formats_[GBUFFER_NORMAL]   = DXGI_FORMAT_R32G32B32A32_FLOAT;
	formats_[GBUFFER_POSITION] = DXGI_FORMAT_R32G32B32A32_FLOAT;

	// GBufferの定義
	for (uint32_t i = 0; i < kCountOfGBuffer; ++i) {
		gBuffers_[i] = std::make_unique<RenderTexture>();
		gBuffers_[i]->Create(Sxavenger::GetDxCommon(), size_, {}, formats_[i]);
	}

}

void SceneRenderTarget::CreateDepthStencil() {

	auto device = Sxavenger::GetDevice();

	// resourceの生成
	{
		D3D12_RESOURCE_DESC desc = {};
		desc.Dimension        = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		desc.Width            = size_.x;
		desc.Height           = size_.y;
		desc.MipLevels        = 1;
		desc.Format           = DXGI_FORMAT_D24_UNORM_S8_UINT;
		desc.Flags            = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
		desc.SampleDesc.Count = 1;
		desc.DepthOrArraySize = 1;

		// propの設定
		D3D12_HEAP_PROPERTIES prop = {};
		prop.Type = D3D12_HEAP_TYPE_DEFAULT;

		// 深度値のクリア設定
		D3D12_CLEAR_VALUE clear = {};
		clear.DepthStencil.Depth = 1.0f;
		clear.Format             = desc.Format;

		auto hr = device->CreateCommittedResource(
			&prop,
			D3D12_HEAP_FLAG_NONE,
			&desc,
			D3D12_RESOURCE_STATE_DEPTH_WRITE,
			&clear,
			IID_PPV_ARGS(&resourceDSV_)
		);

		Assert(SUCCEEDED(hr));
	}

	// depthStensilViewの生成
	{
		// handleの取得
		descriptorDSV_ = Sxavenger::GetDescriptor(DSV);

		// descの設定
		D3D12_DEPTH_STENCIL_VIEW_DESC desc = {};
		desc.Format        = DXGI_FORMAT_D24_UNORM_S8_UINT;
		desc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;

		device->CreateDepthStencilView(
			resourceDSV_.Get(),
			&desc,
			descriptorDSV_.GetCPUHandle()
		);
	}

}
