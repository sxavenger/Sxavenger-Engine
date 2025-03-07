#include "RenderSceneEditor.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/SxavengerSystem.h>
#include <Engine/Asset/SxavengerAsset.h>
#include <Engine/Module/SxavengerModule.h>

//* lib
#include <Lib/Environment.h>

////////////////////////////////////////////////////////////////////////////////////////////
// RenderSceneEditor class methods
////////////////////////////////////////////////////////////////////////////////////////////

void RenderSceneEditor::Init() {

	textures_ = std::make_unique<FSceneTextures>();
	textures_->Create(kMainWindowSize);

	sceneRenderer_ = std::make_unique<FSceneRenderer>();
	sceneRenderer_->GetConfig().isEmptyLightAlbedo = true;

	sceneCamera_ = std::make_unique<APivotCameraActor>();
	sceneCamera_->Init();

	sceneRenderer_->SetCamera(sceneCamera_.get());
	sceneRenderer_->SetTextures(textures_.get());


	checkerBoard_ = SxavengerAsset::TryImport<AssetTexture>("packages/textures/checker_black.png");
}

void RenderSceneEditor::ShowMainMenu() {
}

void RenderSceneEditor::ShowWindow() {
	ShowSceneWindow();
	ShowGameWindow();
	ShowHierarchyWindow();
	ShowInspectorWindow();
}

void RenderSceneEditor::Render() {
	sceneCamera_->UpdateView();
	sceneRenderer_->Render(SxavengerSystem::GetMainThreadContext());

	textures_->BeginForward(SxavengerSystem::GetMainThreadContext());

	SxavengerModule::GetColliderCollection()->Draw();
	SxavengerModule::GetColliderCollection()->GetDrawer()->Render(SxavengerSystem::GetMainThreadContext(), sceneCamera_.get());

	SxavengerModule::DrawGrid(kOrigin3<float>, 16.0f);
	SxavengerModule::GetDebugPrimitive()->DrawToScene(SxavengerSystem::GetMainThreadContext(), sceneCamera_.get());

	textures_->EndForward(SxavengerSystem::GetMainThreadContext());
}

void RenderSceneEditor::SetGameRenderer(FSceneRenderer* renderer) {
	gameRenderer_ = renderer;

	if (gameRenderer_ != nullptr) {
		SetScene(gameRenderer_->GetScene());
	}
}

void RenderSceneEditor::ShowSceneWindow() {
	//* fix window style
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0, 0 });

	//* begin window
	BaseEditor::SetNextWindowDocking();
	ImGui::Begin("Scene ## Render Scene Editor", nullptr, BaseEditor::GetWindowFlag() | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

	auto checker = checkerBoard_.WaitGet();
	SetImGuiImageFullWindow(checker->GetGPUHandleSRV(), checker->GetSize());
	rect_ = SetImGuiImageFullWindow(sceneRenderer_->GetDebugTexture(), sceneRenderer_->GetSize());
	ImGuizmo::SetDrawlist();

	if (ImGui::IsWindowHovered() && (ImGui::IsMouseClicked(ImGuiMouseButton_Middle) || ImGui::IsMouseClicked(ImGuiMouseButton_Right))) {
		//!< window hovered 状態で mouse middle click が押された場合, camera操作(forcus)を許可.
		ImGui::SetWindowFocus();
	}

	if (ImGui::IsWindowFocused()) {
		UpdateSceneCamera();
	}

	ImGui::End();
	ImGui::PopStyleVar();
}

void RenderSceneEditor::ShowGameWindow() {
	//* fix window style
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0, 0 });

	//* begin window
	BaseEditor::SetNextWindowDocking();
	ImGui::Begin("Game ## Render Scene Editor", nullptr, BaseEditor::GetWindowFlag() | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

	auto checker = checkerBoard_.WaitGet();
	SetImGuiImageFullWindow(checker->GetGPUHandleSRV(), checker->GetSize());

	if (gameRenderer_ != nullptr) {
		SetImGuiImageFullWindow(gameRenderer_->GetDebugTexture(), gameRenderer_->GetSize());
	}

	ImGui::End();
	ImGui::PopStyleVar();
}

