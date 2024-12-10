#include "RenderConsole.h"
_DXOBJECT_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* console
#include "../Console.h"

//* engine
#include <Engine/System/SxavengerSystem.h>
#include <Engine/Asset/SxavengerAsset.h>
#include <Engine/Module/SxavengerModule.h>

//* external
#include <ImGuizmo.h>

////////////////////////////////////////////////////////////////////////////////////////////
// RenderConsole class methods
////////////////////////////////////////////////////////////////////////////////////////////

void RenderConsole::Init(Console* console) {
	console_ = console;

	renderPipeline_ = std::make_unique<RenderPipelineCollection>();
	renderPipeline_->Init();

	computePipeline_ = std::make_unique<ComputePipelineCollection>();
	computePipeline_->Init();

	sceneCamera_ = std::make_unique<BlenderDebugCamera3d>();
	sceneCamera_->Create();

	gameCamera_ = std::make_unique<Camera3d>();
	gameCamera_->Create();
	gameCamera_->GetTransform().translate = { 0.0f, 0.0f, -24.0f };
	gameCamera_->UpdateMatrix();
	gameCamera_->SetToConsole();

	presenter_ = std::make_unique<ScreenPresenter>();
	presenter_->Init();

	checkerTexture_ = SxavengerAsset::ImportTexture("asset/textures/checker_black.png");
	checkerTexture_.lock()->Load(SxavengerSystem::GetMainThreadContext());
}

void RenderConsole::Term() {

	renderPipeline_.reset();
	computePipeline_.reset();

	scene_.reset();
	sceneCamera_.reset();

	game_.reset();
	gameCamera_.reset();

	presenter_.reset();

	RemoveUniqueBehavior();
	RemoveUniqueLayer();

	ResetBehavior();
}

void RenderConsole::UpdateConsole() {
	if (isDisplayRenderConsole_) {
		//* behavior *//
		UpdateUniqueRemove();

		//* window *//
		DisplayScene();
		DisplayGame();
		DisplayOutliner();
		DisplayAttribute();
		DisplayCanvas();
		DisplayLayer();
	}
}

void RenderConsole::Draw() {
	DrawScene();
	SxavengerSystem::TransitionAllocator();

	DrawGame();
	SxavengerSystem::TransitionAllocator();
}

BehaviorIterator RenderConsole::SetBehavior(BaseBehavior* behavior) {
	return outliner_.emplace(outliner_.end(), behavior);
}

void RenderConsole::EraseBehavior(const BehaviorIterator& iterator) {
	RemoveAttributeBehavior(iterator);
	outliner_.erase(iterator);
}

void RenderConsole::ResetBehavior() {
	RemoveUniqueBehavior();
	ResetAttributeBehavior();
	outliner_.clear();
}

void RenderConsole::RemoveAttributeBehavior(const BehaviorIterator& iterator) {
	if (attributeIterator_.has_value() && *attributeIterator_.value() == *iterator) {
		if (attributeIterator_.value() == attributeTable_->begin()) {
			ResetAttributeBehavior();

		} else {
			attributeIterator_.value()--;
		}
	}
}

void RenderConsole::ResetAttributeBehavior() {
	attributeIterator_ = std::nullopt;
	attributeTable_    = nullptr;
}

void RenderConsole::RemoveUniqueBehavior() {
	behaviors_.clear();
}

VisualIterator RenderConsole::SetLayer(BaseVisualLayer* layer) {
	return canvas_.emplace(canvas_.end(), layer);
}

void RenderConsole::EraseLayer(const VisualIterator& iterator) {
	RemoveSelectedLayer(iterator);
	canvas_.erase(iterator);
}

void RenderConsole::ResetLayer() {
	canvas_.clear();
}

void RenderConsole::RemoveSelectedLayer(const VisualIterator& iterator) {
	if (visualIterator_.has_value()) {
		if ((*iterator) == (*visualIterator_.value())) {
			visualIterator_ = std::nullopt;
		}
	}
}

void RenderConsole::RemoveUniqueLayer() {
	layers_.clear();
}

