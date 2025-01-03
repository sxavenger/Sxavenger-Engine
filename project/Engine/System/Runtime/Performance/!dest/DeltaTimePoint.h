#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* performance
#include "Performance.h"

////////////////////////////////////////////////////////////////////////////////////////////
// DeltaTimePoint class
////////////////////////////////////////////////////////////////////////////////////////////
template <TimeUnit T>
class DeltaTimePoint {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	void AddDeltaTime() {
		*this += Performance::GetDeltaTime<T>();
	}

	void SubtractDeltaTime() {
		*this -= Performance::GetDeltaTime<T>();
	}

	void Reset() {
		time = 0.0f;
	}

	DeltaTimePoint Mod(DeltaTimePoint divisor) {
		return { std::fmod(time, divisor.time) };
	}

	//=========================================================================================
	// public operator
	//=========================================================================================

	void operator=(DeltaTimePoint other) {
		time = other.time;
	}


	DeltaTimePoint& operator+=(DeltaTimePoint other) {
		time += other.time;
		return *this;
	}

	DeltaTimePoint& operator-=(DeltaTimePoint other) {
		time -= other.time;
		return *this;
	}

	bool operator==(DeltaTimePoint other) const {
		return time == other.time;
	}

	bool operator<(DeltaTimePoint other) const {
		return time < other.time;
	}

	bool operator<=(DeltaTimePoint other) const {
		return time <= other.time;
	}

	bool operator>(DeltaTimePoint other) const {
		return time > other.time;
	}

	bool operator>=(DeltaTimePoint other) const {
		return time >= other.time;
	}

	//=========================================================================================
	// public member
	//=========================================================================================

	float time;

private:
};
