#include "AssetMaterial.h"
SXAVENGER_ENGINE_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/Utility/StreamLogger.h>
#include <Engine/System/System.h>

//* lib
#include <Lib/Adapter/Json/JsonHandler.h>
#include <Lib/Adapter/Json/JsonSerializer.h>

////////////////////////////////////////////////////////////////////////////////////////////
// [Material] MaterialBuffer structure methods
////////////////////////////////////////////////////////////////////////////////////////////

void Asset::Material::MaterialBuffer::Init() {
	*this = MaterialBuffer{};
}

////////////////////////////////////////////////////////////////////////////////////////////
// Material class methods
////////////////////////////////////////////////////////////////////////////////////////////

void Asset::Material::Setup(const aiMaterial* material, const std::filesystem::path& directory) {

	// albedoの取得
	textures_[static_cast<uint8_t>(Texture::Albedo)] = GetTextureId(material, aiTextureType_DIFFUSE, directory);

	// normalの取得
	if (material->GetTextureCount(aiTextureType_HEIGHT) != 0) { //!< .objの場合
		textures_[static_cast<uint8_t>(Texture::Normal)] = GetTextureId(material, aiTextureType_HEIGHT, directory, true);

	} else if (material->GetTextureCount(aiTextureType_NORMALS) != 0) { //!< .gltfの場合
		textures_[static_cast<uint8_t>(Texture::Normal)] = GetTextureId(material, aiTextureType_NORMALS, directory, true);
	}

	// roughnessの取得
	textures_[static_cast<uint8_t>(Texture::Roughness)] = GetTextureId(material, aiTextureType_DIFFUSE_ROUGHNESS, directory, true);

	// metallicの取得
	textures_[static_cast<uint8_t>(Texture::Metallic)] = GetTextureId(material, aiTextureType_METALNESS, directory, true);

	// emissiveの取得
	textures_[static_cast<uint8_t>(Texture::Emissive)] = GetTextureId(material, aiTextureType_EMISSIVE, directory);

	// colorの取得
	aiColor3D color;
	if (material->Get(AI_MATKEY_COLOR_DIFFUSE, color) == AI_SUCCESS) {
		buffer_.At().albedo.value = { color.r, color.g, color.b };
	}

	// roughnessの取得
	float roughness;
	if (material->Get(AI_MATKEY_ROUGHNESS_FACTOR, roughness) == AI_SUCCESS) {
		buffer_.At().roughness.value = roughness;
	}

	// metallicの取得
	float metallic;
	if (material->Get(AI_MATKEY_METALLIC_FACTOR, metallic) == AI_SUCCESS) {
		buffer_.At().metallic.value = metallic;
	}

	// transform
	transform_ = GetTransform2d(material);

	BaseAsset::SetComplete();
	StreamLogger::EngineThreadLog(std::format("[AssetMaterial]: material [assimp] setup complete. uuid: {}", BaseAsset::SerializeId()));
}

void Asset::Material::Setup(const json& data) {
	
	//!< albedoの取得
	if (data.contains("albedo")) {
		const json& albedo = data["albedo"];
		buffer_.At().albedo.value                        = JsonSerializeFormatter<Color3f>::Deserialize(albedo["value"]);
		textures_[static_cast<uint8_t>(Texture::Albedo)] = GetTextureId(albedo["texture"], Texture::Albedo);
	}

	//!< normalの取得
	if (data.contains("normal")) {
		const json& normal = data["normal"];
		textures_[static_cast<uint8_t>(Texture::Normal)] = GetTextureId(normal["texture"], Texture::Normal, true);
	}

	//!< transparencyの取得
	if (data.contains("transparency")) {
		const json& transparency = data["transparency"];
		buffer_.At().transparency.value                        = JsonSerializeFormatter<float>::Deserialize(transparency["value"]);
		textures_[static_cast<uint8_t>(Texture::Transparency)] = GetTextureId(transparency["texture"], Texture::Transparency, true);
	}

	//!< roughnessの取得
	if (data.contains("roughness")) {
		const json& roughness = data["roughness"];
		buffer_.At().roughness.value                        = JsonSerializeFormatter<float>::Deserialize(roughness["value"]);
		textures_[static_cast<uint8_t>(Texture::Roughness)] = GetTextureId(roughness["texture"], Texture::Roughness, true);
	}

	//!< metallicの取得
	if (data.contains("metallic")) {
		const json& metallic = data["metallic"];
		buffer_.At().metallic.value                        = JsonSerializeFormatter<float>::Deserialize(metallic["value"]);
		textures_[static_cast<uint8_t>(Texture::Metallic)] = GetTextureId(metallic["texture"], Texture::Metallic, true);
	}

	//!< emissiveの取得
	if (data.contains("emissive")) {
		const json& emissive = data["emissive"];
		buffer_.At().emissive.value                        = JsonSerializeFormatter<Color3f>::Deserialize(emissive["value"]);
		buffer_.At().emissive.intensity                    = JsonSerializeFormatter<float>::Deserialize(emissive["intensity"]);
		textures_[static_cast<uint8_t>(Texture::Emissive)] = GetTextureId(emissive["texture"], Texture::Emissive, true);
	}

	//!< transformの取得
	if (data.contains("transform")) {
		const json& transform = data["transform"];
		transform_ = Transform2d::Deserialize(transform);
	}

}

void Asset::Material::CreateBuffer() {
	buffer_.Create(System::GetDxDevice());
	buffer_.At() = MaterialBuffer{};
}

std::optional<Uuid> Asset::Material::GetTextureId(const aiMaterial* aiMaterial, aiTextureType type, const std::filesystem::path& directory, bool isIntensity) {
	// TODO: ContentからのTextureの取得
	return std::nullopt;
}

Transform2d Asset::Material::GetTransform2d(const aiMaterial* aiMaterial) {
	Transform2d transform = {};

	aiUVTransform t;
	if (aiMaterial->Get(AI_MATKEY_UVTRANSFORM(aiTextureType_DIFFUSE, 0), t) == AI_SUCCESS) {
		transform.translate = { t.mTranslation.x, t.mTranslation.y };
		transform.rotate    = t.mRotation;
		transform.scale     = { t.mScaling.x, t.mScaling.y };
	}

	return transform;
}

std::optional<Uuid> Asset::Material::GetTextureId(const json& data, Texture texture, bool isIntensity) {
	// TODO: UuidからのTextureの取得
	return std::nullopt;
}

