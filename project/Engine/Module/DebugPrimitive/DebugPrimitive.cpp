#include "DebugPrimitive.h"
_DXOBJECT_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/SxavengerSystem.h>

////////////////////////////////////////////////////////////////////////////////////////////
// BaseDebugPrimitive class methods
////////////////////////////////////////////////////////////////////////////////////////////

void BaseDebugPrimitive::Draw(const DirectXThreadContext* context, const AViewActor* view) {
	context; view;
}

void BaseDebugPrimitive::Reset() {
	inputOffset_ = 0;
	inputCount_  = 0;
}

void BaseDebugPrimitive::CountBufferOffset() {
	inputOffset_ += inputCount_;
	inputCount_   = 0;
}

void BaseDebugPrimitive::CreateInputBuffer(uint32_t size) {
	input_ = std::make_unique<VertexDimensionBuffer<PrimitiveInput>>();
	input_->Create(SxavengerSystem::GetDxDevice(), size);
}

void BaseDebugPrimitive::SetVertexBuffer(const Vector4f& position, const Color4f& color) {
	Assert(inputOffset_ + inputCount_ < input_->GetSize()); //!< indexの超過
	position; color;
}


