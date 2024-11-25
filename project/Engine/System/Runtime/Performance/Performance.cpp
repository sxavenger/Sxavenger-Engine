#include "Performance.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* time
#include "DeltaTimePoint.h"

//* c++
#include <thread>

//=========================================================================================
// static variables
//=========================================================================================

const std::array<float, static_cast<uint8_t>(SecondsUnit::s) + 1> RunTimeTracker::kSecondsConversions_ = {
	1.0f, 0.001f, 0.000001f
};

////////////////////////////////////////////////////////////////////////////////////////////
// RunTimeTracker class methods
////////////////////////////////////////////////////////////////////////////////////////////

void RunTimeTracker::Begin() {
	reference_ = std::chrono::steady_clock::now();
}

void RunTimeTracker::End() {
	std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();

	// deltaTimeの書き込み
	deltaTime_ = static_cast<float>(std::chrono::duration_cast<std::chrono::microseconds>(now - reference_).count());
}

////////////////////////////////////////////////////////////////////////////////////////////
// Performance class methods
////////////////////////////////////////////////////////////////////////////////////////////


