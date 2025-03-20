#include "RenderSceneEditor.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/Component/Components/Transform/TransformComponent.h>
#include <Engine/Component/Components/SpriteRenderer/SpriteRendererComponent.h>
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
	camera_->SetName("editor camera");
	camera_->AddComponent<TransformComponent>();
	camera_->AddComponent<CameraComponent>();

	colliderRenderer_ = std::make_unique<ColliderPrimitiveRenderer>();
	colliderRenderer_->Init();
}

void RenderSceneEditor::ShowMainMenu() {
	if (ImGui::BeginMenu("render")) {
		MenuPadding();
		ImGui::SeparatorText("render");

		ShowSceneMenu();
		ShowColliderMenu();
		
		ImGui::EndMenu();
	}
}

void RenderSceneEditor::ShowWindow() {
	ShowGameWindow();
	ShowSceneWindow();
	ShowCanvasWindow();
}

void RenderSceneEditor::Render() {
	FSceneRenderer::Config config = {};
	config.camera = camera_->GetComponent<CameraComponent>();

	renderer_->Render(SxavengerSystem::GetMainThreadContext(), config);

	//* Debug Render *//
	textures_->BeginTransparentBasePass(SxavengerSystem::GetMainThreadContext());

	// todo: draw lines, etc...
	if (isRenderCollider_) {
		colliderRenderer_->Render(SxavengerSystem::GetMainThreadContext(), camera_->GetComponent<CameraComponent>());
	}

	textures_->EndTransparentBasePass(SxavengerSystem::GetMainThreadContext());
}

