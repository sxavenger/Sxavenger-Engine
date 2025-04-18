#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/Runtime/Performance/TimePoint.h>

//* c++
#include <concepts>
#include <functional>
#include <algorithm>

////////////////////////////////////////////////////////////////////////////////////////////
// MotionMode enum class
////////////////////////////////////////////////////////////////////////////////////////////
enum class MotionMode : uint8_t {
	Default,
	Clamp,
	Wrap,
	Mirror,
};
float GetMotionT(MotionMode mode, float t);

////////////////////////////////////////////////////////////////////////////////////////////
// MotionT class
////////////////////////////////////////////////////////////////////////////////////////////
template <class _Ty>
class MotionT {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// Parameter structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct Parameter {
	public:

		//=========================================================================================
		// public variables
		//=========================================================================================

		_Ty start;
		_Ty end;

		//* motion *//

		MotionMode mode = MotionMode::Default;

		//* function *//

		std::function<_Ty(const _Ty&, const _Ty&, float)> motionFunc = Lerp;
		std::function<float(float)> interpolationFunc = LinearInterpolation;

	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	MotionT() = default;

	MotionT(const _Ty& start, const _Ty& end, MotionMode mode = MotionMode::Default)
		: start_(start), end_(end), mode_(mode) {
		motionFunc_ = Lerp;
		interpolationFunc_ = LinearInterpolation;
	}

	MotionT(const Parameter& param)
		: start_(param.start), end_(param.end), mode_(param.mode) {
		motionFunc_        = param.motionFunc;
		interpolationFunc_ = param.interpolationFunc;
	}

	//* motion methods *//

	_Ty GetMotion(float t) const;

	//* static methods *//
	
	static float LinearInterpolation(float t) {
		return t;
	}

	static _Ty Lerp(const _Ty& begin, const _Ty& end, float t) {
		return begin + (end - begin) * t;
	}

protected:

	//=========================================================================================
	// protected variables
	//=========================================================================================

	_Ty start_;
	_Ty end_;

	//* motion *//

	MotionMode mode_;

	//* function *//

	std::function<_Ty(const _Ty&, const _Ty&, float)> motionFunc_; //!< lerp, slerp, etc...
	std::function<float(float)> interpolationFunc_;

};

////////////////////////////////////////////////////////////////////////////////////////////
// MotionT class template methods
////////////////////////////////////////////////////////////////////////////////////////////

template <class _Ty>
_Ty MotionT<_Ty>::GetMotion(float t) const {
	return motionFunc_(
		start_,
		end_,
		interpolationFunc_(GetMotionT(mode_, t))
	);
}
