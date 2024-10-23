#include "DxCommand.h"
_DXOBJECT_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
// DxObject
#include "DxDevices.h"
#include "DxFence.h"

////////////////////////////////////////////////////////////////////////////////////////////
// Command methods
////////////////////////////////////////////////////////////////////////////////////////////

void Command::Init(Devices* devices) {

	// デバイスの取り出し
	ID3D12Device* device = devices->GetDevice();

	// コマンドキューを生成
	{
		// デスクの設定
		D3D12_COMMAND_QUEUE_DESC desc = {};

		auto hr = device->CreateCommandQueue(
			&desc,
			IID_PPV_ARGS(&commandQueue_)
		);

		Assert(SUCCEEDED(hr));
	}

	// コマンドアロケーターを生成
	{
		for (int i = 0; i < kAllocatorCount_; ++i) {
			auto hr = device->CreateCommandAllocator(
				D3D12_COMMAND_LIST_TYPE_DIRECT,
				IID_PPV_ARGS(&commandAllocator_[i])
			);

			Assert(SUCCEEDED(hr));
		}
	}

	// コマンドリストを生成
	{
		auto hr = device->CreateCommandList(
			0,
			D3D12_COMMAND_LIST_TYPE_DIRECT,
			commandAllocator_[backAllocatorIndex_].Get(),
			nullptr,
			IID_PPV_ARGS(&commandList_)
		);

		Assert(SUCCEEDED(hr));
	}

	Log("[_DXOBJECT]::Command complete init.");
}

void Command::Term() {
}

void Command::Close() {
	auto hr = commandList_->Close();
	Assert(SUCCEEDED(hr));

	ID3D12CommandList* commandLists[] = { commandList_.Get() };
	commandQueue_->ExecuteCommandLists(1, commandLists);
}

void Command::ResetAllAllocator() {
	for (int i = 0; i < kAllocatorCount_; ++i) {
		auto hr = commandAllocator_[i]->Reset();
		Assert(SUCCEEDED(hr));
	}

	// 全リセットしたのでそのままAllocatorを使う
	auto hr = commandList_->Reset(commandAllocator_[backAllocatorIndex_].Get(), nullptr);
	Assert(SUCCEEDED(hr));
}

void Command::ResetBackAllocatorAndTranstion() {
	// backAllocatorIndexの更新
	backAllocatorIndex_++;
	backAllocatorIndex_ %= kAllocatorCount_;

	// backAllocatorとして動かすのでリセット
	auto hr = commandAllocator_[backAllocatorIndex_]->Reset();
	Assert(SUCCEEDED(hr));

	// backAllocatorをセット
	hr = commandList_->Reset(commandAllocator_[backAllocatorIndex_].Get(), nullptr);
	Assert(SUCCEEDED(hr));
}

void Command::ResetBackAllocator() {
	// backAllocatorそのまま使用

	auto hr = commandAllocator_[backAllocatorIndex_]->Reset();
	Assert(SUCCEEDED(hr));

	// backAllocatorをセット
	hr = commandList_->Reset(commandAllocator_[backAllocatorIndex_].Get(), nullptr);
	Assert(SUCCEEDED(hr));
}

void Command::Signal(Fence* fences) {
	commandQueue_->Signal(fences->GetFence(), fences->GetFenceValue());
}