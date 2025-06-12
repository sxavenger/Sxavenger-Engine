#include "RenderSceneEditor.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/UI/SxImGui.h>
#include <Engine/System/UI/SxImGuizmo.h>
#include <Engine/Content/SxavengerContent.h>
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

	operationTexture_[static_cast<uint32_t>(GuizmoOperation::Translate)] = SxavengerAsset::TryImport<AssetTexture>("packages/textures/icon/operation_translate.png");
	operationTexture_[static_cast<uint32_t>(GuizmoOperation::Rotate)] = SxavengerAsset::TryImport<AssetTexture>("packages/textures/icon/operation_rotate.png");
	operationTexture_[static_cast<uint32_t>(GuizmoOperation::Scale)] = SxavengerAsset::TryImport<AssetTexture>("packages/textures/icon/view_cube64.png");

	camera_ = std::make_unique<MonoBehaviour>();
	camera_->SetName("editor camera");
	camera_->AddComponent<TransformComponent>();
	camera_->AddComponent<CameraComponent>();

	textures_ = std::make_unique<FRenderTargetTextures>();
	textures_->Create(kMainWindowSize);

	renderer_ = std::make_unique<FSceneRenderer>();
	renderer_->SetTextures(textures_.get());

	config_ = {};
	config_.camera              = camera_->GetComponent<CameraComponent>();
	config_.isEnableComposite   = true;
	config_.isEnablePostProcess = false;

	colliderRenderer_ = std::make_unique<ColliderPrimitiveRenderer>();
	colliderRenderer_->Init();
}