void RenderConsole::SetGraphicsPipeline(RenderPipelineType type, const DirectXThreadContext* context, const Vector2ui& size) {
	renderPipeline_->SetPipeline(type, context, size);
}

void RenderConsole::BindGraphicsBuffer(RenderPipelineType type, const DirectXThreadContext* context, const DxObject::BindBufferDesc& desc) {
	renderPipeline_->BindGraphicsBuffer(type, context, desc);
}

void RenderConsole::SetComputePipeline(ComputePipelineType type, const DirectXThreadContext* context) {
	computePipeline_->SetPipeline(type, context);
}

void RenderConsole::BindComputeBuffer(ComputePipelineType type, const DirectXThreadContext* context, const DxObject::BindBufferDesc& desc) {
	computePipeline_->BindComputeBuffer(type, context, desc);
}

void RenderConsole::DispatchCompute(const DirectXThreadContext* context, const Vector2ui& size) {
	computePipeline_->Dispatch(context, size);
}

void RenderConsole::PresentToScreen(GameWindow* window, const DirectXThreadContext* context) {
#ifdef _DEBUG
	if (type_ == FullScreenFrameType::kScene) {
		presenter_->Present(window, context, scene_->GetAdaptive()->GetTexture()->GetGPUHandleSRV());

	} else if (type_ == FullScreenFrameType::kGame) {
		presenter_->Present(window, context, game_->GetAdaptive()->GetTexture()->GetGPUHandleSRV());
	}
#else
	presenter_->Present(window, context, game_->GetAdaptive()->GetTexture()->GetGPUHandleSRV());
#endif
}

void RenderConsole::Manipulate(ImGuizmo::OPERATION operation, ImGuizmo::MODE mode, TransformComponent* component) {

	ImGuizmo::SetRect(sceneRect_.pos.x, sceneRect_.pos.y, sceneRect_.size.x, sceneRect_.size.y);

	Matrix4x4 m = component->GetMatrix();

	ImGuizmo::Manipulate(
		reinterpret_cast<const float*>(sceneCamera_->GetView().m),
		reinterpret_cast<const float*>(sceneCamera_->GetProj().m),
		operation,
		mode,
		reinterpret_cast<float*>(m.m)
	);

	EulerTransform transform = {};

	ImGuizmo::DecomposeMatrixToComponents(
		reinterpret_cast<const float*>(m.m),
		&transform.translate.x,
		&transform.rotate.x,
		&transform.scale.x
	);

	// FIXME: rotate and scale
	component->GetTransform().translate = transform.translate;
	//component->GetTransform().rotate    = ToQuaternion2(transform.rotate).Normalize(); 
	//component->GetTransform().scale     = transform.scale;
	component->UpdateMatrix();
}

void RenderConsole::ShowRenderConsoleMenu() {
	if (ImGui::BeginMenu("present")) {
		MenuDummy();

		ImGui::SeparatorText("present");

		if (ImGui::RadioButton("none", reinterpret_cast<int32_t*>(&type_), static_cast<int32_t>(FullScreenFrameType::kNone))) {
			type_ = FullScreenFrameType::kNone;
		}

		ImGui::SameLine();

		if (ImGui::RadioButton("game", reinterpret_cast<int32_t*>(&type_), static_cast<int32_t>(FullScreenFrameType::kGame))) {
			type_ = FullScreenFrameType::kGame;
		}

		ImGui::SameLine();

		if (ImGui::RadioButton("scene", reinterpret_cast<int32_t*>(&type_), static_cast<int32_t>(FullScreenFrameType::kScene))) {
			type_ = FullScreenFrameType::kScene;
		}

		ImGui::EndMenu();
	}

	if (ImGui::BeginMenu("behavior")) {
		MenuDummy();

		ImGui::SeparatorText("outliner");
		if (ImGui::Button("remove unique behavior")) {
			RemoveUniqueBehavior();
		}

		ImGui::EndMenu();
	}

	if (ImGui::BeginMenu("layer and canvas")) {
		MenuDummy();

		ImGui::SeparatorText("canvas");
		if (ImGui::Button("remove unique layer")) {
			RemoveUniqueLayer();
		}

		ImGui::EndMenu();
	}
}

