#include "LapTimer.h"
SXAVENGER_ENGINE_USING

//////////////////////////////////////////////////////////////////////////////////////////
// Point structure methods
//////////////////////////////////////////////////////////////////////////////////////////

void LapTimer::Point::CalcualteDelta(const TimePointd<TimeUnit::millisecond>& previous) {
	delta = elapsed - previous;
}

////////////////////////////////////////////////////////////////////////////////////////////
// LapTimer class methods
////////////////////////////////////////////////////////////////////////////////////////////

void LapTimer::Begin() {
	// lapの更新かどうかの判定
	recordedTimer_ -= runtime_.GetDeltaTime<TimeUnit::second>();
	isRecord_      = (recordedTimer_ <= 0.0f);

	if (isRecord_) { //!< lapの更新開始
		laps_[currentIndex_].clear();
	}

	runtime_.Begin();
}

void LapTimer::End() {
	runtime_.End();

	if (isRecord_) {
		// lapの更新終了
		++currentIndex_ %= kLapCount; //!< 現在のlapIndexを更新
		recordedTimer_  = recordInterval_; //!< 次のlap更新までの時間をリセット
		isRecord_       = false;
	}
}

void LapTimer::Record(const std::string& name) {
	if (!isRecord_) {
		return;
	}

	Point point = {};
	point.name    = name;
	point.elapsed = runtime_.GetElapsedTime<TimeUnit::millisecond>();

	LapTimeContainer& lap = laps_[currentIndex_];

	if (!lap.empty()) {
		//!< 前回のlap時間との差分を計算
		point.CalcualteDelta(lap.back().elapsed);

	} else {
		//!< 最初のlap時間はそのまま記録
		point.delta = point.elapsed;
	}

	lap.emplace_back(std::move(point));
}

const LapTimer::LapTimeContainer& LapTimer::GetLap() const {
	//!< 計測終了したlapを返す
	return laps_[(currentIndex_ + 1) % kLapCount];
}
