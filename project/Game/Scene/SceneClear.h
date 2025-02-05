#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/Adapter/Scene/BaseScene.h>
#include <Engine/!Render/FSceneTextures.h>
#include <Engine/!Render/Canvas/FCanvas.h>
#include <Engine/!Render/Canvas/Layer/Layers/LSprite.h>

//* c++
#include <memory>

////////////////////////////////////////////////////////////////////////////////////////////
// SceneGame class
////////////////////////////////////////////////////////////////////////////////////////////
class SceneClear
	: public BaseScene {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	SceneClear() = default;
	~SceneClear() = default;

	void Init() override;

	void Update() override;

	void Draw() override;

	void Term() override;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	std::unique_ptr<FSceneTextures> textures_;
	std::unique_ptr<FCanvas>        canvas_;

	std::unique_ptr<LSprite> clear_;
};
