#include "AssetMaterial.h"
SXAVENGER_ENGINE_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* asset
#include "AssetTexture.h"
#include "AssetStorage.h"

//* content
#include "../Content/ContentTexture.h"
#include "../Content/ContentStorage.h"

//* engine
#include <Engine/System/Utility/StreamLogger.h>
#include <Engine/System/System.h>

//* lib
#include <Lib/Adapter/Json/JsonHandler.h>
#include <Lib/Adapter/Json/JsonSerializer.h>

////////////////////////////////////////////////////////////////////////////////////////////
// [AssetMaterial] MaterialBuffer structure methods
////////////////////////////////////////////////////////////////////////////////////////////

void AssetMaterial::MaterialBuffer::Init() {
	*this = MaterialBuffer{};
}

void AssetMaterial::MaterialBuffer::SetAlbedoTexture(const DxObject::ReferenceDescriptor& descriptor) {
	albedo.index = descriptor.GetIndex();
	flags.Set(TextureFlag::Albedo);
}

void AssetMaterial::MaterialBuffer::SetAlbedoTexture(std::nullopt_t) {
	albedo.index = NULL;
	flags.Reset(TextureFlag::Albedo);
}

void AssetMaterial::MaterialBuffer::SetAlbedoTextureOptional(const std::optional<DxObject::ReferenceDescriptor>& descriptor) {
	if (descriptor.has_value()) {
		SetAlbedoTexture(descriptor.value());

	} else {
		SetAlbedoTexture(std::nullopt);
	}
}

void AssetMaterial::MaterialBuffer::SetTransparencyTexture(const DxObject::ReferenceDescriptor& descriptor) {
	transparency.index = descriptor.GetIndex();
	flags.Set(TextureFlag::Transparency);
}

void AssetMaterial::MaterialBuffer::SetTransparencyTexture(std::nullopt_t) {
	transparency.index = NULL;
	flags.Reset(TextureFlag::Transparency);
}

void AssetMaterial::MaterialBuffer::SetTransparencyTextureOptional(const std::optional<DxObject::ReferenceDescriptor>& descriptor) {
	if (descriptor.has_value()) {
		SetTransparencyTexture(descriptor.value());

	} else {
		SetTransparencyTexture(std::nullopt);
	}
}

void AssetMaterial::MaterialBuffer::SetNormalTexture(const DxObject::ReferenceDescriptor& descriptor) {
	normal.index = descriptor.GetIndex();
	flags.Set(TextureFlag::Normal);
}

void AssetMaterial::MaterialBuffer::SetNormalTexture(std::nullopt_t) {
	normal.index = NULL;
	flags.Reset(TextureFlag::Normal);
}

void AssetMaterial::MaterialBuffer::SetNormalTextureOptional(const std::optional<DxObject::ReferenceDescriptor>& descriptor) {
	if (descriptor.has_value()) {
		SetNormalTexture(descriptor.value());

	} else {
		SetNormalTexture(std::nullopt);
	}
}

void AssetMaterial::MaterialBuffer::SetRoughnessTexture(const DxObject::ReferenceDescriptor& descriptor) {
	roughness.index = descriptor.GetIndex();
	flags.Set(TextureFlag::Roughness);
}

void AssetMaterial::MaterialBuffer::SetRoughnessTexture(std::nullopt_t) {
	roughness.index = NULL;
	flags.Reset(TextureFlag::Roughness);
}

void AssetMaterial::MaterialBuffer::SetRoughnessTextureOptional(const std::optional<DxObject::ReferenceDescriptor>& descriptor) {
	if (descriptor.has_value()) {
		SetRoughnessTexture(descriptor.value());

	} else {
		SetRoughnessTexture(std::nullopt);
	}
}

void AssetMaterial::MaterialBuffer::SetMetallicTexture(const DxObject::ReferenceDescriptor& descriptor) {
	metallic.index = descriptor.GetIndex();
	flags.Set(TextureFlag::Metallic);
}

