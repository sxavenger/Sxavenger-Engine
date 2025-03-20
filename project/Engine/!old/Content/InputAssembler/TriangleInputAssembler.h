#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/SxavengerSystem.h>
#include <Engine/System/DirectX/DxObject/DxDimensionBuffer.h>
#include <Engine/System/DirectX/DirectXContext.h>

//* c++
#include <memory>

////////////////////////////////////////////////////////////////////////////////////////////
// TriangleInputAssembler class
////////////////////////////////////////////////////////////////////////////////////////////
template <class _Vertex>
class TriangleInputAssembler {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	TriangleInputAssembler() = default;
	~TriangleInputAssembler() { Term(); }

	void Create(uint32_t vertexSize, uint32_t triangleCount);

	void Term();

	//* bind *//

	void BindIABuffer(const DirectXThreadContext* context) const;

	void DrawCall(const DirectXThreadContext* context, UINT instanceCount = 1) const;

	//* getter *//

	bool IsCreate() const { return vertex_ != nullptr && index_ != nullptr; }

	DxObject::VertexDimensionBuffer<_Vertex>* GetVertex() const { return vertex_.get(); }

	DxObject::TriangleIndexDimensionBuffer* GetIndex() const { return index_.get(); }

	//* operator *//

	TriangleInputAssembler(TriangleInputAssembler&&) noexcept            = default;
	TriangleInputAssembler& operator=(TriangleInputAssembler&&) noexcept = default;

protected:

	//=========================================================================================
	// protected variables
	//=========================================================================================

	std::unique_ptr<DxObject::VertexDimensionBuffer<_Vertex>> vertex_;
	std::unique_ptr<DxObject::TriangleIndexDimensionBuffer>   index_;

};

////////////////////////////////////////////////////////////////////////////////////////////
// TriangleInputAssembler class template methods
////////////////////////////////////////////////////////////////////////////////////////////

template <class _Vertex>
inline void TriangleInputAssembler<_Vertex>::Create(uint32_t vertexSize, uint32_t triangleCount) {
	vertex_ = std::make_unique<DxObject::VertexDimensionBuffer<_Vertex>>();
	vertex_->Create(SxavengerSystem::GetDxDevice(), vertexSize);

	index_ = std::make_unique<DxObject::TriangleIndexDimensionBuffer>();
	index_->Create(SxavengerSystem::GetDxDevice(), triangleCount);
}

template <class _Vertex>
inline void TriangleInputAssembler<_Vertex>::Term() {
}

template <class _Vertex>
inline void TriangleInputAssembler<_Vertex>::BindIABuffer(const DirectXThreadContext* context) const {
	auto commandList = context->GetCommandList();

	D3D12_VERTEX_BUFFER_VIEW vbv = vertex_->GetVertexBufferView();
	D3D12_INDEX_BUFFER_VIEW  ibv = index_->GetIndexBufferView();

	commandList->IASetVertexBuffers(0, 1, &vbv);
	commandList->IASetIndexBuffer(&ibv);
}

template <class _Vertex>
inline void TriangleInputAssembler<_Vertex>::DrawCall(const DirectXThreadContext* context, UINT instanceCount) const {
	context->GetCommandList()->DrawIndexedInstanced(index_->GetIndexCount(), instanceCount, 0, 0, 0);
}