void RenderSceneEditor::ShowHierarchyWindow() {
	BaseEditor::SetNextWindowDocking();
	ImGui::Begin("Hierarchy ## Render Scene Editor", nullptr, BaseEditor::GetWindowFlag());

	auto scene = sceneRenderer_->GetScene();

	static char buffer[256] = {};
	ImVec2 cursor = ImGui::GetCursorPos();
	ImGui::InputText("## Search", buffer, sizeof(buffer));

	if (buffer[0] == static_cast<char>(0)) { //!< 検索がない場合
		ImGui::SetCursorPos(cursor);
		ImGui::PushStyleColor(ImGuiCol_Text, disabled_);
		ImGui::Text("sarch...");
		ImGui::PopStyleColor();
	}

	ImGui::Spacing();

	if (scene != nullptr) {
		if (ImGui::BeginTabBar("## Outliner Tab")) {

			//* Geometry *//
			if (ImGui::BeginTabItem("Geometry")) {

				bool isAvailable = false; //!< 選択されたActorが存在するか

				for (const auto& geometry : scene->GetGeometries() | std::views::filter([&](const AActor* ptr) { return ptr->GetName().starts_with(std::string_view{buffer}); })) {

					std::string name = std::format("{} # {:p}", geometry->GetName(), reinterpret_cast<const void*>(geometry));
					bool isSelected  = IsSelectedActor(geometry);

					bool isActive = geometry->IsActive();

					if (!isActive) {
						ImGui::PushStyleColor(ImGuiCol_Text, disabled_);
					}

					if (isSelected) {
						isAvailable = true;
					}

					if (ImGui::Selectable(name.c_str(), isSelected)) {
						selectedActor_ = geometry;
						isAvailable    = true;
					}

					if (!isActive) {
						ImGui::PopStyleColor();
					}
				}

				if (!isAvailable) {
					selectedActor_ = std::nullopt;
				}

				ImGui::EndTabItem();
			}

			//* Light *//
			if (ImGui::BeginTabItem("Light")) {

				bool isAvailable = false; //!< 選択されたActorが存在するか

				for (const auto& light : scene->GetLights() | std::views::filter([&](const AActor* ptr) { return ptr->GetName().starts_with(std::string_view{ buffer }); })) {
					std::string name = std::format("{} # {:p}", light->GetName(), reinterpret_cast<const void*>(light));
					bool isSelected = IsSelectedActor(light);

					bool isActive = light->IsActive();

					if (!isActive) {
						ImGui::PushStyleColor(ImGuiCol_Text, disabled_);
					}

					if (isSelected) {
						isAvailable = true;
					}

					if (ImGui::Selectable(name.c_str(), isSelected)) {
						selectedActor_ = light;
						isAvailable    = true;
					}

					if (!isActive) {
						ImGui::PopStyleColor();
					}
				}

				if (!isAvailable) {
					selectedActor_ = std::nullopt;
				}

				ImGui::EndTabItem();
			}

			//* Camera *//
			if (ImGui::BeginTabItem("Camera")) {
				if (gameRenderer_ != nullptr) {
					bool isAvailable = false; //!< 選択されたActorが存在するか

					auto camera = gameRenderer_->GetCamera();

					bool isSelected = IsSelectedActor(camera);

					if (isSelected) {
						isAvailable = true;
					}

					if (ImGui::Selectable(camera->GetName().c_str(), isSelected)) {
						selectedActor_ = camera;
						isAvailable = true;
					}

					if (!isAvailable) {
						selectedActor_ = std::nullopt;
					}

				} else {
					selectedActor_ = std::nullopt;
				}
				

				ImGui::EndTabItem();
			}

			ImGui::EndTabBar();
		}

	} else {
		ImGui::TextDisabled("scene is not set.");
		selectedActor_ = std::nullopt;
	}

	if (selectedActor_.has_value()) {
		Manipulate(ImGuizmo::TRANSLATE, ImGuizmo::WORLD, selectedActor_.value());
	}

	ImGui::End();
}

