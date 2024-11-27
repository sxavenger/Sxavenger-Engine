#include "Performance.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* time
#include "DeltaTimePoint.h"

//* engine
#include <Engine/Console/Console.h>

//* c++
#include <thread>

//=========================================================================================
// RunTimeTracker static variables
//=========================================================================================

const std::array<float, static_cast<uint8_t>(TimeUnit::s) + 1> RunTimeTracker::kConversions_ = {
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

void RunTimeTracker::WaitForFPS(float fps) {
	const std::chrono::microseconds kWaitTime(static_cast<uint64_t>(1000000.0f / fps));
	const std::chrono::microseconds kCheckWaitTime(static_cast<uint64_t>(1000000.0f / (fps + 5))); //!< 60Hz倍数以外のモニター対策

	// 現在時刻の取得
	std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();

	// 経過所間の取得
	std::chrono::microseconds elapsed
		= std::chrono::duration_cast<std::chrono::microseconds>(now - reference_);

	// frameが指定されたfpsを通過してない場合
	if (elapsed < kCheckWaitTime) {
		while (std::chrono::steady_clock::now() - reference_ < kWaitTime) {
			std::this_thread::sleep_for(std::chrono::microseconds(1));
		}
	}
}

//=========================================================================================
// Performance static variables
//=========================================================================================

RunTimeTracker Performance::runTimeTracker_ = {};
float Performance::lockFPS_                 = 120.0f;

////////////////////////////////////////////////////////////////////////////////////////////
// Performance class methods
////////////////////////////////////////////////////////////////////////////////////////////

void Performance::BeginFrame() {
	runTimeTracker_.Begin();
}

void Performance::EndFrame() {
	runTimeTracker_.WaitForFPS(lockFPS_);

	if (sConsole->IsUpdateRequired()) {
		runTimeTracker_.End();
	}
}