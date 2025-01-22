#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/Runtime/Scene/BaseScene.h>
#include <Engine/!Render/FSceneTextures.h>
#include <Engine/!Render/Canvas/FCanvas.h>
#include <Engine/!Render/Canvas/Layer/Layers/LSprite.h>

//* c++
#include <memory>

////////////////////////////////////////////////////////////////////////////////////////////
// SceneGame class
////////////////////////////////////////////////////////////////////////////////////////////
class SceneTitle
	: public BaseScene {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	SceneTitle() = default;
	~SceneTitle() = default;

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

	std::unique_ptr<LSprite> title_;
};
