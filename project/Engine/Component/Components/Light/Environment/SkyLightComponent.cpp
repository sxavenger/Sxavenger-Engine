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
// SkyLightComponent class methods
////////////////////////////////////////////////////////////////////////////////////////////

void SkyLightComponent::ShowComponentInspector() {
	LightCommon::ShowCommonInspector();
}

void SkyLightComponent::Init() {
	LightCommon::CreateShadowBuffer();

	diffuseParameter_ = std::make_unique<DxObject::DimensionBuffer<DiffuseParameter>>();
	diffuseParameter_->Create(SxavengerSystem::GetDxDevice(), 1);
	diffuseParameter_->At(0).Init();

	specularParameter_ = std::make_unique<DxObject::DimensionBuffer<SpecularParameter>>();
	specularParameter_->Create(SxavengerSystem::GetDxDevice(), 1);
	specularParameter_->At(0).Init();
}

const D3D12_GPU_VIRTUAL_ADDRESS& SkyLightComponent::GetDiffuseParameterBufferAddress() const {
	Exception::Assert(diffuseParameter_ != nullptr, "sky light diffuse parameter is not create.");
	return diffuseParameter_->GetGPUVirtualAddress();
}

const D3D12_GPU_VIRTUAL_ADDRESS& SkyLightComponent::GetSpecularParameterBufferAddress() const {
	Exception::Assert(specularParameter_ != nullptr, "sky light specular parameter is not create.");
	return specularParameter_->GetGPUVirtualAddress();
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
