#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* c++
#include <cstdint>
#include <functional>

////////////////////////////////////////////////////////////////////////////////////////////
// MotionMode enum class
////////////////////////////////////////////////////////////////////////////////////////////
enum class MotionMode : uint8_t {
	Default,
	Clamp,
	Wrap,
	Mirror,
	WrapOne,
	MirrorOne,
};
float GetMotionT(MotionMode mode, float t);

////////////////////////////////////////////////////////////////////////////////////////////
// MotionT class
////////////////////////////////////////////////////////////////////////////////////////////
template <typename T>
class MotionT {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// using
	////////////////////////////////////////////////////////////////////////////////////////////
	using MotionFunction        = std::function<T(const T&, const T&, float)>;
	using InterpolationFunction = std::function<float(float)>;

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* constructor & destructor *//

	MotionT() = default;
	MotionT(const T& start, const T& end, MotionMode mode = MotionMode::Default);
	MotionT(const T& start, const T& end, const InterpolationFunction& interpolation = LinearInterpolation, MotionMode mode = MotionMode::Default);

	//* motion option *//

	T Get(float t) const;

	//* static common methods *//

	static float LinearInterpolation(float t) {
		return t;
	}

	static T Lerp(const T& begin, const T& end, float t) {
		return begin + (end - begin) * t;
	}

	static MotionT<T> CreateLinear(const T& start, const T& end) { return MotionT<T>(start, end, MotionMode::Default); }
	static MotionT<T> CreateMode(const T& start, const T& end, MotionMode mode) { return MotionT<T>(start, end, mode); }
	static MotionT<T> CreateInterpolation(const T& start, const T& end, const InterpolationFunction& interpolation, MotionMode mode) { return MotionT<T>(start, end, interpolation, mode); }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* value *//

	T start_;
	T end_;

	//* mode *//

	MotionMode mode_ = MotionMode::Default;

	//* function *//

	MotionFunction motionFunction_               = Lerp;
	InterpolationFunction interpolationFunction_ = LinearInterpolation;

};

////////////////////////////////////////////////////////////////////////////////////////////
// MotionT class template method
////////////////////////////////////////////////////////////////////////////////////////////

template <typename T>
inline MotionT<T>::MotionT(const T& start, const T& end, MotionMode mode) {
	start_ = start;
	end_   = end;
	mode_ = mode;
}

template <typename T>
inline MotionT<T>::MotionT(const T& start, const T& end, const InterpolationFunction& interpolation, MotionMode mode) {
	start_                 = start;
	end_                   = end;
	interpolationFunction_ = interpolation;
	mode_                  = mode;
}

template <typename T>
inline T MotionT<T>::Get(float t) const {
	t = interpolationFunction_(GetMotionT(mode_, t));
	return motionFunction_(start_, end_, t);
}
