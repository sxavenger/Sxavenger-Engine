#include "Animator.h"

////////////////////////////////////////////////////////////////////////////////////////////
// Animator class methods
////////////////////////////////////////////////////////////////////////////////////////////

void Animator::Load(_MAYBE_UNUSED const DirectXThreadContext* context) {
	if (!BaseAsset::CheckAndBeginLoad()) {
		return;
	}

	// sceneの取得
	Assimp::Importer importer; //!< scene保存するため保管
	const aiScene* aiScene = importer.ReadFile(filepath_.generic_string().c_str(), assimpOption_);

	Assert(aiScene != nullptr, "animatior load failed. filepath: " + filepath_.generic_string(), importer.GetErrorString());

	animations_.resize(aiScene->mNumAnimations);

	for (uint32_t animationIndex = 0; animationIndex < aiScene->mNumAnimations; ++animationIndex) {

		// animationの取得
		const aiAnimation* aiAnimation = aiScene->mAnimations[animationIndex];

		Assert(!animationMap_.contains(aiAnimation->mName.C_Str()), "animation name is conflict.");
		
		// animation name mapping
		animationMap_.try_emplace(aiAnimation->mName.C_Str(), animationIndex);
		Animation& animation = animations_[animationIndex];

		// animation全体時間の取得
		animation.duration = GetTime(aiAnimation->mDuration, aiAnimation->mTicksPerSecond);

		for (uint32_t channelIndex = 0; channelIndex < aiAnimation->mNumChannels; ++channelIndex) {

			// node animationの取得
			const aiNodeAnim* aiNodeAnimation = aiAnimation->mChannels[channelIndex];

			LoadTransformAnimation(aiNodeAnimation, animation, aiAnimation->mTicksPerSecond);
		}
	}

	BaseAsset::EndLoad();
}

void Animator::Term() {
}

const Animation& Animator::GetAnimation(const std::string& name) const {
	uint32_t index = GetAnimationIndex(name);
	return GetAnimation(index);
}

const DeltaTimePoint<TimeUnit::s> Animator::GetDurationTime(const std::string& name) const {
	uint32_t index = GetAnimationIndex(name);
	return GetDurationTime(index);
}

DeltaTimePoint<TimeUnit::s> Animator::GetTime(double time, double ticksPerSeconds) {
	return { static_cast<float>(time / ticksPerSeconds) };
}

void Animator::LoadTransformAnimation(const aiNodeAnim* aiNodeAnimation, Animation& animation, double tickPerSeconds) {

	// transform animation の参照取得
	TransformAnimation& nodeAnimation = animation.nodeAnimations[aiNodeAnimation->mNodeName.C_Str()];

	//* 
	//* positionの取得
	//*
	for (uint32_t keyIndex = 0; keyIndex < aiNodeAnimation->mNumPositionKeys; ++keyIndex) {

		// keyの取得
		const aiVectorKey& aiKey = aiNodeAnimation->mPositionKeys[keyIndex];

		Keyframe<Vector3f>& keyframe = nodeAnimation.translate.emplace_back();

		keyframe.time  = GetTime(aiKey.mTime, tickPerSeconds);
		keyframe.value = { aiKey.mValue.x, aiKey.mValue.y, -aiKey.mValue.z }; //!< 左手座標系に変換
	}

	//* 
	//* rotateの取得
	//*
	for (uint32_t keyIndex = 0; keyIndex < aiNodeAnimation->mNumRotationKeys; ++keyIndex) {

		// keyの取得
		const aiQuatKey& aiKey = aiNodeAnimation->mRotationKeys[keyIndex];

		Keyframe<Quaternion>& keyframe = nodeAnimation.rotate.emplace_back();

		keyframe.time  = GetTime(aiKey.mTime, tickPerSeconds);
		keyframe.value = { -aiKey.mValue.x, -aiKey.mValue.y, aiKey.mValue.z, aiKey.mValue.w }; //!< 左手座標系に変換
	}

	//* 
	//* scaleの取得
	//*
	for (uint32_t keyIndex = 0; keyIndex < aiNodeAnimation->mNumScalingKeys; ++keyIndex) {

		// keyの取得
		const aiVectorKey& aiKey = aiNodeAnimation->mScalingKeys[keyIndex];

		Keyframe<Vector3f>& keyframe = nodeAnimation.scale.emplace_back();

		keyframe.time  = GetTime(aiKey.mTime, tickPerSeconds);
		keyframe.value = { aiKey.mValue.x, aiKey.mValue.y, aiKey.mValue.z }; //!< 左手座標系に変換
	}
}

const uint32_t Animator::GetAnimationIndex(const std::string& name) const {
	Assert(animationMap_.contains(name), "animation is not found. name: " + name);
	return animationMap_.at(name);
}
