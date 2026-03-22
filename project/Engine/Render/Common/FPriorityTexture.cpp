#include "FPriorityTexture.h"
SXAVENGER_ENGINE_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/System.h>

//* lib
#include <Lib/Geometry/VectorComparison.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Option structure
////////////////////////////////////////////////////////////////////////////////////////////

bool FPriorityTexture::Option::Compatible(const Option& other) const {
	bool compatible = true;
	compatible &= Comparison::All(resolution == other.resolution);
	compatible &= clearDepth == other.clearDepth;
	compatible &= clearStencil == other.clearStencil;

	return compatible;
}

////////////////////////////////////////////////////////////////////////////////////////////
// FPriorityTexture class methods
////////////////////////////////////////////////////////////////////////////////////////////

void FPriorityTexture::Create(const Option& option) {

	if (option_.Compatible(option)) {
		return; //!< 互換性のあるオプションがすでに保存されている場合は何もしない
	}

	CreateResource(option);
	CreateDescriptor();

	//!< 引数の保存
	option_ = option;

}

void FPriorityTexture::ClearDepthStencil(const DirectXQueueContext* context) {
	context->GetCommandList()->ClearDepthStencilView(
		descriptorDSV_.GetCPUHandle(),
		D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL,
		option_.clearDepth,
		option_.clearStencil,
		0,
		nullptr
	);
}

const DxObject::Descriptor& FPriorityTexture::GetDescriptorDSV() const {
	StreamLogger::AssertA(descriptorDSV_.HasHandle(), "Depth Stencil View descriptor is not created.");
	return descriptorDSV_;
}

const D3D12_CPU_DESCRIPTOR_HANDLE& FPriorityTexture::GetCPUHandleDSV() const {
	return GetDescriptorDSV().GetCPUHandle();
}

void FPriorityTexture::CreateResource(const Option& option) {

	// propの設定
	D3D12_HEAP_PROPERTIES prop = {};
	prop.Type = D3D12_HEAP_TYPE_DEFAULT;

	// descの設定
	D3D12_RESOURCE_DESC desc = {};
	desc.Dimension        = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	desc.Width            = option.resolution.x;
	desc.Height           = option.resolution.y;
	desc.DepthOrArraySize = 1;
	desc.MipLevels        = 1;
	desc.Format           = DxObject::kDefaultDepthFormat; //!< TODO: UI用のFormatを用意する
	desc.SampleDesc.Count = 1;
	desc.Flags            = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	// 深度値のクリア設定
	D3D12_CLEAR_VALUE clearValue = {};
	clearValue.Format               = desc.Format;
	clearValue.DepthStencil.Depth   = option.clearDepth;
	clearValue.DepthStencil.Stencil = option.clearStencil;

	// resourceの生成
	resource_.CreateCommitted(
		System::GetDxDevice(),
		prop,
		desc,
		kDefaultState,
		clearValue
	);

	resource_.SetName(L"FPriorityTexture");

}

void FPriorityTexture::CreateDescriptor() {

	auto device = System::GetDxDevice()->GetDevice(); //!< deviceの取り出し
	
	{ //!< DSVの生成

		// handleの取得
		if (!descriptorDSV_.HasHandle()) {
			descriptorDSV_ = System::GetDescriptor(DxObject::kDescriptor_DSV);
		}

		// descの設定
		D3D12_DEPTH_STENCIL_VIEW_DESC desc = {};
		desc.Format        = DxObject::kDefaultDepthFormat;
		desc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;

		// DSVの生成
		device->CreateDepthStencilView(
			resource_.Get(),
			&desc,
			descriptorDSV_.GetCPUHandle()
		);
	}

}
