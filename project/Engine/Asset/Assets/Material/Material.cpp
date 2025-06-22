#include "Material.h"

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

void Material::Albedo::Init() {
	type = Type::Value;
	color = kWhite3<float>;
	index = NULL;
}

void Material::Albedo::SetValue(const Color3f& _color) {
	type = Type::Value;
	color = _color;
}

void Material::Albedo::SetTexture(uint32_t _index) {
	type = Type::Texture;
	index = _index;
}

void Material::Albedo::SetMultiply(const Color3f& _color, const std::optional<uint32_t>& _index) {
	type = Type::Multiply;
	color = _color;
	index = _index.value_or(NULL);
}

void Material::Albedo::SetImGuiCommand() {
	if (ImGui::RadioButton("value", type == Type::Value)) {
		type = Type::Value;
	}

	ImGui::SameLine();

	if (ImGui::RadioButton("texture", type == Type::Texture)) {
		type = Type::Texture;
	}

	ImGui::SameLine();

	if (ImGui::RadioButton("multiply", type == Type::Multiply)) {
		type = Type::Multiply;
	}

	switch (type) {
		case Type::Value:
			ImGui::ColorEdit3("color", &color.x);
			break;

		case Type::Texture:
			ImGui::Text("texture index: %d", index);
			break;

		case Type::Multiply:
			ImGui::ColorEdit3("color", &color.x);
			ImGui::Text("texture index: %d", index);
			break;
	};
}

////////////////////////////////////////////////////////////////////////////////////////////
// Transparency structure methods
////////////////////////////////////////////////////////////////////////////////////////////

void Material::Transparency::Init() {
	type = Type::Value;
	alpha = 1.0f;
	index = NULL;
}

void Material::Transparency::SetValue(float _alpha) {
	type = Type::Value;
	alpha = _alpha;
}

void Material::Transparency::SetTexture(uint32_t _index) {
	type = Type::Texture;
	index = _index;
}

