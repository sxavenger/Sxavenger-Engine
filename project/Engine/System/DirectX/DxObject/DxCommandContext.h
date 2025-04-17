#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* DXOBJECT
#include "DxObjectCommon.h"
#include "DxDevice.h"

//* c++
#include <vector>

////////////////////////////////////////////////////////////////////////////////////////////
// DXOBJECT
////////////////////////////////////////////////////////////////////////////////////////////
_DXOBJECT_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// CommandContext class
////////////////////////////////////////////////////////////////////////////////////////////
class CommandContext {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	CommandContext()  = default;
	~CommandContext() { Term(); }

	void Init(Device* device, uint32_t allocatorCount = 1, D3D12_COMMAND_LIST_TYPE type = D3D12_COMMAND_LIST_TYPE_DIRECT);

	void Term();

	//* command option *//

	void TransitionAllocator();

	void ExecuteAllAllocators();

	//* getter *//

	ID3D12GraphicsCommandList6* GetCommandList() const { return commandList_.Get(); }

	ID3D12CommandQueue* GetCommandQueue() const { return commandQueue_.Get(); }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* command *//

	std::vector<ComPtr<ID3D12CommandAllocator>> commandAllocators_;
	ComPtr<ID3D12CommandQueue>                  commandQueue_;
	ComPtr<ID3D12GraphicsCommandList6>          commandList_;

	//* fence *//

	ComPtr<ID3D12Fence>   fence_;
	uint64_t              fenceValue_;
	HANDLE                fenceEvent_;

	std::vector<uint64_t> allocatorFenceValues_ = {}; // 各アロケータの完了を示すフェンス値

	//* parameter *//

	uint32_t allocatorCount_;
	uint32_t currentIndex_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	void CreateCommandAllocator(ID3D12Device* device, D3D12_COMMAND_LIST_TYPE type);
	void CreateCommandQueue(ID3D12Device* device, D3D12_COMMAND_LIST_TYPE type);
	void CreateCommandList(ID3D12Device* device, D3D12_COMMAND_LIST_TYPE type);
	void CreateFence(ID3D12Device* device);

	void Close();
	void Signal();
	void WaitGPU(uint32_t index);
	void Reset(uint32_t index);


};

_DXOBJECT_NAMESPACE_END
