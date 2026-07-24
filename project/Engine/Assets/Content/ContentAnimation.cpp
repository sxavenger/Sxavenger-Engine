#include "ContentAnimation.h"
SXAVENGER_ENGINE_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* asset
#include "../Asset/AssetStorage.h"
#include "../Asset/AssetAnimationClip.h"

//* engine
#include <Engine/System/Utility/StreamLogger.h>

//* lib
#include <Lib/Adapter/Json/JsonHandler.h>

////////////////////////////////////////////////////////////////////////////////////////////
// ContentAnimation class methods
////////////////////////////////////////////////////////////////////////////////////////////

void ContentAnimation::Attach(const std::filesystem::path& filepath, const std::any& parameter) {

	//!< 引数の保存
	BaseContent::Attach(filepath, parameter);

	//!< sceneの取得
	std::shared_ptr<Assimp::Importer> importer = ContentAnimation::LoadImporter(filepath);
	const aiScene* aiScene = importer->GetScene();

	animations_.resize(aiScene->mNumAnimations); //!< idのサイズを確保

	//!< Uuidの割り当て
	AttachUuid(filepath);

	//!< Storageに登録
	for (size_t i = 0; i < animations_.size(); ++i) {
		sAssetStorage->Register<AssetAnimationClip>(animations_[i], filepath);
	}

}

void ContentAnimation::Load(MAYBE_UNUSED const DirectXQueueContext* context) {

	std::shared_ptr<Assimp::Importer> importer = ContentAnimation::LoadImporter(BaseContent::GetFilepath());

#if 1 //!< 非同期Taskとして委任する場合.
	for (size_t i = 0; i < animations_.size(); ++i) {
		System::PushTask(
			Async::Execution::Cpu,
			std::format("AssetAnimationClip {}[{}]", BaseContent::GetFilepath().filename().string(), i),
			[this, i, importer](const Async::ExecutionTask*, const DirectXQueueContext*) {
				std::shared_ptr<AssetAnimationClip> asset = sAssetStorage->Get<AssetAnimationClip>(animations_[i]);
				asset->Setup(importer->GetScene()->mAnimations[i]);
			}
		);
	}

#else //!< 同一Taskとして実行する場合.
	for (size_t i = 0; i < animations_.size(); ++i) {
		std::shared_ptr<AssetAnimationClip> asset = sAssetStorage->Get<AssetAnimationClip>(animations_[i]);
		asset->Setup(importer->GetScene()->mAnimations[i]);
	}
#endif

	BaseContent::SetComplete(); //!< 読み込み完了
}

void ContentAnimation::AttachUuid(const std::filesystem::path& filepath) {

	json meta = BaseContent::LoadMetaData(filepath);

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

		BaseContent::SaveMetaData(meta, filepath);
	}

}

std::shared_ptr<Assimp::Importer> ContentAnimation::LoadImporter(const std::filesystem::path& filepath, uint32_t option) {
	std::shared_ptr<Assimp::Importer> importer = std::make_shared<Assimp::Importer>();
	importer->ReadFile(filepath.generic_string().c_str(), option);

	if (importer->GetScene() == nullptr) {
		StreamLogger::Exception("animation load failed. filepath: " + filepath.generic_string(), importer->GetErrorString());
	}

	return importer;
}
