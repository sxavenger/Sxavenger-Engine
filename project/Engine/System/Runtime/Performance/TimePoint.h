#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* c++
#include <concepts>
#include <cmath>

////////////////////////////////////////////////////////////////////////////////////////////
// TimeUtil enum class
////////////////////////////////////////////////////////////////////////////////////////////
enum class TimeUnit {
	second,
	millisecond,
	microsecond,
};

template <TimeUnit _From, TimeUnit _To, std::floating_point _Value>
constexpr _Value ConvertTimeUtil() {
	if constexpr (_From == TimeUnit::second && _To == TimeUnit::millisecond) {
		return static_cast<_Value>(1000);

	} else if constexpr (_From == TimeUnit::second && _To == TimeUnit::microsecond) {
		return static_cast<_Value>(1000000);

	} else if constexpr (_From == TimeUnit::millisecond && _To == TimeUnit::second) {
		return static_cast<_Value>(0.001);

	} else if constexpr (_From == TimeUnit::millisecond && _To == TimeUnit::microsecond) {
		return static_cast<_Value>(1000);

	} else if constexpr (_From == TimeUnit::microsecond && _To == TimeUnit::second) {
		return static_cast<_Value>(0.000001);

	} else if constexpr (_From == TimeUnit::microsecond && _To == TimeUnit::millisecond) {
		return static_cast<_Value>(0.001);

	} else {
		return static_cast<_Value>(1); // 同じ単位の場合
	}
}

////////////////////////////////////////////////////////////////////////////////////////////
// TimePoint class
////////////////////////////////////////////////////////////////////////////////////////////
template <TimeUnit _Unit, std::floating_point _Value>
class TimePoint {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	constexpr TimePoint() = default;
	constexpr TimePoint(_Value _time) : time(_time) {}
	constexpr TimePoint(const TimePoint& right) : time(right.time) {}
	constexpr TimePoint(TimePoint&& right) : time(std::move(right.time)) {}

	constexpr ~TimePoint() = default;

	constexpr void Reset() { time = static_cast<_Value>(0); }

	static TimePoint Mod(TimePoint left, TimePoint right) { return { std::fmod(left.time, right.time) };}

	//=========================================================================================
	// operator
	//=========================================================================================

	constexpr TimePoint& operator=(const TimePoint& right);
	constexpr TimePoint& operator=(TimePoint&& right);

	constexpr TimePoint& operator+=(const TimePoint& right);
	constexpr TimePoint& operator-=(const TimePoint& right);

	template <TimeUnit _UnitU>
	constexpr operator TimePoint<_UnitU, _Value>() const {
		return TimePoint<_UnitU, _Value>(time * ConvertTimeUtil<_Unit, _UnitU, _Value>());
	}

	constexpr bool operator==(const TimePoint& right) const { return time == right.time; }
	constexpr bool operator!=(const TimePoint& right) const { return time != right.time; }

	constexpr bool operator<(const TimePoint& right) const { return time < right.time; }
	constexpr bool operator<=(const TimePoint& right) const { return time <= right.time; }
	constexpr bool operator>(const TimePoint& right) const { return time > right.time; }
	constexpr bool operator>=(const TimePoint& right) const { return time >= right.time; }

	//=========================================================================================
	// public variables
	//=========================================================================================

	_Value time;

};

////////////////////////////////////////////////////////////////////////////////////////////
// TimePoint class template operator
////////////////////////////////////////////////////////////////////////////////////////////

template <TimeUnit _Unit, std::floating_point _Value>
constexpr TimePoint<_Unit, _Value>& TimePoint<_Unit, _Value>::operator=(const TimePoint& right) {
	time = right.time;
	return *this;
}

template <TimeUnit _Unit, std::floating_point _Value>
constexpr TimePoint<_Unit, _Value>& TimePoint<_Unit, _Value>::operator=(TimePoint&& right) {
	time = std::move(right.time);
	return *this;
}

template <TimeUnit _Unit, std::floating_point _Value>
constexpr TimePoint<_Unit, _Value>& TimePoint<_Unit, _Value>::operator+=(const TimePoint& right) {
	time += right.time;
	return *this;
}

template <TimeUnit _Unit, std::floating_point _Value>
constexpr TimePoint<_Unit, _Value>& TimePoint<_Unit, _Value>::operator-=(const TimePoint& right) {
	time -= right.time;
	return *this;
}

//=========================================================================================
// operator
//=========================================================================================

template <TimeUnit _Unit, std::floating_point _Value>
constexpr TimePoint<_Unit, _Value> operator+(const TimePoint<_Unit, _Value>& left, const TimePoint<_Unit, _Value>& right) {
	return { left.time + right.time };
}

template <TimeUnit _Unit, std::floating_point _Value>
constexpr TimePoint<_Unit, _Value> operator-(const TimePoint<_Unit, _Value>& left, const TimePoint<_Unit, _Value>& right) {
	return { left.time - right.time };
}

////////////////////////////////////////////////////////////////////////////////////////////
// TimePoint methods
////////////////////////////////////////////////////////////////////////////////////////////

template <TimeUnit _Unit, std::floating_point _Value>
TimePoint<_Unit, _Value> Mod(TimePoint<_Unit, _Value> left, TimePoint<_Unit, _Value> right) {
	return { std::fmod(left.time, right.time) };
}

//=========================================================================================
// using
//=========================================================================================

template <TimeUnit _Unit>
using TimePointf = TimePoint<_Unit, float>;

template <TimeUnit _Unit>
using TimePointd = TimePoint<_Unit, double>;
