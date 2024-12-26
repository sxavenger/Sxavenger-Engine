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
	Outliner::Init(console);

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

	checkerTexture_ = SxavengerAsset::TryImport<Texture>("asset/textures/checker_black.png");
	checkerTexture_.Lock()->Load(SxavengerSystem::GetMainThreadContext());

	buffer_ = std::make_unique<DxObject::DimensionBuffer<Vector4f>>();
	buffer_->Create(SxavengerSystem::GetDxDevice(), 1);

	(*buffer_)[0] = { 0.0f, 1.0f, 0.0f, 20.0f };
}

void RenderConsole::Term() {
	Outliner::Term();

	buffer_.reset();

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
		Outliner::UpdateUniqueRemove();

		//* window *//
		DisplayScene();
		DisplayGame();
		Outliner::DisplayOutliner();
		Outliner::DisplayAttribute();
		DisplayCanvas();
		DisplayLayer();

		ImGui::Begin("debug");

		if (ImGui::DragFloat3("Sun direction", &(*buffer_)[0].x, 0.01f)) {
			Vector3f sun = { (*buffer_)[0].x, (*buffer_)[0].y, (*buffer_)[0].z };
			sun = Normalize(sun);
			(*buffer_)[0].x = sun.x;
			(*buffer_)[0].y = sun.y;
			(*buffer_)[0].z = sun.z;
		}

		ImGui::DragFloat("Sun Intencity", &(*buffer_)[0].w, 0.01f);

		ImGui::End();
	}
}

