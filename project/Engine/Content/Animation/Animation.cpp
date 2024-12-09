#include "Animation.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/Utility/Logger.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Animation methods
////////////////////////////////////////////////////////////////////////////////////////////

Vector3f CalculateAnimationCurve(const AnimationCurve<Vector3f>& keyframes, DeltaTimePoint<TimeUnit::s> time) {
	Assert(!keyframes.empty(), "keyframes is empty.");

	if (keyframes.size() == 1 || time <= keyframes.front().time) {
		return keyframes.front().value;
	}

	for (size_t i = 0; i < keyframes.size() - 1; ++i) {

		size_t nextIndex = i + 1;

		if (keyframes[i].time <= time && time <= keyframes[nextIndex].time) { //!< timeが範囲内の場合
			// 範囲内でのtを補間
			float t = (time.time - keyframes[i].time.time) / (keyframes[nextIndex].time.time - keyframes[i].time.time);
			return Lerp(keyframes[i].value, keyframes[nextIndex].value, t);
		}
	}

	return keyframes.back().value;
}

Quaternion CalculateAnimationCurve(const AnimationCurve<Quaternion>& keyframes, DeltaTimePoint<TimeUnit::s> time) {
	Assert(!keyframes.empty(), "keyframes is empty.");

	if (keyframes.size() == 1 || time <= keyframes.front().time) {
		return keyframes.front().value;
	}

	for (size_t i = 0; i < keyframes.size() - 1; ++i) {

		size_t nextIndex = i + 1;

		if (keyframes[i].time <= time && time <= keyframes[nextIndex].time) { //!< timeが範囲内の場合
			// 範囲内でのtを補間
			float t = (time.time - keyframes[i].time.time) / (keyframes[nextIndex].time.time - keyframes[i].time.time);
			return Slerp(keyframes[i].value, keyframes[nextIndex].value, t);
		}
	}

	return keyframes.back().value;
}