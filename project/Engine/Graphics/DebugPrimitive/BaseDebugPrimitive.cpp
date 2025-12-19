#include "BaseDebugPrimitive.h"
SXAVENGER_ENGINE_USING
DXOBJECT_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/Utility/StreamLogger.h>
#include <Engine/System/System.h>

////////////////////////////////////////////////////////////////////////////////////////////
// BaseDebugPrimitive class methods
////////////////////////////////////////////////////////////////////////////////////////////

void BaseDebugPrimitive::Draw(const DirectXQueueContext* context) {
	// commandListの取得
	auto commandList = context->GetCommandList();

	// 登録されたinputの描画
	const D3D12_VERTEX_BUFFER_VIEW vbv = input_->GetVertexBufferView();
	commandList->IASetVertexBuffers(0, 1, &vbv);
	commandList->DrawInstanced(inputCount_, 1, inputOffset_, 0);

	CountBufferOffset();
}

void BaseDebugPrimitive::Reset() {
	inputOffset_ = 0;
	inputCount_  = 0;
}

void BaseDebugPrimitive::CountBufferOffset() {
	inputOffset_ += inputCount_;
	inputCount_  = 0;
}

void BaseDebugPrimitive::CreateInputBuffer(uint32_t size) {
	input_ = std::make_unique<VertexDimensionBuffer<PrimitiveInput>>();
	input_->Create(System::GetDxDevice(), size);
}

void BaseDebugPrimitive::SetVertexBuffer(const Vector3f& position, const Color4f& color, float thickness) {
	StreamLogger::AssertA(inputOffset_ + inputCount_ < input_->GetSize(), "index out of range."); //!< indexの超過

	(*input_)[inputOffset_ + inputCount_].position  = { position.x, position.y, position.z, 1.0f };
	(*input_)[inputOffset_ + inputCount_].color     = color;
	(*input_)[inputOffset_ + inputCount_].thickness = thickness;

	// 登録したので
	inputCount_++;
}
