#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/SxavengerSystem.h>
#include <Engine/System/DirectX/DirectXContext.h>
#include <Engine/System/DirectX/DxObject/DxDimensionBuffer.h>

//* c++
#include <memory>

////////////////////////////////////////////////////////////////////////////////////////////
// InputAssembler class
////////////////////////////////////////////////////////////////////////////////////////////
template <class T>
class InputAssembler {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	InputAssembler()  = default;
	~InputAssembler() { Term(); }

	void Create(uint32_t vertexSize, uint32_t indexSize);

	void Term();

	void BindIABuffer(DirectXThreadContext* context = SxavengerSystem::GetMainThreadContext()) const;

	void DrawCall(UINT instanceCount = 1, DirectXThreadContext* context = SxavengerSystem::GetMainThreadContext()) const;

	//* getter *//

	DxObject::VertexDimensionBuffer<T>* GetVertex() const { return vertex_.get(); }

	DxObject::IndexDimensionBuffer* GetIndex() const { return index_.get(); }

	//* operator *//

	InputAssembler(InputAssembler&& other) noexcept            = default;
	InputAssembler& operator=(InputAssembler&& other) noexcept = default;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	std::unique_ptr<DxObject::VertexDimensionBuffer<T>> vertex_;
	std::unique_ptr<DxObject::IndexDimensionBuffer>     index_;

};

////////////////////////////////////////////////////////////////////////////////////////////
// InputAssembler class methods
////////////////////////////////////////////////////////////////////////////////////////////

template<typename T>
inline void InputAssembler<T>::Create(uint32_t vertexSize, uint32_t indexSize) {
	vertex_ = std::make_unique<DxObject::VertexDimensionBuffer<T>>();
	vertex_->Create(SxavengerSystem::GetDxDevice(), vertexSize);

	index_ = std::make_unique<DxObject::IndexDimensionBuffer>();
	index_->Create(SxavengerSystem::GetDxDevice(), indexSize);
}

template<typename T>
inline void InputAssembler<T>::Term() {
}

template<class T>
inline void InputAssembler<T>::BindIABuffer(DirectXThreadContext* context) const {

	auto commandList = context->GetCommandList();

	D3D12_VERTEX_BUFFER_VIEW vbv = vertex_->GetVertexBufferView();
	D3D12_INDEX_BUFFER_VIEW  ibv = index_->GetIndexBufferView();

	commandList->IASetVertexBuffers(0, 1, &vbv);
	commandList->IASetIndexBuffer(&ibv);
}

template<class T>
inline void InputAssembler<T>::DrawCall(UINT instanceCount, DirectXThreadContext* context) const {

	auto commandList = context->GetCommandList();

	commandList->DrawIndexedInstanced(index_->GetSize(), instanceCount, 0, 0, 0);

}



