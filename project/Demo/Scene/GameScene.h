#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* scene
#include <Engine/Adapter/Scene/BaseScene.h>

//* engine
#include <Engine/Adapter/Actor/PerformanceActor.h>

//* demo
#include <Demo/Object/PerspectiveCamera.h>
#include <Demo/Entity/Player.h>
#include <Demo/Object/CollectibleItems.h>

////////////////////////////////////////////////////////////////////////////////////////////
// GameScene class
////////////////////////////////////////////////////////////////////////////////////////////
class GameScene
	: public BaseScene {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	void Init() override;

	void Start() override;

	void Update() override;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* game objects *//

	std::unique_ptr<MonoBehaviour> skylight_;

	std::unique_ptr<PerformanceActor> performance_;

	std::unique_ptr<PerspectiveCamera> camera_;
	std::unique_ptr<Player> player_;

	std::unique_ptr<CollectibleItems> items_;

	std::unique_ptr<MonoBehaviour> demoText_;
	std::unique_ptr<MonoBehaviour> text_;

	//=========================================================================================
	// public methods
	//=========================================================================================

	void SetCollisionCallback();

};