void AssetMaterial::MaterialBuffer::SetMetallicTexture(std::nullopt_t) {
	metallic.index = NULL;
	flags.Reset(TextureFlag::Metallic);
}

void AssetMaterial::MaterialBuffer::SetMetallicTextureOptional(const std::optional<DxObject::ReferenceDescriptor>& descriptor) {
	if (descriptor.has_value()) {
		SetMetallicTexture(descriptor.value());

	} else {
		SetMetallicTexture(std::nullopt);
	}
}

void AssetMaterial::MaterialBuffer::SetEmissiveTexture(const DxObject::ReferenceDescriptor& descriptor) {
	emissive.index = descriptor.GetIndex();
	flags.Set(TextureFlag::Emissive);
}

void AssetMaterial::MaterialBuffer::SetEmissiveTexture(std::nullopt_t) {
	emissive.index = NULL;
	flags.Reset(TextureFlag::Emissive);
}

void AssetMaterial::MaterialBuffer::SetEmissiveTextureOptional(const std::optional<DxObject::ReferenceDescriptor>& descriptor) {
	if (descriptor.has_value()) {
		SetEmissiveTexture(descriptor.value());
	} else {
		SetEmissiveTexture(std::nullopt);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////
// Material class methods
////////////////////////////////////////////////////////////////////////////////////////////

void AssetMaterial::Setup(const aiMaterial* material, const std::filesystem::path& directory) {

	// 名前の取得
	BaseAsset::SetName(material->GetName().C_Str());

	// albedo, transparencyの取得
	textures_[static_cast<uint8_t>(Texture::Albedo)]       = GetTextureId(material, aiTextureType_DIFFUSE, directory);
	textures_[static_cast<uint8_t>(Texture::Transparency)] = textures_[static_cast<uint8_t>(Texture::Albedo)]; //!< 同一Textureとして使用する.

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

void AssetMaterial::Setup(const json& data) {
	
	//!< albedoの取得
	if (data.contains("albedo")) {
		const json& albedo = data["albedo"];
		buffer_.At().albedo.value                        = JsonSerializeFormatter<Color3f>::Deserialize(albedo["value"]);
		textures_[static_cast<uint8_t>(Texture::Albedo)] = GetTextureId(albedo["texture"]);
	}

	//!< normalの取得
	if (data.contains("normal")) {
		const json& normal = data["normal"];
		textures_[static_cast<uint8_t>(Texture::Normal)] = GetTextureId(normal["texture"], true);
	}

	//!< transparencyの取得
	if (data.contains("transparency")) {
		const json& transparency = data["transparency"];
		buffer_.At().transparency.value                        = JsonSerializeFormatter<float>::Deserialize(transparency["value"]);
		textures_[static_cast<uint8_t>(Texture::Transparency)] = GetTextureId(transparency["texture"], true);
	}

	//!< roughnessの取得
	if (data.contains("roughness")) {
		const json& roughness = data["roughness"];
		buffer_.At().roughness.value                        = JsonSerializeFormatter<float>::Deserialize(roughness["value"]);
		textures_[static_cast<uint8_t>(Texture::Roughness)] = GetTextureId(roughness["texture"], true);
	}

	//!< metallicの取得
	if (data.contains("metallic")) {
		const json& metallic = data["metallic"];
		buffer_.At().metallic.value                        = JsonSerializeFormatter<float>::Deserialize(metallic["value"]);
		textures_[static_cast<uint8_t>(Texture::Metallic)] = GetTextureId(metallic["texture"], true);
	}

	//!< emissiveの取得
	if (data.contains("emissive")) {
		const json& emissive = data["emissive"];
		buffer_.At().emissive.value                        = JsonSerializeFormatter<Color3f>::Deserialize(emissive["value"]);
		buffer_.At().emissive.intensity                    = JsonSerializeFormatter<float>::Deserialize(emissive["intensity"]);
		textures_[static_cast<uint8_t>(Texture::Emissive)] = GetTextureId(emissive["texture"], true);
	}

	//!< transformの取得
	if (data.contains("transform")) {
		const json& transform = data["transform"];
		transform_ = Transform2d::Deserialize(transform);
	}

	BaseAsset::SetComplete();
	StreamLogger::EngineThreadLog(std::format("[AssetMaterial]: material [json] setup complete. uuid: {}", BaseAsset::SerializeId()));
}

void AssetMaterial::Update() {

	auto& parameter = buffer_.At();

	// albedo
	parameter.SetAlbedoTextureOptional(AssetMaterial::GetTextureDescriptor(Texture::Albedo));

	// transparency
	parameter.SetTransparencyTextureOptional(AssetMaterial::GetTextureDescriptor(Texture::Transparency));

	// normal
	parameter.SetNormalTextureOptional(AssetMaterial::GetTextureDescriptor(Texture::Normal));

	// roughness
	parameter.SetRoughnessTextureOptional(AssetMaterial::GetTextureDescriptor(Texture::Roughness));

	// metallic
	parameter.SetMetallicTextureOptional(AssetMaterial::GetTextureDescriptor(Texture::Metallic));

	// emissive
	parameter.SetEmissiveTextureOptional(AssetMaterial::GetTextureDescriptor(Texture::Emissive));

	// transform
	parameter.SetTransformation(transform_.ToMatrix());

}

AssetMaterial& AssetMaterial::operator=(const AssetMaterial& other) {
	textures_  = other.textures_;
	transform_ = other.transform_;

	buffer_.At() = other.buffer_.At();

	return *this;
}

void AssetMaterial::CreateBuffer() {
	buffer_.Create(System::GetDxDevice());
	buffer_.At() = MaterialBuffer{};
}

std::optional<Uuid> AssetMaterial::GetTextureId(const aiMaterial* aiMaterial, aiTextureType type, const std::filesystem::path& directory, bool isIntensity) {

	if (aiMaterial->GetTextureCount(type) == 0) {
		return std::nullopt; //!< テクスチャが存在しない場合はnulloptを返す
	}

	aiString part;
	aiMaterial->GetTexture(type, 0, &part);

	std::filesystem::path filepath = directory / part.C_Str();

	ContentTexture::Option option = {};
	option.encoding         = isIntensity ? ContentTexture::Encoding::Intensity : ContentTexture::Encoding::Lightness;
	option.isGenerateMipmap = true;

	return sContentStorage->Import<ContentTexture>(filepath, option)->GetId(); //!< ContentStorageからIdを取得して返す

}

Transform2d AssetMaterial::GetTransform2d(const aiMaterial* aiMaterial) {
	Transform2d transform = {};

	aiUVTransform t;
	if (aiMaterial->Get(AI_MATKEY_UVTRANSFORM(aiTextureType_DIFFUSE, 0), t) == AI_SUCCESS) {
		transform.translate = { t.mTranslation.x, t.mTranslation.y };
		transform.rotate    = t.mRotation;
		transform.scale     = { t.mScaling.x, t.mScaling.y };
	}

	return transform;
}

std::optional<Uuid> AssetMaterial::GetTextureId(const json& data, bool isIntensity) {
	//!< jsonからのTextureの取得

	if (data.is_null()) {
		return std::nullopt; //!< テクスチャが存在しない場合はnulloptを返す
	}

	std::filesystem::path filepath = data.get<std::string>();

	ContentTexture::Option option = {};
	option.encoding         = isIntensity ? ContentTexture::Encoding::Intensity : ContentTexture::Encoding::Lightness;
	option.isGenerateMipmap = true;

	return sContentStorage->Import<ContentTexture>(filepath, option)->GetId(); //!< ContentStorageからIdを取得して返す
}

std::optional<DxObject::ReferenceDescriptor> AssetMaterial::GetTextureDescriptor(Texture texture) const {

	if (textures_[static_cast<uint8_t>(texture)].has_value()) {
		std::shared_ptr<AssetTexture> asset
			= sAssetStorage->Get<AssetTexture>(textures_[static_cast<uint8_t>(texture)].value());

		if (asset->IsComplete()) {
			return asset->GetDescriptorSRV();
		}
	}

	return std::nullopt;
}

