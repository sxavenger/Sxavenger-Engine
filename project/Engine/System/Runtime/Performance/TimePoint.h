#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* c++
#include <concepts>

////////////////////////////////////////////////////////////////////////////////////////////
// TimeUtil enum class
////////////////////////////////////////////////////////////////////////////////////////////
enum class TimeUnit {
	second,
	millisecond,
	microsecond,
};

template <TimeUnit _From, TimeUnit _To, std::floating_point _Value>
_Value ConvertTimeUtil() {
	// todo
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
	constexpr ~TimePoint() = default;

	//=========================================================================================
	// operator
	//=========================================================================================

	constexpr TimePoint& operator=(const TimePoint& right);
	constexpr TimePoint& operator=(TimePoint&& right);

	constexpr TimePoint& operator+=(const TimePoint& right);
	constexpr TimePoint& operator-=(const TimePoint& right);

	template<TimeUnit _UnitR>
	constexpr operator TimePoint<_UnitR, _Value>() const;

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
	return TimePoint<_Unit, _Value>(left.time + right.time);
}

template <TimeUnit _Unit, std::floating_point _Value>
constexpr TimePoint<_Unit, _Value> operator-(const TimePoint<_Unit, _Value>& left, const TimePoint<_Unit, _Value>& right) {
	return TimePoint<_Unit, _Value>(left.time - right.time);
}

template<TimeUnit _Unit, std::floating_point _Value>
template<TimeUnit _UnitR>
inline constexpr TimePoint<_Unit, _Value>::operator TimePoint<_UnitR, _Value>() const {
	// todo
}

//=========================================================================================
// using
//=========================================================================================

template <TimeUnit _Unit>
using TimePointf = TimePoint<_Unit, float>;
