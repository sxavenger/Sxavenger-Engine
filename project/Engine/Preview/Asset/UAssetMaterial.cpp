#include "UAssetMaterial.h"
_DXOBJECT_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* uasset
#include "UAssetStorage.h"

//* ucontent
#include "../Content/UContentStorage.h"

//* engine
#include <Engine/System/SxavengerSystem.h>

//* external
#include <magic_enum.hpp>

////////////////////////////////////////////////////////////////////////////////////////////
// Albedo structure methods
////////////////////////////////////////////////////////////////////////////////////////////

void UAssetMaterial::Albedo::Init() {
	type  = Type::Value;
	color = kWhite3<float>;
	index = NULL;
}

void UAssetMaterial::Albedo::SetValue(const Color3f& _color) {
	type  = Type::Value;
	color = _color;
}

void UAssetMaterial::Albedo::SetTexture(uint32_t _index) {
	type  = Type::Texture;
	index = _index;
}

void UAssetMaterial::Albedo::SetValueMultiply(const Color3f& _color) {
	if (type != Type::Texture) { 
		SetValue(_color); //!< Textureが設定されていない場合はValueとして設定
		return;
	}

	type  = Type::Multiply;
	color = _color;
}

void UAssetMaterial::Albedo::SetTextureMultiply(uint32_t _index) {
	type  = Type::Multiply;
	index = _index;
}

