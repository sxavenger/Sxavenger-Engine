#include "ChessBoard.h"

////////////////////////////////////////////////////////////////////////////////////////////
// ChessBoard class methods
////////////////////////////////////////////////////////////////////////////////////////////

void ChessBoard::Init() {
	model_ = SxavengerAsset::TryImport<Model>("asset/model/chessBoard/chessBoard.gltf");
	model_.value().Lock()->SetUseMeshShader(true);
	SxavengerAsset::PushTask(model_.value().Lock());

	ModelBehavior::SetRenderingFlag(BehaviorRenderingType::kSystematic);

	collider_ = std::make_unique<Collider>();
	collider_->SetToCollection();
	collider_->SetColliderBoundingCapsule();
	collider_->SetParent(this);
}

void ChessBoard::Term() {
}

void ChessBoard::Update() {
	TransformComponent::transform_.rotate *= MakeAxisAngle({0.0f, 1.0f, 0.0f}, 0.01f);
	TransformComponent::UpdateMatrix();
	collider_->UpdateMatrix();
}

void ChessBoard::SetAttributeImGui() {
	if (ImGui::TreeNode("collider")) {
		collider_->SetImGuiCommand();
		ImGui::TreePop();
	}
}
