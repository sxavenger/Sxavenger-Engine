#include "AssetAnimationClip.h"
SXAVENGER_ENGINE_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/Utility/StreamLogger.h>

////////////////////////////////////////////////////////////////////////////////////////////
// AnimationClip class methods
////////////////////////////////////////////////////////////////////////////////////////////

void AssetAnimationClip::Setup(const aiAnimation* animation) {
	// animation全体時間の取得
	animation_.duration = GetTime(animation->mDuration, animation->mTicksPerSecond);

	for (uint32_t channelIndex = 0; channelIndex < animation->mNumChannels; ++channelIndex) {
		const aiNodeAnim* aiNodeAnimation = animation->mChannels[channelIndex];
		animation_.nodeAnimations[aiNodeAnimation->mNodeName.C_Str()] = LoadAnimation(aiNodeAnimation, animation->mTicksPerSecond);
	}

	BaseAsset::SetComplete();
	StreamLogger::EngineThreadLog(std::format("[AssetAnimationClip]: animation setup complete. uuid: {}", BaseAsset::SerializeId()));
}

TimePointd<TimeUnit::second> AssetAnimationClip::GetTime(double time, double ticksPerSeconds) {
	return { time / ticksPerSeconds };
}

TransformAnimation AssetAnimationClip::LoadAnimation(const aiNodeAnim* aiNodeAnimation, double tickPerSeconds) {

	TransformAnimation animation;

	//* 
	//* positionの取得
	//*
	for (uint32_t keyIndex = 0; keyIndex < aiNodeAnimation->mNumPositionKeys; ++keyIndex) {

		// keyの取得
		const aiVectorKey& aiKey = aiNodeAnimation->mPositionKeys[keyIndex];

		Keyframe<Vector3f>& keyframe = animation.translate.emplace_back();

		keyframe.time  = GetTime(aiKey.mTime, tickPerSeconds);
		keyframe.value = { aiKey.mValue.x, aiKey.mValue.y, -aiKey.mValue.z }; //!< 左手座標系に変換
	}

	//* 
	//* rotateの取得
	//*
	for (uint32_t keyIndex = 0; keyIndex < aiNodeAnimation->mNumRotationKeys; ++keyIndex) {

		// keyの取得
		const aiQuatKey& aiKey = aiNodeAnimation->mRotationKeys[keyIndex];

		Keyframe<Quaternion>& keyframe = animation.rotate.emplace_back();

		keyframe.time  = GetTime(aiKey.mTime, tickPerSeconds);
		keyframe.value = { -aiKey.mValue.x, -aiKey.mValue.y, aiKey.mValue.z, aiKey.mValue.w }; //!< 左手座標系に変換
	}

	//* 
	//* scaleの取得
	//*
	for (uint32_t keyIndex = 0; keyIndex < aiNodeAnimation->mNumScalingKeys; ++keyIndex) {

		// keyの取得
		const aiVectorKey& aiKey = aiNodeAnimation->mScalingKeys[keyIndex];

		Keyframe<Vector3f>& keyframe = animation.scale.emplace_back();

		keyframe.time  = GetTime(aiKey.mTime, tickPerSeconds);
		keyframe.value = { aiKey.mValue.x, aiKey.mValue.y, aiKey.mValue.z }; //!< 左手座標系に変換
	}

	return animation;

}
