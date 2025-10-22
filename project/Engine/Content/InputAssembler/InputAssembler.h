#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/SxavengerSystem.h>
#include <Engine/System/DirectX/DxObject/DxDimensionBuffer.h>
#include <Engine/System/DirectX/Context/DirectXQueueContext.h>

//* c++
#include <memory>

////////////////////////////////////////////////////////////////////////////////////////////
// InputAssembler class
////////////////////////////////////////////////////////////////////////////////////////////
//! @brief InputAssemblerを管理するクラス.
template <class _Vertex>
class InputAssembler {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	InputAssembler()  = default;
	~InputAssembler() { Term(); }

	void Create(uint32_t vertexSize, uint32_t indexSize);

	void Term();

	//* bind *//

	void BindIABuffer(const DirectXQueueContext* context) const;

	void DrawCall(const DirectXQueueContext* context, UINT instanceCount = 1) const;

	//* getter *//

	DxObject::VertexDimensionBuffer<_Vertex>* GetVertex() const { return vertex_.get(); }

	DxObject::IndexDimensionBuffer* GetIndex() const { return index_.get(); }

	//* operator *//

	InputAssembler(InputAssembler&&) noexcept            = default;
	InputAssembler& operator=(InputAssembler&&) noexcept = default;

protected:

	//=========================================================================================
	// protected variables
	//=========================================================================================

	std::unique_ptr<DxObject::VertexDimensionBuffer<_Vertex>> vertex_;
	std::unique_ptr<DxObject::IndexDimensionBuffer>     index_;

};

////////////////////////////////////////////////////////////////////////////////////////////
// InputAssembler class methods
////////////////////////////////////////////////////////////////////////////////////////////

template <class _Vertex>
inline void InputAssembler<_Vertex>::Create(uint32_t vertexSize, uint32_t indexSize) {
	vertex_ = std::make_unique<DxObject::VertexDimensionBuffer<_Vertex>>();
	vertex_->Create(SxavengerSystem::GetDxDevice(), vertexSize);

	index_ = std::make_unique<DxObject::IndexDimensionBuffer>();
	index_->Create(SxavengerSystem::GetDxDevice(), indexSize);
}

template <class _Vertex>
inline void InputAssembler<_Vertex>::Term() {
}

template <class _Vertex>
inline void InputAssembler<_Vertex>::BindIABuffer(const DirectXQueueContext* context) const {

	auto commandList = context->GetCommandList();

	D3D12_VERTEX_BUFFER_VIEW vbv = vertex_->GetVertexBufferView();
	D3D12_INDEX_BUFFER_VIEW  ibv = index_->GetIndexBufferView();

	commandList->IASetVertexBuffers(0, 1, &vbv);
	commandList->IASetIndexBuffer(&ibv);
}

template <class _Vertex>
inline void InputAssembler<_Vertex>::DrawCall(const DirectXQueueContext* context, UINT instanceCount) const {
	context->GetCommandList()->DrawIndexedInstanced(index_->GetIndexCount(), instanceCount, 0, 0, 0);
}



