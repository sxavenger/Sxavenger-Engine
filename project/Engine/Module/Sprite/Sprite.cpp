#include "Sprite.h"
_DXOBJECT_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/SxavengerSystem.h>
#include <Engine/Module/SxavengerModule.h>

////////////////////////////////////////////////////////////////////////////////////////
// Sprite class methods
////////////////////////////////////////////////////////////////////////////////////////

void Sprite::Init() {
	Transform2dComponent::CreateBuffer();

	ia_.Create(4, 6);
	ia_.GetIndex()->At(0) = 0; ia_.GetIndex()->At(1) = 1; ia_.GetIndex()->At(2) = 2;
	ia_.GetIndex()->At(3) = 1; ia_.GetIndex()->At(4) = 3; ia_.GetIndex()->At(5) = 2;

	SetColor({ 1.0f, 1.0f, 1.0f, 1.0f });
}

void Sprite::SetPosition(const Vector2f& position) {
	Transform2dComponent::GetTransform2d().translate = position;
	Transform2dComponent::UpdateMatrix();
}

void Sprite::SetSize(const Vector2f& size) {
	Transform2dComponent::GetTransform2d().scale = size;
	Transform2dComponent::UpdateMatrix();
}

void Sprite::SetColor(const Color4f& color, VertexId id) {
	ia_.GetVertex()->At(static_cast<uint32_t>(id)).color = color;
}

void Sprite::SetColor(const Color4f& color) {
	SetColor(color, VertexId::LeftTop);
	SetColor(color, VertexId::RightTop);
	SetColor(color, VertexId::LeftBottom);
	SetColor(color, VertexId::RightBottom);
}

void Sprite::Draw() {
	if (!handle_.has_value()) {
		return;
	}

	UpdateVertex();
	UpdateTexcoord();

	SxavengerModule::SetSpritePipeline(SxavengerSystem::GetMainThreadContext());

	ia_.BindIABuffer(SxavengerSystem::GetMainThreadContext());

	DxObject::BindBufferDesc desc = {};
	desc.SetAddress("gTransform", Transform2dComponent::GetGPUVirtualAddress());
	desc.SetHandle("gTexture", handle_.value());

	SxavengerModule::BindSpriteBuffer(SxavengerSystem::GetMainThreadContext(), desc);

	ia_.DrawCall(1, SxavengerSystem::GetMainThreadContext());
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

	ia_.GetVertex()->At(static_cast<uint32_t>(VertexId::LeftTop)).position = {left, top, 0.0f};
	ia_.GetVertex()->At(static_cast<uint32_t>(VertexId::RightTop)).position    = { right, top, 0.0f };
	ia_.GetVertex()->At(static_cast<uint32_t>(VertexId::LeftBottom)).position  = { left, bottom, 0.0f };
	ia_.GetVertex()->At(static_cast<uint32_t>(VertexId::RightBottom)).position = { right, bottom, 0.0f };

}

void Sprite::UpdateTexcoord() {
	float left   = 0.0f - pivot_.x;
	float right  = 1.0f - pivot_.x;
	float top    = 0.0f - pivot_.y;
	float bottom = 1.0f - pivot_.y;

	ia_.GetVertex()->At(static_cast<uint32_t>(VertexId::LeftTop)).texcoord     = { left, top };
	ia_.GetVertex()->At(static_cast<uint32_t>(VertexId::RightTop)).texcoord    = { right, top };
	ia_.GetVertex()->At(static_cast<uint32_t>(VertexId::LeftBottom)).texcoord  = { left, bottom };
	ia_.GetVertex()->At(static_cast<uint32_t>(VertexId::RightBottom)).texcoord = { right, bottom };
}