void Material::Transparency::SetImGuiCommand() {
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

void Material::Normal::Init() {
	type = Type::None;
	index = NULL;
}

void Material::Normal::SetNone() {
	type = Type::None;
}

void Material::Normal::SetTexture(uint32_t _index) {
	type = Type::Texture;
	index = _index;
}

void Material::Normal::SetImGuiCommand() {
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

void Material::Property::Init() {
	type = Type::Value;
	value = 0.0f;
	index = NULL;
}

void Material::Property::SetValue(float _value) {
	type = Type::Value;
	value = _value;
}

void Material::Property::SetTexture(uint32_t _index) {
	type = Type::Texture;
	index = _index;
}

void Material::Property::SetImGuiCommand() {
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

void Material::SurfaceProperties::Init() {
	ao.Init();
	roughness.Init();
	metallic.Init();
}

void Material::SurfaceProperties::SetImGuiCommand() {
	if (ImGui::TreeNode("ao")) {
		ao.SetImGuiCommand();
		ImGui::TreePop();
	}

	if (ImGui::TreeNode("roughness")) {
		roughness.SetImGuiCommand();
		ImGui::TreePop();
	}

	if (ImGui::TreeNode("metallic")) {
		metallic.SetImGuiCommand();
		ImGui::TreePop();
	}
}

////////////////////////////////////////////////////////////////////////////////////////////
// Transform structure methods
////////////////////////////////////////////////////////////////////////////////////////////

void Material::UVTransformation::Init() {
	mat = Matrix4x4::Identity();
}

void Material::UVTransformation::Transfer(const Matrix4x4& _mat) {
	mat = _mat;
}

////////////////////////////////////////////////////////////////////////////////////////////
// MaterialBuffer structure methods
////////////////////////////////////////////////////////////////////////////////////////////

void Material::MaterialBuffer::Init() {
	transformation.Init();
	albedo.Init();
	transparency.Init();
	normal.Init();
	properties.Init();
}

////////////////////////////////////////////////////////////////////////////////////////////
// Material class methods
////////////////////////////////////////////////////////////////////////////////////////////

void Material::CreateBuffer() {
	if (buffer_ != nullptr) {
		return;
	}

	buffer_ = std::make_unique<DxObject::DimensionBuffer<MaterialBuffer>>();
	buffer_->Create(SxavengerSystem::GetDxDevice(), 1);
	buffer_->At(0).Init();
}

const Material::MaterialBuffer& Material::GetBuffer() const {
	Exception::Assert(buffer_ != nullptr, "buffer is not create.");
	return buffer_->At(0);
}

Material::MaterialBuffer& Material::GetBuffer() {
	Exception::Assert(buffer_ != nullptr, "buffer is not create.");
	return buffer_->At(0);
}

void Material::Transfer() {
	if (buffer_ != nullptr) {
		buffer_->At(0).transformation.Transfer(transform_.ToMatrix());
	}
}

const D3D12_GPU_VIRTUAL_ADDRESS& Material::GetGPUVirtualAddress() const {
	Exception::Assert(buffer_ != nullptr, "buffer is not create.");
	return buffer_->GetGPUVirtualAddress();
}

void Material::SetImGuiCommand() {
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

json Material::PerseToJson() const {
	json root;

	auto& albedo = root["albedo"];
	albedo["type"]  = static_cast<uint32_t>(GetBuffer().albedo.type);
	albedo["color"] = GeometryJsonSerializer::ToJson(GetBuffer().albedo.color);
	//albedo["index"] = GetBuffer().albedo.index; //!< todo: textureのfilepathを保存する

	auto& transparency = root["transparency"];
	transparency["type"]  = static_cast<uint32_t>(GetBuffer().transparency.type);
	transparency["alpha"] = GetBuffer().transparency.alpha;
	//transparency["index"] = GetBuffer().transparency.index; //!< todo: textureのfilepathを保存する

	auto& normal = root["normal"];
	normal["type"]    = static_cast<uint32_t>(GetBuffer().normal.type);
	//normal["index"] = GetBuffer().normal.index; //!< todo: textureのfilepathを保存する

	auto& properties = root["properties"];
	auto& metallic = properties["metallic"];
	metallic["type"]  = static_cast<uint32_t>(GetBuffer().properties.metallic.type);
	metallic["value"] = GetBuffer().properties.metallic.value;
	//metallic["index"] = GetBuffer().properties.metallic.index; //!< todo: textureのfilepathを保存する

	//auto& specular = properties["specular"];
	//specular["type"]  = static_cast<uint32_t>(GetBuffer().properties.specular.type);
	//specular["value"] = GetBuffer().properties.specular.value;
	//specular["index"] = GetBuffer().properties.specular.index; //!< todo: textureのfilepathを保存する

	auto& roughness = properties["roughness"];
	roughness["type"]   = static_cast<uint32_t>(GetBuffer().properties.roughness.type);
	roughness["value"]  = GetBuffer().properties.roughness.value;
	//roughness["index"] = GetBuffer().properties.roughness.index; //!< todo: textureのfilepathを保存する

	return root;
}

void Material::InputJson(const json& data) {
	const auto& albedo = data.at("albedo");
	GetBuffer().albedo.type    = static_cast<Material::Albedo::Type>(albedo.at("type"));
	GetBuffer().albedo.color   = GeometryJsonSerializer::JsonToColor3f(albedo.at("color"));
	//GetBuffer().albedo.index = albedo.at("index");

	const auto& transparency = data.at("transparency");
	GetBuffer().transparency.type    = static_cast<Material::Transparency::Type>(transparency.at("type"));
	GetBuffer().transparency.alpha   = transparency.at("alpha");
	//GetBuffer().transparency.index = transparency.at("index");

	const auto& normal = data.at("normal");
	GetBuffer().normal.type    = static_cast<Material::Normal::Type>(normal.at("type"));
	//GetBuffer().normal.index = normal.at("index");

	const auto& properties = data.at("properties");
	const auto& metallic = properties.at("metallic");
	GetBuffer().properties.metallic.type    = static_cast<Material::Property::Type>(metallic.at("type"));
	GetBuffer().properties.metallic.value   = metallic.at("value");
	//GetBuffer().properties.metallic.index = metallic.at("index");

	//const auto& specular = properties.at("specular");
	//GetBuffer().properties.specular.type    = static_cast<Material::Property::Type>(specular.at("type"));
	//GetBuffer().properties.specular.value   = specular.at("value");
	//GetBuffer().properties.specular.index = specular.at("index");

	const auto& roughness = properties.at("roughness");
	GetBuffer().properties.roughness.type    = static_cast<Material::Property::Type>(roughness.at("type"));
	GetBuffer().properties.roughness.value   = roughness.at("value");
	//GetBuffer().properties.roughness.index = roughness.at("index");

}


