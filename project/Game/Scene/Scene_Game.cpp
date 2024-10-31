#include "Scene_Game.h"

////////////////////////////////////////////////////////////////////////////////////////////
// Scene_Game class methods
////////////////////////////////////////////////////////////////////////////////////////////

void Scene_Game::Init() {

	/*kipfel_ = std::make_unique<Kipfel>();
	kipfel_->Init();
	kipfel_->SetToConsole();*/

	chessBorad_ = std::make_unique<ChessBoard>();
	chessBorad_->Init();
	chessBorad_->SetToConsole();

}

void Scene_Game::Term() {
}

void Scene_Game::Update() {
}

void Scene_Game::Draw() {
}
