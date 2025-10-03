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

void UContentModel::AttachUuid() {
	UBaseContent::CheckExist();

	// sceneの取得
	Assimp::Importer importer;
	const aiScene* aiScene = importer.ReadFile(UBaseContent::GetFilepath().generic_string().c_str(), 0);

	if (aiScene == nullptr) {
		Exception::Assert(false, "model load failed. filepath: " + UBaseContent::GetFilepath().generic_string(), importer.GetErrorString());
		return;
	}

	// idのサイズを確保
	meshes_.resize(aiScene->mNumMeshes);
	materials_.resize(aiScene->mNumMaterials);

	// idを取得
	GetUuid();

	// storageに登録
	for (size_t i = 0; i < meshes_.size(); ++i) {
		auto asset = std::make_shared<UAssetMesh>(meshes_[i]);
		sUAssetStorage->Register(asset, UBaseContent::GetFilepath());
	}

	for (size_t i = 0; i < materials_.size(); ++i) {
		auto asset = std::make_shared<UAssetMaterial>(materials_[i]);
		sUAssetStorage->Register(asset, UBaseContent::GetFilepath());
	}

	{
		auto asset = std::make_shared<UAssetSkeleton>(skeleton_);
		sUAssetStorage->Register(asset, UBaseContent::GetFilepath());
	}
	
}

void UContentModel::ShowInspector() {
	UBaseContent::ShowInspector();

	// mesh
	if (ImGui::CollapsingHeader("meshes ## header", ImGuiTreeNodeFlags_DefaultOpen)) {
		for (size_t i = 0; i < meshes_.size(); ++i) {
			if (ImGui::Button(std::format("mesh {}", i).c_str())) {
				UBaseContent::SelectInspector(sUAssetStorage->GetAsset<UAssetMesh>(meshes_[i]).get());
			}
		}
	}

	// material
	if (ImGui::CollapsingHeader("materials ## header", ImGuiTreeNodeFlags_DefaultOpen)) {
		for (size_t i = 0; i < materials_.size(); ++i) {
			if (ImGui::Button(std::format("material {}", i).c_str())) {
				UBaseContent::SelectInspector(sUAssetStorage->GetAsset<UAssetMaterial>(materials_[i]).get());
			}
		}
	}

	// skeleton
	if (ImGui::CollapsingHeader("skeleton ## header", ImGuiTreeNodeFlags_DefaultOpen)) {
		if (ImGui::Button("skeleton")) {
			UBaseContent::SelectInspector(sUAssetStorage->GetAsset<UAssetSkeleton>(skeleton_).get());
		}
	}
}

void UContentModel::Load(const std::filesystem::path& filepath, uint32_t assimpOption) {

	// sceneの取得
	Assimp::Importer importer;
	const aiScene* aiScene = importer.ReadFile(filepath.generic_string().c_str(), assimpOption);

	if (aiScene == nullptr) {
		Exception::Assert(false, "model load failed. filepath: " + filepath.generic_string(), importer.GetErrorString());
		return;
	}

	// idのサイズを確保
	meshes_.resize(aiScene->mNumMeshes);
	materials_.resize(aiScene->mNumMaterials);

	// meshの読み込み
	LoadMeshes(aiScene);

	// materialの読み込み
	LoadMaterials(aiScene, filepath);

	// nodeの読み込み
	LoadSkeleton(aiScene);
}

void UContentModel::GetUuid() {
	//!< multi threadにする場合, thread safeにする必要がある.

	std::filesystem::path filepath = UBaseContent::GetContentPath();

	json data;
	JsonHandler::LoadFromJson(filepath, data);

	if (data.contains("meshes") && data.contains("materials") && data.contains("skeleton")) {
		//!< Idが既に存在する場合は、Json形式で読み込む
		
		const auto& meshIds = data["meshes"].get<std::vector<std::string>>();
		for (size_t i = 0; i < meshIds.size(); ++i) {
			meshes_[i] = Uuid::Deserialize(meshIds[i]);
		}

		const auto& materialIds = data["materials"].get<std::vector<std::string>>();
		for (size_t i = 0; i < materialIds.size(); ++i) {
			materials_[i] = Uuid::Deserialize(materialIds[i]);
		}

		skeleton_ = Uuid::Deserialize(data["skeleton"].get<std::string>());

	} else {
		//!< Idが存在しない場合は、生成して保存する
		
		std::generate(meshes_.begin(), meshes_.end(), []() { return Uuid::Generate(); });
		std::generate(materials_.begin(), materials_.end(), []() { return Uuid::Generate(); });
		skeleton_ = Uuid::Generate();

		data["meshes"]    = json::array();
		data["materials"] = json::array();
		data["skeleton"]  = json::object();

		for (const auto& mesh : meshes_) {
			data["meshes"].emplace_back(mesh.Serialize());
		}

		for (const auto& material : materials_) {
			data["materials"].emplace_back(material.Serialize());
		}

		data["skeleton"] = skeleton_.Serialize();

		JsonHandler::OverwriteToJson(filepath, data);
		//!< Animation側と競合しないようにするため, 上書きする.
	}
}

void UContentModel::LoadMeshes(const aiScene* aiScene) {
	// meshの数だけの要素を確保
	materialIndices_.resize(aiScene->mNumMeshes);

	for (uint32_t i = 0; i < aiScene->mNumMeshes; ++i) {
		// meshの取得
		const aiMesh* mesh = aiScene->mMeshes[i];

		// assetの取得
		auto asset = sUAssetStorage->GetAsset<UAssetMesh>(meshes_[i]);
		asset->Setup(mesh);

		// material indexの格納
		materialIndices_[i] = mesh->mMaterialIndex;
	}
}

void UContentModel::LoadMaterials(const aiScene* aiScene, const std::filesystem::path& filepath) {
	for (uint32_t i = 0; i < aiScene->mNumMaterials; ++i) {
		// materialの取得
		const aiMaterial* material = aiScene->mMaterials[i];

		// assetの取得
		auto asset = sUAssetStorage->GetAsset<UAssetMaterial>(materials_[i]);
		asset->Setup(material, filepath.parent_path());
	}
}

BornNode UContentModel::ReadNode(aiNode* node) {

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

void UContentModel::LoadSkeleton(const aiScene* aiScene) {
	// nodeの読み込み
	root_ = ReadNode(aiScene->mRootNode);

	// assetの取得
	auto asset = sUAssetStorage->GetAsset<UAssetSkeleton>(skeleton_);
	asset->Setup(root_);
}

Vector3f UContentModel::ConvertNormal(const aiVector3D& aiVector) {
	return { aiVector.x, aiVector.y, -aiVector.z }; //!< 左手座標系に変換
}

Vector3f UContentModel::ConvertPosition3(const aiVector3D& aiVector) {
	return { aiVector.x, aiVector.y, -aiVector.z }; //!< 左手座標系に変換
}

Vector4f UContentModel::ConvertPosition4(const aiVector3D& aiVector) {
	return { aiVector.x, aiVector.y, -aiVector.z, 1.0f }; //!< 左手座標系に変換
}

Quaternion UContentModel::ConvertQuaternion(const aiQuaternion& aiQuaternion) {
	return { -aiQuaternion.x, -aiQuaternion.y, aiQuaternion.z, aiQuaternion.w }; //!< 左手座標系に変換
}