void RenderSceneEditor::ShowInspectorWindow() {
	BaseEditor::SetNextWindowDocking();
	ImGui::Begin("Inspector ## Render Scene Editor", nullptr, BaseEditor::GetWindowFlag());

	if (selectedActor_.has_value()) {

		auto actor = selectedActor_.value();

		bool isActive = actor->IsActive();
		ImGui::Checkbox("## active", &isActive);
		actor->SetActive(isActive);

		ImGui::SameLine();

		if (actor->IsActive()) {
			ImGui::Text(actor->GetName().c_str());

		} else {
			ImGui::TextDisabled(actor->GetName().c_str());
		}

		ImGui::Separator();

		actor->SetImGuiCommand();

		ImGui::Dummy({ 0, 4.0f });

		actor->InspectorImGui();
	}

	ImGui::End();
}

RenderSceneEditor::WindowRect RenderSceneEditor::SetImGuiImageFullWindow(const D3D12_GPU_DESCRIPTOR_HANDLE& handle, const Vector2ui& size) {

	// タブ等を排除した全体のwindowSize計算
	ImVec2 regionMax = ImGui::GetWindowContentRegionMax();
	ImVec2 regionMin = ImGui::GetWindowContentRegionMin();
	ImVec2 windowSize = { regionMax.x - regionMin.x, regionMax.y - regionMin.y };

	// 画像アス比と分割したWindowアス比の計算
	float textureAspectRatio = static_cast<float>(size.x) / static_cast<float>(size.y);
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

	// 画像の描画
	ImGui::SetCursorPos(leftTop);
	ImGui::Image(handle.ptr, displayTextureSize);

	WindowRect rect = {};
	rect.pos  = { leftTop.x + ImGui::GetWindowPos().x, leftTop.y + ImGui::GetWindowPos().y };
	rect.size = { displayTextureSize.x, displayTextureSize.y };

	return rect;

}

void RenderSceneEditor::UpdateSceneCamera() {

	auto mouse = SxavengerSystem::GetInput()->GetMouseInput();

	if (mouse->IsPress(MouseId::MOUSE_MIDDLE)) {
		Vector2f delta = mouse->GetDeltaPosition();
		static const Vector2f kSensitivity = { 0.01f, 0.01f };

		sceneCamera_->GetParameter().angle += delta * kSensitivity;
		sceneCamera_->GetParameter().angle.x = std::fmod(sceneCamera_->GetParameter().angle.x, pi_v * 2.0f);
		sceneCamera_->GetParameter().angle.y = std::clamp(sceneCamera_->GetParameter().angle.y, -pi_v / 2.0f, pi_v / 2.0f);

	}

	if (mouse->IsPress(MouseId::MOUSE_RIGHT)) {
		Vector2f delta = mouse->GetDeltaPosition();
		static const Vector2f kSensitivity = { 0.01f, 0.01f };

		Vector3f right = RotateVector({ 1.0f, 0.0f, 0.0f }, sceneCamera_->GetTransform().rotate);
		Vector3f up    = RotateVector({ 0.0f, 1.0f, 0.0f }, sceneCamera_->GetTransform().rotate);

		sceneCamera_->GetParameter().point -= right * delta.x * kSensitivity.x;
		sceneCamera_->GetParameter().point += up    * delta.y * kSensitivity.y;
	}

	sceneCamera_->GetParameter().distance = std::max(sceneCamera_->GetParameter().distance - mouse->GetDeltaWheel(), 0.0f);
	sceneCamera_->UpdateView();
}

void RenderSceneEditor::Manipulate(ImGuizmo::OPERATION operation, ImGuizmo::MODE mode, TransformComponent* component) const {
	operation = ImGuizmo::TRANSLATE; // FIXME: translateしか使えない

	ImGuizmo::SetRect(rect_.pos.x, rect_.pos.y, rect_.size.x, rect_.size.y);

	Matrix4x4 m = component->GetMatrix();

	ImGuizmo::Manipulate(
		reinterpret_cast<const float*>(sceneCamera_->GetCamera().view.m),
		reinterpret_cast<const float*>(sceneCamera_->GetCamera().proj.m),
		operation,
		mode,
		reinterpret_cast<float*>(m.m)
	);

	if (component->HasParent()) {
		return;
	}

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

bool RenderSceneEditor::IsSelectedActor(AActor* actor) const {
	if (selectedActor_.has_value()) {
		return selectedActor_.value() == actor;
	}

	return false;
}
