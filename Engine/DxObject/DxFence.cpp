#include "DxFence.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
// DxObject
#include "DxDevices.h"

#include <Logger.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Fence class
////////////////////////////////////////////////////////////////////////////////////////////

DxObject::Fence::Fence(Devices* devices) { Init(devices); }

DxObject::Fence::~Fence() { Term(); }

void DxObject::Fence::Init(Devices* devices) {

	ID3D12Device* device = devices->GetDevice();

	// フェンスの生成
	{
		fenceValue_ = 0;

		auto hr = device->CreateFence(
			fenceValue_,
			D3D12_FENCE_FLAG_NONE,
			IID_PPV_ARGS(&fence_)
		);

		assert(SUCCEEDED(hr));
		Log("[DxObject.Fence]: fence_ << Complete Create");
	}

	// Fenceのシグナルを待つためのイベントを作成
	{
		fenceEvent_ = CreateEvent(
			NULL, FALSE, FALSE, NULL
		);

		assert(fenceEvent_ != nullptr);
	}
}

void DxObject::Fence::Term() {
	CloseHandle(fenceEvent_);
}

void DxObject::Fence::AddFenceValue() {
	fenceValue_++;
}

void DxObject::Fence::WaitGPU() {
	if (fence_->GetCompletedValue() < fenceValue_) {
		// 指定したSiganlにたどり着いていないので, たどり着くまで待つようにイベントを設定
		fence_->SetEventOnCompletion(fenceValue_, fenceEvent_);
		// イベントを待機
		WaitForSingleObject(fenceEvent_, INFINITE);
	}
}