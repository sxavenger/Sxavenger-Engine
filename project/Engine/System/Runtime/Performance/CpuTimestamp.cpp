#include "CpuTimestamp.h"
SXAVENGER_ENGINE_USING

//////////////////////////////////////////////////////////////////////////////////////////
// Stamp structure methods
//////////////////////////////////////////////////////////////////////////////////////////

void CpuTimestamp::Stamp::CalculateDelta(const TimePointd<TimeUnit::millisecond>& previous) {
	delta = elapsed - previous;
}

////////////////////////////////////////////////////////////////////////////////////////////
// CpuTimestamp class methods
////////////////////////////////////////////////////////////////////////////////////////////

void CpuTimestamp::Begin() {
	// timestampの更新かどうかの判定
	recordedTimer_ -= runtime_.GetDeltaTime<TimeUnit::second>();
	isRecord_      = (recordedTimer_ <= 0.0f);

	if (isRecord_) { //!< timestampの更新開始
		timestamps_[currentIndex_].clear();
	}

	runtime_.Begin();
}

void CpuTimestamp::End() {
	runtime_.End();

	if (isRecord_) {
		//!< timestampの更新終了
		++currentIndex_ %= kLapCount;      //!< 現在のtimestampIndexを更新
		recordedTimer_  = recordInterval_; //!< 次のtimestamp更新までの時間をリセット
		isRecord_       = false;
	}
}

void CpuTimestamp::Record(const std::string& name) {
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

const CpuTimestamp::Timestamp& CpuTimestamp::GetTimestamp() const {
	//!< 計測終了済みのtimestampを返す
	return timestamps_[(currentIndex_ + kLapCount - 1) % kLapCount];
}
