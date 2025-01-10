//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* base
#include <Engine/System/Runtime/GameLoop/GameLoop.h>

//* engine
#include <Engine/System/Window/GameWindow.h>
#include <Engine/Asset/AssetCollection.h>

//* test
#include <Engine/!Render/FScene.h>
#include <Engine/!Render/FSceneRenderer.h>
#include <Engine/!Render/Actor/Camera/CameraActor/ACineCameraActor.h>
#include <Engine/!Render/Actor/Geometry/GeometryActors/AModelActor.h>

////////////////////////////////////////////////////////////////////////////////////////////
// BetaSystemGameLoop class
////////////////////////////////////////////////////////////////////////////////////////////
class BetaSystemGameLoop
	: public GameLoop::Interface {
public:

	//=========================================================================================
	// public method
	//=========================================================================================

	void Init(GameLoop::Context* context) override;

	void Term() override;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	std::shared_ptr<GameWindow> main_;

	//* render system *//

	std::unique_ptr<FScene> scene_;
	std::unique_ptr<FSceneRenderer> renderer_;

	std::unique_ptr<ACineCameraActor> camera_;

	std::unique_ptr<AModelActor> model_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	void InitSystem();

	void TermSystem();

	void UpdateSystem();

	void DrawSystem();

};
