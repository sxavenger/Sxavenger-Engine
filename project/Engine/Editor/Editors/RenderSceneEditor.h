#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* editor
#include "BaseEditor.h"

//* engine
#include <Engine/Asset/Texture/AssetTexture.h>
#include <Engine/Asset/Observer/AssetObserver.h>
#include <Engine/!Render/Actor/Camera/CameraActors/APivotCameraActor.h>
#include <Engine/!Render/FSceneRenderer.h>

//* external
#include <imgui.h>
#include <ImGuizmo.h>

//* c++
#include <optional>
#include <memory>

////////////////////////////////////////////////////////////////////////////////////////////
// RenderSceneEditor class
////////////////////////////////////////////////////////////////////////////////////////////
class RenderSceneEditor
	: public BaseEditor {
public:

	//=========================================================================================
	// public method
	//=========================================================================================

	RenderSceneEditor(EditorEngine* editor) : BaseEditor(editor) {}
	~RenderSceneEditor() = default;

	void Init() override;

	void ShowMainMenu() override;

	void ShowWindow() override;

	void Draw();

	//* setter *//

	void SetScene(FScene* scene) { sceneRenderer_->SetScene(scene); }

	void SetGameRenderer(FSceneRenderer* renderer);

private:

	////////////////////////////////////////////////////////////////////////////////////////////
	// WindowRect structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct WindowRect {
		Vector2f pos;
		Vector2f size;
	};

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* scene render *//

	std::unique_ptr<APivotCameraActor> sceneCamera_; //!< debug camera
	std::unique_ptr<FSceneRenderer> sceneRenderer_;  //!< debug scene

	FSceneRenderer* gameRenderer_ = nullptr; //!< game scene

	AssetObserver<AssetTexture> checkerBoard_;

	WindowRect rect_ = {};

	//* outliner *//

	std::optional<AActor*> selectedActor_ = std::nullopt;

	//=========================================================================================
	// private methods
	//=========================================================================================

	//* show imgui component *//

	void ShowSceneWindow();
	void ShowGameWindow();
	void ShowHierarchyWindow();

	//* sub method *//

	WindowRect SetImGuiImageFullWindow(const D3D12_GPU_DESCRIPTOR_HANDLE& handle, const Vector2ui& size);

	void UpdateSceneCamera();

	void Manipulate(ImGuizmo::OPERATION operation, ImGuizmo::MODE mode, TransformComponent* component) const;

	bool IsSelectedActor(AActor* actor) const;

};
