#include "SkyLightComponent.h"
_DXOBJECT_USING

////////////////////////////////////////////////////////////////////////////////////////////
// DiffuseParameter structure methods
////////////////////////////////////////////////////////////////////////////////////////////

void SkyLightComponent::DiffuseParameter::Init() {
	index = NULL;
}

void SkyLightComponent::DiffuseParameter::SetTexture(uint32_t _index) {
	index = _index;
}

void SkyLightComponent::DiffuseParameter::SetTexture(const AssetObserver<AssetTexture>& texture) {
	index = texture.WaitGet()->GetDescriptorSRV().GetIndex();
}

////////////////////////////////////////////////////////////////////////////////////////////
// SpecularParameter structure
////////////////////////////////////////////////////////////////////////////////////////////

void SkyLightComponent::SpecularParameter::Init() {
	index    = NULL;
	miplevels = 0;
}

void SkyLightComponent::SpecularParameter::SetTexture(uint32_t _index, uint32_t _miplevels) {
	index     = _index;
	miplevels = _miplevels;
}

void SkyLightComponent::SpecularParameter::SetTexture(const AssetObserver<AssetTexture>& texture) {
	index     = texture.WaitGet()->GetTexture().GetDescriptorSRV().GetIndex();
	miplevels = texture.WaitGet()->GetTexture().GetMetadata().miplevels;
}

////////////////////////////////////////////////////////////////////////////////////////////
// Parameter structure
////////////////////////////////////////////////////////////////////////////////////////////

void SkyLightComponent::Parameter::Init() {
	intensity = 1.0f;
}

////////////////////////////////////////////////////////////////////////////////////////////
// SkyLightComponent class methods
////////////////////////////////////////////////////////////////////////////////////////////

void SkyLightComponent::ShowComponentInspector() {
	auto& parameter = GetParameter();
	ImGui::DragFloat("intensity", &parameter.intensity, 0.01f, 0.0f, std::numeric_limits<float>::max());
}

void SkyLightComponent::Init() {

	parameter_ = std::make_unique<DxObject::DimensionBuffer<Parameter>>();
	parameter_->Create(SxavengerSystem::GetDxDevice(), 1);
	parameter_->At(0).Init();

	diffuseParameter_ = std::make_unique<DxObject::DimensionBuffer<DiffuseParameter>>();
	diffuseParameter_->Create(SxavengerSystem::GetDxDevice(), 1);
	diffuseParameter_->At(0).Init();

	specularParameter_ = std::make_unique<DxObject::DimensionBuffer<SpecularParameter>>();
	specularParameter_->Create(SxavengerSystem::GetDxDevice(), 1);
	specularParameter_->At(0).Init();
}

const D3D12_GPU_VIRTUAL_ADDRESS& SkyLightComponent::GetParameterBufferAddress() const {
	Exception::Assert(parameter_ != nullptr, "sky light parameter is not create.");
	return parameter_->GetGPUVirtualAddress();
}

const D3D12_GPU_VIRTUAL_ADDRESS& SkyLightComponent::GetDiffuseParameterBufferAddress() const {
	Exception::Assert(diffuseParameter_ != nullptr, "sky light diffuse parameter is not create.");
	return diffuseParameter_->GetGPUVirtualAddress();
}

const D3D12_GPU_VIRTUAL_ADDRESS& SkyLightComponent::GetSpecularParameterBufferAddress() const {
	Exception::Assert(specularParameter_ != nullptr, "sky light specular parameter is not create.");
	return specularParameter_->GetGPUVirtualAddress();
}

const SkyLightComponent::Parameter& SkyLightComponent::GetParameter() const {
	Exception::Assert(parameter_ != nullptr, "sky light parameter is not create.");
	return parameter_->At(0);
}

SkyLightComponent::Parameter& SkyLightComponent::GetParameter() {
	Exception::Assert(parameter_ != nullptr, "sky light parameter is not create.");
	return parameter_->At(0);
}

const SkyLightComponent::DiffuseParameter& SkyLightComponent::GetDiffuseParameter() const {
	Exception::Assert(diffuseParameter_ != nullptr, "sky light diffuse parameter is not create.");
	return diffuseParameter_->At(0);
}

SkyLightComponent::DiffuseParameter& SkyLightComponent::GetDiffuseParameter() {
	Exception::Assert(diffuseParameter_ != nullptr, "sky light diffuse parameter is not create.");
	return diffuseParameter_->At(0);
}

const SkyLightComponent::SpecularParameter& SkyLightComponent::GetSpecularParameter() const {
	Exception::Assert(specularParameter_ != nullptr, "sky light specular parameter is not create.");
	return specularParameter_->At(0);
}

SkyLightComponent::SpecularParameter& SkyLightComponent::GetSpecularParameter() {
	Exception::Assert(specularParameter_ != nullptr, "sky light specular parameter is not create.");
	return specularParameter_->At(0);
}
