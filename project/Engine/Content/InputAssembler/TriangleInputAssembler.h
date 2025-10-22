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
// TriangleInputAssembler class
////////////////////////////////////////////////////////////////////////////////////////////
//! @brief 三角形描画用のInputAssemblerを管理するクラス.
template <class _Vertex>
class TriangleInputAssembler {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// using
	////////////////////////////////////////////////////////////////////////////////////////////

	using InputVertex = DxObject::VertexDimensionBuffer<_Vertex>;
	using InputIndex  = DxObject::TriangleIndexDimensionBuffer;

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	TriangleInputAssembler() = default;
	~TriangleInputAssembler() { Term(); }

	void Create(uint32_t vertexSize, uint32_t triangleCount);

	void Term();

	//* bind *//

	void BindIABuffer(const DirectXQueueContext* context) const;

	void DrawCall(const DirectXQueueContext* context, UINT instanceCount = 1) const;

	//* getter *//

	bool IsCreate() const { return vertex_ != nullptr && index_ != nullptr; }

	InputVertex* GetVertex() const { return vertex_.get(); }

	InputIndex* GetIndex() const { return index_.get(); }

	//* operator *//

	TriangleInputAssembler(TriangleInputAssembler&&) noexcept            = default;
	TriangleInputAssembler& operator=(TriangleInputAssembler&&) noexcept = default;

protected:

	//=========================================================================================
	// protected variables
	//=========================================================================================

	std::unique_ptr<InputVertex> vertex_;
	std::unique_ptr<InputIndex>  index_;

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
inline void TriangleInputAssembler<_Vertex>::BindIABuffer(const DirectXQueueContext* context) const {
	auto commandList = context->GetCommandList();

	D3D12_VERTEX_BUFFER_VIEW vbv = vertex_->GetVertexBufferView();
	D3D12_INDEX_BUFFER_VIEW  ibv = index_->GetIndexBufferView();

	commandList->IASetVertexBuffers(0, 1, &vbv);
	commandList->IASetIndexBuffer(&ibv);
}

template <class _Vertex>
inline void TriangleInputAssembler<_Vertex>::DrawCall(const DirectXQueueContext* context, UINT instanceCount) const {
	context->GetCommandList()->DrawIndexedInstanced(index_->GetIndexCount(), instanceCount, 0, 0, 0);
}
