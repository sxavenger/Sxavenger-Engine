#include "SpriteRendererComponent.h"
_DXOBJECT_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* component
#include "../../Entity/MonoBehaviour.h"

////////////////////////////////////////////////////////////////////////////////////////////
// SpriteRendererComponent class methods
////////////////////////////////////////////////////////////////////////////////////////////

void SpriteRendererComponent::CreateBuffer() {
	input_.Create(4, 2);
	input_.GetIndex()->At(0) = { 0, 1, 2 };
	input_.GetIndex()->At(1) = { 2, 1, 3 };

	// 頂点の初期化
	input_.GetVertex()->Fill(UIVertexData{ .color = kWhite4<float> });

	bufferUV_ = std::make_unique<DimensionBuffer<Matrix4x4>>();
	bufferUV_->Create(SxavengerSystem::GetDxDevice(), 1);
	bufferUV_->At(0) = Matrix4x4::Identity();
}

void SpriteRendererComponent::ShowComponentInspector() {
}

void SpriteRendererComponent::SetVertexColor(const Color4f& color, VertexPoint point) {
	input_.GetVertex()->At(static_cast<size_t>(point)).color = color;
}

void SpriteRendererComponent::SetColor(const Color4f& color) {
	for (uint8_t i = 0; i < magic_enum::enum_count<VertexPoint>(); ++i) {
		SetVertexColor(color, static_cast<VertexPoint>(i));
	}
}

void SpriteRendererComponent::BindAIBuffer(const DirectXQueueContext* context) {

	// 頂点の更新
	TransferPosition();
	TransferTexcoord();

	// uv変換行列を更新
	bufferUV_->At(0) = transformUV_.ToMatrix();
	
	input_.BindIABuffer(context);
}

void SpriteRendererComponent::DrawCall(const DirectXQueueContext* context) {
	input_.DrawCall(context);
}

const RectTransformComponent* SpriteRendererComponent::GetRectTransform() const {
	return BaseComponent::GetBehaviour()->RequireComponent<RectTransformComponent>();
}

void SpriteRendererComponent::TransferPosition() {

	Vector2f leftTop     = Vector2f{ 0.0f, 0.0f };
	Vector2f rightBottom = Vector2f{ 1.0f, 1.0f };

	if (isFlip2d_.test(0)) {
		std::swap(leftTop.x, rightBottom.x);
	}

	if (isFlip2d_.test(1)) {
		std::swap(leftTop.y, rightBottom.y);
	}

	auto vertices = input_.GetVertex();

	vertices->At(static_cast<uint8_t>(VertexPoint::LeftTop)).position     = { leftTop.x, leftTop.y, 0.0f };
	vertices->At(static_cast<uint8_t>(VertexPoint::RightTop)).position    = { rightBottom.x, leftTop.y, 0.0f };
	vertices->At(static_cast<uint8_t>(VertexPoint::LeftBottom)).position  = { leftTop.x, rightBottom.y, 0.0f };
	vertices->At(static_cast<uint8_t>(VertexPoint::RightBottom)).position = { rightBottom.x, rightBottom.y, 0.0f };
}

void SpriteRendererComponent::TransferTexcoord() {
	Vector2f leftTop     = Vector2f{ 0.0f, 0.0f };
	Vector2f rightBottom = Vector2f{ 1.0f, 1.0f };

	if (isFlip2d_.test(0)) {
		std::swap(leftTop.x, rightBottom.x);
	}

	if (isFlip2d_.test(1)) {
		std::swap(leftTop.y, rightBottom.y);
	}

	auto vertices = input_.GetVertex();

	vertices->At(static_cast<uint8_t>(VertexPoint::LeftTop)).texcoord     = { leftTop.x, leftTop.y };
	vertices->At(static_cast<uint8_t>(VertexPoint::RightTop)).texcoord    = { rightBottom.x, leftTop.y };
	vertices->At(static_cast<uint8_t>(VertexPoint::LeftBottom)).texcoord  = { leftTop.x, rightBottom.y };
	vertices->At(static_cast<uint8_t>(VertexPoint::RightBottom)).texcoord = { rightBottom.x, rightBottom.y };
}
