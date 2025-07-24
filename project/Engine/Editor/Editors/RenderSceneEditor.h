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
#include <Engine/Render/FRenderTargetBuffer.h>
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
		DirectionalLight,
		PointLight,
		Camera
	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// GBuffer enum class
	////////////////////////////////////////////////////////////////////////////////////////////
	enum class GBuffer : uint32_t {
		Scene,
		Albedo,
		Normal,
		MaterialARM,
		Position,
		Deferred_GBuffer,
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

	AssetObserver<AssetTexture> checkerboard_; //!< checker board texture

	AssetObserver<AssetTexture> operationTexture_[3];
	AssetObserver<AssetTexture> modeTexture_[2];

	//* renderer *//

	bool isRender_ = true;

	std::unique_ptr<FRenderTargetBuffer> textures_; //!< debug textures
	std::unique_ptr<FSceneRenderer>      renderer_; //!< scene renderer
	std::unique_ptr<MonoBehaviour>       camera_;   //!< scene camera

	FSceneRenderer::Config config_ = {};

	bool isFocusGameWindow_  = false;
	bool isFocusSceneWindow_ = false;

	//* parameter *//

	//FRenderTargetTextures::GBufferLayout layout_ = FRenderTargetTextures::GBufferLayout::Main;

	GBuffer buffer_ = GBuffer::Scene;

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
	GuizmoOperation gizmoOperation_      = GuizmoOperation::Translate;
	SxImGuizmo::Mode gizmoMode_          = SxImGuizmo::Mode::World;

	//* icon *//

	AssetObserver<AssetTexture> icons_[static_cast<uint32_t>(Icon::Camera) + 1];

	//=========================================================================================
	// private methods
	//=========================================================================================

	//* show imgui component *//

	void ShowSceneMenu();
	void ShowGizmoMenu();
	void ShowColliderMenu();
	void ShowCaptureMenu();

	void ShowSceneWindow();
	void ShowGameWindow();
	void ShowCanvasWindow();

	void ShowIconScene();
	void ShowInfoTextScene();

	//* sub methods *//

	WindowRect SetImGuiImageFullWindow(const D3D12_GPU_DESCRIPTOR_HANDLE& handle, const Vector2ui& size) const;

	void SetImGuiImagesFullWindow(const std::vector<std::pair<D3D12_GPU_DESCRIPTOR_HANDLE, GBuffer>>& handles, const Vector2ui& size);

	void UpdateCamera();
	void UpdateView();

	//* helper methods *//

	void DisplayGBufferTexture(GBuffer buffer);

	void RenderIcon(Icon icon, const Vector3f& position, const Color4f& color);

	void RenderTextSceneWindow(ImVec2& position, const std::string& text, ImU32 color = ImGui::GetColorU32(ImGuiCol_Text));

};
