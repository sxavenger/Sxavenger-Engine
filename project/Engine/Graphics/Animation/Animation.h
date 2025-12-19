#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/Foundation.h>

//* lib
#include <Lib/Geometry/Vector3.h>
#include <Lib/Geometry/Quaternion.h>
#include <Lib/Geometry/GeometryMath.h>
#include <Lib/Adapter/Time/TimePoint.h>

//* c++
#include <vector>
#include <unordered_map>
#include <string>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// Keyframe structure
////////////////////////////////////////////////////////////////////////////////////////////
//! @brief キーフレーム構造体
template <class T>
struct Keyframe {
public:

	//=========================================================================================
	// public variables
	//=========================================================================================

	TimePointd<TimeUnit::second> time;  //!< キーフレーム時刻 /*_sec*/
	T                            value; //!< value
};

////////////////////////////////////////////////////////////////////////////////////////////
// Animation structure
////////////////////////////////////////////////////////////////////////////////////////////

template <class T>
using AnimationCurve = std::vector<Keyframe<T>>;

struct TransformAnimation {
public:

	//=========================================================================================
	// public variables
	//=========================================================================================

	AnimationCurve<Vector3f>   scale;
	AnimationCurve<Quaternion> rotate;
	AnimationCurve<Vector3f>   translate;

};

struct Animation {

	//=========================================================================================
	// public variables
	//=========================================================================================

	TimePointd<TimeUnit::second>                        duration;
	std::unordered_map<std::string, TransformAnimation> nodeAnimations;
	//!< key: node名, value: Nodeのanimation
};

////////////////////////////////////////////////////////////////////////////////////////////
// Animation methods
////////////////////////////////////////////////////////////////////////////////////////////

Vector3f CalculateAnimationCurve(const AnimationCurve<Vector3f>& keyframes, TimePointd<TimeUnit::second> time);
Quaternion CalculateAnimationCurve(const AnimationCurve<Quaternion>& keyframes, TimePointd<TimeUnit::second> time);

SXAVENGER_ENGINE_NAMESPACE_END
