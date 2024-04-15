#include "DxrCommand.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
// DxObject
#include <DxrDevices.h>
#include <DxrFence.h>

#include <Logger.h>
#include <cassert>

////////////////////////////////////////////////////////////////////////////////////////////
// Command class methods
////////////////////////////////////////////////////////////////////////////////////////////

void DxrObject::Command::Init(Devices* devices) {

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
		Log("[DxrObject::Command]: commandQueue_ << Complete Create \n");
	}

	// コマンドアロケーターを生成
	{
		auto hr = device->CreateCommandAllocator(
			D3D12_COMMAND_LIST_TYPE_DIRECT,
			IID_PPV_ARGS(&commandAllocator_)
		);

		assert(SUCCEEDED(hr));
		Log("[DxrObject::Command]: commandAllocator_ << Complete Create \n");
	}

	// コマンドリストを生成
	{
		auto hr = device->CreateCommandList(
			0,
			D3D12_COMMAND_LIST_TYPE_DIRECT,
			commandAllocator_.Get(),
			nullptr,
			IID_PPV_ARGS(&commandList_)
		);

		assert(SUCCEEDED(hr));
		Log("[DxrObject::Command]: commandList_ << Complete Create \n");
	}
}

void DxrObject::Command::Term() {
}

void DxrObject::Command::Close() {
	auto hr = commandList_->Close();
	assert(SUCCEEDED(hr));

	ID3D12CommandList* commandLists[] = { commandList_.Get() };
	commandQueue_->ExecuteCommandLists(1, commandLists);
}

void DxrObject::Command::Reset() {
	auto hr = commandAllocator_->Reset();
	assert(SUCCEEDED(hr));

	hr = commandList_->Reset(commandAllocator_.Get(), nullptr);
	assert(SUCCEEDED(hr));
}

void DxrObject::Command::Signal(Fence* fences) {
	commandQueue_->Signal(fences->GetFence(), fences->GetFenceValue());
}