#include "TextureComponent.h"
_DXOBJECT_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/SxavengerSystem.h>

////////////////////////////////////////////////////////////////////////////////////////////
// GPUBuffer strcutre methods
////////////////////////////////////////////////////////////////////////////////////////////

void TextureComponent::GPUBuffer::Init() {
	mat   = Matrix4x4::Identity();
	color = { 1.0f, 1.0f, 1.0f, 1.0f };
}

void TextureComponent::GPUBuffer::Transfer(const Transform2d& transform, const Color4f& _color) {
	mat   = transform.ToMatrix();
	color = _color;
}

////////////////////////////////////////////////////////////////////////////////////////////
// TextureComponent class methods
////////////////////////////////////////////////////////////////////////////////////////////

void TextureComponent::CreateBuffer() {
	buffer_ = std::make_unique<DimensionBuffer<GPUBuffer>>();
	buffer_->Create(SxavengerSystem::GetDxDevice(), 1);
	buffer_->At(0).Init();
}

void TextureComponent::Transfer() {
	buffer_->At(0).Transfer(transform2d_, color_);
}