void RenderConsole::Draw() {
	DrawScene();
	SxavengerSystem::TransitionAllocator();

	DrawGame();
	SxavengerSystem::TransitionAllocator();
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
			auto& behavior = Outliner::behaviors_.emplace_back();
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

void RenderConsole::ShowDebugMenu() {
	if (ImGui::BeginMenu("Camera")) {
		MenuDummy();
		ImGui::SeparatorText("camera");
		ImGui::ColorEdit4("frustum", &frustumColor_.r);
		ImGui::EndMenu();
	}

	if (ImGui::BeginMenu("Collider")) {
		MenuDummy();

		ImGui::SeparatorText("collider");
		auto colliderCollection = SxavengerModule::GetColliderCollection();
		colliderCollection->SystemDebugGui();

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
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0, 0 });

	console_->DockingConsole();
	ImGui::Begin("Scene ## Render Console", nullptr, console_->GetWindowFlag() | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

	if (scene_ != nullptr) {
		ShowTextureImGuiFullWindow(checkerTexture_.ReloadAndLock().get()); //!< HACK
		sceneRect_ = ShowTextureImGuiFullWindow(scene_->GetAdaptive()->GetTexture());
		ImGuizmo::SetDrawlist();
	}

	if (ImGui::IsWindowHovered() && (ImGui::IsMouseClicked(ImGuiMouseButton_Middle) || ImGui::IsMouseClicked(ImGuiMouseButton_Right))) {
		//!< window hovered 状態で mouse middle click が押された場合, camera操作(forcus)を許可.
		ImGui::SetWindowFocus();
	}

	if (ImGui::IsWindowFocused()) {
		sceneCamera_->Update();
	}

	ImGui::End();

	ImGui::PopStyleVar();
}

void RenderConsole::DisplayGame() {
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0, 0 });

	console_->DockingConsole();
	ImGui::Begin("Game ## Render Console", nullptr, console_->GetWindowFlag() | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

	if (game_ != nullptr) {
		ShowTextureImGuiFullWindow(checkerTexture_.ReloadAndLock().get()); //< HACK
		ShowTextureImGuiFullWindow(game_->GetAdaptive()->GetTexture());
	}

	ImGui::End();

	ImGui::PopStyleVar();
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

		DrawSystematicBehavior(scene_.get(), Outliner::GetOutliner());

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
		DrawAdaptiveBehavior(scene_.get(), Outliner::GetOutliner());
		DrawLateAdaptiveBehavior(scene_.get(), Outliner::GetOutliner());

		if (game_ != nullptr && game_->GetCamera() != nullptr) {
			game_->GetCamera()->DrawFrustum(ToColor4f(0xFAFA00FF), 4.0f);
		}

		SxavengerModule::DrawGrid(kOrigin3<float>, 12.0f);
		SxavengerModule::DrawCollider();
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

		DrawSystematicBehavior(game_.get(), Outliner::GetOutliner());

		game_->EndSystematic(context);
	}

	{ //* transition

		game_->BeginXclipse(context);

		{ //* transition
			game_->GetXclipse()->ResetResultBufferIndex();
			computePipeline_->SetPipeline(kTransition_SampleLighting, context);

			BindBufferDesc bind = game_->GetTransitionSystematicBindDesc();
			computePipeline_->BindComputeBuffer(kTransition_SampleLighting, context, bind);

			computePipeline_->Dispatch(context, game_->GetSize());
		}

		//{ //!< ssao
		//	game_->GetXclipse()->NextResultBufferIndex();

		//	computePipeline_->SetPipeline(kXclipse_SSAO, context);

		//	BindBufferDesc bind = {};
		//	bind.SetHandle("gInput", game_->GetXclipse()->GetPrevBuffer()->GetGPUHandleUAV());
		//	bind.SetAddress("gCamera", game_->GetCamera()->GetGPUVirtualAddress());
		//	bind.SetAddress("gConfig", game_->GetGetConfigVirtualAddress());
		//	bind.SetHandle("gNormal", game_->GetSystematic()->GetGPUHandleSRV(SystematicRenderFrame::GBuffer::kNormal));
		//	bind.SetHandle("gPosition", game_->GetSystematic()->GetGPUHandleSRV(SystematicRenderFrame::GBuffer::kPosition));
		//	bind.SetHandle("gDepth", game_->GetDepthBufferController()->GetRasterizerGPUHandleSRV());
		//	bind.SetHandle("gOutput", game_->GetXclipse()->GetResultBuffer()->GetGPUHandleUAV());

		//	computePipeline_->BindComputeBuffer(kXclipse_SSAO, context, bind);
		//	computePipeline_->Dispatch(context, game_->GetSize());

		//	
		//}

		{ //* atmosphere

			game_->GetXclipse()->NextResultBufferIndex();

			computePipeline_->SetPipeline(kXclipse_Atmosphere, context);

			BindBufferDesc bind = {};
			bind.SetHandle("gInput", game_->GetXclipse()->GetPrevBuffer()->GetGPUHandleSRV());
			bind.SetAddress("gSun", buffer_->GetGPUVirtualAddress());
			bind.SetAddress("gCamera", game_->GetCamera()->GetGPUVirtualAddress());
			bind.SetAddress("gConfig", game_->GetGetConfigVirtualAddress());
			bind.SetHandle("gOutput", game_->GetXclipse()->GetResultBuffer()->GetGPUHandleUAV());

			computePipeline_->BindComputeBuffer(kXclipse_Atmosphere, context, bind);
			computePipeline_->Dispatch(context, game_->GetSize());

		}

		game_->EndXclipse(context);
	}

	game_->TransitionXclipseToAdaptive(context);

	{
		game_->BeginAdaptive(context);
		DrawAdaptiveBehavior(game_.get(), Outliner::GetOutliner());
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
		DrawLateAdaptiveBehavior(game_.get(), Outliner::GetOutliner());
		SxavengerModule::DrawToScene(SxavengerSystem::GetMainThreadContext(), game_->GetCamera());
		game_->EndAdaptive(context);
	}
}

void RenderConsole::MenuDummy() {
	static const ImVec2 size = { 240.0f, 0.0f };
	ImGui::Dummy(size);
}

RenderConsole::WindowRect RenderConsole::ShowTextureImGuiFullWindow(const MultiViewTextureBuffer* texture) {

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

RenderConsole::WindowRect RenderConsole::ShowTextureImGuiFullWindow(const Texture* texture) {

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
