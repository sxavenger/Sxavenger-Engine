#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* c++
#include <chrono>
#include <array>

////////////////////////////////////////////////////////////////////////////////////////////
// TimeUnit enum class
////////////////////////////////////////////////////////////////////////////////////////////
enum class TimeUnit : uint8_t {
	us, //!< マイクロ秒
	ms, //!< ミリ秒
	s   //!< 秒
};

//-----------------------------------------------------------------------------------------
// forward
//-----------------------------------------------------------------------------------------
template <TimeUnit T>
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

	template <TimeUnit T>
	DeltaTimePoint<T> GetDeltaTime() const;

	template <TimeUnit T>
	DeltaTimePoint<T> GetElapsedTime() const;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* chrono *//

	std::chrono::steady_clock::time_point reference_;

	//* delta time *//

	float deltaTime_;

	static const std::array<float, static_cast<uint8_t>(TimeUnit::s) + 1> kSecondsConversions_;

};

////////////////////////////////////////////////////////////////////////////////////////////
// RunTimeTracker class template methods
////////////////////////////////////////////////////////////////////////////////////////////

template<TimeUnit T>
inline DeltaTimePoint<T> RunTimeTracker::GetDeltaTime() const {
	return { deltaTime_ * kSecondsConversions_[static_cast<uint8_t>(T)] };
}

template<TimeUnit T>
inline DeltaTimePoint<T> RunTimeTracker::GetElapsedTime() const {
	std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();

	// deltaTimeの書き込み
	float elapsed = static_cast<float>(std::chrono::duration_cast<std::chrono::microseconds>(now - reference_).count());
	return { elapsed * kSecondsConversions_[static_cast<uint8_t>(T)] };
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

	template <TimeUnit T>
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

template<TimeUnit T>
inline DeltaTimePoint<T> Performance::GetDeltaTime() {
	return runTimeTracker_.GetDeltaTime<T>();
}
