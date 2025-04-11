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
// Motion class
////////////////////////////////////////////////////////////////////////////////////////////
template <class _Ty>
class Motion {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// Mode enum class
	////////////////////////////////////////////////////////////////////////////////////////////
	enum class Mode : uint8_t {
		Default,
		Clamp,
		Wrap,
		Mirror,
	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	Motion() = default;
	Motion(
		const _Ty& begin, const _Ty& end,
		Mode mode = Mode::Clamp, float t = 0.0f,
		const std::function<_Ty(const _Ty&, const _Ty&, float)>& motionFunc = Lerp, const std::function<float(float)>& interpolationFunc = LinearInterpolation) :
		begin_(begin), end_(end),
		mode_(mode), t_(t),
		motionFunc_(motionFunc), interpolationFunc_(interpolationFunc) {
	}

	//* motion option *//

	void SetT(float t) { t_ = ApplyT(mode_, t); }

	_Ty Get() const;

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

	_Ty begin_, end_;

	Mode mode_;
	float t_;

	std::function<_Ty(const _Ty&, const _Ty&, float)> motionFunc_; //!< lerp, slerp, etc...
	std::function<float(float)> interpolationFunc_;

	//=========================================================================================
	// protected methods
	//=========================================================================================

	static float ApplyT(Mode mode, float t);

};

////////////////////////////////////////////////////////////////////////////////////////////
// Motion class template methods
////////////////////////////////////////////////////////////////////////////////////////////

template <class _Ty>
inline _Ty Motion<_Ty>::Get() const {
	return motionFunc_(begin_, end_, interpolationFunc_(t_));
}

template <class _Ty>
inline float Motion<_Ty>::ApplyT(Mode mode, float t) {
	switch (mode) {
		case Mode::Clamp:
			return std::clamp(t, 0.0f, 1.0f);

		case Mode::Wrap:
			return std::fmod(t, 1.0f);

		case Mode::Mirror:
			return 1.0f - std::abs(std::fmod(t, 2.0f) - 1.0f);

		default:
			return t;
	}
}
