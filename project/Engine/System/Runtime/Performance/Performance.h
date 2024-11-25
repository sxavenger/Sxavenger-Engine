#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* c++
#include <chrono>
#include <array>

////////////////////////////////////////////////////////////////////////////////////////////
// SecondsUnit enum class
////////////////////////////////////////////////////////////////////////////////////////////
enum class SecondsUnit : uint8_t {
	us, //!< マイクロ秒
	ms, //!< ミリ秒
	s   //!< 秒
};

//-----------------------------------------------------------------------------------------
// forward
//-----------------------------------------------------------------------------------------
template <SecondsUnit T>
class DeltaTimePoint;

////////////////////////////////////////////////////////////////////////////////////////////
// RunTimeTracker class
////////////////////////////////////////////////////////////////////////////////////////////
class RunTimeTracker {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	RunTimeTracker()  = default;
	~RunTimeTracker() = default;

	void Begin();

	void End();

	void WaitForFPS(float fps);

	template <SecondsUnit T>
	DeltaTimePoint<T> GetDeltaTime() const;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* chrono *//

	std::chrono::steady_clock::time_point reference_;

	//* delta time *//

	float deltaTime_;

	static const std::array<float, static_cast<uint8_t>(SecondsUnit::s) + 1> kSecondsConversions_;

};

////////////////////////////////////////////////////////////////////////////////////////////
// RunTimeTracker class template methods
////////////////////////////////////////////////////////////////////////////////////////////

template<SecondsUnit T>
inline DeltaTimePoint<T> RunTimeTracker::GetDeltaTime() const {
	return { deltaTime_ * kSecondsConversions_[static_cast<uint8_t>(T)] };
}

////////////////////////////////////////////////////////////////////////////////////////////
// Performance class
////////////////////////////////////////////////////////////////////////////////////////////
class Performance {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	static void BeginFrame();

	static void EndFrame();

	template <SecondsUnit T>
	static DeltaTimePoint<T> GetDeltaTime();

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	static RunTimeTracker runTimeTracker_;
	static float lockFPS_;

};

////////////////////////////////////////////////////////////////////////////////////////////
// Performance class template methods
////////////////////////////////////////////////////////////////////////////////////////////

template<SecondsUnit T>
inline DeltaTimePoint<T> Performance::GetDeltaTime() {
	return runTimeTracker_.GetDeltaTime<T>();
}
