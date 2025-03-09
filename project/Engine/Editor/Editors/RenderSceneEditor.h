#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* editor
#include "BaseEditor.h"

//* engine
#include <Engine/Component/MonoBehaviour.h>
#include <Engine/Component/Components/Collider/ColliderPrimitiveDrawer.h>
#include <Engine/Asset/Texture/AssetTexture.h>
#include <Engine/Asset/Observer/AssetObserver.h>
#include <Engine/Render/FRenderTargetTextures.h>
#include <Engine/Render/Scene/FSceneRenderer.h>

//* lib
#include <Lib/Geometry/Vector2.h>
#include <Lib/Geometry/Vector3.h>

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

	void Manipulate(MonoBehaviour* behaviour, ImGuizmo::OPERATION operation, ImGuizmo::MODE mode);

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

	//* rect *//

	WindowRect rect_ = {};

	//* texture *//

	AssetObserver<AssetTexture> checkerboard_; //!< checker board texture

	//* renderer *//

	std::unique_ptr<FRenderTargetTextures> textures_; //!< debug textures
	std::unique_ptr<FSceneRenderer>        renderer_; //!< scene renderer
	std::unique_ptr<MonoBehaviour>         camera_;   //!< scene camera

	//* parameter *//

	FRenderTargetTextures::GBufferLayout layout_ = FRenderTargetTextures::GBufferLayout::Main;

	//* window *//

	std::shared_ptr<GameWindow> window_;
	// todo: screen window の描画

	//* camera *//

	Vector3f point_ = kOrigin3<float>;
	Vector2f angle_ = { 0.0f, pi_v / 16.0f };
	float distance_ = 12.0f;

	//* collider *//

	std::unique_ptr<ColliderPrimitiveDrawer> colliderDrawer_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	//* show imgui component *//

	void ShowSceneMenu();

	void ShowSceneWindow();
	void ShowGameWindow();

	//* sub methods *//

	WindowRect SetImGuiImageFullWindow(const D3D12_GPU_DESCRIPTOR_HANDLE& handle, const Vector2ui& size);

	void UpdateCamera();

};
