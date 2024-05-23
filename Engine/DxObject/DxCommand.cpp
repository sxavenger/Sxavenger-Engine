#include "DxCommand.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
// DxObject
#include <DxDevices.h>
#include <DxFence.h>

#include <Logger.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Command methods
////////////////////////////////////////////////////////////////////////////////////////////

DxObject::Command::Command(Devices* devices) { Init(devices); }

DxObject::Command::~Command() { Term(); }

void DxObject::Command::Init(Devices* devices) {

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

		assert(SUCCEEDED(hr));
		Log("[DxObject.Command]: commandQueue_ << Complete Create");
	}

	// コマンドアロケーターを生成
	{
		for (int i = 0; i < kAllocatorCount; ++i) {
			auto hr = device->CreateCommandAllocator(
				D3D12_COMMAND_LIST_TYPE_DIRECT,
				IID_PPV_ARGS(&commandAllocator_[i])
			);

			assert(SUCCEEDED(hr));
		}
		
		Log("[DxObject.Command]: commandAllocator_ << Complete Create");
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

		assert(SUCCEEDED(hr));
		Log("[DxObject.Command]: commandList_ << Complete Create");
	}
}

void DxObject::Command::Term() {
}

void DxObject::Command::Close() {
	auto hr = commandList_->Close();
	assert(SUCCEEDED(hr));

	ID3D12CommandList* commandLists[] = { commandList_.Get() };
	commandQueue_->ExecuteCommandLists(1, commandLists);
}

void DxObject::Command::ResetAll() {
	for (int i = 0; i < kAllocatorCount; ++i) {
		auto hr = commandAllocator_[i]->Reset();
		assert(SUCCEEDED(hr));
	}
	
	// 全リセットしたのでそのままAllocatorを使う
	auto hr = commandList_->Reset(commandAllocator_[backAllocatorIndex_].Get(), nullptr);
	assert(SUCCEEDED(hr));
}

void DxObject::Command::ResetBackAllocator() {
	// backAllocatorIndexの更新
	backAllocatorIndex_++;
	backAllocatorIndex_ %= kAllocatorCount;

	// backAllocatorとして動かすのでリセット
	auto hr = commandAllocator_[backAllocatorIndex_]->Reset();
	assert(SUCCEEDED(hr));

	// backAllocatorをセット
	hr = commandList_->Reset(commandAllocator_[backAllocatorIndex_].Get(), nullptr);
	assert(SUCCEEDED(hr));
}

void DxObject::Command::Signal(Fence* fences) {
	commandQueue_->Signal(fences->GetFence(), fences->GetFenceValue());
}