#include "ContentModel.h"
SXAVENGER_ENGINE_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* asset
#include "../Asset/AssetStorage.h"

//* engine
#include <Engine/System/Utility/StreamLogger.h>

////////////////////////////////////////////////////////////////////////////////////////////
// ContentModel class methods
////////////////////////////////////////////////////////////////////////////////////////////

void ContentModel::Attach(const std::filesystem::path& filepath, const std::any& parameter) {

	//!< 引数の保存
	BaseContent::Attach(filepath, parameter);

	uint32_t option = GetOption();

	//!< sceneの取得
	std::shared_ptr<Assimp::Importer> importer = ContentModel::LoadImporter(filepath, option);
	const aiScene* aiScene = importer->GetScene();

	
	meshes_.resize(aiScene->mNumMeshes);
	materials_.resize(aiScene->mNumMaterials); //!< idのサイズを確保

	//!< Uuidの割り当て
	AttachUuid(filepath);

	{ //!< Storageに登録

		for (size_t i = 0; i < meshes_.size(); ++i) {
			sAssetStorage->Register<AssetMesh>(meshes_[i], filepath);
		}

		for (size_t i = 0; i < materials_.size(); ++i) {
			sAssetStorage->Register<AssetMaterial>(materials_[i], filepath);
		}

		sAssetStorage->Register<AssetSkeleton>(skeleton_, filepath);
	}
	
}

void ContentModel::Load(MAYBE_UNUSED const DirectXQueueContext* context) {

	uint32_t option = GetOption();

	std::shared_ptr<Assimp::Importer> importer = ContentModel::LoadImporter(BaseContent::GetFilepath(), option);

	//!< sceneからcontentに必要な情報をセットアップ
	SetupMaterialIndices(importer->GetScene());
	SetupBornNode(importer->GetScene()->mRootNode);

#if 1 //!< 非同期Taskとして委任する場合.
	//!< materialのセットアップ
	for (size_t i = 0; i < materials_.size(); ++i) {
		System::PushTask(
			Async::Execution::Compute,
			std::format("AssetMaterial {}[{}]", BaseContent::GetFilepath().filename().string(), i),
			[this, i, importer](const Async::ExecutionTask*, const DirectXQueueContext*) {
				std::shared_ptr<AssetMaterial> asset = sAssetStorage->Get<AssetMaterial>(materials_[i]);
				asset->Setup(importer->GetScene()->mMaterials[i], BaseContent::GetFilepath().parent_path());
			}
		);
	}

	//!< meshのセットアップ
	for (size_t i = 0; i < meshes_.size(); ++i) {
		System::PushTask(
			Async::Execution::Compute,
			std::format("AssetMesh {}[{}]", BaseContent::GetFilepath().filename().string(), i),
			[this, i, importer](const Async::ExecutionTask*, const DirectXQueueContext* context) {
				std::shared_ptr<AssetMesh> asset = sAssetStorage->Get<AssetMesh>(meshes_[i]);
				asset->Setup(context, importer->GetScene()->mMeshes[i]);
			}
		);
	}

	//!< skeletonのセットアップ
	{
		std::shared_ptr<AssetSkeleton> asset = sAssetStorage->Get<AssetSkeleton>(skeleton_);
		asset->Setup(root_);
	}
	
#else //!< 同一Taskとして実行する場合.
	//!< materialのセットアップ
	for (size_t i = 0; i < materials_.size(); ++i) {
		std::shared_ptr<AssetMaterial> asset = sAssetStorage->Get<AssetMaterial>(materials_[i]);
		asset->Setup(importer->GetScene()->mMaterials[i], BaseContent::GetFilepath().parent_path());
	}

	//!< meshのセットアップ
	for (size_t i = 0; i < meshes_.size(); ++i) {
		std::shared_ptr<AssetMesh> asset = sAssetStorage->Get<AssetMesh>(meshes_[i]);
		asset->Setup(context, importer->GetScene()->mMeshes[i]);
	}

	//!< skeletonのセットアップ
	{
		
	}
#endif

	BaseContent::SetComplete(); //!< 読み込み完了
}

void ContentModel::AttachUuid(const std::filesystem::path& filepath) {

	json meta = BaseContent::LoadMetaData(filepath);

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

		BaseContent::SaveMetaData(meta, filepath);
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

		BaseContent::SaveMetaData(meta, filepath);
	}

	//!< skeletonのid取得
	if (meta.contains("skeleton")) {
		//!< idが既に存在する場合は、metaから取得する
		skeleton_ = Uuid::Deserialize(meta["skeleton"].get<std::string>());

	} else {
		//!< idが存在しない場合は、新しくidを生成し, metaに保存する
		skeleton_ = Uuid::Generate();

		meta["skeleton"] = skeleton_.Serialize();
		BaseContent::SaveMetaData(meta, filepath);
	}

}

uint32_t ContentModel::GetOption() {

	uint32_t option
		= aiProcess_FlipWindingOrder
		| aiProcess_FlipUVs
		| aiProcess_Triangulate
		| aiProcess_CalcTangentSpace
		| aiProcess_ImproveCacheLocality;
	//!< default option

	if (BaseContent::GetParameter().has_value()) {
		option = std::any_cast<uint32_t>(BaseContent::GetParameter());
	}

	return option;
}

std::shared_ptr<Assimp::Importer> ContentModel::LoadImporter(const std::filesystem::path& filepath, uint32_t option) {
	std::shared_ptr<Assimp::Importer> importer = std::make_shared<Assimp::Importer>();
	importer->ReadFile(filepath.generic_string().c_str(), option);

	if (importer->GetScene() == nullptr) {
		StreamLogger::Exception("model load failed. filepath: " + filepath.generic_string(), importer->GetErrorString());
	}

	return importer;
}

void ContentModel::SetupMaterialIndices(const aiScene* aiScene) {
	// meshの数だけの要素を確保
	materialIndices_.resize(aiScene->mNumMeshes);

	for (size_t i = 0; i < materialIndices_.size(); ++i) {
		const aiMesh* aiMesh = aiScene->mMeshes[i];
		materialIndices_[i] = aiMesh->mMaterialIndex;
	}
}

BornNode ContentModel::ReadNode(const aiNode* node) {

	BornNode result = {};

	// transformの取得
	aiVector3D scale, translate;
	aiQuaternion rotate;

	node->mTransformation.Decompose(scale, rotate, translate);

	// resultに代入
	result.transform.scale     = { scale.x, scale.y, scale.z };
	result.transform.rotate    = AssetMesh::ConvertQuaternion(rotate);
	result.transform.translate = AssetMesh::ConvertVector3(translate);

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

void ContentModel::SetupBornNode(const aiNode* aiNode) {
	// nodeの読み込み
	root_ = ReadNode(aiNode);
}
