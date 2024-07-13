#include "Animation.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
// c++
#include <cassert>

// lib
#include <MathLib.h>

////////////////////////////////////////////////////////////////////////////////////////////
// AnimationMethods namespace methods
////////////////////////////////////////////////////////////////////////////////////////////

Vector3f AnimationMethods::CalculateValue(const std::vector<KeyframeVector3>& keyframes, float time) {

	assert(!keyframes.empty()); //!< keyframeが登録されてない

	if (keyframes.size() == 1 || time <= keyframes[0].time) { //!< keyframeが1つ || 現在の時刻がキーフレーム前の場合
		return keyframes[0].value;
	}

	for (size_t i = 0; i < keyframes.size() - 1; ++i) {

		size_t nextIndex = i + 1;

		if (keyframes[i].time <= time && time <= keyframes[nextIndex].time) { //!< timeが範囲内の場合
			// 範囲内でのtを補間
			float t = (time - keyframes[i].time) / (keyframes[nextIndex].time - keyframes[i].time);
			return Lerp(keyframes[i].value, keyframes[nextIndex].value, t);
		}
	}

	// ここまで来た場合は最後のkeyframeより後なので最後の値を返す
	return (*keyframes.rbegin()).value;
}

Quaternion AnimationMethods::CalculateValue(const std::vector<KeyframeQuaternion>& keyframes, float time) {

	assert(!keyframes.empty()); //!< keyframeが登録されてない

	if (keyframes.size() == 1 || time <= keyframes[0].time) { //!< keyframeが1つ || 現在の時刻がキーフレーム前の場合
		return keyframes[0].value;
	}

	for (size_t i = 0; i < keyframes.size() - 1; ++i) {

		size_t nextIndex = i + 1;

		if (keyframes[i].time <= time && time <= keyframes[nextIndex].time) { //!< timeが範囲内の場合
			// 範囲内でのtを補間
			float t = (time - keyframes[i].time) / (keyframes[nextIndex].time - keyframes[i].time);
			return Slerp(keyframes[i].value, keyframes[nextIndex].value, t);
		}
	}

	// ここまで来た場合は最後のkeyframeより後なので最後の値を返す
	return (*keyframes.rbegin()).value;

}

void AnimationMethods::ApplyAnimation(Skeleton& skeleton, const Animation& animation, float time) {

	for (auto& joint : skeleton.joints) {
		// 対象のJointのAnimationがあれば, 値の適応
		if (auto it = animation.nodeAnimations.find(joint.name); it != animation.nodeAnimations.end()) { //!< animationに対象のJointがある場合
			// animationの参照取得
			const NodeAnimation& nodeAnimation = (*it).second;

			// 値の適応
			joint.transform.scale     = CalculateValue(nodeAnimation.scale, time);
			joint.transform.rotate    = CalculateValue(nodeAnimation.rotate, time);
			joint.transform.translate = CalculateValue(nodeAnimation.translate, time);
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////
// Skeleton structure methods
////////////////////////////////////////////////////////////////////////////////////////////

void Skeleton::UpdateMatrix() {
	// すべてのJointの更新. 親が若いので(indexが子より親の方が小さいので)通常ループで処理可能
	for (auto& joint : joints) {
		// localMatrixの更新
		joint.localMatrix = Matrix::MakeAffine(joint.transform.scale, joint.transform.rotate, joint.transform.translate);

		// skeletonSpaceMatrixの更新
		if (joint.parent) { //!< 親がいる場合
			joint.skeletonSpaceMatrix = joint.localMatrix * joints[*joint.parent].skeletonSpaceMatrix;

		} else {
			//!< 親がいないのでlocalMatrixとskeletonSpaceMatrixは同じにする
			joint.skeletonSpaceMatrix = joint.localMatrix;
		}
	}
}
