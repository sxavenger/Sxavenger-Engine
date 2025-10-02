#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* editor
#include "BaseEditor.h"

//* engine
#include <Engine/Component/Entity/MonoBehaviour.h>
#include <Engine/Render/FRenderTargetBuffer.h>
#include <Engine/Render/Pass/FBaseRenderPass.h>
#include <Engine/Preview/Asset/UAssetTexture.h>
#include <Engine/Preview/Asset/UAssetParameter.h>

//* lib
#include <Lib/Geometry/Vector2.h>
#include <Lib/Geometry/Vector3.h>

//* external
#include <imgui.h>
#include <Engine/System/UI/SxImGuizmo.h>

//* c++
#include <memory>

#include "Engine/System/DirectX/DxObject/DxUnorderedDimensionBuffer.h"
#include "Engine/Content/DebugPrimitive/DebugPrimitive.h"

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

	//* manipulator option *//

	void Manipulate(MonoBehaviour* behaviour);

	void ManipulateCanvas(MonoBehaviour* behaviour);

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
		Lighting_GBuffer,
		Albedo,
		Normal,
		MaterialARM,
		Position,
		Velocity,
		Direct,
		Indirect,
		Indirect_Reservoir,
	};

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* rect *//

	WindowRect sceneRect_  = {};
	WindowRect canvasRect_ = {};

	//* texture *//

	UAssetParameter<UAssetTexture> checkerboard_; //!< checker board texture

	UAssetParameter<UAssetTexture> operationTexture_[3];
	UAssetParameter<UAssetTexture> modeTexture_[2];
	UAssetParameter<UAssetTexture> gridTexture_;

	//* renderer *//

	bool isRender_ = true;

	std::unique_ptr<FRenderTargetBuffer> textures_; //!< debug textures
	std::unique_ptr<MonoBehaviour>       camera_;   //!< scene camera

	FBaseRenderPass::Config config_ = {};

	bool isFocusGameWindow_  = false;
	bool isFocusSceneWindow_ = false;

	//* parameter *//

	GBuffer buffer_ = GBuffer::Scene;

	//* camera *//

	Vector3f point_ = kOrigin3<float>;
	Vector2f angle_ = { 0.0f, kPi / 16.0f };
	float distance_ = 12.0f;

	bool isMoveCamera_ = false;

	//* grid *//

	bool isRenderGrid_ = true;

	//* imgui *//

	ImDrawList* sceneWindow_  = nullptr;
	ImDrawList* canvasWindow_ = nullptr;

	std::optional<GuizmoUsed> gizmoUsed_ = std::nullopt;
	GuizmoOperation gizmoOperation_      = GuizmoOperation::Translate;
	SxImGuizmo::Mode gizmoMode_          = SxImGuizmo::Mode::World;

	//* icon *//

	UAssetParameter<UAssetTexture> icons_[static_cast<uint32_t>(Icon::Camera) + 1];

	Vector2f iconSize_ = { 32.0f, 32.0f };

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
	void ShowCameraInfomation(const WindowRect& rect);
	void UpdateView();

	//* helper methods *//

	void DisplayGBufferTexture(GBuffer buffer);

	void RenderIcon(BaseInspector* inspector, Icon icon, const Vector3f& position, const Color4f& color);

	void RenderTextSceneWindow(ImVec2& position, const std::string& text, ImU32 color = ImGui::GetColorU32(ImGuiCol_Text));

};
