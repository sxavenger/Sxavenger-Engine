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

	void Init(uint32_t allocatorCount);

	void Term();

	//* command context option *//

	void TransitionAllocator();

	void ExecuteAllAllocators();

	ID3D12GraphicsCommandList6* GetCommandList() const;

	ID3D12CommandQueue* GetCommandQueue() const;

	DxObject::CommandContext* GetDxCommand() const { return command_.get(); }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* DXOBJECT *//

	std::unique_ptr<DxObject::CommandContext> command_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	void SetDescriptorHeap();

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

	void Init(Window* window, DirectXThreadContext* threadContext, const Color4f& clearColor);

	void Term();

	void BeginRendering();

	void EndRendering();

	void ClearWindow(bool isClearColor = true, bool isClearDepth = true);

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

};