#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
// DxObject
#include <DxBufferResource.h>

// engine
#include <MyEngine.h>

// c++
#include <memory>

// objectStructure
#include <ObjectStructure.h>

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

	void Init(uint32_t vertexSize, uint32_t indexSize);

	void Term();

	void SetBuffer();

	void DrawCall(UINT instanceCount = 1);

	//* Getter *//

	DxObject::BufferResource<T>* GetVertexBuffer() const { return vertices_.get(); }

	DxObject::IndexBufferResource* GetIndexBuffer() const { return indices_.get(); }

	//* operator *//

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* external *//

	ID3D12GraphicsCommandList* commandList_ = MyEngine::GetCommandList();
	//!< static化してもいい

	//* buffer *//

	std::unique_ptr<DxObject::BufferResource<T>>   vertices_;
	std::unique_ptr<DxObject::IndexBufferResource> indices_;
	//!< 内部部分で配列となっているので複数形

};

////////////////////////////////////////////////////////////////////////////////////////////
// InputAssembler class methods
////////////////////////////////////////////////////////////////////////////////////////////

template<typename T>
void InputAssembler<T>::Init(uint32_t vertexSize, uint32_t indexSize) {
	vertices_ = std::make_unique<DxObject::BufferResource<T>>(MyEngine::GetDevicesObj(), vertexSize);
	indices_  = std::make_unique<DxObject::IndexBufferResource>(MyEngine::GetDevicesObj(), indexSize);
}

template<typename T>
void InputAssembler<T>::Term() {
}

template<typename T>
void InputAssembler<T>::SetBuffer() {

	D3D12_VERTEX_BUFFER_VIEW vbv = vertices_->GetVertexBufferView();
	D3D12_INDEX_BUFFER_VIEW  ibv = indices_->GetIndexBufferView();

	commandList_->IASetVertexBuffers(0, 1, &vbv);
	commandList_->IASetIndexBuffer(&ibv);
}

template<typename T>
inline void InputAssembler<T>::DrawCall(UINT instanceCount) {
	commandList_->DrawIndexedInstanced(indices_->GetIndexSize(), instanceCount, 0, 0, 0);
}

////////////////////////////////////////////////////////////////////////////////////////////
// using
////////////////////////////////////////////////////////////////////////////////////////////

using DefaultInputAssembler = InputAssembler<VertexData>;