void RenderSceneEditor::ShowMainMenu() {
	if (ImGui::BeginMenu("render")) {
		MenuPadding();
		ImGui::SeparatorText("render");

		ShowSceneMenu();
		ShowSceneConfig();
		ShowGizmoMenu();
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
	
	renderer_->Render(SxavengerSystem::GetMainThreadContext(), config_);

	//* Debug Render *//
	textures_->BeginTransparentBasePass(SxavengerSystem::GetMainThreadContext());

	// todo: draw lines, etc...
	if (isRenderCollider_) {
		colliderRenderer_->Render(SxavengerSystem::GetMainThreadContext(), camera_->GetComponent<CameraComponent>());
	}

	if (isMoveCamera_) {
		SxavengerContent::PushAxis(point_, 1.0f);
	}

	CameraComponent* camera = camera_->GetComponent<CameraComponent>();
	SxavengerContent::GetDebugPrimitive()->DrawToScene(SxavengerSystem::GetMainThreadContext(), camera);

	textures_->EndTransparentBasePass(SxavengerSystem::GetMainThreadContext());
}

void RenderSceneEditor::Manipulate(MonoBehaviour* behaviour) {
	if (gizmoUsed_.has_value() && gizmoUsed_.value() != GuizmoUsed::Scene) {
		return;
	}

	SxImGuizmo::SetDrawlist(sceneWindow_);

	SxImGuizmo::Operation operation = SxImGuizmo::NONE;

	// todo: flagに変更
	if (gizmoOperation_ == GuizmoOperation::Scale) {
		operation = SxImGuizmo::SCALE;
	}

	if (gizmoOperation_ == GuizmoOperation::Translate) {
		operation = SxImGuizmo::TRANSLATE;
	}

	if (gizmoOperation_ == GuizmoOperation::Rotate) {
		operation = SxImGuizmo::ROTATE;
	}

	// transform component の取得
	auto component = behaviour->GetComponent<TransformComponent>();

	if (component == nullptr) { //!< transform component が存在してない場合
		return;
	}

	SxImGuizmo::SetRect({ sceneRect_.pos.x, sceneRect_.pos.y }, { sceneRect_.size.x, sceneRect_.size.y });

	Matrix4x4 m = component->GetMatrix();
	//Matrix4x4 delta = Matrix4x4::Identity();

	SxImGuizmo::Enable(!component->HasParent());

	bool isEdit = false;

	SxImGuizmo::GizmoOutput output = {};

	isEdit = SxImGuizmo::Manipulate(
		reinterpret_cast<const float*>(camera_->GetComponent<CameraComponent>()->GetCamera().view.m.data()),
		reinterpret_cast<const float*>(camera_->GetComponent<CameraComponent>()->GetCamera().proj.m.data()),
		reinterpret_cast<float*>(m.m.data()),
		output,
		operation,
		gizmoMode_
	);

	SxImGuizmo::Enable(true);

	gizmoUsed_ = SxImGuizmo::IsUsing() ? std::optional<GuizmoUsed>{GuizmoUsed::Scene} : std::nullopt;

	if (component->HasParent()) {
		return;
	}

	if (!isEdit) {
		return;
	}

	switch (output.type) {
		case SxImGuizmo::GizmoOutput::OutputType::Translation:
			component->GetTransform().translate += { output.value.x, output.value.y, output.value.z };
			break;

		case SxImGuizmo::GizmoOutput::OutputType::Scale:
			component->GetTransform().scale = { output.value.x, output.value.y, output.value.z };
			break;

		case SxImGuizmo::GizmoOutput::OutputType::RotationLocal:
			component->GetTransform().rotate *= Quaternion(output.value.x, output.value.y, output.value.z, output.value.w);
			break;

		case SxImGuizmo::GizmoOutput::OutputType::RotationWorld:
			component->GetTransform().rotate = Quaternion(output.value.x, output.value.y, output.value.z, output.value.w) * component->GetTransform().rotate;
			break;
	}

	component->UpdateMatrix();
}

void RenderSceneEditor::ManipulateCanvas(MonoBehaviour* behaviour) {
	if (gizmoUsed_.has_value() && gizmoUsed_.value() != GuizmoUsed::Canvas) {
		return;
	}

	ImGuizmo::SetDrawlist(canvasWindow_);
	ImGuizmo::SetOrthographic(true);

	ImGuizmo::OPERATION operation = ImGuizmo::NONE;

	// todo: flagに変更
	if (gizmoOperation_ == GuizmoOperation::Scale) {
		operation = ImGuizmo::SCALE_X | ImGuizmo::SCALE_Y;
	}

	if (gizmoOperation_ == GuizmoOperation::Translate) {
		operation = ImGuizmo::TRANSLATE_X | ImGuizmo::TRANSLATE_Y;
	}

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

	gizmoUsed_ = ImGuizmo::IsUsing() ? std::make_optional<GuizmoUsed>(GuizmoUsed::Canvas) : std::nullopt;

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

	if (gizmoOperation_ == GuizmoOperation::Scale) {
		component->GetTransform2d().scale = { transform.scale.x, transform.scale.y };
	}

	if (gizmoOperation_ == GuizmoOperation::Translate) {
		component->GetTransform2d().translate = { transform.translate.x, transform.translate.y };
	}

	ImGuizmo::SetOrthographic(false);
}

void RenderSceneEditor::SetCameraPoint(const Vector3f& point) {
	point_ = point;
	UpdateView();
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

void RenderSceneEditor::ShowSceneConfig() {
	if (ImGui::BeginMenu("config")) {
		MenuPadding();
		ImGui::SeparatorText("config");

		ImGui::Checkbox("enable post process", &config_.isEnablePostProcess);
		ImGui::Checkbox("enable composite",    &config_.isEnableComposite);
		
		ImGui::EndMenu();
	}
}

void RenderSceneEditor::ShowGizmoMenu() {
	if (ImGui::BeginMenu("gizmo")) {
		MenuPadding();
		ImGui::SeparatorText("gizmo");

		ImGui::Text("operation");
		ImGui::Separator();

		SxImGui::RadioButton("translate", &gizmoOperation_, GuizmoOperation::Translate);
		ImGui::SameLine();
		SxImGui::RadioButton("rotate", &gizmoOperation_, GuizmoOperation::Rotate);
		ImGui::SameLine();
		SxImGui::RadioButton("scale", &gizmoOperation_, GuizmoOperation::Scale);

		ImGui::Text("mode");
		ImGui::Separator();

		SxImGui::RadioButton("world", &gizmoMode_, SxImGuizmo::World);
		ImGui::SameLine();
		SxImGui::RadioButton("local", &gizmoMode_, SxImGuizmo::Local);

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
	BaseEditor::SetNextWindowDocking();

	//* fix window style
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0, 0 });
	ImGui::Begin("Scene ## Render Scene Editor", nullptr, BaseEditor::GetWindowFlag() | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_MenuBar);

	//* menu bar
	if (ImGui::BeginMenuBar()) {

		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 0, 0 });

		//* gizmo operation menu *//
		//* translate
		if (SxImGui::SelectImageButton("## gizmo translate", operationTexture_[static_cast<uint32_t>(GuizmoOperation::Translate)].WaitGet()->GetGPUHandleSRV().ptr, { 16, 16 }, gizmoOperation_ == GuizmoOperation::Translate)) {
			gizmoOperation_ = GuizmoOperation::Translate;
		}

		//* rotate
		if (SxImGui::SelectImageButton("## gizmo rotate", operationTexture_[static_cast<uint32_t>(GuizmoOperation::Rotate)].WaitGet()->GetGPUHandleSRV().ptr, { 16, 16 }, gizmoOperation_ == GuizmoOperation::Rotate)) {
			gizmoOperation_ = GuizmoOperation::Rotate;
		}

		//* scale
		if (SxImGui::SelectImageButton("## gizmo scale", operationTexture_[static_cast<uint32_t>(GuizmoOperation::Scale)].WaitGet()->GetGPUHandleSRV().ptr, { 16, 16 }, gizmoOperation_ == GuizmoOperation::Scale)) {
			gizmoOperation_ = GuizmoOperation::Scale;
		}

		ImGui::PopStyleVar();
		ImGui::EndMenuBar();
	}

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

	isFocusSceneWindow_ = ImGui::IsWindowFocused();

	if (isFocusSceneWindow_) {
		UpdateCamera();
	}

	ImGui::End();
	ImGui::PopStyleVar();
}

