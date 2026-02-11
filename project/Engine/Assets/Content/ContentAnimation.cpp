#include "ContentAnimation.h"
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
// ContentAnimation class methods
////////////////////////////////////////////////////////////////////////////////////////////

void ContentAnimation::AsyncLoad(MAYBE_UNUSED const DirectXQueueContext* context) {
	BaseContent::CheckExist();

	uint32_t option = 0; //!< default option

	if (param_.has_value()) {
		option = std::any_cast<uint32_t>(param_);
	}

	Load(BaseContent::GetFilepath(), option);
}

void ContentAnimation::AttachUuid() {
	BaseContent::CheckExist();

	// sceneの取得
	Assimp::Importer importer;
	const aiScene* aiScene = importer.ReadFile(BaseContent::GetFilepath().generic_string().c_str(), 0);

	if (aiScene == nullptr) {
		StreamLogger::Exception("animation load failed. filepath: " + BaseContent::GetFilepath().generic_string(), importer.GetErrorString());
		return;
	}

	// idのサイズを確保
	animations_.resize(aiScene->mNumAnimations);

	// idを取得
	AssignUuid();

	// storageに登録
	for (size_t i = 0; i < animations_.size(); ++i) {
		auto asset = std::make_shared<AssetAnimation>(animations_[i]);
		sAssetStorage->Register(asset, BaseContent::GetFilepath());
	}

}

void ContentAnimation::Load(const std::filesystem::path& filepath, uint32_t assimpOption) {

	// sceneの取得
	Assimp::Importer importer;
	const aiScene* aiScene = importer.ReadFile(filepath.generic_string().c_str(), assimpOption);

	if (aiScene == nullptr) {
		StreamLogger::Exception("animation load failed. filepath: " + filepath.generic_string(), importer.GetErrorString());
		return;
	}

	// idのサイズを確保
	animations_.resize(aiScene->mNumAnimations);

	// Animationの読み込み
	LoadAnimations(aiScene);
}

void ContentAnimation::AssignUuid() {
	//!< multi threadにする場合, thread safeにする必要がある.

	json meta = BaseContent::LoadMeta();

	if (meta.contains("animations")) {
		//!< idが既に存在する場合は、metaから取得する

		const auto& animationIds = meta["animations"].get<std::vector<std::string>>();
		for (size_t i = 0; i < animationIds.size(); ++i) {
			animations_[i] = Uuid::Deserialize(animationIds[i]);
		}

	} else {
		//!< idが存在しない場合は、新しくidを生成し, metaに保存する
		
		std::generate(animations_.begin(), animations_.end(), []() { return Uuid::Generate(); });

		meta["animations"] = json::array();

		for (const auto& animation : animations_) {
			meta["animations"].emplace_back(animation.Serialize());
		}
		BaseContent::SaveMeta(meta);
	}
	
}

void ContentAnimation::LoadAnimations(const aiScene* aiScene) {

	for (size_t i = 0; i < aiScene->mNumAnimations; ++i) {
		// animationの取得
		const aiAnimation* animation = aiScene->mAnimations[i];

		// assetの生成
		auto asset = sAssetStorage->GetAsset<AssetAnimation>(animations_[i]);
		asset->Setup(animation);
	}

}
