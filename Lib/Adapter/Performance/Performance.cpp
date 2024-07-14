#include "Performance.h"

//=========================================================================================
// static variables
//=========================================================================================
float Performance::deltaTime_ = 0.0f; //!< 初期値は60fps = 0.016...秒と設定

std::chrono::steady_clock::time_point Performance::startFrame_, Performance::endFrame_;

float Performance::secondsConversions_[SecondsUnit::kCountOfSecondsUnit] = {
	1.0f, 0.001f, 0.000001f
};

////////////////////////////////////////////////////////////////////////////////////////////
// Performance class methods
////////////////////////////////////////////////////////////////////////////////////////////

void Performance::BeginFrame() {
	startFrame_ = std::chrono::steady_clock::now();
}

void Performance::EndFrame() {
	endFrame_ = std::chrono::steady_clock::now();

	deltaTime_ = static_cast<float>(std::chrono::duration_cast<std::chrono::microseconds>(endFrame_ - startFrame_).count());
}
