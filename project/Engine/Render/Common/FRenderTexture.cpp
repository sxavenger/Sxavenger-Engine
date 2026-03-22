#include "FRenderTexture.h"
SXAVENGER_ENGINE_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/System.h>

//* lib
#include <Lib/Geometry/VectorComparison.h>
#include <Lib/Adapter/String/EncodedString.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Option structure methods
////////////////////////////////////////////////////////////////////////////////////////////

D3D12_RESOURCE_FLAGS FRenderTexture::Option::GetResourceFlags() const {
	D3D12_RESOURCE_FLAGS flags = D3D12_RESOURCE_FLAG_NONE;

	if (flag.Test(Flag::RenderTarget)) {
		flags |= D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
	}

	if (flag.Test(Flag::UnorderedAccess)) {
		flags |= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
	}

	return flags;
}

std::optional<D3D12_CLEAR_VALUE> FRenderTexture::Option::GetClearValue() const {
	if (!flag.Test(Flag::RenderTarget)) {
		return std::nullopt;
	}

	D3D12_CLEAR_VALUE clearValue = {};
	clearValue.Format = format;
	clearValue.Color[0] = clearColor.r;
	clearValue.Color[1] = clearColor.g;
	clearValue.Color[2] = clearColor.b;
	clearValue.Color[3] = clearColor.a;

	return clearValue;
}

bool FRenderTexture::Option::Compatible(const Option& other) const {
	bool compatible = true;
	compatible &= Comparison::All(resolution == other.resolution);
	compatible &= format == other.format;
	compatible &= flag == other.flag;

	return compatible;
}


////////////////////////////////////////////////////////////////////////////////////////////
// FRenderTexture class methods
////////////////////////////////////////////////////////////////////////////////////////////

void FRenderTexture::Create(const Option& option) {

	if (option_.Compatible(option)) {
		return; //!< 互換性のあるオプションがすでに保存されている場合は何もしない
	}

	CreateResource(option);
	CreateDescriptor(option);

	// 引数の保存
	option_ = option;

}

void FRenderTexture::Reset() {
	resource_.Reset();
}

void FRenderTexture::SetName(const std::wstring& name) const {
	resource_.SetName(name);
}

void FRenderTexture::SetName(const std::string& name) const {
	resource_.SetName(EncodedString::Convert(name));
}

std::optional<D3D12_RESOURCE_BARRIER> FRenderTexture::GetTransitionState(D3D12_RESOURCE_STATES state) {
	return resource_.GetTransition(state);
}

void FRenderTexture::SetTransitionState(std::vector<D3D12_RESOURCE_BARRIER>& barriers, D3D12_RESOURCE_STATES state) {
	resource_.SetTransition(barriers, state);
}

void FRenderTexture::TransitionState(const DirectXQueueContext* context, D3D12_RESOURCE_STATES state) {
	resource_.Transition(context->GetDxCommand(), state);
}

std::optional<D3D12_RESOURCE_BARRIER> FRenderTexture::GetTransitionDefaultState() {
	return GetTransitionState(kDefaultState);
}

void FRenderTexture::SetTransitionDefaultState(std::vector<D3D12_RESOURCE_BARRIER>& barriers) {
	SetTransitionState(barriers, kDefaultState);
}

void FRenderTexture::TransitionDefaultState(const DirectXQueueContext* context) {
	TransitionState(context, kDefaultState);
}

std::optional<D3D12_RESOURCE_BARRIER> FRenderTexture::GetTransitionRenderTarget() {
	StreamLogger::AssertA(option_.flag.Test(Flag::RenderTarget), "RenderTarget flag is not set.");
	return GetTransitionState(D3D12_RESOURCE_STATE_RENDER_TARGET);
}

void FRenderTexture::SetTransitionRenderTarget(std::vector<D3D12_RESOURCE_BARRIER>& barriers) {
	StreamLogger::AssertA(option_.flag.Test(Flag::RenderTarget), "RenderTarget flag is not set.");
	SetTransitionState(barriers, D3D12_RESOURCE_STATE_RENDER_TARGET);
}

void FRenderTexture::TransitionRenderTarget(const DirectXQueueContext* context) {
	StreamLogger::AssertA(option_.flag.Test(Flag::RenderTarget), "RenderTarget flag is not set.");
	TransitionState(context, D3D12_RESOURCE_STATE_RENDER_TARGET);
}

void FRenderTexture::ClearRenderTarget(const DirectXQueueContext* context) {
	StreamLogger::AssertA(option_.flag.Test(Flag::RenderTarget), "RenderTarget flag is not set.");
	TransitionRenderTarget(context);

	context->GetCommandList()->ClearRenderTargetView(
		descriptorRTV_.GetCPUHandle(),
		&option_.clearColor.r,
		0, nullptr
	);
}

