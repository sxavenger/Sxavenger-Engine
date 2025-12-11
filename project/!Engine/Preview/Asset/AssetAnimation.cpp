#include "AssetAnimation.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/Utility/Logger.h>

////////////////////////////////////////////////////////////////////////////////////////////
// AssetAnimation class methods
////////////////////////////////////////////////////////////////////////////////////////////

void AssetAnimation::Setup(const aiAnimation* animation) {

	// animation全体時間の取得
	animation_.duration = GetTime(animation->mDuration, animation->mTicksPerSecond);

	for (uint32_t channelIndex = 0; channelIndex < animation->mNumChannels; ++channelIndex) {
		LoadTransformAnimation(animation->mChannels[channelIndex], animation->mTicksPerSecond);
	}

	BaseAsset::Complete();
	Logger::EngineThreadLog(std::format("[AssetAnimation]: animation setup complete. uuid: {}", BaseAsset::GetId().Serialize()));
}

TimePointd<TimeUnit::second> AssetAnimation::GetTime(double time, double ticksPerSeconds) {
	return { time / ticksPerSeconds };
}

void AssetAnimation::LoadTransformAnimation(const aiNodeAnim* aiNodeAnimation, double tickPerSeconds) {

	// transform animation の参照取得
	TransformAnimation& nodeAnimation = animation_.nodeAnimations[aiNodeAnimation->mNodeName.C_Str()];

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
