#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
// engine
#include <Engine/System/Sxavenger.h>

// DxObject
#include <Engine/System/DxObject/DxBufferResource.h>

// c++
#include <memory>

////////////////////////////////////////////////////////////////////////////////////////////
// InputAssembler class
////////////////////////////////////////////////////////////////////////////////////////////
template <typename T>
class InputAssembler {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	InputAssembler() = default;
	~InputAssembler() { Term(); }

	void Create(uint32_t vertexSize, uint32_t indexSize);

	void Term();

	void BindIABuffer() const;

	void DrawCall(UINT instanceCount = 1) const;

	//* Getter *//

	DxObject::BufferResource<T>* GetVertexBuffer() const { return vertices_.get(); }

	DxObject::IndexBufferResource* GetIndexBuffer() const { return indices_.get(); }

	//* operator *//

	InputAssembler(InputAssembler&& other) noexcept            = default;
	InputAssembler& operator=(InputAssembler&& other) noexcept = default;

protected:

	//=========================================================================================
	// protected variables
	//=========================================================================================

	//* buffer *//

	std::unique_ptr<DxObject::BufferResource<T>>   vertices_;
	std::unique_ptr<DxObject::IndexBufferResource> indices_;
	//!< 内部部分で配列となっているので複数形

};

////////////////////////////////////////////////////////////////////////////////////////////
// InputAssembler class methods
////////////////////////////////////////////////////////////////////////////////////////////

template<typename T>
void InputAssembler<T>::Create(uint32_t vertexSize, uint32_t indexSize) {
	vertices_ = std::make_unique<DxObject::BufferResource<T>>(Sxavenger::GetDevicesObj(), vertexSize);
	indices_  = std::make_unique<DxObject::IndexBufferResource>(Sxavenger::GetDevicesObj(), indexSize);
}

template<typename T>
void InputAssembler<T>::Term() {
}

template<typename T>
void InputAssembler<T>::BindIABuffer() const {

	auto commandList = Sxavenger::GetCommandList();

	D3D12_VERTEX_BUFFER_VIEW vbv = vertices_->GetVertexBufferView();
	D3D12_INDEX_BUFFER_VIEW  ibv = indices_->GetIndexBufferView();

	commandList->IASetVertexBuffers(0, 1, &vbv);
	commandList->IASetIndexBuffer(&ibv);
}

template<typename T>
inline void InputAssembler<T>::DrawCall(UINT instanceCount) const {

	auto commandList = Sxavenger::GetCommandList();

	commandList->DrawIndexedInstanced(indices_->GetIndexSize(), instanceCount, 0, 0, 0);
}