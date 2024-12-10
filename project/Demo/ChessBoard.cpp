#include "ChessBoard.h"

////////////////////////////////////////////////////////////////////////////////////////////
// ChessBoard class methods
////////////////////////////////////////////////////////////////////////////////////////////

void ChessBoard::Init() {
	model_ = SxavengerAsset::ImportModel("asset/model/chessBoard/chessBoard.gltf");
	//model_ = SxavengerAsset::ImportModel("asset/model/demo/teapot.obj");
	model_->AsyncLoad(Model::GetDefaultAssimpOption());
	SxavengerSystem::PushTask(model_);

	ModelBehavior::model_ = model_.get();
	ModelBehavior::SetRenderingFlag(BehaviorRenderingType::kSystematic);
}

void ChessBoard::Term() {
}

void ChessBoard::Update() {
	TransformComponent::transform_.rotate *= MakeAxisAngle({0.0f, 1.0f, 0.0f}, 0.01f);
	TransformComponent::UpdateMatrix();
}

void ChessBoard::DrawSystematic(_MAYBE_UNUSED const SxavGraphicsFrame* frame) {
	if (model_->IsCompleted()) {
		ModelBehavior::DrawSystematic(frame);
	}
}