void RenderConsole::ShowGraphicsMenu() {
	if (ImGui::BeginMenu("Behavior Create")) {
		MenuDummy();

		if (ImGui::Selectable("base behavior", false)) {
			auto& behavior = behaviors_.emplace_back();
			behavior = std::make_unique<RenderBehavior>();
		}

		ImGui::EndMenu();
	}

	if (ImGui::BeginMenu("Layer Create")) {
		MenuDummy();

		if (ImGui::Selectable("visual dof", false)) {
			auto& layer = layers_.emplace_back();
			layer = std::make_unique<VisualDoF>();
			layer->Init();
			layer->SetToConsole("unique layer dof");
		}

		ImGui::EndMenu();
	}
}

void RenderConsole::CreateFrame(const Vector2ui& size) {
	scene_ = std::make_unique<SxavGraphicsFrame>();
	scene_->Create(size, SxavGraphicsFrameType::kDebug);
	scene_->SetCamera(sceneCamera_.get());

	game_ = std::make_unique<SxavGraphicsFrame>();
	game_->Create(size, SxavGraphicsFrameType::kGame);
	game_->SetCamera(gameCamera_.get());
}

void RenderConsole::DisplayOutliner() {
	console_->DockingConsole();
	ImGui::Begin("Outliner ## Render Console", nullptr, console_->GetWindowFlag());

	SelectableBehavior(outliner_);

	ImGui::End();
}

void RenderConsole::DisplayAttribute() {
	console_->DockingConsole();
	ImGui::Begin("Attribute ## Render Console", nullptr, console_->GetWindowFlag());

	if (attributeIterator_.has_value()) {
		auto behavior = (*attributeIterator_.value());

		ImGui::SeparatorText(behavior->GetName().c_str());

		behavior->BaseAttributeImGui();
		ImGui::Separator();
		ImGui::Spacing();

		behavior->SystemAttributeImGui();
		ImGui::Spacing();
		behavior->SetAttributeImGui();

		if (ImGui::IsWindowFocused()) {
			//localCamera_->Update();
		}
	}

	ImGui::End();
}

void RenderConsole::DisplayCanvas() {
	console_->DockingConsole();
	ImGui::Begin("Canvas ## Render Console", nullptr, console_->GetWindowFlag());

	SelectableLayer(canvas_);

	ImGui::End();
}

void RenderConsole::DisplayLayer() {
	console_->DockingConsole();
	ImGui::Begin("Layer ## Render Console", nullptr, console_->GetWindowFlag());

	if (visualIterator_.has_value()) {
		ImGui::SeparatorText((*visualIterator_.value())->GetName().c_str());
		(*visualIterator_.value())->SetLayerImGui();
	}

	ImGui::End();
}

