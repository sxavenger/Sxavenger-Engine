#include "Stage.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/Asset/SxavengerAsset.h>
#include <Engine/Component/Components/Transform/TransformComponent.h>
#include <Engine/Component/ComponentHelper.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Stage class methods
////////////////////////////////////////////////////////////////////////////////////////////

void Stage::Load() {
	chessboardModel_ = SxavengerAsset::TryImport<AssetModel>("assets/models/chessboard/ABeautifulGame.gltf");
	stageModel_      = SxavengerAsset::TryImport<AssetModel>("assets/models/scene/scene.gltf");
}

void Stage::Awake() {
	//stage_ = std::make_unique<MonoBehaviour>();
	//stage_->SetName("stage");
	//stage_->AddComponent<TransformComponent>();
	//stageModel_.WaitGet()->CreateStaticMeshBehaviour(stage_.get());
	//GameObject::AddChild(stage_.get());

	chessboard_ = std::make_unique<MonoBehaviour>();
	chessboard_->SetName("chessboard");
	chessboard_->AddComponent<TransformComponent>()->scale = { 48, 48, 48 };

	chessboardModel_.WaitGet()->CreateStaticNodeMeshBehaviour(chessboard_.get());
	GameObject::AddChild(chessboard_.get());
}

void Stage::Start() {

}