void RenderSceneEditor::ShowGameWindow() {
	BaseEditor::SetNextWindowDocking();

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

	isFocusGameWindow_ = ImGui::IsWindowFocused();

	ImGui::End();
	ImGui::PopStyleVar();
}

void RenderSceneEditor::ShowCanvasWindow() {
	BaseEditor::SetNextWindowDocking();

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

	isMoveCamera_ = false;

	auto mouse     = SxavengerSystem::GetInput()->GetMouseInput();
	auto transform = camera_->GetComponent<TransformComponent>();

	if (mouse->IsPress(MouseId::MOUSE_MIDDLE)) {
		Vector2f delta = mouse->GetDeltaPosition();
		static const Vector2f kSensitivity = { 0.01f, 0.01f };

		angle_   += delta * kSensitivity;
		angle_.x = std::fmod(angle_.x, kPi * 2.0f);
		angle_.y = std::clamp(angle_.y, -kPi / 2.0f, kPi / 2.0f);

		isMoveCamera_ = true;
	}

	if (mouse->IsPress(MouseId::MOUSE_RIGHT)) {
		Vector2f delta = mouse->GetDeltaPosition();
		static const Vector2f kSensitivity = { 0.01f, 0.01f };

		Vector3f right = Quaternion::RotateVector(kUnitX3<float>, transform->GetTransform().rotate);
		Vector3f up    = Quaternion::RotateVector(kUnitY3<float>, transform->GetTransform().rotate);

		point_ -= right * delta.x * kSensitivity.x;
		point_ += up * delta.y * kSensitivity.y;

		isMoveCamera_ = true;
	}

	distance_ = std::max(distance_ - mouse->GetDeltaWheel(), 0.0f);

	UpdateView();
}

void RenderSceneEditor::UpdateView() {
	auto transform = camera_->GetComponent<TransformComponent>();

	Quaternion r = Quaternion::AxisAngle(kUnitY3<float>, angle_.x) * Quaternion::AxisAngle(kUnitX3<float>, angle_.y);

	Vector3f direciton = Quaternion::RotateVector(kBackward3<float>, r);

	transform->GetTransform().translate = point_ + direciton * distance_;
	transform->GetTransform().rotate = r;
	transform->UpdateMatrix();

	camera_->GetComponent<CameraComponent>()->UpdateView();
}
