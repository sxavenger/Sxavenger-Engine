#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* editor
#include "BaseEditor.h"

//* engine
#include <Engine/Asset/Texture/AssetTexture.h>
#include <Engine/Asset/Observer/AssetObserver.h>
#include <Engine/Render/Scene/Actor/Camera/CameraActors/APivotCameraActor.h>
#include <Engine/Render/Scene/FSceneRenderer.h>
#include <Engine/Render/Canvas/FCanvas.h>

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

	void SetCanvas(FCanvas* canvas) { canvas_ = canvas; }

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

	//* window *//

	WindowRect rect_ = {};

	//* scene render *//

	std::unique_ptr<FSceneTextures> textures_;       //!< debug textures
	std::unique_ptr<FSceneRenderer> sceneRenderer_;  //!< debug scene
	std::unique_ptr<APivotCameraActor> sceneCamera_; //!< debug camera

	FSceneRenderer* gameRenderer_ = nullptr; //!< game scene

	AssetObserver<AssetTexture> checkerBoard_;

	//* canvas *//

	FCanvas* canvas_ = nullptr;

	//* hierarchy *//

	std::optional<AActor*> selectedActor_ = std::nullopt;

	//* paraemter *//

	ImVec4 disabled_ = { 0.4f, 0.4f, 0.4f, 1.0f };

	//=========================================================================================
	// private methods
	//=========================================================================================

	//* show imgui component *//

	void ShowSceneWindow();
	void ShowGameWindow();
	void ShowCanvasWindow();
	void ShowHierarchyWindow();
	void ShowInspectorWindow();

	//* sub method *//

	WindowRect SetImGuiImageFullWindow(const D3D12_GPU_DESCRIPTOR_HANDLE& handle, const Vector2ui& size);

	void UpdateSceneCamera();

	void Manipulate(ImGuizmo::OPERATION operation, ImGuizmo::MODE mode, TransformComponent* component) const;

	bool IsSelectedActor(AActor* actor) const;

};
