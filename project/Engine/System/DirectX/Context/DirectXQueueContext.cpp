#include "DirectXQueueContext.h"
_DXOBJECT_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/Utility/Logger.h>
#include <Engine/System/SxavengerSystem.h>

////////////////////////////////////////////////////////////////////////////////////////////
// DirectXQueueContext class methods
////////////////////////////////////////////////////////////////////////////////////////////

void DirectXQueueContext::Init(uint32_t allocator, RenderQueue type) {
	
	// 引数の保存
	type_ = type;

	if (type == RenderQueue::None) {
		return;
	}

	// contextの生成
	context_ = std::make_unique<DxObject::CommandContext>();
	context_->Init(SxavengerSystem::GetDxDevice(), allocator, GetCommandListType(type));
	SetDescriptorHeap();
}

void DirectXQueueContext::SetName(const std::wstring& name) const {
	context_->SetName(std::format(L"{} | {}", name, ToWString(magic_enum::enum_name(type_))));
}

void DirectXQueueContext::TransitionAllocator() const {
	if (context_ == nullptr) {
		return;
	}

	context_->TransitionAllocator();
	SetDescriptorHeap();
}

void DirectXQueueContext::ExecuteAllAllocators() const {
	if (context_ == nullptr) {
		return;
	}

	context_->ExecuteAllAllocators();
	SetDescriptorHeap();
}

bool DirectXQueueContext::IsSupportQueue(RenderQueue type) const {
	return type <= type_;
}

void DirectXQueueContext::RequestQueue(RenderQueue type) const {
	Exception::Assert(IsSupportQueue(type), "[DirectXQueueContext] request queue type don't match.");
}

DxObject::CommandContext* DirectXQueueContext::GetDxCommand() const {
	Exception::Assert(context_ != nullptr, "[DirectXQueueContext] context is not create.");
	return context_.get();
}

ID3D12GraphicsCommandList6* DirectXQueueContext::GetCommandList() const {
	Exception::Assert(context_ != nullptr, "[DirectXQueueContext] context is not create.");
	return context_->GetCommandList();
}

ID3D12CommandQueue* DirectXQueueContext::GetCommandQueue() const {
	Exception::Assert(context_ != nullptr, "[DirectXQueueContext] context is not create.");
	return context_->GetCommandQueue();
}


D3D12_COMMAND_LIST_TYPE DirectXQueueContext::GetCommandListType(RenderQueue type) {
	switch (type) {
		case RenderQueue::Copy:
			return D3D12_COMMAND_LIST_TYPE_COPY;

		case RenderQueue::Compute:
			return D3D12_COMMAND_LIST_TYPE_COMPUTE;

		case RenderQueue::Direct:
			return D3D12_COMMAND_LIST_TYPE_DIRECT;

		default:
			return D3D12_COMMAND_LIST_TYPE_NONE;
	}
}

void DirectXQueueContext::SetDescriptorHeap() const {
	if (!IsSupportQueue(RenderQueue::Compute)) { //!< ComputeQueue以上
		return;
	}

	ID3D12DescriptorHeap* descriptorHeaps[] = {
		SxavengerSystem::GetDxDescriptorHeaps()->GetDescriptorHeap(kDescriptor_CBV_SRV_UAV)
	};

	context_->GetCommandList()->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);
}
