#include "SkyLightComponent.h"
_DXOBJECT_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/SxavengerSystem.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Parameter structure methods
////////////////////////////////////////////////////////////////////////////////////////////

void SkyLightComponent::Parameter::Init() {
	flags = Flag::None;

	environmentIndex = NULL;
	irradianceIndex  = NULL;
	radianceIndex    = NULL;

	intensity = 1.0f;
	radianceMipmaps = NULL;
}

void SkyLightComponent::Parameter::SetEnvironment(uint32_t _index) {
	flags |= Flag::Environment;
	environmentIndex = _index;
}

void SkyLightComponent::Parameter::SetIrradiance(uint32_t _index) {
	flags |= Flag::Irradiance;
	irradianceIndex = _index;
}

void SkyLightComponent::Parameter::SetRadiance(uint32_t _index, uint32_t _mipmaps) {
	flags |= Flag::Radiance;
	radianceIndex   = _index;
	radianceMipmaps = _mipmaps;
}

////////////////////////////////////////////////////////////////////////////////////////////
// SkyLightComponent class methods
////////////////////////////////////////////////////////////////////////////////////////////

void SkyLightComponent::ShowComponentInspector() {
}

void SkyLightComponent::Init() {
	parameter_ = std::make_unique<DxObject::DimensionBuffer<Parameter>>();
	parameter_->Create(SxavengerSystem::GetDxDevice(), 1);
	parameter_->At(0).Init();
}

void SkyLightComponent::SetEnvironment(const UAssetParameter<UAssetTexture>& texture) {
	environment_ = texture;
	parameter_->At(0).SetEnvironment(environment_.WaitRequire()->GetDescriptorSRV().GetIndex());
}

void SkyLightComponent::SetEnvironment(const DxObject::Descriptor& texture) {
	environment_.Reset();
	parameter_->At(0).SetEnvironment(texture.GetIndex());
}

void SkyLightComponent::SetIrradiance(const UAssetParameter<UAssetTexture>& texture) {
	irradiance_ = texture;
	parameter_->At(0).SetIrradiance(irradiance_.WaitRequire()->GetDescriptorSRV().GetIndex());
}

void SkyLightComponent::SetIrradiance(const DxObject::Descriptor& texture) {
	irradiance_.Reset();
	parameter_->At(0).SetIrradiance(texture.GetIndex());
}

void SkyLightComponent::SetRadiance(const UAssetParameter<UAssetTexture>& texture) {
	radiance_ = texture;
	parameter_->At(0).SetRadiance(radiance_.WaitRequire()->GetDescriptorSRV().GetIndex(), radiance_.WaitRequire()->GetMetadata().miplevels);
}

void SkyLightComponent::SetRadiance(const DxObject::Descriptor& texture, uint32_t mipmaps) {
	radiance_.Reset();
	parameter_->At(0).SetRadiance(texture.GetIndex(), mipmaps);
}

void SkyLightComponent::SetIntensity(float intensity) {
	parameter_->At(0).intensity = intensity;
}

const D3D12_GPU_VIRTUAL_ADDRESS& SkyLightComponent::GetGPUVirtualAddress() const {
	return parameter_->GetGPUVirtualAddress();
}

bool SkyLightComponent::IsEnableEnvironment() const {
	return parameter_->At(0).flags.Test(SkyLightComponent::Flag::Environment);
}

bool SkyLightComponent::IsEnableIrradiance() const {
	return parameter_->At(0).flags.Test(SkyLightComponent::Flag::Irradiance);
}

bool SkyLightComponent::IsEnableRadiance() const {
	return parameter_->At(0).flags.Test(SkyLightComponent::Flag::Radiance);
}

