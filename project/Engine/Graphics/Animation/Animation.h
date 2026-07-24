#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/Foundation.h>

//* lib
#include <Lib/Math/Vector3.h>
#include <Lib/Math/Quaternion.h>
#include <Lib/Math/GeometryMath.h>
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

//! @brief 1ノードのSRTそれぞれのアニメーションカーブを保持する構造体
struct TransformAnimation {
public:

	//=========================================================================================
	// public variables
	//=========================================================================================

	AnimationCurve<Vector3f>   scale;
	AnimationCurve<Quaternion> rotate;
	AnimationCurve<Vector3f>   translate;

	//=========================================================================================
	// public methods
	//=========================================================================================

	static Vector3f CalculateAnimationCurve(const AnimationCurve<Vector3f>& keyframes, TimePointd<TimeUnit::second> time);

	static Quaternion CalculateAnimationCurve(const AnimationCurve<Quaternion>& keyframes, TimePointd<TimeUnit::second> time);

};

//! @brief 1つのアニメーションクリップ(全体時間とノード名→カーブのマップ)
struct Animation {

	//=========================================================================================
	// public variables
	//=========================================================================================

	TimePointd<TimeUnit::second>                        duration;       //!< アニメーション全体時間
	std::unordered_map<std::string, TransformAnimation> nodeAnimations; //!< ノード名とTransformAnimationのマップ

};

SXAVENGER_ENGINE_NAMESPACE_END
