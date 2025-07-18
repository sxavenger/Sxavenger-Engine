#include "LightCommon.h"
_DXOBJECT_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/SxavengerSystem.h>

////////////////////////////////////////////////////////////////////////////////////////////
// InlineShadow structure methods
////////////////////////////////////////////////////////////////////////////////////////////

void LightCommon::InlineShadow::Init() {
	strength = 0.5f;
	flag     = D3D12_RAY_FLAG_CULL_BACK_FACING_TRIANGLES;
}

////////////////////////////////////////////////////////////////////////////////////////////
// LightCommon class methods
////////////////////////////////////////////////////////////////////////////////////////////

void LightCommon::CreateShadowBuffer() {
	shadow_ = std::make_unique<DimensionBuffer<InlineShadow>>();
	shadow_->Create(SxavengerSystem::GetDxDevice(), 1);
	shadow_->At(0).Init();
}

const D3D12_GPU_VIRTUAL_ADDRESS& LightCommon::GetShadowBufferAddress() const {
	Exception::Assert(shadow_ != nullptr, "shadow buffer is not create.");
	return shadow_->GetGPUVirtualAddress();
}

const LightCommon::InlineShadow& LightCommon::GetShadowParameter() const {
	Exception::Assert(shadow_ != nullptr, "shadow buffer is not create.");
	return shadow_->At(0);
}

LightCommon::InlineShadow& LightCommon::GetShadowParameter() {
	Exception::Assert(shadow_ != nullptr, "shadow buffer is not create.");
	return shadow_->At(0);
}

void LightCommon::ShowCommonInspector() {
	ImGui::Separator();
	auto& shadow = shadow_->At(0);
	ImGui::SliderFloat("shadow strength", &shadow.strength, 0.0f, 1.0f);
	ImGui::CheckboxFlags("cull back face", &shadow.flag.Get(), D3D12_RAY_FLAG_CULL_BACK_FACING_TRIANGLES);
}