void RenderConsole::DisplayScene() {

	console_->DockingConsole();
	ImGui::Begin("Scene ## Render Console", nullptr, console_->GetWindowFlag() | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

	if (scene_ != nullptr) {
		ShowTextureImGuiFullWindow(checkerTexture_.lock().get()); //< HACK
		sceneRect_ = ShowTextureImGuiFullWindow(scene_->GetAdaptive()->GetTexture());
		ImGuizmo::SetDrawlist();
	}

	if (ImGui::IsWindowHovered() && ImGui::IsMouseClicked(2)) {
		//!< window hovered 状態で mouse middle click が押された場合, camera操作(forcus)を許可.
		ImGui::SetWindowFocus();
	}

	if (ImGui::IsWindowFocused()) {
		sceneCamera_->Update();
	}

	ImGui::End();
}

void RenderConsole::DisplayGame() {
	console_->DockingConsole();
	ImGui::Begin("Game ## Render Console", nullptr, console_->GetWindowFlag() | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

	if (game_ != nullptr) {
		ShowTextureImGuiFullWindow(checkerTexture_.lock().get()); //< HACK
		ShowTextureImGuiFullWindow(game_->GetAdaptive()->GetTexture());
	}

	ImGui::End();
}

bool RenderConsole::IsSelectedBehavior(BaseBehavior* behavior) {
	if (attributeIterator_.has_value()) {
		return behavior == (*attributeIterator_.value());
	}

	return false;
}

void RenderConsole::SelectableBehavior(const BehaviorContainer& container) {

	for (auto itr = container.begin(); itr != container.end(); ++itr) {
		BaseBehavior* behavior = (*itr);
		bool isSelected        = IsSelectedBehavior(behavior);

		std::string label = behavior->GetName() + std::format("##{:p}", reinterpret_cast<void*>(behavior)); //!< 名前重複対策

		// 非activeの場合, 灰色に
		if (!behavior->IsActive()) {
			ImGui::PushStyleColor(ImGuiCol_Text, kBehaviorDisableColor_);
		}

		if (behavior->GetChildren().empty()) { //!< 子がいない場合

			if (ImGui::Selectable(label.c_str(), isSelected)) {
				attributeIterator_ = itr;
				attributeTable_    = &container;
				//localCamera_->Reset();
			}

		} else {

			ImGuiTreeNodeFlags flags
				= ImGuiTreeNodeFlags_OpenOnDoubleClick
				| ImGuiTreeNodeFlags_OpenOnArrow;

			if (isSelected) {
				flags |= ImGuiTreeNodeFlags_Selected;
			}

			bool isOpenTreeNode = ImGui::TreeNodeEx(label.c_str(), flags);

			if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen()) { //!< selectされた場合
				attributeIterator_ = itr;
				attributeTable_    = &container;
				//localCamera_->Reset();
			}

			if (isOpenTreeNode) {
				SelectableBehavior(behavior->GetChildren());
				ImGui::TreePop();
			}
		}

		if (!behavior->IsActive()) {
			ImGui::PopStyleColor();
		}
	}
}

void RenderConsole::UpdateUniqueRemove() {
	behaviors_.remove_if([](const std::unique_ptr<RenderBehavior>& behavior) {
		return behavior->IsDelete();
	});
}

void RenderConsole::DrawSystematicBehavior(SxavGraphicsFrame* frame, const BehaviorContainer& container) {
	for (const auto& behavior : container) {
		if (behavior->IsActive()) {
			if (behavior->CheckRenderingFlag(BehaviorRenderingType::kSystematic)) {
				behavior->DrawSystematic(frame);
			}

			if (!behavior->GetChildren().empty()) {
				DrawSystematicBehavior(frame, behavior->GetChildren());
			}
		}
	}
}

void RenderConsole::DrawAdaptiveBehavior(SxavGraphicsFrame* frame, const BehaviorContainer& container) {
	for (const auto& behavior : container) {
		if (behavior->IsActive()) {
			if (behavior->CheckRenderingFlag(BehaviorRenderingType::kAdaptive)) {
				behavior->DrawAdaptive(frame);
			}

			if (!behavior->GetChildren().empty()) {
				DrawAdaptiveBehavior(frame, behavior->GetChildren());
			}
		}
	}
}

void RenderConsole::DrawLateAdaptiveBehavior(SxavGraphicsFrame* frame, const BehaviorContainer& container) {
	for (const auto& behavior : container) {
		if (behavior->IsActive()) {
			if (behavior->CheckRenderingFlag(BehaviorRenderingType::kLateAdaptive)) {
				behavior->DrawLateAdaptive(frame);
			}

			if (!behavior->GetChildren().empty()) {
				DrawLateAdaptiveBehavior(frame, behavior->GetChildren());
			}
		}
	}
}

bool RenderConsole::IsSelectedLayer(BaseVisualLayer* layer) {
	if (visualIterator_.has_value()) {
		return (layer == (*visualIterator_.value()));
	}

	return false;
}

void RenderConsole::SelectableLayer(const VisualCanvas& canvas) {
	for (auto itr = canvas.begin(); itr != canvas.end(); ++itr) {

		bool isSelected = IsSelectedLayer((*itr));


		if (ImGui::Selectable((*itr)->GetName().c_str(), isSelected)) {
			visualIterator_ = itr;
		}
	}
}

void RenderConsole::ProcessVisual(SxavGraphicsFrame* frame, const VisualCanvas& canvas) {
	for (auto& layer : canvas) {
		layer->Process(SxavengerSystem::GetMainThreadContext(), frame);
	}
}

void RenderConsole::DrawScene() {
	if (scene_ == nullptr) {
		return;
	}

	auto context = SxavengerSystem::GetMainThreadContext();

	{
		scene_->BeginSystematic(context);
		scene_->ClearSystematic(context);
		scene_->ClearRasterizerDepth(context);

		DrawSystematicBehavior(scene_.get(), outliner_);

		scene_->EndSystematic(context);
	}

	{ //* transition

		scene_->BeginXclipse(context);
		
		computePipeline_->SetPipeline(kTransition_SampleLighting, context);

		BindBufferDesc bind = scene_->GetTransitionSystematicBindDesc();
		computePipeline_->BindComputeBuffer(kTransition_SampleLighting, context, bind);

		computePipeline_->Dispatch(context, scene_->GetSize());

		scene_->EndXclipse(context);
	}

	scene_->TransitionXclipseToAdaptive(context);

	{
		scene_->BeginAdaptive(context);
		DrawAdaptiveBehavior(scene_.get(), outliner_);
		DrawLateAdaptiveBehavior(scene_.get(), outliner_);

		if (game_ != nullptr && game_->GetCamera() != nullptr) {
			game_->GetCamera()->DrawFrustum(ToColor4f(0xFAFA00FF), 4.0f);
		}

		SxavengerModule::DrawGrid(kOrigin3<float>, 12.0f);
		SxavengerModule::DrawToScene(SxavengerSystem::GetMainThreadContext(), scene_->GetCamera());
		
		scene_->EndAdaptive(context);
	}
}

void RenderConsole::DrawGame() {
	if (game_ == nullptr) {
		return;
	}

	auto context = SxavengerSystem::GetMainThreadContext();

	{
		game_->BeginSystematic(context);
		game_->ClearSystematic(context);
		game_->ClearRasterizerDepth(context);

		DrawSystematicBehavior(game_.get(), outliner_);

		game_->EndSystematic(context);
	}

	{ //* transition

		game_->BeginXclipse(context);
		
		computePipeline_->SetPipeline(kTransition_SampleLighting, context);

		BindBufferDesc bind = game_->GetTransitionSystematicBindDesc();
		computePipeline_->BindComputeBuffer(kTransition_SampleLighting, context, bind);

		computePipeline_->Dispatch(context, game_->GetSize());

		game_->EndXclipse(context);
	}

	game_->TransitionXclipseToAdaptive(context);

	{
		game_->BeginAdaptive(context);
		DrawAdaptiveBehavior(game_.get(), outliner_);
		SxavengerModule::DrawToScene(SxavengerSystem::GetMainThreadContext(), game_->GetCamera());
		game_->EndAdaptive(context);
	}

	game_->TransitionAdaptiveToVisual(context);

	{
		game_->BeginVisual(context);
		ProcessVisual(game_.get(), canvas_);
		game_->EndVisual(context);
	}

	game_->TransitionVisualToAdaptive(context);

	{
		game_->BeginAdaptive(context);
		DrawLateAdaptiveBehavior(game_.get(), outliner_);
		SxavengerModule::DrawToScene(SxavengerSystem::GetMainThreadContext(), game_->GetCamera());
		game_->EndAdaptive(context);
	}
}

void RenderConsole::MenuDummy() {
	static const ImVec2 size = { 240.0f, 0.0f };
	ImGui::Dummy(size);
}

RenderConsole::WindowRect RenderConsole::ShowTextureImGuiFullWindow(const MultiViewTexture* texture) {

	// タブ等を排除した全体のwindowSize計算
	ImVec2 regionMax  = ImGui::GetWindowContentRegionMax();
	ImVec2 regionMin  = ImGui::GetWindowContentRegionMin();
	ImVec2 windowSize = { regionMax.x - regionMin.x, regionMax.y - regionMin.y };

	Vector2f textureSize = texture->GetSize();
	
	// 画像アス比と分割したWindowアス比の計算
	float textureAspectRatio = textureSize.x / textureSize.y;
	float windowAspectRatio  = windowSize.x / windowSize.y;
	
	// 出力する画像サイズの設定
	ImVec2 displayTextureSize = windowSize;
	
	// 画像サイズの調整
	if (textureAspectRatio <= windowAspectRatio) {
		displayTextureSize.x *= textureAspectRatio / windowAspectRatio;
	
	} else {
		displayTextureSize.y *= windowAspectRatio / textureAspectRatio;
	}
	
	// 出力場所の調整
	ImVec2 leftTop = {
		(windowSize.x - displayTextureSize.x) * 0.5f + regionMin.x,
		(windowSize.y - displayTextureSize.y) * 0.5f + regionMin.y,
	};
	
	ImGui::SetCursorPos(leftTop);
	ImGui::Image(texture->GetGPUHandleSRV().ptr, displayTextureSize);

	WindowRect rect = {};
	rect.pos  = { leftTop.x + ImGui::GetWindowPos().x, leftTop.y + ImGui::GetWindowPos().y };
	rect.size = { displayTextureSize.x, displayTextureSize.y };

	return rect;
}

RenderConsole::WindowRect RenderConsole::ShowTextureImGuiFullWindow(const BaseTexture* texture) {

	// タブ等を排除した全体のwindowSize計算
	ImVec2 regionMax  = ImGui::GetWindowContentRegionMax();
	ImVec2 regionMin  = ImGui::GetWindowContentRegionMin();
	ImVec2 windowSize = { regionMax.x - regionMin.x, regionMax.y - regionMin.y };

	Vector2f textureSize = texture->GetSize();
	
	// 画像アス比と分割したWindowアス比の計算
	float textureAspectRatio = textureSize.x / textureSize.y;
	float windowAspectRatio  = windowSize.x / windowSize.y;
	
	// 出力する画像サイズの設定
	ImVec2 displayTextureSize = windowSize;
	
	// 画像サイズの調整
	if (textureAspectRatio <= windowAspectRatio) {
		displayTextureSize.x *= textureAspectRatio / windowAspectRatio;
	
	} else {
		displayTextureSize.y *= windowAspectRatio / textureAspectRatio;
	}
	
	// 出力場所の調整
	ImVec2 leftTop = {
		(windowSize.x - displayTextureSize.x) * 0.5f + regionMin.x,
		(windowSize.y - displayTextureSize.y) * 0.5f + regionMin.y,
	};
	
	ImGui::SetCursorPos(leftTop);
	ImGui::Image(texture->GetGPUHandleSRV().ptr, displayTextureSize);

	WindowRect rect = {};
	rect.pos  = { leftTop.x + ImGui::GetWindowPos().x, leftTop.y + ImGui::GetWindowPos().y };
	rect.size = { displayTextureSize.x, displayTextureSize.y };

	return rect;

}

void RenderConsole::ShowDemoGrid(const Camera3d* camera, const WindowRect& rect, float length) {

	ImGuizmo::SetDrawlist();

	ImGuizmo::SetRect(rect.pos.x, rect.pos.y, rect.size.x, rect.size.y);

	ImGuizmo::DrawGrid(
		reinterpret_cast<const float*>(camera->GetView().m),
		reinterpret_cast<const float*>(camera->GetProj().m),
		reinterpret_cast<const float*>(Matrix4x4::Identity().m),
		length
	);

	// this test

	Matrix4x4 m = Matrix4x4::Identity();

	ImGuizmo::Manipulate(
		reinterpret_cast<const float*>(camera->GetView().m),
		reinterpret_cast<const float*>(camera->GetProj().m),
		ImGuizmo::TRANSLATE,
		ImGuizmo::WORLD,
		reinterpret_cast<float*>(m.m)
	);
}
