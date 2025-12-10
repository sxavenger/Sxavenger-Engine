#include "SkyLightComponent.h"
DXOBJECT_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/SxavengerSystem.h>
#include <Engine/System/UI/SxImGui.h>

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
	SxImGui::DragFloat("intensity", &parameter_->At().intensity, 0.01f, 0.0f);

	ImGui::RadioButton("Environment", parameter_->At().flags.Test(SkyLightComponent::Flag::Environment));
	ImGui::RadioButton("Irradiance",  parameter_->At().flags.Test(SkyLightComponent::Flag::Irradiance));
	ImGui::RadioButton("Radiance",    parameter_->At().flags.Test(SkyLightComponent::Flag::Radiance));
}

void SkyLightComponent::Init() {
	parameter_ = std::make_unique<ConstantBuffer<Parameter>>();
	parameter_->Create(SxavengerSystem::GetDxDevice());
	parameter_->At().Init();
}

void SkyLightComponent::SetEnvironment(const AssetParameter<AssetTexture>& texture) {
	environment_ = texture;
	parameter_->At().SetEnvironment(environment_.WaitRequire()->GetDescriptorSRV().GetIndex());
}

void SkyLightComponent::SetEnvironment(const DxObject::Descriptor& texture) {
	environment_.Reset();
	parameter_->At().SetEnvironment(texture.GetIndex());
}

void SkyLightComponent::SetIrradiance(const AssetParameter<AssetTexture>& texture) {
	irradiance_ = texture;
	parameter_->At().SetIrradiance(irradiance_.WaitRequire()->GetDescriptorSRV().GetIndex());
}

void SkyLightComponent::SetIrradiance(const DxObject::Descriptor& texture) {
	irradiance_.Reset();
	parameter_->At().SetIrradiance(texture.GetIndex());
}

void SkyLightComponent::SetRadiance(const AssetParameter<AssetTexture>& texture) {
	radiance_ = texture;
	parameter_->At().SetRadiance(radiance_.WaitRequire()->GetDescriptorSRV().GetIndex(), radiance_.WaitRequire()->GetMetadata().miplevels);
}

void SkyLightComponent::SetRadiance(const DxObject::Descriptor& texture, uint32_t mipmaps) {
	radiance_.Reset();
	parameter_->At().SetRadiance(texture.GetIndex(), mipmaps);
}

void SkyLightComponent::SetIntensity(float intensity) {
	parameter_->At().intensity = intensity;
}

const D3D12_GPU_VIRTUAL_ADDRESS& SkyLightComponent::GetGPUVirtualAddress() const {
	return parameter_->GetGPUVirtualAddress();
}

bool SkyLightComponent::IsEnableEnvironment() const {
	return parameter_->At().flags.Test(SkyLightComponent::Flag::Environment);
}

bool SkyLightComponent::IsEnableIrradiance() const {
	return parameter_->At().flags.Test(SkyLightComponent::Flag::Irradiance);
}

bool SkyLightComponent::IsEnableRadiance() const {
	return parameter_->At().flags.Test(SkyLightComponent::Flag::Radiance);
}

