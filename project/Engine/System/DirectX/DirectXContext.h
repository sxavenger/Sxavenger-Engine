#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* DXOBJECT
#include "DxObject/DxCommandContext.h"
#include "DxObject/DxSwapChain.h"
#include "DxObject/DxDepthStencil.h"

//* lib
#include <Lib/Geometry/Vector2.h>
#include <Lib/Geometry/Vector4.h>
#include <Lib/Geometry/Color4.h>

//* c++
#include <memory>

////////////////////////////////////////////////////////////////////////////////////////////
// DirectXThreadContext class
////////////////////////////////////////////////////////////////////////////////////////////
class DirectXThreadContext {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	DirectXThreadContext()  = default;
	~DirectXThreadContext() { Term(); }

	void Init(uint32_t allocatorCount, D3D12_COMMAND_LIST_TYPE type = D3D12_COMMAND_LIST_TYPE_DIRECT);

	void Term();

	//* command context option *//

	void TransitionAllocator() const;

	void ExecuteAllAllocators() const;

	ID3D12GraphicsCommandList6* GetCommandList() const;

	ID3D12CommandQueue* GetCommandQueue() const;

	DxObject::CommandContext* GetDxCommand() const { return command_.get(); }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* DXOBJECT *//

	std::unique_ptr<DxObject::CommandContext> command_;

	//* parameter *//

	D3D12_COMMAND_LIST_TYPE type_ = D3D12_COMMAND_LIST_TYPE_NONE;

	//=========================================================================================
	// private methods
	//=========================================================================================

	void SetDescriptorHeap() const;

};

////////////////////////////////////////////////////////////////////////////////////////////
// DirectXWindowContext class
////////////////////////////////////////////////////////////////////////////////////////////
class DirectXWindowContext {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	DirectXWindowContext()  = default;
	~DirectXWindowContext() { Term(); }

	void Init(Window* window, DirectXThreadContext* context, const Color4f& clearColor);

	void Term();

	void BeginRendering();

	void EndRendering();

	void ClearWindow(bool isClearColor = true, bool isClearDepth = true);

	void TryClearWindow(bool isClearColor = true, bool isClearDepth = true);

	void Present(bool isTransitonAllocator = true);

private:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* extrenal *//

	DirectXThreadContext* threadContext_ = nullptr;

	//* DXOBJECT *//

	std::unique_ptr<DxObject::SwapChain>    swapChain_;
	std::unique_ptr<DxObject::DepthStencil> depthStencil_;

	//* paraemeter *//

	Color4f clearColor_ = {};

	bool isClearWindow_ = false;

};
