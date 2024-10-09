#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* c++
#include <random>

//* engine
#include <Engine/System/Logger.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Random class
////////////////////////////////////////////////////////////////////////////////////////////
class Random {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	template <std::floating_point T>
	static T Generate(T min, T max);

	template <std::integral T>
	static T Generate(T min, T max);

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	static std::mt19937 seed_;
};

template <std::floating_point T>
inline T Random::Generate(T min, T max) {
	std::uniform_real_distribution<T> dist(min, max);
	return dist(seed_);
}

template <std::integral T>
inline T Random::Generate(T min, T max) {
	std::uniform_int_distribution<T> dist(min, max);
	return dist(seed_);
}
