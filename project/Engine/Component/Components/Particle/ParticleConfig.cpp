#include "ParticleConfig.h"
_DXOBJECT_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/SxavengerSystem.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Albedo structure methods
////////////////////////////////////////////////////////////////////////////////////////////

void ParticleConfig::Albedo::Init() {
	type   = Type::Value;
	albedo = kWhite3<float>;
	index  = NULL;
}

void ParticleConfig::Albedo::SetValue(const Color3f& _albedo) {
	type   = Type::Value;
	albedo = _albedo;
}

void ParticleConfig::Albedo::SetTexture(uint32_t _index, const Color3f& _albedo) {
	type   = Type::Texture;
	index  = _index;
	albedo = _albedo;
}

////////////////////////////////////////////////////////////////////////////////////////////
// Transparent structure methods
////////////////////////////////////////////////////////////////////////////////////////////

void ParticleConfig::Transparent::Init() {
	type        = Type::Value;
	transparent = 1.0f;
	index       = NULL;
}

void ParticleConfig::Transparent::SetValue(float _transparent) {
	type        = Type::Value;
	transparent = _transparent;
}

void ParticleConfig::Transparent::SetTexture(uint32_t _index, float _transparent) {
	type        = Type::Texture;
	index       = _index;
	transparent = _transparent;
}

////////////////////////////////////////////////////////////////////////////////////////////
// Config sturcture methods
////////////////////////////////////////////////////////////////////////////////////////////

void ParticleConfig::Config::Init() {
	albedo.Init();
	transparent.Init();

	isBillboard = true;
}

////////////////////////////////////////////////////////////////////////////////////////////
// ParticleConfig class methods
////////////////////////////////////////////////////////////////////////////////////////////

void ParticleConfig::Create(uint32_t count) {
	buffer_ = std::make_unique<DxObject::DimensionBuffer<Config>>();
	buffer_->Create(SxavengerSystem::GetDxDevice(), count);

	for (uint32_t i = 0; i < count; ++i) {
		buffer_->At(i).Init();
	}
}

ParticleConfig::Config& ParticleConfig::At(size_t index) {
	Exception::Assert(buffer_ != nullptr, "particle config buffer is not created.");
	return buffer_->At(index);
}

const ParticleConfig::Config& ParticleConfig::At(size_t index) const {
	Exception::Assert(buffer_ != nullptr, "particle config buffer is not created.");
	return buffer_->At(index);
}

const D3D12_GPU_VIRTUAL_ADDRESS& ParticleConfig::GetGPUVirtualAddress() const {
	Exception::Assert(buffer_ != nullptr, "particle config buffer is not created.");
	return buffer_->GetGPUVirtualAddress();
}
