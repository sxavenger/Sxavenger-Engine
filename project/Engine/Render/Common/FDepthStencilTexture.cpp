#include "FDepthStencilTexture.h"
SXAVENGER_ENGINE_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/System.h>

//* lib
#include <Lib/Geometry/VectorComparison.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Option structure methods
////////////////////////////////////////////////////////////////////////////////////////////

bool FDepthStencilTexture::Option::Compatible(const Option& other) const {

	bool compatible = true;
	compatible &= Comparison::All(resolution == other.resolution);
	compatible &= clearDepth == other.clearDepth;
	compatible &= clearStencil == other.clearStencil;

	return compatible;
}

////////////////////////////////////////////////////////////////////////////////////////////
// FDepthStencilTexture class methods
////////////////////////////////////////////////////////////////////////////////////////////

void FDepthStencilTexture::Create(const Option& option) {

	if (option_.Compatible(option)) {
		return; //!< 互換性のあるオプションがすでに保存されている場合は何もしない
	}

	CreateResource(option);
	CreateDescriptor();

	//!< 引数の保存
	option_ = option;

}

void FDepthStencilTexture::Reset() {
	resource_.Reset();
}

std::optional<D3D12_RESOURCE_BARRIER> FDepthStencilTexture::GetTransitionState(D3D12_RESOURCE_STATES state) {
	return resource_.GetTransition(state);
}

void FDepthStencilTexture::SetTransitionState(std::vector<D3D12_RESOURCE_BARRIER>& barriers, D3D12_RESOURCE_STATES state) {
	resource_.SetTransition(barriers, state);
}

void FDepthStencilTexture::TransitionState(const DirectXQueueContext* context, D3D12_RESOURCE_STATES state) {
	resource_.Transition(context->GetDxCommand(), state);
}

std::optional<D3D12_RESOURCE_BARRIER> FDepthStencilTexture::GetTransitionDefaultState() {
	return GetTransitionState(kDefaultState);
}

void FDepthStencilTexture::SetTransitionDefaultState(std::vector<D3D12_RESOURCE_BARRIER>& barriers) {
	SetTransitionState(barriers, kDefaultState);
}

void FDepthStencilTexture::TransitionDefaultState(const DirectXQueueContext* context) {
	TransitionState(context, kDefaultState);
}

std::optional<D3D12_RESOURCE_BARRIER> FDepthStencilTexture::GetTransitionDepthWrite() {
	return GetTransitionState(D3D12_RESOURCE_STATE_DEPTH_WRITE);
}

void FDepthStencilTexture::SetTransitionDepthWrite(std::vector<D3D12_RESOURCE_BARRIER>& barriers) {
	SetTransitionState(barriers, D3D12_RESOURCE_STATE_DEPTH_WRITE);
}

void FDepthStencilTexture::TransitionDepthWrite(const DirectXQueueContext* context) {
	TransitionState(context, D3D12_RESOURCE_STATE_DEPTH_WRITE);
}

void FDepthStencilTexture::ClearDepthStencil(const DirectXQueueContext* context) {
	TransitionDepthWrite(context); //!< 深度書き込み状態に遷移

	// 深度とステンシルをクリア
	context->GetCommandList()->ClearDepthStencilView(
		descriptorDSV_.GetCPUHandle(),
		D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL,
		option_.clearDepth,
		option_.clearStencil,
		0, nullptr
	);
}

NODISCARD std::optional<D3D12_RESOURCE_BARRIER> FDepthStencilTexture::GetTransitionDepthRead() {
	return GetTransitionState(D3D12_RESOURCE_STATE_DEPTH_READ);
}

void FDepthStencilTexture::SetTransitionDepthRead(std::vector<D3D12_RESOURCE_BARRIER>& barriers) {
	SetTransitionState(barriers, D3D12_RESOURCE_STATE_DEPTH_READ);
}

void FDepthStencilTexture::TransitionDepthRead(const DirectXQueueContext* context) {
	TransitionState(context, D3D12_RESOURCE_STATE_DEPTH_READ);
}

const DxObject::Descriptor& FDepthStencilTexture::GetDescriptorDSV() const {
	StreamLogger::AssertA(descriptorDSV_.HasHandle(), "Depth Stencil View descriptor is not created.");
	return descriptorDSV_;
}

const D3D12_CPU_DESCRIPTOR_HANDLE& FDepthStencilTexture::GetCPUHandleDSV() const {
	return GetDescriptorDSV().GetCPUHandle();
}

const DxObject::Descriptor& FDepthStencilTexture::GetDescriptorSRV() const {
	StreamLogger::AssertA(descriptorSRV_.HasHandle(), "Shader Resource View descriptor is not created.");
	return descriptorSRV_;
}

const D3D12_GPU_DESCRIPTOR_HANDLE& FDepthStencilTexture::GetGPUHandleSRV() const {
	return GetDescriptorSRV().GetGPUHandle();
}

void FDepthStencilTexture::CreateResource(const Option& option) {

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
	desc.Format           = DxObject::kDefaultDepthFormat;
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

	resource_.SetName(L"FDepthStencilTexture");
}

void FDepthStencilTexture::CreateDescriptor() {

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

	{ //!< SRVの生成

		// handleの取得
		if (!descriptorSRV_.HasHandle()) {
			descriptorSRV_ = System::GetDescriptor(DxObject::kDescriptor_SRV);
		}

		// descの設定
		D3D12_SHADER_RESOURCE_VIEW_DESC desc = {};
		desc.Format                  = DxObject::kDefaultDepthViewFormat;
		desc.ViewDimension           = D3D12_SRV_DIMENSION_TEXTURE2D;
		desc.Texture2D.MipLevels     = 1;
		desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

		// SRVの生成
		device->CreateShaderResourceView(
			resource_.Get(),
			&desc,
			descriptorSRV_.GetCPUHandle()
		);

	}
}
