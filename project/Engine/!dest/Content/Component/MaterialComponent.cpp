#include "MaterialComponent.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/SxavengerSystem.h>

//* external
#include <imgui.h>

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

void MaterialComponent::Albedo::SetImGuiCommand() {
	if (ImGui::RadioButton("value", type == Type::Value)) {
		type = Type::Value;
	}

	ImGui::SameLine();

	if (ImGui::RadioButton("texture", type == Type::Texture)) {
		type = Type::Texture;
	}

	if (type == Type::Value) {
		ImGui::ColorEdit3("color", &color.x);

	} else {
		ImGui::Text("texture index: %d", index);
	}
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

void MaterialComponent::Transparency::SetImGuiCommand() {
	if (ImGui::RadioButton("value", type == Type::Value)) {
		type = Type::Value;
	}

	ImGui::SameLine();

	if (ImGui::RadioButton("texture", type == Type::Texture)) {
		type = Type::Texture;
	}

	if (type == Type::Value) {
		ImGui::DragFloat("alpha", &alpha, 0.01f, 0.0f, 1.0f);

	} else {
		ImGui::Text("texture index: %d", index);
	}
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

void MaterialComponent::Normal::SetImGuiCommand() {
	if (ImGui::RadioButton("none", type == Type::None)) {
		type = Type::None;
	}

	ImGui::SameLine();

	if (ImGui::RadioButton("texture", type == Type::Texture)) {
		type = Type::Texture;
	}

	if (type == Type::Texture) {
		ImGui::Text("texture index: %d", index);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////
// Property structure methods
////////////////////////////////////////////////////////////////////////////////////////////

void MaterialComponent::Property::Init() {
	type = Type::Value;
	value = 0.0f;
	index = NULL;
}

void MaterialComponent::Property::SetValue(float _value) {
	type  = Type::Value;
	value = _value;
}

void MaterialComponent::Property::SetTexture(uint32_t _index) {
	type  = Type::Texture;
	index = _index;
}

void MaterialComponent::Property::SetImGuiCommand() {
	if (ImGui::RadioButton("value", type == Type::Value)) {
		type = Type::Value;
	}

	ImGui::SameLine();

	if (ImGui::RadioButton("texture", type == Type::Texture)) {
		type = Type::Texture;
	}

	if (type == Type::Value) {
		ImGui::DragFloat("v", &value, 0.01f, 0.0f, 1.0f);

	} else {
		ImGui::Text("texture index: %d", index);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////
// SurfaceProperties structure
////////////////////////////////////////////////////////////////////////////////////////////

void MaterialComponent::SurfaceProperties::Init() {
	metallic.Init();
	specular.Init();
	roughness.Init();
}

void MaterialComponent::SurfaceProperties::SetImGuiCommand() {
	if (ImGui::TreeNode("metallic")) {
		metallic.SetImGuiCommand();
		ImGui::TreePop();
	}

	if (ImGui::TreeNode("specular")) {
		specular.SetImGuiCommand();
		ImGui::TreePop();
	}

	if (ImGui::TreeNode("roughness")) {
		roughness.SetImGuiCommand();
		ImGui::TreePop();
	}
}

////////////////////////////////////////////////////////////////////////////////////////////
// MaterialBuffer structure methods
////////////////////////////////////////////////////////////////////////////////////////////

void MaterialComponent::MaterialBuffer::Init() {
	albedo.Init();
	transparency.Init();
	normal.Init();
	properties.Init();
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

void MaterialComponent::SetImGuiCommand() {
	if (buffer_ != nullptr) {
		if (ImGui::TreeNode("albedo")) {
			buffer_->At(0).albedo.SetImGuiCommand();
			ImGui::TreePop();
		}

		if (ImGui::TreeNode("transparency")) {
			buffer_->At(0).transparency.SetImGuiCommand();
			ImGui::TreePop();
		}

		if (ImGui::TreeNode("normal")) {
			buffer_->At(0).normal.SetImGuiCommand();
			ImGui::TreePop();
		}

		if (ImGui::TreeNode("properties")) {
			buffer_->At(0).properties.SetImGuiCommand();
			ImGui::TreePop();
		}

	} else {
		ImGui::Text("not create buffer.");
	}
}
