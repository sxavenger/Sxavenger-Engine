#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include <random>

////////////////////////////////////////////////////////////////////////////////////////////
// Random class
////////////////////////////////////////////////////////////////////////////////////////////
class Random {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	template <typename T, typename = std::enable_if_t<std::is_same<T, int>::value || std::is_same<T, float>::value>>
	static T Generate(T min, T max) {
		std::uniform_real_distribution<T> dist(min, max);
		return dist(seed_);
	}

private:

	//=========================================================================================
	// private variables
	//=========================================================================================
	static std::mt19937 seed_;
};