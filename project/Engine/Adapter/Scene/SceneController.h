#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* scene
#include "BaseScene.h"
#include "BaseSceneFactory.h"

////////////////////////////////////////////////////////////////////////////////////////////
// SceneController class
////////////////////////////////////////////////////////////////////////////////////////////
class SceneController {
public:

	//=========================================================================================
	// public method
	//=========================================================================================

	SceneController()  = default;
	~SceneController() = default;

	void Init(const std::string& startSceneKey);

	void Term();

	//* scene factory option *//

	void RequestNextScene(const std::string& key);

	void SetSceneFactory(std::unique_ptr<const BaseSceneFactory>&& factory) { factory_ = std::move(factory); }

	void ActivateNextScene();

	//* scene option *//

	void InitScene();

	void UpdateScene();

	void DrawScene();

	void TermScene();

	//* getter *//

	BaseScene* GetScene() const { return scene_.get(); }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	std::unique_ptr<BaseScene> scene_;
	std::unique_ptr<BaseScene> nextScene_ = nullptr;

	std::unique_ptr<const BaseSceneFactory> factory_;

	//=========================================================================================
	// private methods
	//=========================================================================================

};
