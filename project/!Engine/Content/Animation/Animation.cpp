#include "Animation.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/Utility/Logger.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Animation methods
////////////////////////////////////////////////////////////////////////////////////////////

Vector3f CalculateAnimationCurve(const AnimationCurve<Vector3f>& keyframes, TimePointd<TimeUnit::second> time) {
	Exception::Assert(!keyframes.empty(), "keyframes is empty.");

	if (keyframes.size() == 1 || time <= keyframes.front().time) {
		return keyframes.front().value;
	}

	for (size_t i = 0; i < keyframes.size() - 1; ++i) {

		size_t nextIndex = i + 1;

		if (keyframes[i].time <= time && time <= keyframes[nextIndex].time) { //!< timeが範囲内の場合
			// 範囲内でのtを補間
			double t = (time.time - keyframes[i].time.time) / (keyframes[nextIndex].time.time - keyframes[i].time.time);
			return Vector3f::Lerp(keyframes[i].value, keyframes[nextIndex].value, static_cast<float>(t));
		}
	}

	return keyframes.back().value;
}

Quaternion CalculateAnimationCurve(const AnimationCurve<Quaternion>& keyframes, TimePointd<TimeUnit::second> time) {
	Exception::Assert(!keyframes.empty(), "keyframes is empty.");

	if (keyframes.size() == 1 || time <= keyframes.front().time) {
		return keyframes.front().value;
	}

	for (size_t i = 0; i < keyframes.size() - 1; ++i) {

		size_t nextIndex = i + 1;

		if (keyframes[i].time <= time && time <= keyframes[nextIndex].time) { //!< timeが範囲内の場合
			// 範囲内でのtを補間
			double t = (time.time - keyframes[i].time.time) / (keyframes[nextIndex].time.time - keyframes[i].time.time);
			return Quaternion::Slerp(keyframes[i].value, keyframes[nextIndex].value, static_cast<float>(t));
		}
	}

	return keyframes.back().value;
}
