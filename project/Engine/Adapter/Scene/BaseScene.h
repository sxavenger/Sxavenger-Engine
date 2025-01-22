#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/DirectX/DirectXContext.h>
#include <Engine/!Render/Canvas/FCanvas.h>
#include <Engine/!Render/Scene/FScene.h>
#include <Engine/!Render/Scene/FSceneRenderer.h>

//* c++
#include <concepts>
#include <string>

//-----------------------------------------------------------------------------------------
// forward
//-----------------------------------------------------------------------------------------
class SceneController;

////////////////////////////////////////////////////////////////////////////////////////////
// Base Scene class
////////////////////////////////////////////////////////////////////////////////////////////
class BaseScene {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// SceneComponent structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct SceneComponent {

		//* member *//

		std::unique_ptr<FScene>         scene_    = nullptr;
		std::unique_ptr<FSceneRenderer> renderer_ = nullptr;
		std::unique_ptr<FCanvas>        canvas_   = nullptr;

		//* method *//

		void Init() {
			scene_    = std::make_unique<FScene>();
			renderer_ = std::make_unique<FSceneRenderer>();
			canvas_   = std::make_unique<FCanvas>();

			renderer_->SetScene(scene_.get());
		}

		void Render(const DirectXThreadContext* context, FSceneTextures* textures) const {
			renderer_->SetTextures(textures);
			renderer_->Render(context);

			canvas_->SetTextures(textures);
			canvas_->Render(context);
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

	void RequestNextScene(const std::string& name);

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
