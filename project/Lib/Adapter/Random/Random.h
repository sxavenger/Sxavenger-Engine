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

	template <std::floating_point T = float>
	static T NormalDistributionRange(T min, T max) {
		std::normal_distribution<T> dist(0.0f, 0.3f);
		T distribution = std::max(std::abs(dist(seed_)), 1.0f);
		return min + (max - min) * distribution;
	}

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	static inline std::mt19937 seed_ = std::mt19937(std::random_device{}());

};

////////////////////////////////////////////////////////////////////////////////////////////
// Seed structure
////////////////////////////////////////////////////////////////////////////////////////////
template <typename T, size_t N>
class Seed {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	Seed() { Set(); }

	void Set() {
		std::generate(seed.begin(), seed.end(), []() {
			return Random::UniformDistribution<T>(
				std::numeric_limits<T>::min(),
				std::numeric_limits<T>::max()
			);
		});
	}

	//=========================================================================================
	// public variables
	//=========================================================================================

	std::array<T, N> seed;

};
