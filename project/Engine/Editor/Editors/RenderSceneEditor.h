#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* editor
#include "BaseEditor.h"

//* engine
#include <Engine/Component/Entity/MonoBehaviour.h>
#include <Engine/Component/Components/Collider/ColliderPrimitiveRenderer.h>
#include <Engine/Asset/Assets/Texture/AssetTexture.h>
#include <Engine/Asset/Observer/AssetObserver.h>
#include <Engine/Render/FRenderTargetTextures.h>
#include <Engine/Render/Scene/FSceneRenderer.h>

//* lib
#include <Lib/Geometry/Vector2.h>
#include <Lib/Geometry/Vector3.h>

//* external
#include <imgui.h>
#include <Engine/System/UI/SxImGuizmo.h>

//* c++
#include <memory>

////////////////////////////////////////////////////////////////////////////////////////////
// RenderSceneEditor class
////////////////////////////////////////////////////////////////////////////////////////////
class RenderSceneEditor
	: public BaseEditor {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	RenderSceneEditor(EditorEngine* editor) : BaseEditor(editor) {}
	~RenderSceneEditor() = default;

	void Init() override;

	void ShowMainMenu() override;

	void ShowWindow() override;

	//* editor option *//

	void Render();

	//* manipulator *//

	void Manipulate(MonoBehaviour* behaviour);

	void ManipulateCanvas(MonoBehaviour* behaviour);

private:

	////////////////////////////////////////////////////////////////////////////////////////////
	// WindowRect structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct WindowRect {
		Vector2f pos;
		Vector2f size;
	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// GuizmoEnable enum class
	////////////////////////////////////////////////////////////////////////////////////////////
	enum class GuizmoUsed : uint32_t {
		Scene,
		Canvas,
	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// GuizmoOperation enum class
	////////////////////////////////////////////////////////////////////////////////////////////
	enum class GuizmoOperation : uint32_t {
		Scale,
		Translate,
		Rotate,
	};

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* rect *//

	WindowRect sceneRect_  = {};
	WindowRect canvasRect_ = {};

	//* texture *//

	AssetObserver<AssetTexture> checkerboard_; //!< checker board texture

	//* renderer *//

	std::unique_ptr<FRenderTargetTextures> textures_; //!< debug textures
	std::unique_ptr<FSceneRenderer>        renderer_; //!< scene renderer
	std::unique_ptr<MonoBehaviour>         camera_;   //!< scene camera

	FSceneRenderer::Config config_ = {};

	//* parameter *//

	FRenderTargetTextures::GBufferLayout layout_ = FRenderTargetTextures::GBufferLayout::Main;

	//* window *//

	std::shared_ptr<GameWindow> window_;
	// todo: screen window の描画

	//* camera *//

	Vector3f point_ = kOrigin3<float>;
	Vector2f angle_ = { 0.0f, kPi / 16.0f };
	float distance_ = 12.0f;

	bool isMoveCamera_ = false;

	//* collider *//

	std::unique_ptr<ColliderPrimitiveRenderer> colliderRenderer_;
	bool isRenderCollider_ = true;

	//* imgui *//

	ImDrawList* sceneWindow_  = nullptr;
	ImDrawList* canvasWindow_ = nullptr;

	std::optional<GuizmoUsed> gizmoUsed_ = std::nullopt;
	GuizmoOperation gizmoOperation_ = GuizmoOperation::Translate;
	SxImGuizmo::Mode gizmoMode_ = SxImGuizmo::Mode::World;

	//=========================================================================================
	// private methods
	//=========================================================================================

	//* show imgui component *//

	void ShowSceneMenu();
	void ShowSceneConfig();
	void ShowGizmoMenu();
	void ShowColliderMenu();

	void ShowSceneWindow();
	void ShowGameWindow();
	void ShowCanvasWindow();

	//* sub methods *//

	WindowRect SetImGuiImageFullWindow(const D3D12_GPU_DESCRIPTOR_HANDLE& handle, const Vector2ui& size);

	void UpdateCamera();

};
