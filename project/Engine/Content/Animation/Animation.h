#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/Runtime/Performance/TimePoint.h>

//* lib
#include <Lib/Geometry/Vector3.h>
#include <Lib/Geometry/Quaternion.h>
#include <Lib/Geometry/GeometryMath.h>

//* c++
#include <vector>
#include <unordered_map>
#include <string>

////////////////////////////////////////////////////////////////////////////////////////////
// Keyframe structure
////////////////////////////////////////////////////////////////////////////////////////////
template <class T>
struct Keyframe {
	TimePointf<TimeUnit::second> time;  //!< キーフレーム時刻 /*_sec*/
	T                            value; //!< value
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
	TimePointf<TimeUnit::second>                        duration;
	std::unordered_map<std::string, TransformAnimation> nodeAnimations; //!< key: node名, value: Nodeのanimation
};

////////////////////////////////////////////////////////////////////////////////////////////
// Animation methods
////////////////////////////////////////////////////////////////////////////////////////////

Vector3f CalculateAnimationCurve(const AnimationCurve<Vector3f>& keyframes, TimePointf<TimeUnit::second> time);
Quaternion CalculateAnimationCurve(const AnimationCurve<Quaternion>& keyframes, TimePointf<TimeUnit::second> time);


