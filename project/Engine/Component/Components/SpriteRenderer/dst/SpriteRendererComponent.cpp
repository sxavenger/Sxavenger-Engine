#include "SpriteRendererComponent.h"

////////////////////////////////////////////////////////////////////////////////////////////
// Transformation2d structure methods
////////////////////////////////////////////////////////////////////////////////////////////

void SpriteRendererComponent::Transformation2d::Init() {
	mat = Matrix4x4::Identity();
}

void SpriteRendererComponent::Transformation2d::Transfer(const Matrix4x4& _mat) {
	mat = _mat;
}

////////////////////////////////////////////////////////////////////////////////////////////
// GetDescriptorVisitor structure
////////////////////////////////////////////////////////////////////////////////////////////

const D3D12_GPU_DESCRIPTOR_HANDLE& SpriteRendererComponent::GetDescriptorVisitor::operator()(const D3D12_GPU_DESCRIPTOR_HANDLE& handle) {
	return handle;
}

const D3D12_GPU_DESCRIPTOR_HANDLE& SpriteRendererComponent::GetDescriptorVisitor::operator()(const UAssetParameter<UAssetTexture>& parameter) {
	return parameter.WaitGet()->GetGPUHandleSRV();
}

////////////////////////////////////////////////////////////////////////////////////////////
// SpriteRendererComponent class methods
////////////////////////////////////////////////////////////////////////////////////////////

void SpriteRendererComponent::Create() {
	CreateInputAssembler();

	for (auto& color : colors_) {
		color = kWhite4<>;
	}

	buffer_ = std::make_unique<DxObject::DimensionBuffer<Transformation2d>>();
	buffer_->Create(SxavengerSystem::GetDxDevice(), 1);

	bufferUV_ = std::make_unique<DxObject::DimensionBuffer<Transformation2d>>();
	bufferUV_->Create(SxavengerSystem::GetDxDevice(), 1);
}

void SpriteRendererComponent::ShowComponentInspector() {
	if (ImGui::TreeNode("transform2d")) {
		transform2d_.SetImGuiCommand(1.0f, 1.0f);
		ImGui::TreePop();
	}

	if (ImGui::TreeNode("transformUV")) {
		transformUV_.SetImGuiCommand();
		ImGui::TreePop();
	}

	ImGui::Text("texture");

	if (texture_.has_value()) {
		ImGui::Text("texture is set.");

	} else {
		ImGui::Text("texture is not set.");
	}

	ImGui::DragFloat2("anchor", &anchor_.x, 0.01f);
	ImGui::DragFloat2("pivot",  &pivot_.x, 0.01f);

	ImGui::DragScalar("priority", ImGuiDataType_U16, &priority_, 1.0f);

	// todo: flip

	if (ImGui::ColorEdit4("color", &colors_[0].r)) {
		SetColor(colors_[0]);
	}

	if (ImGui::TreeNode("vertex colors")) {
		ImGui::ColorEdit4("left top",     &colors_[static_cast<uint8_t>(VertexPoint::LeftTop)].r);
		ImGui::ColorEdit4("right top",    &colors_[static_cast<uint8_t>(VertexPoint::RightTop)].r);
		ImGui::ColorEdit4("left bottom",  &colors_[static_cast<uint8_t>(VertexPoint::LeftBottom)].r);
		ImGui::ColorEdit4("right bottom", &colors_[static_cast<uint8_t>(VertexPoint::RightBottom)].r);
		ImGui::TreePop();
	}
}

void SpriteRendererComponent::SetPosition(const Vector2f& position) {
	transform2d_.translate = position;
}

void SpriteRendererComponent::SetRotate(float rotate) {
	transform2d_.rotate = rotate;
}

void SpriteRendererComponent::SetSize(const Vector2f& size) {
	transform2d_.scale = size;
}

void SpriteRendererComponent::SetVertexColor(const Color4f& color, VertexPoint point) {
	colors_[static_cast<uint8_t>(point)] = color;
}

void SpriteRendererComponent::SetColor(const Color4f& color) {
	for (uint8_t i = 0; i < kVertexPointCount; ++i) {
		SetVertexColor(color, static_cast<VertexPoint>(i));
	}
}

