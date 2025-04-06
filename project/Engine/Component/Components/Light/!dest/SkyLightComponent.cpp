#include "SkyLightComponent.h"
_DXOBJECT_USING

////////////////////////////////////////////////////////////////////////////////////////////
// Parameter structure methods
////////////////////////////////////////////////////////////////////////////////////////////

void SkyLightComponent::Parameter::Init() {
	type  = Type::None;
	index = NULL;
}

void SkyLightComponent::Parameter::SetTextureCube(uint32_t _index) {
	index = _index;
	type  = Type::TextureCube;
}

void SkyLightComponent::Parameter::SetTexture2d(uint32_t _index) {
	index = _index;
	type  = Type::Texture2d;
}

void SkyLightComponent::Parameter::SetTexture(const AssetObserver<AssetTexture>& texture) {
	if (texture.WaitGet()->GetMetadata().isCubemap) {
		SetTextureCube(texture.WaitGet()->GetDescriptorSRV().GetIndex());

	} else {
		SetTexture2d(texture.WaitGet()->GetDescriptorSRV().GetIndex());
	}
}

////////////////////////////////////////////////////////////////////////////////////////////
// SkyLightComponent class methods
////////////////////////////////////////////////////////////////////////////////////////////

void SkyLightComponent::ShowComponentInspector() {
}

void SkyLightComponent::Init() {
	parameter_ = std::make_unique<DimensionBuffer<Parameter>>();
	parameter_->Create(SxavengerSystem::GetDxDevice(), 1);
	parameter_->At(0).Init();
}

const D3D12_GPU_VIRTUAL_ADDRESS& SkyLightComponent::GetParameterBufferAddress() const {
	Assert(parameter_ != nullptr, "sky light parameter is not create.");
	return parameter_->GetGPUVirtualAddress();
}

const SkyLightComponent::Parameter& SkyLightComponent::GetParameter() const {
	Assert(parameter_ != nullptr, "sky light parameter is not create.");
	return parameter_->At(0);
}

SkyLightComponent::Parameter& SkyLightComponent::GetParameter() {
	Assert(parameter_ != nullptr, "sky light parameter is not create.");
	return parameter_->At(0);
}
