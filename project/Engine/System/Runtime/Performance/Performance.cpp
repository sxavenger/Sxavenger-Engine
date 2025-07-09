#include "Performance.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/Config/SxavengerConfig.h>

//* c++
#include <thread>

////////////////////////////////////////////////////////////////////////////////////////////
// Performance class methods
////////////////////////////////////////////////////////////////////////////////////////////

void Performance::Begin() {
	runtime_.Begin();
}

void Performance::End() {
	WaitFrame();
	runtime_.End();

	// lapの更新かどうかの判定
	recordedTime_ -= runtime_.GetDeltaTime<TimeUnit::second>();
	isRecord_ = (recordedTime_ <= 0.0f);

	if (isRecord_) {
		// lapの更新
		recordedTime_ = recordInterval_;
		++lapIndex_ %= lapCount_;
		laps_[lapIndex_].clear();
	}
}

void Performance::RecordLap(const std::string& name) {
	if (isRecord_) {
		laps_[lapIndex_].emplace_back(name, runtime_.GetElapsedTime<TimeUnit::millisecond>());
	}
}

void Performance::SystemDebugGui() {
}

void Performance::WaitFrame() const {
	if (!SxavengerConfig::GetConfig().isLockFrameRate) {
		return;
	}

	const auto& reference = runtime_.GetReference();

	float targetFrameRate = SxavengerConfig::GetConfig().targetFrameRate;

	const std::chrono::microseconds kWaitTime(static_cast<uint64_t>(1000000.0f / targetFrameRate));
	const std::chrono::microseconds kCheckWaitTime(static_cast<uint64_t>(1000000.0f / (targetFrameRate + 5))); //!< 60Hz倍数以外のモニター対策

	// 現在時刻の取得
	std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();

	// 経過所間の取得
	std::chrono::microseconds elapsed
		= std::chrono::duration_cast<std::chrono::microseconds>(now - reference);

	// frameが指定されたfpsを通過してない場合
	if (elapsed < kCheckWaitTime) {
		while (std::chrono::steady_clock::now() - reference < kWaitTime) {
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}
	}

}