std::optional<D3D12_RESOURCE_BARRIER> FRenderTexture::GetTransitionUnorderedAccess() {
	StreamLogger::AssertA(option_.flag.Test(Flag::UnorderedAccess), "UnorderedAccess flag is not set.");
	return GetTransitionState(D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
}

void FRenderTexture::SetTransitionUnorderedAccess(std::vector<D3D12_RESOURCE_BARRIER>& barriers) {
	StreamLogger::AssertA(option_.flag.Test(Flag::UnorderedAccess), "UnorderedAccess flag is not set.");
	SetTransitionState(barriers, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
}

void FRenderTexture::TransitionUnorderedAccess(const DirectXQueueContext* context) {
	StreamLogger::AssertA(option_.flag.Test(Flag::UnorderedAccess), "UnorderedAccess flag is not set.");
	TransitionState(context, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
}

void FRenderTexture::BarrierUnorderedAccess(const DirectXQueueContext* context) {
	if (!option_.flag.Test(Flag::UnorderedAccess)) {
		return;
	}

	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Type          = D3D12_RESOURCE_BARRIER_TYPE_UAV;
	barrier.UAV.pResource = resource_.Get();

	context->GetCommandList()->ResourceBarrier(1, &barrier);
}

const DxObject::Descriptor& FRenderTexture::GetDescriptorRTV() const {
	StreamLogger::AssertA(option_.flag.Test(Flag::RenderTarget), "RenderTarget flag is not set.");
	StreamLogger::AssertA(descriptorRTV_.HasHandle(), "RenderTarget View is not created.");
	return descriptorRTV_;
}

const D3D12_CPU_DESCRIPTOR_HANDLE& FRenderTexture::GetCPUHandleRTV() const {
	return GetDescriptorRTV().GetCPUHandle();
}

const DxObject::Descriptor& FRenderTexture::GetDescriptorSRV() const {
	StreamLogger::AssertA(option_.flag.Test(Flag::ShaderResource), "ShaderResource flag is not set.");
	StreamLogger::AssertA(descriptorSRV_.HasHandle(), "ShaderResource View is not created.");
	return descriptorSRV_;
}

const D3D12_GPU_DESCRIPTOR_HANDLE& FRenderTexture::GetGPUHandleSRV() const {
	return GetDescriptorSRV().GetGPUHandle();
}

const DxObject::Descriptor& FRenderTexture::GetDescriptorUAV() const {
	StreamLogger::AssertA(option_.flag.Test(Flag::UnorderedAccess), "UnorderedAccess flag is not set.");
	StreamLogger::AssertA(descriptorUAV_.HasHandle(), "UnorderedAccess View is not created.");
	return descriptorUAV_;
}

const D3D12_GPU_DESCRIPTOR_HANDLE& FRenderTexture::GetGPUHandleUAV() const {
	return GetDescriptorUAV().GetGPUHandle();
}

void FRenderTexture::CreateResource(const Option& option) {

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
	desc.Format           = option.format;
	desc.SampleDesc.Count = 1;
	desc.Flags            = option.GetResourceFlags();

	// resourceの生成
	resource_.CreateCommitted(
		System::GetDxDevice(),
		prop,
		desc,
		kDefaultState,
		option.GetClearValue()
	);

	resource_.SetName(L"FRenderTexture");
}

void FRenderTexture::CreateDescriptor(const Option& option) {

	auto device = System::GetDxDevice()->GetDevice(); //!< deviceの取り出し

	//!< RTVの生成
	if (option.flag.Test(Flag::RenderTarget)) {

		// handleの取得
		if (!descriptorRTV_.HasHandle()) {
			descriptorRTV_ = System::GetDescriptor(DxObject::kDescriptor_RTV);
		}

		// descの設定
		D3D12_RENDER_TARGET_VIEW_DESC desc = {};
		desc.Format        = option.format;
		desc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;

		// RTVの生成
		device->CreateRenderTargetView(
			resource_.Get(),
			&desc,
			descriptorRTV_.GetCPUHandle()
		);
	}

	//!< UAVの生成
	if (option.flag.Test(Flag::UnorderedAccess)) { 

		// handleの取得
		if (!descriptorUAV_.HasHandle()) {
			descriptorUAV_ = System::GetDescriptor(DxObject::kDescriptor_UAV);
		}

		// descの設定
		D3D12_UNORDERED_ACCESS_VIEW_DESC desc = {};
		desc.Format        = option.format;
		desc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;

		// UAVの生成
		device->CreateUnorderedAccessView(
			resource_.Get(),
			nullptr,
			&desc,
			descriptorUAV_.GetCPUHandle()
		);
	}

	//!< SRVの生成
	if (option.flag.Test(Flag::ShaderResource)) { 

		// handleの取得
		if (!descriptorSRV_.HasHandle()) {
			descriptorSRV_ = System::GetDescriptor(DxObject::kDescriptor_SRV);
		}

		// descの設定
		D3D12_SHADER_RESOURCE_VIEW_DESC desc = {};
		desc.Format                  = option.format;
		desc.ViewDimension           = D3D12_SRV_DIMENSION_TEXTURE2D;
		desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		desc.Texture2D.MipLevels     = 1;

		// SRVの生成
		device->CreateShaderResourceView(
			resource_.Get(),
			&desc,
			descriptorSRV_.GetCPUHandle()
		);
	}

}
