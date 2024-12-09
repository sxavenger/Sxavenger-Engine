#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/Runtime/Performance/DeltaTimePoint.h>

//* lib
#include <Lib/Geometry/Vector3.h>
#include <Lib/Geometry/Quaternion.h>

//* c++
#include <vector>
#include <unordered_map>
#include <string>

////////////////////////////////////////////////////////////////////////////////////////////
// Keyframe structure
////////////////////////////////////////////////////////////////////////////////////////////
template <class T>
struct Keyframe {
	DeltaTimePoint<TimeUnit::s> time;  //!< キーフレーム時刻 /*_sec*/
	T                           value; //!< value
};

////////////////////////////////////////////////////////////////////////////////////////////
// Animation structure
////////////////////////////////////////////////////////////////////////////////////////////

template <class T>
using AnimationCurve = std::vector<Keyframe<T>>;

struct TransformAnimation {
	AnimationCurve<Vector3f>   scale;
	AnimationCurve<Quaternion> rotate;
	AnimationCurve<Vector3f>   translate;
};

struct Animation {
	DeltaTimePoint<TimeUnit::s>                         duration;
	std::unordered_map<std::string, TransformAnimation> nodeAnimations; //!< key: node名, value: Nodeのanimation
};

////////////////////////////////////////////////////////////////////////////////////////////
// Animation methods
////////////////////////////////////////////////////////////////////////////////////////////

Vector3f CalculateAnimationCurve(const AnimationCurve<Vector3f>& keyframes, DeltaTimePoint<TimeUnit::s> time);
Quaternion CalculateAnimationCurve(const AnimationCurve<Quaternion>& keyframes, DeltaTimePoint<TimeUnit::s> time);


