#include "UContentModel.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* uasset
#include "../Asset/UAssetStorage.h"

//* lib
#include <Lib/Adapter/Json/JsonHandler.h>

////////////////////////////////////////////////////////////////////////////////////////////
// UContentModel class methods
////////////////////////////////////////////////////////////////////////////////////////////

void UContentModel::AsyncLoad(_MAYBE_UNUSED const DirectXQueueContext* context) {
	UBaseContent::CheckExist();

	// idを取得
	GetUuid();

	uint32_t option
		= aiProcess_FlipWindingOrder
		| aiProcess_FlipUVs
		| aiProcess_Triangulate
		| aiProcess_CalcTangentSpace
		| aiProcess_ImproveCacheLocality;
	//!< default option

	if (param_.has_value()) {
		option = std::any_cast<uint32_t>(param_);
	}

	Load(UBaseContent::GetFilepath(), option);
}

void UContentModel::Load(const std::filesystem::path& filepath, uint32_t assimpOption) {

	// sceneの取得
	Assimp::Importer importer; //!< scene保存するため保管
	const aiScene* aiScene = importer.ReadFile(filepath.generic_string().c_str(), assimpOption);

	if (aiScene == nullptr) {
		Exception::Assert(false, "model load failed. filepath: " + filepath.generic_string(), importer.GetErrorString());
		return;
	}

	// idのサイズを確保
	meshes_.resize(aiScene->mNumMeshes);
	materials_.resize(aiScene->mNumMaterials);

	// idを取得
	GetUuid();

	// meshの読み込み
	LoadMeshes(aiScene, filepath);

	// materialの読み込み
	LoadMaterials(aiScene, filepath);
}

void UContentModel::GetUuid() {
	//!< multi threadにする場合, thread safeにする必要がある.

	std::filesystem::path filepath = UBaseContent::GetContentPath();

	json data;
	JsonHandler::LoadFromJson(filepath, data);

	if (data.contains("meshes") && data.contains("materials")) {
		//!< Idが既に存在する場合は、Json形式で読み込む
		
		const auto& meshIds = data["meshes"].get<std::vector<std::string>>();
		for (size_t i = 0; i < meshIds.size(); ++i) {
			meshes_[i] = Uuid::Deserialize(meshIds[i]);
		}

		const auto& materialIds = data["materials"].get<std::vector<std::string>>();
		for (size_t i = 0; i < materialIds.size(); ++i) {
			materials_[i] = Uuid::Deserialize(materialIds[i]);
		}

	} else {
		//!< Idが存在しない場合は、生成して保存する
		
		std::generate(meshes_.begin(), meshes_.end(), []() { return Uuid::Generate(); });
		std::generate(materials_.begin(), materials_.end(), []() { return Uuid::Generate(); });

		data["meshes"]    = json::array();
		data["materials"] = json::array();

		for (const auto& mesh : meshes_) {
			data["meshes"].emplace_back(mesh.Serialize());
		}

		for (const auto& material : materials_) {
			data["materials"].emplace_back(material.Serialize());
		}

		JsonHandler::OverwriteToJson(filepath, data);
		//!< Animation側と競合しないようにするため, 上書きする.
	}
}

void UContentModel::LoadMeshes(const aiScene* aiScene, const std::filesystem::path& filepath) {
	for (uint32_t i = 0; i < aiScene->mNumMeshes; ++i) {
		// meshの取得
		const aiMesh* mesh = aiScene->mMeshes[i];

		auto asset = std::make_shared<UAssetMesh>(meshes_[i]);
		sUAssetStorage->Register(asset, filepath);

		asset->Setup(mesh, materials_[mesh->mMaterialIndex]);
	}
}

void UContentModel::LoadMaterials(const aiScene* aiScene, const std::filesystem::path& filepath) {
	for (uint32_t i = 0; i < aiScene->mNumMaterials; ++i) {
		// materialの取得
		const aiMaterial* material = aiScene->mMaterials[i];

		auto asset = std::make_shared<UAssetMaterial>(materials_[i]);
		sUAssetStorage->Register(asset, filepath);

		asset->Setup(material, filepath.parent_path());
	}
}
