#include "ExecutionSpeed.h"

//=========================================================================================
// static variables
//=========================================================================================
float ExecutionSpeed::freamsParSec_ = 1.0f / 60.0f; //!< 初期値は60fps = 0.016...秒と設定

std::chrono::system_clock::time_point ExecutionSpeed::start_;
std::chrono::system_clock::time_point ExecutionSpeed::end_;

////////////////////////////////////////////////////////////////////////////////////////////
// ExecutionSpeed methods
////////////////////////////////////////////////////////////////////////////////////////////

void ExecutionSpeed::Begin() {
	start_ = std::chrono::system_clock::now();
}

void ExecutionSpeed::End() {
	end_ = std::chrono::system_clock::now();

	// fpsを書き込み micro->sに変換
	freamsParSec_ = static_cast<float>(std::chrono::duration_cast<std::chrono::microseconds>(end_ - start_).count() / 1000000.0);
}
