//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* base
#include <Engine/System/Runtime/GameLoop/GameLoop.h>

//* engine
#include <Engine/System/Window/GameWindow.h>
#include <Engine/Asset/AssetCollection.h>

//* test !render
#include <Engine/!Render/Scene/FScene.h>
#include <Engine/!Render/Scene/FSceneRenderer.h>
#include <Engine/!Render/Scene/Actor/Camera/CameraActors/ACineCameraActor.h>
#include <Engine/!Render/Scene/Actor/Camera/CameraActors/APivotCameraActor.h>
#include <Engine/!Render/Scene/Actor/Geometry/GeometryActors/AModelActor.h>
#include <Engine/!Render/Scene/Actor/Geometry/GeometryActors/AModelAnimationActor.h>
#include <Engine/!Render/Scene/Actor/Light/LightActors/APointLightActor.h>
#include <Engine/!Render/Scene/Actor/Light/LightActors/ADirectionalLightActor.h>
#include <Engine/!Render/Scene/Actor/Light/LightActors/ASpotLightActor.h>
#include <Engine/!Render/Canvas/FCanvas.h>
#include <Engine/!Render/Canvas/Layer/Layers/LSprite.h>
#include <Engine/!Render/FPresenter.h>

#include <Engine/Editor/EditorComponent/AttributeComponent.h>

#include <Engine/Module/Collider/Collider.h>

#include "Demo/Actor/AFloorActor.h"

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

	std::unique_ptr<FSceneTextures> textures_;

	std::unique_ptr<FScene> scene_;
	std::unique_ptr<FSceneRenderer> renderer_;

	std::unique_ptr<ACineCameraActor> camera_;

	std::unique_ptr<AModelActor> model_;
	std::unique_ptr<AFloorActor> floor_;

	std::unique_ptr<AModelAnimationActor> animation_;
	AssetObserver<AssetAnimator> animator_;
	TimePointf<TimeUnit::second> time_;

	std::unique_ptr<APointLightActor> light1_;
	std::unique_ptr<APointLightActor> light2_;
	std::unique_ptr<ADirectionalLightActor> light3_;
	std::unique_ptr<ASpotLightActor> light4_;

	std::unique_ptr<FCanvas> canvas_;
	std::unique_ptr<LSprite> sprite_;

	std::unique_ptr<AttributeComponent> attribute_;

	std::unique_ptr<Collider> collider_;

	//* presenter *//

	FPresenter presenter_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	void InitSystem();

	void TermSystem();

	void UpdateSystem();

	void DrawSystem();

};
