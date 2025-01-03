#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* performance
#include "TimePoint.h"

//* c++
#include <chrono>

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

	template <TimeUnit _Unit>
	TimePointf<_Unit> GetDeltaTime() const;

	template <TimeUnit _Unit>
	TimePointf<_Unit> GetElapsedTime() const;

	const std::chrono::steady_clock::time_point& GetReference() const { return reference_; }

	void SetDeltaTime(const TimePointf<TimeUnit::microsecond>& time) { deltaTime_ = time; }

protected:

	//=========================================================================================
	// protected variables
	//=========================================================================================

	//* time point *//

	std::chrono::steady_clock::time_point reference_;

	TimePointf<TimeUnit::microsecond> deltaTime_;

};

////////////////////////////////////////////////////////////////////////////////////////////
// RunTimeTracker class template methods
////////////////////////////////////////////////////////////////////////////////////////////

template <TimeUnit _Unit>
inline TimePointf<_Unit> RunTimeTracker::GetDeltaTime() const {
	return deltaTime_;
}

template<TimeUnit _Unit>
inline TimePointf<_Unit> RunTimeTracker::GetElapsedTime() const {
	auto now = std::chrono::steady_clock::now();

	auto duration = std::chrono::duration_cast<std::chrono::microseconds>(now - reference_);
	TimePointf<TimeUnit::microsecond> elapsed = { static_cast<float>(duration.count()) };

	return elapsed;
}