void SpriteRendererComponent::BindAIBuffer(const DirectXQueueContext* context) {
	TransferPosition();
	TransferColor();
	TransferTexcoord();
	TransferMatrix();

	ia_.BindIABuffer(context);
}

void SpriteRendererComponent::DrawCall(const DirectXQueueContext* context) {
	ia_.DrawCall(context);
}

const D3D12_GPU_DESCRIPTOR_HANDLE& SpriteRendererComponent::GetTexture() const {
	Exception::Assert(texture_.has_value(), "texture is not set.");
	static GetDescriptorVisitor visitor;
	return std::visit(visitor, texture_.value());
}

void SpriteRendererComponent::CreateInputAssembler() {
	ia_.Create(4, 2);
	ia_.GetIndex()->At(0) = { 0, 1, 2 };
	ia_.GetIndex()->At(1) = { 2, 1, 3 };
}

void SpriteRendererComponent::TransferPosition() {
	Vector2f leftTop     = Vector2f{ 0.0f, 0.0f } - anchor_;
	Vector2f rightBottom = Vector2f{ 1.0f, 1.0f } - anchor_;

	if (isFlip2d_.test(0)) {
		std::swap(leftTop.x, rightBottom.x);
	}

	if (isFlip2d_.test(1)) {
		std::swap(leftTop.y, rightBottom.y);
	}

	float priorityDepth = static_cast<float>(priority_) / UINT16_MAX;

	ia_.GetVertex()->At(static_cast<uint8_t>(VertexPoint::LeftTop)).position     = { leftTop.x, leftTop.y, priorityDepth };
	ia_.GetVertex()->At(static_cast<uint8_t>(VertexPoint::RightTop)).position    = { rightBottom.x, leftTop.y, priorityDepth };
	ia_.GetVertex()->At(static_cast<uint8_t>(VertexPoint::LeftBottom)).position  = { leftTop.x, rightBottom.y, priorityDepth };
	ia_.GetVertex()->At(static_cast<uint8_t>(VertexPoint::RightBottom)).position = { rightBottom.x, rightBottom.y, priorityDepth };

	// todo: depth
}

void SpriteRendererComponent::TransferColor() {
	ia_.GetVertex()->At(static_cast<uint8_t>(VertexPoint::LeftTop)).color     = colors_[static_cast<uint8_t>(VertexPoint::LeftTop)];
	ia_.GetVertex()->At(static_cast<uint8_t>(VertexPoint::RightTop)).color    = colors_[static_cast<uint8_t>(VertexPoint::RightTop)];
	ia_.GetVertex()->At(static_cast<uint8_t>(VertexPoint::LeftBottom)).color  = colors_[static_cast<uint8_t>(VertexPoint::LeftBottom)];
	ia_.GetVertex()->At(static_cast<uint8_t>(VertexPoint::RightBottom)).color = colors_[static_cast<uint8_t>(VertexPoint::RightBottom)];
}

void SpriteRendererComponent::TransferTexcoord() {
	Vector2f leftTop     = Vector2f{ 0.0f, 0.0f } - pivot_;
	Vector2f rightBottom = Vector2f{ 1.0f, 1.0f } - pivot_;

	if (isFlip2d_.test(0)) {
		std::swap(leftTop.x, rightBottom.x);
	}

	if (isFlip2d_.test(1)) {
		std::swap(leftTop.y, rightBottom.y);
	}

	ia_.GetVertex()->At(static_cast<uint8_t>(VertexPoint::LeftTop)).texcoord     = { leftTop.x, leftTop.y };
	ia_.GetVertex()->At(static_cast<uint8_t>(VertexPoint::RightTop)).texcoord    = { rightBottom.x, leftTop.y };
	ia_.GetVertex()->At(static_cast<uint8_t>(VertexPoint::LeftBottom)).texcoord  = { leftTop.x, rightBottom.y };
	ia_.GetVertex()->At(static_cast<uint8_t>(VertexPoint::RightBottom)).texcoord = { rightBottom.x, rightBottom.y };
}

void SpriteRendererComponent::TransferMatrix() {
	buffer_->At(0).Transfer(transform2d_.ToMatrix());
	bufferUV_->At(0).Transfer(transformUV_.ToMatrix());
}
