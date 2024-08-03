#include "Performance.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include <MyEngine.h>
#include <Console.h>

//-----------------------------------------------------------------------------------------
// using
//-----------------------------------------------------------------------------------------
using namespace DxObject;

//=========================================================================================
// static variables
//=========================================================================================
//* chrono *//
float Performance::deltaTime_ = 0.0f; //!< 初期値は60fps = 0.016...秒と設定
std::chrono::steady_clock::time_point Performance::startFrame_, Performance::endFrame_;

//* buffer *//
std::unique_ptr<BufferResource<Performance::PerFrame>> Performance::perFrameBuffer_;

float Performance::secondsConversions_[SecondsUnit::kCountOfSecondsUnit] = {
	1.0f, 0.001f, 0.000001f
};

////////////////////////////////////////////////////////////////////////////////////////////
// Performance class methods
////////////////////////////////////////////////////////////////////////////////////////////

void Performance::BeginFrame() {
	if (!console->IsUpdateRequired()) {
		return;
	}

	startFrame_ = std::chrono::steady_clock::now();
}

void Performance::EndFrame() {
	if (!console->IsUpdateRequired()) {
		return;
	}

	endFrame_ = std::chrono::steady_clock::now();

	deltaTime_ = static_cast<float>(std::chrono::duration_cast<std::chrono::microseconds>(endFrame_ - startFrame_).count());

	if (perFrameBuffer_ != nullptr) { //!< bufferが存在する場合
		(*perFrameBuffer_)[0].deltaTime = GetDeltaTime(s); //!< 秒でのdeltaTimeをBufferに書き込み
	}
}

void Performance::CreateBuffer() {
	perFrameBuffer_ = std::make_unique<BufferResource<PerFrame>>(MyEngine::GetDevicesObj(), 1);

	// 現在の時間をBufferに書き込み
	auto now = std::chrono::high_resolution_clock::now();
	(*perFrameBuffer_)[0].time = std::chrono::duration<float>(now.time_since_epoch()).count();
}

void Performance::TermBuffer() {
	perFrameBuffer_.reset();
}

const D3D12_GPU_VIRTUAL_ADDRESS Performance::GetGPUVirtualAddress() {
	assert(perFrameBuffer_); //!< bufferが存在しない場合, 値を返せない
	return perFrameBuffer_->GetGPUVirtualAddress();
}