void RenderSceneEditor::Manipulate(MonoBehaviour* behaviour) {
	if (guizmoUsed_.has_value() && guizmoUsed_.value() != GuizmoUsed::Scene) {
		return;
	}

	ImGuizmo::SetDrawlist(sceneWindow_);

	ImGuizmo::OPERATION operation = ImGuizmo::TRANSLATE; // FIXME: translateしか使えない

	// transform component の取得
	auto component = behaviour->GetComponent<TransformComponent>();

	if (component == nullptr) { //!< transform component が存在してない場合
		return;
	}

	ImGuizmo::SetRect(sceneRect_.pos.x, sceneRect_.pos.y, sceneRect_.size.x, sceneRect_.size.y);

	Matrix4x4 m = component->GetMatrix();

	ImGuizmo::Enable(!component->HasParent());

	bool isEdit = false;

	isEdit = ImGuizmo::Manipulate(
		reinterpret_cast<const float*>(camera_->GetComponent<CameraComponent>()->GetCamera().view.m.data()),
		reinterpret_cast<const float*>(camera_->GetComponent<CameraComponent>()->GetCamera().proj.m.data()),
		operation,
		ImGuizmo::WORLD,
		reinterpret_cast<float*>(m.m.data())
	);

	ImGuizmo::Enable(true);

	guizmoUsed_ = ImGuizmo::IsUsing() ? std::optional<GuizmoUsed>{GuizmoUsed::Scene} : std::nullopt;

	if (component->HasParent()) {
		return;
	}

	if (!isEdit) {
		return;
	}

	EulerTransform transform = {};

	ImGuizmo::DecomposeMatrixToComponents(
		reinterpret_cast<const float*>(m.m.data()),
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

void RenderSceneEditor::ManipulateCanvas(MonoBehaviour* behaviour) {
	if (guizmoUsed_.has_value() && guizmoUsed_.value() != GuizmoUsed::Canvas) {
		return;
	}

	ImGuizmo::SetDrawlist(canvasWindow_);
	ImGuizmo::SetOrthographic(true);

	ImGuizmo::OPERATION operation = ImGuizmo::TRANSLATE_X | ImGuizmo::TRANSLATE_Y; //!< translate only

	// sprite component の取得
	auto component = behaviour->GetComponent<SpriteRendererComponent>();

	if (component == nullptr) {
		return;
	}

	ImGuizmo::SetRect(canvasRect_.pos.x, canvasRect_.pos.y, canvasRect_.size.x, canvasRect_.size.y);

	Matrix4x4 m = component->GetTransform2d().ToMatrix();

	static const Matrix4x4 view = Matrix4x4::Identity();
	static const Matrix4x4 proj = Matrix4x4::Orthographic(0.0f, 0.0f, static_cast<float>(kMainWindowSize.x), static_cast<float>(kMainWindowSize.y), 0.0f, 128.0f);

	bool isEdit = false;

	isEdit = ImGuizmo::Manipulate(
		reinterpret_cast<const float*>(view.m.data()),
		reinterpret_cast<const float*>(proj.m.data()),
		operation,
		ImGuizmo::WORLD,
		reinterpret_cast<float*>(m.m.data())
	);

	guizmoUsed_ = ImGuizmo::IsUsing() ? std::optional<GuizmoUsed>{GuizmoUsed::Canvas} : std::nullopt;

	if (!isEdit) {
		return;
	}

	EulerTransform transform = {};

	ImGuizmo::DecomposeMatrixToComponents(
		reinterpret_cast<const float*>(m.m.data()),
		&transform.translate.x,
		&transform.rotate.x,
		&transform.scale.x
	);

	component->GetTransform2d().translate = { transform.translate.x, transform.translate.y };
	/*component->GetTransform2d().rotate    = transform.rotate.z;
	component->GetTransform2d().scale     = { transform.scale.x, transform.scale.y };*/

	ImGuizmo::SetOrthographic(false);
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
			"UI",
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

void RenderSceneEditor::ShowColliderMenu() {
	if (ImGui::BeginMenu("collider")) {
		MenuPadding();
		ImGui::SeparatorText("collider");

		ImGui::Checkbox("render", &isRenderCollider_);
		ImGui::Separator();

		ImGui::BeginDisabled(!isRenderCollider_);
		colliderRenderer_->SetImGuiCommand();
		ImGui::EndDisabled();

		ImGui::EndMenu();
	}
}

void RenderSceneEditor::ShowSceneWindow() {
	//* fix window style
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0, 0 });
	ImGui::Begin("Scene ## Render Scene Editor", nullptr, BaseEditor::GetWindowFlag() | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

	sceneWindow_ = ImGui::GetWindowDrawList();

	SetImGuiImageFullWindow(
		checkerboard_.WaitGet()->GetGPUHandleSRV(),
		kMainWindowSize
	);

	sceneRect_ = SetImGuiImageFullWindow(
		textures_->GetGBuffer(layout_)->GetGPUHandleSRV(),
		kMainWindowSize
	);

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

	SetImGuiImageFullWindow(
		FMainRender::GetInstance()->GetTextures()->GetGBuffer(FRenderTargetTextures::GBufferLayout::UI)->GetGPUHandleSRV(),
		kMainWindowSize
	);

	ImGui::End();
	ImGui::PopStyleVar();
}

void RenderSceneEditor::ShowCanvasWindow() {
	//* fix window style
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0, 0 });
	ImGui::Begin("Canvas ## Render Scene Editor", nullptr, BaseEditor::GetWindowFlag() | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

	canvasWindow_ = ImGui::GetWindowDrawList();

	SetImGuiImageFullWindow(
		checkerboard_.WaitGet()->GetGPUHandleSRV(),
		kMainWindowSize
	);

	canvasRect_ = SetImGuiImageFullWindow(
		FMainRender::GetInstance()->GetTextures()->GetGBuffer(FRenderTargetTextures::GBufferLayout::UI)->GetGPUHandleSRV(),
		kMainWindowSize
	);

	//ImGuizmo::SetDrawlist();

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
		angle_.x = std::fmod(angle_.x, kPi * 2.0f);
		angle_.y = std::clamp(angle_.y, -kPi / 2.0f, kPi / 2.0f);
	}

	if (mouse->IsPress(MouseId::MOUSE_RIGHT)) {
		Vector2f delta = mouse->GetDeltaPosition();
		static const Vector2f kSensitivity = { 0.01f, 0.01f };

		Vector3f right = Quaternion::RotateVector(kUnitX3<float>, transform->GetTransform().rotate);
		Vector3f up    = Quaternion::RotateVector(kUnitY3<float>, transform->GetTransform().rotate);

		point_ -= right * delta.x * kSensitivity.x;
		point_ += up * delta.y * kSensitivity.y;
	}

	distance_ = std::max(distance_ - mouse->GetDeltaWheel(), 0.0f);

	Quaternion r = Quaternion::AxisAngle(kUnitY3<float>, angle_.x) * Quaternion::AxisAngle(kUnitX3<float>, angle_.y);

	Vector3f direciton = Quaternion::RotateVector(kBackward3<float>, r);

	transform->GetTransform().translate = point_ + direciton * distance_;
	transform->GetTransform().rotate    = r;
	transform->UpdateMatrix();

	camera_->GetComponent<CameraComponent>()->UpdateView();
}
