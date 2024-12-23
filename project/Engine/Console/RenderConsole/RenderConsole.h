#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* console
#include "Outliner.h"

//* engine
#include <Engine/Asset/AssetLib/Texture/AssetTexture.h>
#include <Engine/Module/Behavior/BaseBehavior.h>
#include <Engine/Module/Pipeline/RenderPipelineCollection.h>
#include <Engine/Module/Pipeline/ComputePipelineCollection.h>
#include <Engine/Module/SxavengerGraphics/SxavGraphicsFrame.h>
#include <Engine/Module/SxavengerGraphics/ScreenPresenter.h>
#include <Engine/Module/VisualLayer/VisualLayer.h>
#include <Engine/Module/VisualLayer/VisualDoF.h>
#include <Engine/Module/Camera/Camera3d.h>
#include <Engine/Module/Camera/DebugCamera3d.h>

//* c++
#include <optional>

//* external
#include <imgui.h>
#include <ImGuizmo.h>

//-----------------------------------------------------------------------------------------
// forward
//-----------------------------------------------------------------------------------------
class Console;

////////////////////////////////////////////////////////////////////////////////////////////
// FullScreenFrameType enum class
////////////////////////////////////////////////////////////////////////////////////////////
enum class FullScreenFrameType : uint8_t {
	kNone,
	kScene,
	kGame,
};

////////////////////////////////////////////////////////////////////////////////////////////
// RenderConsole class
////////////////////////////////////////////////////////////////////////////////////////////
class RenderConsole
	: public Outliner {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	RenderConsole()  = default;
	~RenderConsole() = default;

	void Init(Console* console);

	void Term();

	void UpdateConsole();

	void Draw();

	//* visual iterator option *//

	VisualIterator SetLayer(BaseVisualLayer* layer);

	void EraseLayer(const VisualIterator& iterator);

	void ResetLayer();

	void RemoveSelectedLayer(const VisualIterator& iterator);

	void RemoveUniqueLayer();

	//* pipeline option *//

	void SetGraphicsPipeline(RenderPipelineType type, const DirectXThreadContext* context, const Vector2ui& size);

	void BindGraphicsBuffer(RenderPipelineType type, const DirectXThreadContext* context, const DxObject::BindBufferDesc& desc);

	void SetComputePipeline(ComputePipelineType type, const DirectXThreadContext* context);

	void BindComputeBuffer(ComputePipelineType type, const DirectXThreadContext* context, const DxObject::BindBufferDesc& desc);

	void DispatchCompute(const DirectXThreadContext* context, const Vector2ui& size);

	void PresentToScreen(GameWindow* window, const DirectXThreadContext* context);

	//* imgui methods option *//

	void SetManipulateImGuiCommand();
	void Manipulate(ImGuizmo::OPERATION operation, ImGuizmo::MODE mode, TransformComponent* component);

protected:

	//=========================================================================================
	// protected variables
	//=========================================================================================

	bool isDisplayRenderConsole_ = true;

	//=========================================================================================
	// protected methods
	//=========================================================================================

	//* menu *//

	void ShowRenderConsoleMenu();
	void ShowGraphicsMenu();
	void ShowDebugMenu();

	//* sub *//

	void CreateFrame(const Vector2ui& size);

private:

	////////////////////////////////////////////////////////////////////////////////////////////
	// WindowRect structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct WindowRect {
		Vector2f pos;
		Vector2f size;
	};

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* external *//

	Console* console_ = nullptr;

	//* visual *//

	VisualCanvas canvas_;

	std::optional<VisualIterator> visualIterator_;

	std::list<std::unique_ptr<BaseVisualLayer>> layers_;

	//* pipeline *// HACK:...

	std::unique_ptr<RenderPipelineCollection> renderPipeline_;
	std::unique_ptr<ComputePipelineCollection> computePipeline_;

	std::unique_ptr<ScreenPresenter> presenter_;

	//* frames *//

	std::weak_ptr<AssetTexture> checkerTexture_;

	std::unique_ptr<SxavGraphicsFrame>    scene_;
	std::unique_ptr<BlenderDebugCamera3d> sceneCamera_;

	std::unique_ptr<SxavGraphicsFrame> game_;
	std::unique_ptr<Camera3d>          gameCamera_;
	// CONSENDER: frameはどこに持たせるべきか.

	FullScreenFrameType type_ = FullScreenFrameType::kGame;

	Color4f frustumColor_ = ToColor4f(0xFAFA00FF);

	//* Manipulate *//

	WindowRect sceneRect_ = {};

	ImGuizmo::OPERATION operation_ = ImGuizmo::TRANSLATE;
	ImGuizmo::MODE mode_           = ImGuizmo::LOCAL;

	//* parameter *//

	std::unique_ptr<DxObject::DimensionBuffer<Vector4f>> buffer_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	//* window *//

	void DisplayCanvas();
	void DisplayLayer();

	void DisplayScene();
	void DisplayGame();

	//* layer methods *//

	bool IsSelectedLayer(BaseVisualLayer* layer);
	void SelectableLayer(const VisualCanvas& canvas);

	void ProcessVisual(SxavGraphicsFrame* frame, const VisualCanvas& canvas);

	//* frame methods *//

	void DrawScene();
	void DrawGame();

	//* sub methods *//

	static void MenuDummy();

	static WindowRect ShowTextureImGuiFullWindow(const MultiViewTexture* texture);
	static WindowRect ShowTextureImGuiFullWindow(const BaseTexture* texture);

	static void ShowDemoGrid(const Camera3d* camera, const WindowRect& rect, float length);

};
