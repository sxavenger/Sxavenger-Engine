#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//*
#include "Performance.h"

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

	DeltaTimePoint Mod(DeltaTimePoint& other) {
		return { std::fmod(time, other.time) };
	}

	//=========================================================================================
	// public operator
	//=========================================================================================

	void operator=(const DeltaTimePoint& other) {
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