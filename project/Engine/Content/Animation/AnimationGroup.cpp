#include "AnimationGroup.h"

////////////////////////////////////////////////////////////////////////////////////////////
// AnimationGroup class methods
////////////////////////////////////////////////////////////////////////////////////////////

void AnimationGroup::Load(const std::filesystem::path& filepath) {

	// sceneの取得
	Assimp::Importer importer; //!< scene保存するため保管
	const aiScene* aiScene = importer.ReadFile(filepath.generic_string().c_str(), 0);

	animations_.resize(aiScene->mNumAnimations);

	for (uint32_t animationIndex = 0; animationIndex < aiScene->mNumAnimations; ++animationIndex) {

		// animationの取得
		const aiAnimation* aiAnimation = aiScene->mAnimations[animationIndex];

		
		// animation name mapping
		animationMap_.emplace(aiAnimation->mName.C_Str(), animationIndex);

		Animation& animation = animations_[animationIndex];
		animation;


	}
}
