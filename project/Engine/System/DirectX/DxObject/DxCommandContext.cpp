#include "DxCommandContext.h"
_DXOBJECT_USING

////////////////////////////////////////////////////////////////////////////////////////////
// CommandContext class methods
////////////////////////////////////////////////////////////////////////////////////////////

void CommandContext::Init(Device* devices, uint32_t allocatorCount) {

	ID3D12Device* device = devices->GetDevice();

	// 引数の保存
	allocatorCount_ = allocatorCount;
	currentIndex_   = 0;

	commandAllocators_.resize(allocatorCount_);
	allocatorFenceValues_.resize(allocatorCount_);

	CreateCommandAllocator(device);
	CreateCommandQueue(device);
	CreateCommandList(device);
	CreateFence(device);
}

void CommandContext::Term() {
}

void CommandContext::TransitionAllocator() {

	Close();

	Signal();

	uint32_t nextIndex = (currentIndex_ + 1) % allocatorCount_;
	Reset(nextIndex);

}

void CommandContext::ExecuteAllAllocators() {

	Close();

	Signal();

	Reset(currentIndex_);
}

void CommandContext::CreateCommandAllocator(ID3D12Device* device) {


	// コマンドアロケーターを生成
	for (uint32_t i = 0; i < allocatorCount_; ++i) {
		auto hr = device->CreateCommandAllocator(
			D3D12_COMMAND_LIST_TYPE_DIRECT,
			IID_PPV_ARGS(&commandAllocators_[i])
		);
		Assert(SUCCEEDED(hr));
	}
}

void CommandContext::CreateCommandQueue(ID3D12Device* device) {
	// デスクの設定
	D3D12_COMMAND_QUEUE_DESC desc = {};

	// コマンドキューを生成
	auto hr = device->CreateCommandQueue(
		&desc,
		IID_PPV_ARGS(&commandQueue_)
	);

	Assert(SUCCEEDED(hr));
}

void CommandContext::CreateCommandList(ID3D12Device* device) {
	// コマンドリストを生成
	auto hr = device->CreateCommandList(
		0,
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		commandAllocators_[currentIndex_].Get(),
		nullptr,
		IID_PPV_ARGS(&commandList_)
	);
	Assert(SUCCEEDED(hr));
	
}

void CommandContext::CreateFence(ID3D12Device* device) {

	fenceValue_ = 0;

	auto hr = device->CreateFence(
		fenceValue_,
		D3D12_FENCE_FLAG_NONE,
		IID_PPV_ARGS(&fence_)
	);
	Assert(SUCCEEDED(hr));

	fenceEvent_ = CreateEvent(
		NULL, FALSE, FALSE, NULL
	);
	Assert(fenceEvent_ != nullptr);
}

void CommandContext::Close() {

	auto hr = commandList_->Close();
	Assert(SUCCEEDED(hr));

	ID3D12CommandList* commandLists[] = { commandList_.Get() };
	commandQueue_->ExecuteCommandLists(1, commandLists);
}

void CommandContext::Signal() {
	fenceValue_++;
	commandQueue_->Signal(fence_.Get(), fenceValue_);

	allocatorFenceValues_[currentIndex_] = fenceValue_;
}

void CommandContext::WaitGPU(uint32_t index) {

	uint64_t fenceValue = allocatorFenceValues_[index];

	if (fence_->GetCompletedValue() < fenceValue) {
		// 指定したSiganlにたどり着いていないので, たどり着くまで待つようにイベントを設定
		fence_->SetEventOnCompletion(fenceValue, fenceEvent_);
		// イベントを待機
		WaitForSingleObject(fenceEvent_, INFINITE);
	}

}

void CommandContext::Reset(uint32_t index) {
	//!< indexのallocatorの実行が終わるまで待機
	WaitGPU(index);

	// allocatorのリセット
	auto hr = commandAllocators_[index]->Reset();
	Assert(SUCCEEDED(hr));

	// allocatorをセット
	hr = commandList_->Reset(commandAllocators_[index].Get(), nullptr);
	Assert(SUCCEEDED(hr));

	// 現在のindexとして設定
	currentIndex_ = index;
}
