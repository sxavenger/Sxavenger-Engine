#include "Sprite.h"
_DXOBJECT_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/SxavengerSystem.h>

////////////////////////////////////////////////////////////////////////////////////////
// Sprite class methods
////////////////////////////////////////////////////////////////////////////////////////

void Sprite::Init() {
	Transform2dComponent::CreateBuffer();

	vertex_ = std::make_unique<VertexDimensionBuffer<SpriteVertex>>();
	vertex_->Create(SxavengerSystem::GetDxDevice(), 4);
}

void Sprite::SetSize(const Vector2f& size) {
	Transform2dComponent::GetTransform2d().scale = size;
	Transform2dComponent::UpdateMatrix();
}

void Sprite::SetColor(const Color4f& color, VertexId id) {
	vertex_->At(static_cast<uint32_t>(id)).color = color;
}

void Sprite::SetColor(const Color4f& color) {
	SetColor(color, VertexId::LeftTop);
	SetColor(color, VertexId::RightTop);
	SetColor(color, VertexId::LeftBottom);
	SetColor(color, VertexId::RightBottom);
}

void Sprite::UpdateVertex() {
	float left   = 0.0f - anchor_.x;
	float right  = 1.0f - anchor_.x;
	float top    = 0.0f - anchor_.y;
	float bottom = 1.0f - anchor_.y;

	if (isFlip_.test(0)) {
		left  = -left;
		right = -right;
	}

	if (isFlip_.test(1)) {
		top    = -top;
		bottom = -bottom;
	}

	vertex_->At(static_cast<uint32_t>(VertexId::LeftTop)).position     = { left, top, 0.0f };
	vertex_->At(static_cast<uint32_t>(VertexId::RightTop)).position    = { right, top, 0.0f };
	vertex_->At(static_cast<uint32_t>(VertexId::LeftBottom)).position  = { left, bottom, 0.0f };
	vertex_->At(static_cast<uint32_t>(VertexId::RightBottom)).position = { right, bottom, 0.0f };

}

void Sprite::UpdateTexcoord() {
	float left   = 0.0f - pivot_.x;
	float right  = 1.0f - pivot_.x;
	float top    = 0.0f - pivot_.y;
	float bottom = 1.0f - pivot_.y;

	vertex_->At(static_cast<uint32_t>(VertexId::LeftTop)).texcoord     = { left, top };
	vertex_->At(static_cast<uint32_t>(VertexId::RightTop)).texcoord    = { right, top };
	vertex_->At(static_cast<uint32_t>(VertexId::LeftBottom)).texcoord  = { left, bottom };
	vertex_->At(static_cast<uint32_t>(VertexId::RightBottom)).texcoord = { right, bottom };
}
