#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/Runtime/GameLoop/GameLoop.h>
#include <Engine/System/DirectX/Context/DirectXWindowContext.h>
#include <Engine/Adapter/Actor/AtmosphereActor.h>

//* demo
#include <Demo/Entity/Player.h>

////////////////////////////////////////////////////////////////////////////////////////////
// DemoGameLoop class
////////////////////////////////////////////////////////////////////////////////////////////
class DemoGameLoop
	: public GameLoop::Interface {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	void Init(GameLoop::Context* context) override;

	void Term() override;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	std::shared_ptr<DirectXWindowContext> main_;

	//* game objects *//

	std::unique_ptr<Player> player_;

	std::unique_ptr<MonoBehaviour> skylight_;

	std::unique_ptr<MonoBehaviour> volume_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	//* game logic *//

	void InitGame();

	void TermGame();

	void UpdateGame();

	void DrawGame();

	//* sub methods *//

	void SetCollisionCallback();

};
