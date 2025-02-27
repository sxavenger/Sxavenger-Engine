#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/Render/Canvas/FCanvas.h>
#include <Engine/Render/Scene/FScene.h>
#include <Engine/Render/Scene/FSceneRenderer.h>

//* c++
#include <concepts>

//-----------------------------------------------------------------------------------------
// forward
//-----------------------------------------------------------------------------------------
class SceneController;

////////////////////////////////////////////////////////////////////////////////////////////
// Interface Scene class
////////////////////////////////////////////////////////////////////////////////////////////
class BaseScene {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// SceneComponent structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct SceneComponent {

		//* member *//

		std::unique_ptr<FScene>  scene_                   = nullptr;
		std::unique_ptr<FAmbientProcessSetting> ambient_  = nullptr;
		std::unique_ptr<FPostProcessSetting> postProcess_ = nullptr;
		std::unique_ptr<FCanvas> canvas_                  = nullptr;

		//* method *//

		SceneComponent() {
			scene_       = std::make_unique<FScene>();
			ambient_     = std::make_unique<FAmbientProcessSetting>();
			postProcess_ = std::make_unique<FPostProcessSetting>();
			canvas_      = std::make_unique<FCanvas>();
		}
	};

public:

	//=========================================================================================
	// public method
	//=========================================================================================

	virtual ~BaseScene() = default;

	virtual void Init() = 0;

	virtual void Update() = 0;

	virtual void Draw() = 0;

	virtual void Term() = 0;

	void SetController(SceneController* controller) { controller_ = controller; }

	const SceneComponent& GetComponent() const { return component_; }

protected:

	//=========================================================================================
	// protected variables
	//=========================================================================================

	SceneController* controller_ = nullptr;

	SceneComponent component_;

};

////////////////////////////////////////////////////////////////////////////////////////////
// concept
////////////////////////////////////////////////////////////////////////////////////////////
template <class T>
concept DerivedFromScene = std::derived_from<T, BaseScene>;
