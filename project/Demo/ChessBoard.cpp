#include "ChessBoard.h"

////////////////////////////////////////////////////////////////////////////////////////////
// ChessBoard class methods
////////////////////////////////////////////////////////////////////////////////////////////

void ChessBoard::Init() {
	//model_ = SxavengerAsset::ImportModel("asset/model/chessBoard/chessBoard.gltf");
	model_ = SxavengerAsset::ImportModel("asset/model/demo/teapot.obj");
	model_->AsyncLoad(Model::GetDefaultAssimpOption() | aiProcess_Triangulate);
	SxavengerSystem::PushTask(model_);

	//model_->AsyncLoad("resources/model/demo/teapot.obj", Model::GetDefaultAssimpOption() | aiProcess_Triangulate);
	//model_->AsyncLoad("resources/model/chessBoard/chessBoard.gltf", Model::GetDefaultAssimpOption() | aiProcess_Triangulate);

	ModelBehavior::model_ = model_.get();
	ModelBehavior::renderingFlag_ = kBehaviorRendering_Systematic;
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
