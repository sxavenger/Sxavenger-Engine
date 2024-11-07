#include "DxFence.h"
_DXOBJECT_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
// DxObject
#include "DxDevices.h"

////////////////////////////////////////////////////////////////////////////////////////////
// Fence class
////////////////////////////////////////////////////////////////////////////////////////////

void Fence::Init(Devices* devices) {

	ID3D12Device* device = devices->GetDevice();

	// フェンスの生成
	{
		fenceValue_ = 0;

		auto hr = device->CreateFence(
			fenceValue_,
			D3D12_FENCE_FLAG_NONE,
			IID_PPV_ARGS(&fence_)
		);

		Assert(SUCCEEDED(hr));
	}

	// Fenceのシグナルを待つためのイベントを作成
	{
		fenceEvent_ = CreateEvent(
			NULL, FALSE, FALSE, NULL
		);

		Assert(fenceEvent_ != nullptr);
	}

	EngineLog("[_DXOBJECT]::Fence complete init.");
}

void Fence::Term() {
	CloseHandle(fenceEvent_);
}

void Fence::AddFenceValue() {
	fenceValue_++;
}

void Fence::WaitGPU() {
	if (fence_->GetCompletedValue() < fenceValue_) {
		// 指定したSiganlにたどり着いていないので, たどり着くまで待つようにイベントを設定
		fence_->SetEventOnCompletion(fenceValue_, fenceEvent_);
		// イベントを待機
		WaitForSingleObject(fenceEvent_, INFINITE);
	}
}