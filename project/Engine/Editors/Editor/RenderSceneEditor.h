#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* editor
#include "BaseEditor.h"

//* engine
#include <Engine/Foundation.h>
#include <Engine/System/UI/SxImGuizmo.h>
#include <Engine/Assets/Asset/AssetTexture.h>
#include <Engine/Assets/Asset/AssetParameter.h>
#include <Engine/Module/GameObject/GameObject.h>
#include <Engine/Render/FRenderTargetBuffer.h>
#include <Engine/Render/Pass/FBaseRenderPass.h>

//* lib
#include <Lib/Geometry/Vector2.h>
#include <Lib/Geometry/Vector3.h>

//* external
#include <imgui.h>

//* c++
#include <memory>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// RenderSceneEditor class
////////////////////////////////////////////////////////////////////////////////////////////
//! @brief Rendering用SceneEditorクラス
class RenderSceneEditor final
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

	//* manipulator option *//

	void Manipulate(EntityBehaviour* behaviour);

	void ManipulateCanvas(EntityBehaviour* behaviour);

	//* camera option *//

	void SetCameraPoint(const Vector3f& point);

	//* getter *//

	bool IsFocusGameWindow() const { return isFocusGameWindow_; }

	bool IsForcusSceneWindow() const { return isFocusSceneWindow_; }

private:

	////////////////////////////////////////////////////////////////////////////////////////////
	// WindowRect structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct WindowRect {
		Vector2f pos;
		Vector2f size;
	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// ScreenRect structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct ScreenRect {
		Vector2f min;
		Vector2f max;
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

	////////////////////////////////////////////////////////////////////////////////////////////
	// Icon enum class
	////////////////////////////////////////////////////////////////////////////////////////////
	enum class Icon : uint32_t {
		Volume,
		DirectionalLight,
		PointLight,
		SpotLight,
		Camera,
	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// GBuffer enum class
	////////////////////////////////////////////////////////////////////////////////////////////
	enum class GBuffer : uint32_t {
		Scene,
		Deferred_GBuffer,
		Albedo,
		Normal,
		MaterialARM,
		Position,
		Velocity,
		Lighting_GBuffer,
		Direct,
		Indirect,
	};

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* rect *//

	WindowRect sceneRect_  = {};
	WindowRect canvasRect_ = {};

	//* texture *//

	AssetParameter<AssetTexture> checkerboard_; //!< checker board texture

	AssetParameter<AssetTexture> operationTexture_[3];
	AssetParameter<AssetTexture> modeTexture_[2];
	AssetParameter<AssetTexture> gridTexture_;

	//* renderer *//

	bool isRender_ = true;

	std::unique_ptr<FRenderTargetBuffer> textures_; //!< debug textures
	std::unique_ptr<GameObject>          camera_;   //!< scene camera

	FBaseRenderPass::Config config_ = {};
	bool isDebugCulling_ = false;

	bool isFocusGameWindow_  = false;
	bool isFocusSceneWindow_ = false;

	//* parameter *//

	GBuffer buffer_ = GBuffer::Scene;

	//* camera *//

	Vector3f point_ = kOrigin3<float>;
	Vector2f angle_ = { 0.0f, kPi / 16.0f };
	float distance_ = 12.0f;

	bool isMoveCamera_ = false;

	//* draw primitive *//

	bool isRenderGrid_  = true;
	bool isRenderProbe_ = false;

	//* imgui *//

	ImDrawList* sceneWindowDrawer_  = nullptr;
	ImDrawList* canvasWindowDrawer_ = nullptr;

	std::optional<GuizmoUsed> gizmoUsed_ = std::nullopt;
	GuizmoOperation gizmoOperation_      = GuizmoOperation::Translate;
	SxImGuizmo::Mode gizmoMode_          = SxImGuizmo::Mode::World;

	//* icon *//

	AssetParameter<AssetTexture> icons_[static_cast<uint32_t>(Icon::Camera) + 1];

	Vector2f iconSize_ = { 32.0f, 32.0f };

	//* window *//

	//std::weak_ptr<DirectXWindowContext> gameWindow_;
	//!< MainRendererの描画はUser依存なので現システムだと描画できない

	std::weak_ptr<DirectXWindowContext> sceneWindow_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	//* show imgui component *//

	void ShowSceneMenu();
	void ShowGameMenu();
	void ShowGizmoMenu();
	void ShowCaptureMenu();

	void ShowSceneWindow();
	void ShowGameWindow();
	void ShowCanvasWindow();

	void ShowInfoTextScene();
	void ShowIconScene();

	void UpdateKeyShortcut();

	//* sub methods *//

	WindowRect SetImGuiImageFullWindow(const D3D12_GPU_DESCRIPTOR_HANDLE& handle, const Vector2ui& size) const;

	void SetImGuiImageFullWindowEnable(const D3D12_GPU_DESCRIPTOR_HANDLE& handle, const Vector2ui& size, bool isEnable);
	void SetImGuiImagesFullWindowEnable(const std::vector<std::pair<D3D12_GPU_DESCRIPTOR_HANDLE, GBuffer>>& handles, const Vector2ui& size, bool isEnable);

	void UpdateCamera();
	void ShowCameraInformation(const WindowRect& rect);
	void UpdateView();

	//* helper methods *//

	void DisplayGBufferTexture(GBuffer buffer);

	void RenderIcon(BaseInspector* inspector, Icon icon, const Vector3f& position, const Color4f& color);

	void RenderTextSceneWindow(ImVec2& position, const std::string& text, ImU32 color = ImGui::GetColorU32(ImGuiCol_Text));

};

SXAVENGER_ENGINE_NAMESPACE_END
