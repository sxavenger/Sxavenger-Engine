#include "MaterialComponent.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/SxavengerSystem.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Albedo structure methods
////////////////////////////////////////////////////////////////////////////////////////////

void MaterialComponent::Albedo::Init() {
	type  = Type::Value;
	color = kWhite3;
	index = NULL;
}

void MaterialComponent::Albedo::SetValue(const Color3f& _color) {
	type  = Type::Value;
	color = _color;
}

void MaterialComponent::Albedo::SetTexture(uint32_t _index) {
	type  = Type::Texture;
	index = _index;
}

////////////////////////////////////////////////////////////////////////////////////////////
// Transparency structure methods
////////////////////////////////////////////////////////////////////////////////////////////

void MaterialComponent::Transparency::Init() {
	type  = Type::Value;
	alpha = 1.0f;
	index = NULL;
}

void MaterialComponent::Transparency::SetValue(float _alpha) {
	type  = Type::Value;
	alpha = _alpha;
}

void MaterialComponent::Transparency::SetTexture(uint32_t _index) {
	type  = Type::Texture;
	index = _index;
}

////////////////////////////////////////////////////////////////////////////////////////////
// Normal structure methods
////////////////////////////////////////////////////////////////////////////////////////////

void MaterialComponent::Normal::Init() {
	type  = Type::None;
	index = NULL;
}

void MaterialComponent::Normal::SetNone() {
	type = Type::None;
}

void MaterialComponent::Normal::SetTexture(uint32_t _index) {
	type  = Type::Texture;
	index = _index;
}

////////////////////////////////////////////////////////////////////////////////////////////
// MaterialBuffer structure methods
////////////////////////////////////////////////////////////////////////////////////////////

void MaterialComponent::MaterialBuffer::Init() {
	albedo.Init();
	transparency.Init();
	normal.Init();
}

////////////////////////////////////////////////////////////////////////////////////////////
// MaterialComponent class methods
////////////////////////////////////////////////////////////////////////////////////////////

void MaterialComponent::Create() {
	buffer_ = std::make_unique<DxObject::DimensionBuffer<MaterialBuffer>>();
	buffer_->Create(SxavengerSystem::GetDxDevice(), 1);
	buffer_->At(0).Init();
}

const D3D12_GPU_VIRTUAL_ADDRESS& MaterialComponent::GetGPUVirtualAddress() const {
	return buffer_->GetGPUVirtualAddress();
}

const MaterialComponent::MaterialBuffer& MaterialComponent::GetMaterial() const {
	Assert(buffer_ != nullptr, "buffer is not create.");
	return buffer_->At(0);
}

MaterialComponent::MaterialBuffer& MaterialComponent::GetMaterial() {
	Assert(buffer_ != nullptr, "buffer is not create.");
	return buffer_->At(0);
}
