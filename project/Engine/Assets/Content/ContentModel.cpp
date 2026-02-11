#include "ContentModel.h"
SXAVENGER_ENGINE_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* asset
#include "../Asset/AssetStorage.h"

//* engine
#include <Engine/System/Utility/StreamLogger.h>

//* lib
#include <Lib/Adapter/Json/JsonHandler.h>

////////////////////////////////////////////////////////////////////////////////////////////
// ContentModel class methods
////////////////////////////////////////////////////////////////////////////////////////////

void ContentModel::AsyncLoad(MAYBE_UNUSED const DirectXQueueContext* context) {
	BaseContent::CheckExist();

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

	Load(BaseContent::GetFilepath(), option);
}

void ContentModel::AttachUuid() {
	BaseContent::CheckExist();

	// sceneの取得
	Assimp::Importer importer;
	const aiScene* aiScene = importer.ReadFile(BaseContent::GetFilepath().generic_string().c_str(), 0);

	if (aiScene == nullptr) {
		StreamLogger::Exception("model load failed. filepath: " + BaseContent::GetFilepath().generic_string(), importer.GetErrorString());
		return;
	}

	// idのサイズを確保
	meshes_.resize(aiScene->mNumMeshes);
	materials_.resize(aiScene->mNumMaterials);

	// idを取得
	AssignUuid();

	// storageに登録
	for (size_t i = 0; i < meshes_.size(); ++i) {
		auto asset = std::make_shared<AssetMesh>(meshes_[i]);
		sAssetStorage->Register(asset, BaseContent::GetFilepath());
	}

	for (size_t i = 0; i < materials_.size(); ++i) {
		auto asset = std::make_shared<AssetMaterial>(materials_[i]);
		sAssetStorage->Register(asset, BaseContent::GetFilepath());
	}

	{
		auto asset = std::make_shared<AssetSkeleton>(skeleton_);
		sAssetStorage->Register(asset, BaseContent::GetFilepath());
	}
	
}

void ContentModel::ShowInspector() {
	BaseContent::ShowInspector();

	// mesh
	if (ImGui::CollapsingHeader("meshes ## header", ImGuiTreeNodeFlags_DefaultOpen)) {
		for (size_t i = 0; i < meshes_.size(); ++i) {
			if (ImGui::Button(std::format("mesh {}", i).c_str())) {
				BaseContent::SelectInspector(sAssetStorage->GetAsset<AssetMesh>(meshes_[i]).get());
			}
		}
	}

	// material
	if (ImGui::CollapsingHeader("materials ## header", ImGuiTreeNodeFlags_DefaultOpen)) {
		for (size_t i = 0; i < materials_.size(); ++i) {
			if (ImGui::Button(std::format("material {}", i).c_str())) {
				BaseContent::SelectInspector(sAssetStorage->GetAsset<AssetMaterial>(materials_[i]).get());
			}
		}
	}

	// skeleton
	if (ImGui::CollapsingHeader("skeleton ## header", ImGuiTreeNodeFlags_DefaultOpen)) {
		if (ImGui::Button("skeleton")) {
			BaseContent::SelectInspector(sAssetStorage->GetAsset<AssetSkeleton>(skeleton_).get());
		}
	}
}

void ContentModel::Load(const std::filesystem::path& filepath, uint32_t assimpOption) {

	// sceneの取得
	Assimp::Importer importer;
	const aiScene* aiScene = importer.ReadFile(filepath.generic_string().c_str(), assimpOption);

	if (aiScene == nullptr) {
		StreamLogger::Exception("model load failed. filepath: " + filepath.generic_string(), importer.GetErrorString());
		return;
	}

	// idのサイズを確保
	meshes_.resize(aiScene->mNumMeshes);
	materials_.resize(aiScene->mNumMaterials);

	// materialの読み込み
	LoadMaterials(aiScene, filepath);

	// meshの読み込み
	LoadMeshes(aiScene);

	// nodeの読み込み
	LoadSkeleton(aiScene);
}

