#include "UContentAnimation.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* uasset
#include "../Asset/UAssetStorage.h"

//* lib
#include <Lib/Adapter/Json/JsonHandler.h>

////////////////////////////////////////////////////////////////////////////////////////////
// UContentAnimation class methods
////////////////////////////////////////////////////////////////////////////////////////////

void UContentAnimation::AsyncLoad(_MAYBE_UNUSED const DirectXQueueContext* context) {
	UBaseContent::CheckExist();

	uint32_t option = 0; //!< default option

	if (param_.has_value()) {
		option = std::any_cast<uint32_t>(param_);
	}

	Load(UBaseContent::GetFilepath(), option);
}

void UContentAnimation::Load(const std::filesystem::path& filepath, uint32_t assimpOption) {

	// sceneの取得
	Assimp::Importer importer; //!< scene保存するため保管
	const aiScene* aiScene = importer.ReadFile(filepath.generic_string().c_str(), assimpOption);

	// todo: Assert

	// idのサイズを確保
	animations_.resize(aiScene->mNumAnimations);

	// idを取得
	GetUuid();

	// Animationの読み込み
	LoadAnimations(aiScene, filepath);
}

void UContentAnimation::GetUuid() {
	//!< multi threadにする場合, thread safeにする必要がある.
	
	std::filesystem::path filepath = UBaseContent::GetContentPath();

	json data;
	JsonHandler::LoadFromJson(filepath, data);

	if (data.contains("animations")) {
		//!< Idが既に存在する場合は、Json形式で読み込む

		const auto& animationIds = data["animations"].get<std::vector<std::string>>();
		for (size_t i = 0; i < animationIds.size(); ++i) {
			animations_[i] = Uuid::Deserialize(animationIds[i]);
		}

	} else {
		//!< Idが存在しない場合は、生成して保存する
		
		std::generate(animations_.begin(), animations_.end(), []() { return Uuid::Generate(); });

		data["animations"] = json::array();

		for (const auto& animation : animations_) {
			data["animations"].emplace_back(animation.Serialize());
		}

		JsonHandler::OverwriteToJson(filepath, data);
		//!< Model側と競合しないようにするため, 上書きする.
	}
	
}

void UContentAnimation::LoadAnimations(const aiScene* aiScene, const std::filesystem::path& filepath) {

	for (size_t i = 0; i < aiScene->mNumAnimations; ++i) {
		// animationの取得
		const aiAnimation* animation = aiScene->mAnimations[i];

		// assetの生成
		auto asset = std::make_shared<UAssetAnimation>(animations_[i]);
		UAssetStorage::GetInstance()->Register(asset, filepath);

		asset->Setup(animation);
	}

}
