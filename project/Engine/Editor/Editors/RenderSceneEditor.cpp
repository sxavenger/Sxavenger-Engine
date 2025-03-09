#include "RenderSceneEditor.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/Component/Components/Transform/TransformComponent.h>
#include <Engine/Component/Components/Camera/CameraComponent.h>
#include <Engine/Asset/SxavengerAsset.h>
#include <Engine/Render/FMainRender.h>

////////////////////////////////////////////////////////////////////////////////////////////
// RenderSceneEditor class methods
////////////////////////////////////////////////////////////////////////////////////////////

void RenderSceneEditor::Init() {

	checkerboard_ = SxavengerAsset::TryImport<AssetTexture>("packages/textures/checker_black.png");

	textures_ = std::make_unique<FRenderTargetTextures>();
	textures_->Create(kMainWindowSize);

	renderer_ = std::make_unique<FSceneRenderer>();
	renderer_->SetTextures(textures_.get());

	camera_ = std::make_unique<MonoBehaviour>();
	camera_->AddComponent<TransformComponent>();
	camera_->AddComponent<CameraComponent>();

	colliderDrawer_ = std::make_unique<ColliderPrimitiveDrawer>();
	colliderDrawer_->Init();
}

void RenderSceneEditor::ShowMainMenu() {
	if (ImGui::BeginMenu("render")) {
		MenuPadding();

		ImGui::SeparatorText("render");
		ShowSceneMenu();
		
		ImGui::EndMenu();
	}
}

void RenderSceneEditor::ShowWindow() {
	ShowSceneWindow();
	ShowGameWindow();
}

void RenderSceneEditor::Render() {
	FSceneRenderer::Config config = {};
	config.camera = camera_->GetComponent<CameraComponent>();

	renderer_->Render(SxavengerSystem::GetMainThreadContext(), config);

	textures_->BeginTransparentBasePass(SxavengerSystem::GetMainThreadContext());

	// todo: collider, draw lines, etc...
	colliderDrawer_->Render(SxavengerSystem::GetMainThreadContext(), camera_->GetComponent<CameraComponent>());

	textures_->EndTransparentBasePass(SxavengerSystem::GetMainThreadContext());
}

void RenderSceneEditor::Manipulate(MonoBehaviour* behaviour, ImGuizmo::OPERATION operation, ImGuizmo::MODE mode) {
	operation = ImGuizmo::TRANSLATE; // FIXME: translateしか使えない

	// transform component の取得
	auto component = behaviour->GetComponent<TransformComponent>();

	if (component == nullptr) { //!< transform component が存在してない場合
		return;
	}

	ImGuizmo::SetRect(rect_.pos.x, rect_.pos.y, rect_.size.x, rect_.size.y);

	Matrix4x4 m = component->GetMatrix();

	ImGuizmo::Enable(!component->HasParent());

	ImGuizmo::Manipulate(
		reinterpret_cast<const float*>(camera_->GetComponent<CameraComponent>()->GetCamera().view.m),
		reinterpret_cast<const float*>(camera_->GetComponent<CameraComponent>()->GetCamera().proj.m),
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

void RenderSceneEditor::ShowSceneMenu() {
	if (ImGui::BeginMenu("scene")) {
		MenuPadding();
		ImGui::SeparatorText("scene");

		static const LPCSTR kLayouts[] = {
			"Normal",
			"Material_AO",
			"Albedo",
			"Position",
			"Main",
		};

		if (ImGui::BeginCombo("GBuffer", kLayouts[static_cast<uint8_t>(layout_)])) {
			for (uint8_t i = 0; i < FRenderTargetTextures::kGBufferLayoutCount; ++i) {
				if (ImGui::Selectable(kLayouts[i], (i == static_cast<uint8_t>(layout_)))) {
					layout_ = static_cast<FRenderTargetTextures::GBufferLayout>(i);
				}
			}
			ImGui::EndCombo();
		}
		
		ImGui::EndMenu();
	}
}

void RenderSceneEditor::ShowSceneWindow() {
	//* fix window style
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0, 0 });
	ImGui::Begin("Scene ## Render Scene Editor", nullptr, BaseEditor::GetWindowFlag() | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

	SetImGuiImageFullWindow(
		checkerboard_.WaitGet()->GetGPUHandleSRV(),
		kMainWindowSize
	);

	rect_ = SetImGuiImageFullWindow(
		textures_->GetGBuffer(layout_)->GetGPUHandleSRV(),
		kMainWindowSize
	);

	ImGuizmo::SetDrawlist();

	if (ImGui::IsWindowHovered() && (ImGui::IsMouseClicked(ImGuiMouseButton_Middle) || ImGui::IsMouseClicked(ImGuiMouseButton_Right))) {
		//!< window hovered 状態で mouse middle click が押された場合, camera操作(forcus)を許可.
		ImGui::SetWindowFocus();
	}

	if (ImGui::IsWindowFocused()) {
		UpdateCamera();
	}

	ImGui::End();
	ImGui::PopStyleVar();
}

void RenderSceneEditor::ShowGameWindow() {
	//* fix window style
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0, 0 });
	ImGui::Begin("Game ## Render Scene Editor", nullptr, BaseEditor::GetWindowFlag() | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

	SetImGuiImageFullWindow(
		checkerboard_.WaitGet()->GetGPUHandleSRV(),
		kMainWindowSize
	);

	SetImGuiImageFullWindow(
		FMainRender::GetInstance()->GetTextures()->GetGBuffer(FRenderTargetTextures::GBufferLayout::Main)->GetGPUHandleSRV(),
		kMainWindowSize
	);

	ImGui::End();
	ImGui::PopStyleVar();
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

void RenderSceneEditor::UpdateCamera() {

	auto mouse     = SxavengerSystem::GetInput()->GetMouseInput();
	auto transform = camera_->GetComponent<TransformComponent>();

	if (mouse->IsPress(MouseId::MOUSE_MIDDLE)) {
		Vector2f delta = mouse->GetDeltaPosition();
		static const Vector2f kSensitivity = { 0.01f, 0.01f };

		angle_   += delta * kSensitivity;
		angle_.x = std::fmod(angle_.x, pi_v * 2.0f);
		angle_.y = std::clamp(angle_.y, -pi_v / 2.0f, pi_v / 2.0f);
	}

	if (mouse->IsPress(MouseId::MOUSE_RIGHT)) {
		Vector2f delta = mouse->GetDeltaPosition();
		static const Vector2f kSensitivity = { 0.01f, 0.01f };

		Vector3f right = RotateVector({ 1.0f, 0.0f, 0.0f }, transform->GetTransform().rotate);
		Vector3f up    = RotateVector({ 0.0f, 1.0f, 0.0f }, transform->GetTransform().rotate);

		point_ -= right * delta.x * kSensitivity.x;
		point_ += up * delta.y * kSensitivity.y;
	}

	distance_ = std::max(distance_ - mouse->GetDeltaWheel(), 0.0f);

	Quaternion r = AxisAngle({ 0.0f, 1.0f, 0.0f }, angle_.x) * AxisAngle({ 1.0f, 0.0f, 0.0f }, angle_.y);

	Vector3f direciton = RotateVector({ 0.0f, 0.0f, -1.0f }, r);

	transform->GetTransform().translate = point_ + direciton * distance_;
	transform->GetTransform().rotate    = r;
	transform->UpdateMatrix();

	camera_->GetComponent<CameraComponent>()->UpdateView();
}
