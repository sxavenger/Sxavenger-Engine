#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* c++
#include <array>

////////////////////////////////////////////////////////////////////////////////////////////
// Curve class
////////////////////////////////////////////////////////////////////////////////////////////
template <class T, size_t _Size>
class Curve {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	Curve()  = default;
	~Curve() = default;

	//* methods *//

	void SortPoints();

	//* user interface *//

	virtual T CatmullRomInterpolation(const T& v0, const T& v1, const T& v2, const T& v3, float t) const;

	//* setter *//

	void SetPoint(uint32_t index, const T& value, float t);

	//* getter *//

	T GetValue(float t) const;

	const std::array<std::pair<T, float>, _Size>& GetPoints() const { return points_; }
	std::array<std::pair<T, float>, _Size>& GetPoints() { return points_; }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	std::array<std::pair<T, float>, _Size> points_;
	//! [vector]
	//!  [pair]
	//!   first  : value
	//!   second : control point
	//!  size : n

private:
	static_assert(_Size >= 2, "curve point is over 2.");
};

////////////////////////////////////////////////////////////////////////////////////////////
// Curve class methods
////////////////////////////////////////////////////////////////////////////////////////////

template<class T, size_t _Size>
inline void Curve<T, _Size>::SortPoints() {
	std::stable_sort(points_.begin(), points_.end(), [](const auto& a, const auto& b) {
		return a.second < b.second;
	});
}

template<class T, size_t _Size>
inline T Curve<T, _Size>::CatmullRomInterpolation(const T& v0, const T& v1, const T& v2, const T& v3, float t) const {
	float t2 = t * t;
	float t3 = t2 * t;

	float e3 = -v0 + (v1 - v2) * 3.0f + v3; // t^3
	float e2 = v0 * 2.0f - v1 * 5.0f + v2 * 4.0f - v3; // t^2
	float e1 = -v0 + v2; // t
	float e0 = v1 * 2.0f;

	return (e3 * t3 + e2 * t2 + e1 * t + e0) * 0.5f;
}

template<class T, size_t _Size>
inline void Curve<T, _Size>::SetPoint(uint32_t index, const T& value, float t) {
	points_[index] = std::make_pair(value, t);
}

template<class T, size_t _Size>
inline T Curve<T, _Size>::GetValue(float t) const {

	// check range
	if (t <= points_.front().second) {
		return points_.front().first;

	} else if (t >= points_.back().second) {
		return points_.back().first;
	}

	// find range index
	uint32_t index = 0;

	for (; index < points_.size() - 1; ++index) {
		if (t >= points_[index].second && t <= points_[index + 1].second) {
			break;
		}
	}

	size_t i0 = index - 1;
	size_t i1 = index;
	size_t i2 = index + 1;
	size_t i3 = index + 2;

	// fix index
	if (index == 0) {
		i0 = 0;
	}

	if (index >= points_.size() - 2) {
		i3 = points_.size() - 1;
	}

	const auto& p0 = points_[i0];
	const auto& p1 = points_[i1];
	const auto& p2 = points_[i2];
	const auto& p3 = points_[i3];

	float intermediateT = (t - p1.second) / (p2.second - p1.second);

	return CatmullRomInterpolation(p0.first, p1.first, p2.first, p3.first, intermediateT);

}
