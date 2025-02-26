//[DemoGameLoop.h]
#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/Runtime/GameLoop/GameLoop.h>
#include <Engine/System/Window/GameWindow.h>
#include <Engine/Render/FPresenter.h>
#include <Engine/Render/FSceneTextures.h>
#include <Engine/Render/Scene/FSceneRenderer.h>
#include <Engine/Render/Scene/FScene.h>
#include <Engine/Render/Scene/Actor/Camera/CameraActors/ACineCameraActor.h>
#include <Engine/Render/Scene/Actor/Geometry/GeometryActors/AModelActor.h>
#include <Engine/Render/Scene/Actor/Light/LightActors/ADirectionalLightActor.h>

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

	std::shared_ptr<GameWindow> main_;

	//* render関係 *//

	std::unique_ptr<FSceneTextures> textures_; //!< テクスチャを管理するクラス
	FPresenter presenter_;                     //!< windowに描画するクラス

	std::unique_ptr<FSceneRenderer> renderer_; //!< sceneを描画するクラス
	std::unique_ptr<FScene> scene_;            //!< 描画するためのactorを管理するクラス

	//* actor関係 *//

	std::unique_ptr<ACineCameraActor> camera_; //!< カメラ

	std::unique_ptr<AModelActor> model_; //!< モデル

	std::unique_ptr<ADirectionalLightActor> light_; //!< ライト

	//=========================================================================================
	// private methods
	//=========================================================================================

	void InitGame();

	void TermGame();

	void UpdateGame();

	void DrawGame();

};