void UAssetMaterial::Albedo::SetImGuiCommand() {
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

void UAssetMaterial::Transparency::Init() {
	type = Type::Value;
	alpha = 1.0f;
	index = NULL;
}

void UAssetMaterial::Transparency::SetValue(float _alpha) {
	type = Type::Value;
	alpha = _alpha;
}

void UAssetMaterial::Transparency::SetTexture(uint32_t _index) {
	type = Type::Texture;
	index = _index;
}

void UAssetMaterial::Transparency::SetImGuiCommand() {
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

void UAssetMaterial::Normal::Init() {
	type = Type::None;
	index = NULL;
}

void UAssetMaterial::Normal::SetNone() {
	type = Type::None;
}

void UAssetMaterial::Normal::SetTexture(uint32_t _index) {
	type = Type::Texture;
	index = _index;
}

void UAssetMaterial::Normal::SetImGuiCommand() {
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

void UAssetMaterial::Property::Init() {
	type = Type::Value;
	value = 0.0f;
	index = NULL;
}

void UAssetMaterial::Property::SetValue(float _value) {
	type = Type::Value;
	value = _value;
}

void UAssetMaterial::Property::SetTexture(uint32_t _index) {
	type = Type::Texture;
	index = _index;
}

void UAssetMaterial::Property::SetImGuiCommand() {
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

void UAssetMaterial::SurfaceProperties::Init() {
	ao.Init();
	roughness.Init();
	metallic.Init();
}

void UAssetMaterial::SurfaceProperties::SetImGuiCommand() {
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

void UAssetMaterial::UVTransformation::Init() {
	mat = Matrix4x4::Identity();
}

void UAssetMaterial::UVTransformation::Transfer(const Matrix4x4& _mat) {
	mat = _mat;
}

////////////////////////////////////////////////////////////////////////////////////////////
// MaterialBuffer structure methods
////////////////////////////////////////////////////////////////////////////////////////////

void UAssetMaterial::MaterialBuffer::Init() {
	transformation.Init();
	albedo.Init();
	transparency.Init();
	normal.Init();
	properties.Init();
}

////////////////////////////////////////////////////////////////////////////////////////////
// UAssetMaterial class methods
////////////////////////////////////////////////////////////////////////////////////////////

void UAssetMaterial::Setup(const aiMaterial* material, const std::filesystem::path& directory) {

	// albedoの取得
	textures_[static_cast<uint8_t>(TextureType::Albedo)] = GetTextureId(material, aiTextureType_DIFFUSE, directory);

	// normalの取得
	if (material->GetTextureCount(aiTextureType_HEIGHT) != 0) { //!< .objの場合
		textures_[static_cast<uint8_t>(TextureType::Bump)] = GetTextureId(material, aiTextureType_HEIGHT, directory, true);

	} else if (material->GetTextureCount(aiTextureType_NORMALS) != 0) { //!< .gltfの場合
		textures_[static_cast<uint8_t>(TextureType::Bump)] = GetTextureId(material, aiTextureType_NORMALS, directory, true);
	}

	// roughnessの取得
	textures_[static_cast<uint8_t>(TextureType::Roughness)] = GetTextureId(material, aiTextureType_DIFFUSE_ROUGHNESS, directory, true);

	// metallicの取得
	textures_[static_cast<uint8_t>(TextureType::Metallic)] = GetTextureId(material, aiTextureType_METALNESS, directory, true);

	// ambient occlusionの取得
	textures_[static_cast<uint8_t>(TextureType::AmbientOcclusion)] = GetTextureId(material, aiTextureType_AMBIENT_OCCLUSION, directory, true);

	// colorの取得
	aiColor3D color;
	if (material->Get(AI_MATKEY_COLOR_DIFFUSE, color) == AI_SUCCESS) {
		color_ = { color.r, color.g, color.b };
	}

	// roughnessの取得
	float roughness;
	if (material->Get(AI_MATKEY_ROUGHNESS_FACTOR, roughness) == AI_SUCCESS) {
		roughness_ = roughness;
	}

	// metallicの取得
	float metallic;
	if (material->Get(AI_MATKEY_METALLIC_FACTOR, metallic) == AI_SUCCESS) {
		metallic_ = metallic;
	}

	// todo: specularFactorの設定

	UBaseAsset::Complete();
	Logger::EngineThreadLog(std::format("[UAssetMaterial]: material setup complete. uuid: {}", UBaseAsset::GetId().Serialize()));
}

void UAssetMaterial::Update() {

	auto& parameter = buffer_->At(0);

	// diffuse
	if (textures_[static_cast<uint8_t>(TextureType::Albedo)].has_value()) {
		auto texture = sUAssetStorage->GetAsset<UAssetTexture>(textures_[static_cast<uint8_t>(TextureType::Albedo)].value());

		parameter.albedo.SetTexture(texture->GetDescriptorSRV().GetIndex());
		parameter.transparency.SetTexture(texture->GetDescriptorSRV().GetIndex());

	} else {
		parameter.albedo.SetValue(color_);
		parameter.transparency.SetValue(1.0f);
	}

	// bump
	if (textures_[static_cast<uint8_t>(TextureType::Bump)].has_value()) {
		auto texture = sUAssetStorage->GetAsset<UAssetTexture>(textures_[static_cast<uint8_t>(TextureType::Bump)].value());

		parameter.normal.SetTexture(texture->GetDescriptorSRV().GetIndex());
	}

	// roughness
	if (textures_[static_cast<uint8_t>(TextureType::Roughness)].has_value()) {
		auto texture = sUAssetStorage->GetAsset<UAssetTexture>(textures_[static_cast<uint8_t>(TextureType::Roughness)].value());

		parameter.properties.roughness.SetTexture(texture->GetDescriptorSRV().GetIndex());

	} else {
		parameter.properties.roughness.SetValue(roughness_);
	}

	// metallic
	if (textures_[static_cast<uint8_t>(TextureType::Metallic)].has_value()) {
		auto texture = sUAssetStorage->GetAsset<UAssetTexture>(textures_[static_cast<uint8_t>(TextureType::Metallic)].value());
		
		parameter.properties.metallic.SetTexture(texture->GetDescriptorSRV().GetIndex());

	} else {
		parameter.properties.metallic.SetValue(metallic_);
	}

	// ambient occlusion
	if (textures_[static_cast<uint8_t>(TextureType::AmbientOcclusion)].has_value()) {
		auto texture = sUAssetStorage->GetAsset<UAssetTexture>(textures_[static_cast<uint8_t>(TextureType::AmbientOcclusion)].value());

		parameter.properties.ao.SetTexture(texture->GetDescriptorSRV().GetIndex());
	}

}

void UAssetMaterial::Copy(const UAssetMaterial& material) {

	textures_ = material.textures_;

	color_     = material.color_;
	roughness_ = material.roughness_;
	metallic_  = material.metallic_;

	transform_ = material.transform_;

	mode_ = material.mode_;

	buffer_->At(0) = material.buffer_->At(0);
}

void UAssetMaterial::ShowInspector() {
	UBaseAsset::ShowInspector();

	if (!UBaseAsset::IsComplete()) { //!< loadが完了していない場合
		ImGui::Text("loading...");
		return;
	}

	if (ImGui::BeginCombo("mode", magic_enum::enum_name(mode_).data())) {
		for (const auto& [value, name] : magic_enum::enum_entries<Mode>()) {
			if (ImGui::Selectable(name.data(), mode_ == value)) {
				mode_ = value;
			}
		}
		ImGui::EndCombo();
	}



}

const D3D12_GPU_VIRTUAL_ADDRESS& UAssetMaterial::GetGPUVirtualAddress() const {
	Exception::Assert(buffer_ != nullptr, "buffer is not create.");
	return buffer_->GetGPUVirtualAddress();
}

const UAssetMaterial::MaterialBuffer& UAssetMaterial::GetBuffer() const {
	Exception::Assert(buffer_ != nullptr, "buffer is not create.");
	return buffer_->At(0);
}

UAssetMaterial::MaterialBuffer& UAssetMaterial::GetBuffer() {
	Exception::Assert(buffer_ != nullptr, "buffer is not create.");
	return buffer_->At(0);
}

std::optional<Uuid> UAssetMaterial::GetTextureId(const aiMaterial* aiMaterial, aiTextureType type, const std::filesystem::path& directory, bool isIntensity) {
	if (aiMaterial->GetTextureCount(type) == 0) {
		return std::nullopt; //!< テクスチャが存在しない場合はnulloptを返す
	}

	aiString part;
	aiMaterial->GetTexture(type, 0, &part);

	std::filesystem::path filepath = directory / part.C_Str();

	UContentTexture::Option option = isIntensity ? UContentTexture::Option{ UContentTexture::Encoding::Intensity, true } : UContentTexture::Option{ UContentTexture::Encoding::Lightness, true };

	return sUContentStorage->Import<UContentTexture>(filepath, option)->GetId(); //!< UContentStorageからIdを取得して返す
}

void UAssetMaterial::CreateBuffer() {
	buffer_ = std::make_unique<DimensionBuffer<MaterialBuffer>>();
	buffer_->Create(SxavengerSystem::GetDxDevice(), 1);
	buffer_->At(0).Init();
}
