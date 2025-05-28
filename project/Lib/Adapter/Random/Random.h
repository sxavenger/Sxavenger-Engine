#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* c++
#include <random>

////////////////////////////////////////////////////////////////////////////////////////////
// Random class
////////////////////////////////////////////////////////////////////////////////////////////
class Random {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* uniform distribution *//

	template <std::floating_point T = float>
	static T UniformDistribution(T min, T max) {
		std::uniform_real_distribution<T> dist(min, max);
		return dist(seed_);
	}

	template <std::integral T = int32_t>
	static T UniformDistribution(T min, T max) {
		std::uniform_int_distribution<T> dist(min, max);
		return dist(seed_);
	}

	//* normal distribution *//

	template <std::floating_point T = float>
	static T NormalDistribution(T mean = 0.0, T stddev = 0.5) {
		std::normal_distribution<T> dist(mean, stddev);
		return dist(seed_);
	}

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	static inline std::mt19937 seed_ = std::mt19937(std::random_device{}());

};
