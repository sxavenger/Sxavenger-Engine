#include "ChessBoard.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/Game/SxavengerGame.h>

////////////////////////////////////////////////////////////////////////////////////////////
// ChessBoard class methods
////////////////////////////////////////////////////////////////////////////////////////////

void ChessBoard::Init() {
	SetName("chess board");

	ModelBehavior::model_ = SxavengerGame::LoadModel("resources/model/ChessBoard", "chessBoard.gltf");
	model_->ApplyRaytracing();
	CreateRaytracingRecorder();

	transform_.transform.scale = { 16.0f, 16.0f, 16.0f };
	transform_.UpdateMatrix();

	renderingFlag_ = kBehaviorRender_Raytracing;
}

void ChessBoard::SetAttributeImGui() {
}