void ContentModel::AssignUuid() {
	//!< multi threadにする場合, thread safeにする必要がある.

	json meta = BaseContent::LoadMeta();

	//!< meshのid取得
	if (meta.contains("meshes")) {
		//!< idが既に存在する場合は、metaから取得する
		const auto& ids = meta["meshes"].get<std::vector<std::string>>();
		for (size_t i = 0; i < ids.size(); ++i) {
			meshes_[i] = Uuid::Deserialize(ids[i]);
		}

	} else {
		//!< idが存在しない場合は、新しくidを生成し, metaに保存する
		std::generate(meshes_.begin(), meshes_.end(), []() { return Uuid::Generate(); });

		meta["meshes"] = json::array();
		for (const auto& mesh : meshes_) {
			meta["meshes"].emplace_back(mesh.Serialize());
		}

		BaseContent::SaveMeta(meta);
	}

	//!< materialのid取得
	if (meta.contains("materials")) {
		//!< idが既に存在する場合は、metaから取得する
		const auto& ids = meta["materials"].get<std::vector<std::string>>();
		for (size_t i = 0; i < ids.size(); ++i) {
			materials_[i] = Uuid::Deserialize(ids[i]);
		}

	} else {
		//!< idが存在しない場合は、新しくidを生成し, metaに保存する
		std::generate(materials_.begin(), materials_.end(), []() { return Uuid::Generate(); });

		meta["materials"] = json::array();
		for (const auto& material : materials_) {
			meta["materials"].emplace_back(material.Serialize());
		}

		BaseContent::SaveMeta(meta);
	}

	//!< skeletonのid取得
	if (meta.contains("skeleton")) {
		//!< idが既に存在する場合は、metaから取得する
		skeleton_ = Uuid::Deserialize(meta["skeleton"].get<std::string>());

	} else {
		//!< idが存在しない場合は、新しくidを生成し, metaに保存する
		skeleton_ = Uuid::Generate();

		meta["skeleton"] = skeleton_.Serialize();
		BaseContent::SaveMeta(meta);
	}

}

void ContentModel::LoadMeshes(const aiScene* aiScene) {
	// meshの数だけの要素を確保
	materialIndices_.resize(aiScene->mNumMeshes);

	for (uint32_t i = 0; i < aiScene->mNumMeshes; ++i) {
		// meshの取得
		const aiMesh* mesh = aiScene->mMeshes[i];

		// assetの取得
		auto asset = sAssetStorage->GetAsset<AssetMesh>(meshes_[i]);
		asset->Setup(mesh);

		// material indexの格納
		materialIndices_[i] = mesh->mMaterialIndex;
	}
}

void ContentModel::LoadMaterials(const aiScene* aiScene, const std::filesystem::path& filepath) {
	for (uint32_t i = 0; i < aiScene->mNumMaterials; ++i) {
		// materialの取得
		const aiMaterial* material = aiScene->mMaterials[i];

		// assetの取得
		auto asset = sAssetStorage->GetAsset<AssetMaterial>(materials_[i]);
		asset->Setup(material, filepath.parent_path());
	}
}

BornNode ContentModel::ReadNode(aiNode* node) {

	BornNode result = {};

	// transformの取得
	aiVector3D scale, translate;
	aiQuaternion rotate;

	node->mTransformation.Decompose(scale, rotate, translate);

	// resultに代入
	result.transform.scale     = { scale.x, scale.y, scale.z };
	result.transform.rotate    = ConvertQuaternion(rotate);
	result.transform.translate = ConvertPosition3(translate);

	// nodeのlocalMatの取得
	aiMatrix4x4 aiLocalMatrix = node->mTransformation;
	aiLocalMatrix.Transpose();

	// resultにcopy
	std::memcpy(&result.localMatrix, &aiLocalMatrix, sizeof(Matrix4x4));

	// ノード名を格納
	result.name = node->mName.C_Str();

	// meshのindexの取得
	for (uint32_t meshIndex = 0; meshIndex < node->mNumMeshes; ++meshIndex) {
		result.meshIndices.emplace_back(node->mMeshes[meshIndex]);
	}

	// 子の数だけの要素の格納
	result.children.resize(node->mNumChildren);

	// 子のノード情報の取得
	for (uint32_t childIndex = 0; childIndex < node->mNumChildren; ++childIndex) {
		result.children[childIndex] = ReadNode(node->mChildren[childIndex]);
	}

	return result;
}

void ContentModel::LoadSkeleton(const aiScene* aiScene) {
	// nodeの読み込み
	root_ = ReadNode(aiScene->mRootNode);

	// assetの取得
	auto asset = sAssetStorage->GetAsset<AssetSkeleton>(skeleton_);
	asset->Setup(root_);
}

Vector3f ContentModel::ConvertNormal(const aiVector3D& aiVector) {
	return { aiVector.x, aiVector.y, -aiVector.z }; //!< 左手座標系に変換
}

Vector3f ContentModel::ConvertPosition3(const aiVector3D& aiVector) {
	return { aiVector.x, aiVector.y, -aiVector.z }; //!< 左手座標系に変換
}

Vector4f ContentModel::ConvertPosition4(const aiVector3D& aiVector) {
	return { aiVector.x, aiVector.y, -aiVector.z, 1.0f }; //!< 左手座標系に変換
}

Quaternion ContentModel::ConvertQuaternion(const aiQuaternion& aiQuaternion) {
	return { -aiQuaternion.x, -aiQuaternion.y, aiQuaternion.z, aiQuaternion.w }; //!< 左手座標系に変換
}
