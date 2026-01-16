#include "TimestampCpu.h"
SXAVENGER_ENGINE_USING

//////////////////////////////////////////////////////////////////////////////////////////
// Stamp structure methods
//////////////////////////////////////////////////////////////////////////////////////////

void TimestampCpu::Stamp::CalculateDelta(const TimePointd<TimeUnit::millisecond>& previous) {
	delta = elapsed - previous;
}

////////////////////////////////////////////////////////////////////////////////////////////
// TimestampCpu class methods
////////////////////////////////////////////////////////////////////////////////////////////

void TimestampCpu::Begin() {
	// timestampの更新かどうかの判定
	recordedTimer_ -= runtime_.GetDeltaTime<TimeUnit::second>();
	isRecord_      = (recordedTimer_ <= 0.0f);

	if (isRecord_) { //!< timestampの更新開始
		timestamps_[currentIndex_].clear();
	}

	runtime_.Begin();
}

void TimestampCpu::End() {
	runtime_.End();

	if (isRecord_) {
		//!< timestampの更新終了
		++currentIndex_ %= kTimestampCount; //!< 現在のtimestampIndexを更新
		recordedTimer_  = recordInterval_;  //!< 次のtimestamp更新までの時間をリセット
		isRecord_       = false;
	}
}

void TimestampCpu::Record(const std::string& name) {
	if (!isRecord_) {
		return;
	}

	Stamp stamp = {};
	stamp.name    = name;
	stamp.elapsed = runtime_.GetElapsedTime<TimeUnit::millisecond>();

	Timestamp& timestamp = timestamps_[currentIndex_];

	//!< 前回のtimestamp時間との差分を計算
	stamp.CalculateDelta(timestamp.empty() ? 0.0 : timestamp.back().elapsed);

	timestamp.emplace_back(std::move(stamp));
}

const TimestampCpu::Timestamp& TimestampCpu::GetTimestamp() const {
	//!< 計測終了済みのtimestampを返す
	return timestamps_[(currentIndex_ + kTimestampCount - 1) % kTimestampCount];
}
