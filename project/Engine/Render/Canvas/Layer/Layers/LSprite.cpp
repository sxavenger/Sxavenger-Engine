#include "LSprite.h"
_DXOBJECT_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* render
#include <Engine/Render/FRenderCore.h>

////////////////////////////////////////////////////////////////////////////////////////////
// LSprite class methods
////////////////////////////////////////////////////////////////////////////////////////////

void LSprite::Init() {
	ia_.Create(4, 2);
	ia_.GetIndex()->At(0) = { 0, 1, 2 };
	ia_.GetIndex()->At(1) = { 2, 1, 3 };

	Transform2dComponent::CreateBuffer();
	TextureComponent::CreateBuffer();

	TransferColor({ 1.0f, 1.0f, 1.0f, 1.0f }); 
}

void LSprite::Render(const RenderContext& context) {
	if (!texture_.has_value()) {
		return;
	}

	UpdatePosition();
	UpdateTexcoord();

	FRenderCore::GetInstance()->GetLayer()->SetPipeline(
		FRenderCoreLayer::PipelineType::Sprite, context.context, context.size
	);

	ia_.BindIABuffer(context.context);

	BindBufferDesc parameter = context.parameter;
	parameter.SetAddress("gTransform",        Transform2dComponent::GetGPUVirtualAddress());
	parameter.SetAddress("gTextureComponent", TextureComponent::GetGPUVirtualAddress());

	if (std::holds_alternative<D3D12_GPU_DESCRIPTOR_HANDLE>(texture_.value())) {
		parameter.SetHandle("gTexture", std::get<D3D12_GPU_DESCRIPTOR_HANDLE>(texture_.value()));

	} else {
		auto& observer = std::get<AssetObserver<AssetTexture>>(texture_.value());
		auto texture = observer.WaitGet();
		parameter.SetHandle("gTexture", texture->GetGPUHandleSRV());
	}

	FRenderCore::GetInstance()->GetLayer()->BindGraphicsBuffer(
		FRenderCoreLayer::PipelineType::Sprite, context.context, parameter
	);

	ia_.DrawCall(context.context);
}

void LSprite::TransferColor(const Color4f& color, VertexPoint point) {
	ia_.GetVertex()->At(static_cast<uint32_t>(point)).color = color;
}

void LSprite::TransferColor(const Color4f& color) {
	TransferColor(color, VertexPoint::LeftTop);
	TransferColor(color, VertexPoint::RightTop);
	TransferColor(color, VertexPoint::LeftBottom);
	TransferColor(color, VertexPoint::RightBottom);
}

void LSprite::SetPosition(const Vector2f& position) {
	Transform2dComponent::GetTransform2d().translate = position;
	Transform2dComponent::UpdateMatrix();
}

void LSprite::SetSize(const Vector2f& size) {
	Transform2dComponent::GetTransform2d().scale = size;
	Transform2dComponent::UpdateMatrix();
}

void LSprite::UpdatePosition() {
	float left   = 0.0f - anchor_.x;
	float top    = 0.0f - anchor_.y;
	float right  = 1.0f - anchor_.x;
	float bottom = 1.0f - anchor_.y;

	if (isFlip_.test(0)) {
		left  = -left;
		right = -right;
	}

	if (isFlip_.test(1)) {
		top    = -top;
		bottom = -bottom;
	}

	ia_.GetVertex()->At(static_cast<uint32_t>(VertexPoint::LeftTop)).position     = { left, top, depth_ };
	ia_.GetVertex()->At(static_cast<uint32_t>(VertexPoint::RightTop)).position    = { right, top, depth_ };
	ia_.GetVertex()->At(static_cast<uint32_t>(VertexPoint::LeftBottom)).position  = { left, bottom, depth_ };
	ia_.GetVertex()->At(static_cast<uint32_t>(VertexPoint::RightBottom)).position = { right, bottom, depth_ };
}

void LSprite::UpdateTexcoord() {
	float left   = 0.0f - pivot_.x;
	float right  = 1.0f - pivot_.x;
	float top    = 0.0f - pivot_.y;
	float bottom = 1.0f - pivot_.y;

	ia_.GetVertex()->At(static_cast<uint32_t>(VertexPoint::LeftTop)).texcoord     = { left, top };
	ia_.GetVertex()->At(static_cast<uint32_t>(VertexPoint::RightTop)).texcoord    = { right, top };
	ia_.GetVertex()->At(static_cast<uint32_t>(VertexPoint::LeftBottom)).texcoord  = { left, bottom };
	ia_.GetVertex()->At(static_cast<uint32_t>(VertexPoint::RightBottom)).texcoord = { right, bottom };
}


