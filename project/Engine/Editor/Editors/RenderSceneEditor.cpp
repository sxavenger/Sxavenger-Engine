#include "RenderSceneEditor.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* editor
#include "../EditorEngine.h"

//* engine
#include <Engine/System/UI/SxImGui.h>
#include <Engine/System/UI/SxImGuizmo.h>
#include <Engine/Content/Exporter/TextureExporter.h>
#include <Engine/Content/SxavengerContent.h>
#include <Engine/Component/Components/Transform/TransformComponent.h>
#include <Engine/Component/Components/SpriteRenderer/SpriteRendererComponent.h>
#include <Engine/Component/Components/Camera/CameraComponent.h>
#include <Engine/Component/Components/PostProcessLayer/PostProcessLayerComponent.h>
#include <Engine/Asset/SxavengerAsset.h>
#include <Engine/Render/FMainRender.h>

//* externals
#include <magic_enum.hpp>

////////////////////////////////////////////////////////////////////////////////////////////
// RenderSceneEditor class methods
////////////////////////////////////////////////////////////////////////////////////////////

void RenderSceneEditor::Init() {

	checkerboard_ = SxavengerAsset::TryImport<AssetTexture>("packages/textures/checker_black.png");

	operationTexture_[static_cast<uint32_t>(GuizmoOperation::Translate)] = SxavengerAsset::TryImport<AssetTexture>("packages/textures/icon/operation_translate.png");
	operationTexture_[static_cast<uint32_t>(GuizmoOperation::Rotate)]    = SxavengerAsset::TryImport<AssetTexture>("packages/textures/icon/operation_rotate.png");
	operationTexture_[static_cast<uint32_t>(GuizmoOperation::Scale)]     = SxavengerAsset::TryImport<AssetTexture>("packages/textures/icon/operation_scale.png");

	modeTexture_[SxImGuizmo::World] = SxavengerAsset::TryImport<AssetTexture>("packages/textures/icon/mode_world.png");
	modeTexture_[SxImGuizmo::Local] = SxavengerAsset::TryImport<AssetTexture>("packages/textures/icon/mode_local.png");

	gridTexture_ = SxavengerAsset::TryImport<AssetTexture>("packages/textures/icon/grid.png");

	camera_ = std::make_unique<MonoBehaviour>();
	camera_->SetName("editor camera");
	camera_->AddComponent<TransformComponent>();
	camera_->AddComponent<CameraComponent>();

	auto camera = camera_->GetComponent<CameraComponent>();
	camera->GetProjection().focal = 16.0f;
	camera->UpdateProj();

	textures_ = std::make_unique<FRenderTargetBuffer>();
	textures_->Create(kMainWindowSize);

	renderer_ = std::make_unique<FSceneRenderer>();

	config_ = {};
	config_.buffer              = textures_.get();
	config_.camera              = camera_->GetComponent<CameraComponent>();
	config_.isEnablePostProcess = false;
	config_.isElableTonemap     = true;

	colliderRenderer_ = std::make_unique<ColliderPrimitiveRenderer>();
	colliderRenderer_->Init();

	icons_[static_cast<uint32_t>(Icon::Volume)]           = SxavengerAsset::TryImport<AssetTexture>("packages/textures/icon/scene_volume.png");
	icons_[static_cast<uint32_t>(Icon::DirectionalLight)] = SxavengerAsset::TryImport<AssetTexture>("packages/textures/icon/scene_directionalLight.png");
	icons_[static_cast<uint32_t>(Icon::PointLight)]       = SxavengerAsset::TryImport<AssetTexture>("packages/textures/icon/scene_pointLight.png");
	icons_[static_cast<uint32_t>(Icon::Camera)]           = SxavengerAsset::TryImport<AssetTexture>("packages/textures/icon/scene_camera.png");
	

}

void RenderSceneEditor::ShowMainMenu() {
	if (ImGui::BeginMenu("render")) {
		MenuPadding();
		ImGui::SeparatorText("render");

		ShowSceneMenu();
		ShowGizmoMenu();
		ShowColliderMenu();
		ShowCaptureMenu();
		
		ImGui::EndMenu();
	}
}

void RenderSceneEditor::ShowWindow() {
	UpdateKeyShortcut();

	ShowGameWindow();
	ShowCanvasWindow();
	ShowSceneWindow();
}

void RenderSceneEditor::Render() {
	if (!BaseEditor::IsDisplay()) {
		return;
	}

	if (!isRender_) {
		return;
	}

	config_.colorSpace = SxavengerSystem::GetMainWindow()->GetColorSpace();
	renderer_->Render(SxavengerSystem::GetDirectQueueContext(), config_);

	//* Debug Render *//
	textures_->BeginRenderTargetMainTransparent(SxavengerSystem::GetDirectQueueContext());

	CameraComponent* camera = camera_->GetComponent<CameraComponent>();

	if (isRenderGrid_) {
		SxavengerContent::PushGrid(camera, { 64, 64 }, 64);
	}
	
	if (isRenderCollider_) {
		colliderRenderer_->Render(SxavengerSystem::GetDirectQueueContext(), camera_->GetComponent<CameraComponent>());
	}

	if (isMoveCamera_) {
		SxavengerContent::PushAxis(point_, 1.0f);
	}

	SxavengerContent::GetDebugPrimitive()->DrawToScene(SxavengerSystem::GetDirectQueueContext(), camera);

	textures_->EndRenderTargetMainTransparent(SxavengerSystem::GetDirectQueueContext());
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

		// render
		ImGui::Checkbox("render scene", &isRender_);

		ImGui::BeginDisabled(!isRender_);

		// layout display
		ImGui::Text("layout");
		ImGui::Separator();

		if (ImGui::BeginCombo("GBuffer", magic_enum::enum_name(buffer_).data())) {
		
			for (const auto& [value, name] : magic_enum::enum_entries<GBuffer>()) {
				if (ImGui::Selectable(name.data(), (value == buffer_))) {
					buffer_ = value;
				}
			}
		
			ImGui::EndCombo();
		}

		SxImGui::HelpMarker("(!)", "[alt] + [up] || [down]");

		// process
		ImGui::Text("process");
		ImGui::Separator();
		ImGui::Checkbox("enable post process", &config_.isEnablePostProcess);
		ImGui::Checkbox("enable tonemap",      &config_.isElableTonemap);

		ImGui::Text("lighting");
		ImGui::Separator();
		ImGui::Checkbox("enable indirect lighting", &config_.isEnableIndirectLighting);

		if (ImGui::Button("reset resourviour")) {
			renderer_->ResetReservoir();
		}

		ImGui::BeginDisabled(!config_.isEnableIndirectLighting);
		ImGui::Text("sample count: %u", renderer_->GetReservoirSampleCount());
		renderer_->DebugGui(); //!< HACK
		ImGui::EndDisabled();

		ImGui::EndDisabled();
		
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

void RenderSceneEditor::ShowCaptureMenu() {
	if (ImGui::BeginMenu("capture")) {
		MenuPadding();
		ImGui::SeparatorText("capture");

		if (ImGui::Button("scene window capture")) {
			TextureExporter::Export(
				SxavengerSystem::GetDirectQueueContext(),
				TextureExporter::TextureDimension::Texture2D,
				textures_->GetGBuffer(FMainGBuffer::Layout::Scene)->GetResource(),
				DxObject::kDefaultScreenViewFormat,
				"capture_scene.png"
			);
		}
		
		//if (ImGui::Button("game window capture")) {
		//	FMainRender::GetInstance()->GetTextures()->CaptureGBuffer(FRenderTargetTextures::GBufferLayout::Main, SxavengerSystem::GetDirectQueueContext(), "capture_game.png");
		//}

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
		ImGui::PushStyleVarX(ImGuiStyleVar_ItemSpacing, 0);

		static ImVec4 kSelectedColor = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
		static ImVec4 kNonSelectedColor = ImVec4(0.4f, 0.4f, 0.4f, 1.0f);

		//* gizmo operation menu *//
		
		//* translate
		if (SxImGui::ImageButton(
			"## gizmo translate",
			operationTexture_[static_cast<uint32_t>(GuizmoOperation::Translate)].WaitGet()->GetGPUHandleSRV().ptr,
			{ 16, 16 },
			gizmoOperation_ == GuizmoOperation::Translate ? kSelectedColor : kNonSelectedColor)) {
			gizmoOperation_ = GuizmoOperation::Translate;
		}

		//* rotate
		if (SxImGui::ImageButton(
			"## gizmo rotate",
			operationTexture_[static_cast<uint32_t>(GuizmoOperation::Rotate)].WaitGet()->GetGPUHandleSRV().ptr,
			{ 16, 16 },
			gizmoOperation_ == GuizmoOperation::Rotate ? kSelectedColor : kNonSelectedColor)) {
			gizmoOperation_ = GuizmoOperation::Rotate;
		}

		//* scale
		if (SxImGui::ImageButton(
			"## gizmo scale",
			operationTexture_[static_cast<uint32_t>(GuizmoOperation::Scale)].WaitGet()->GetGPUHandleSRV().ptr,
			{ 16, 16 },
			gizmoOperation_ == GuizmoOperation::Scale ? kSelectedColor : kNonSelectedColor)) {
			gizmoOperation_ = GuizmoOperation::Scale;
		}

		ImGui::Dummy({ 8, 0 });
		ImGui::Separator();
		ImGui::Dummy({ 8, 0 });

		//* gizmo mode menu *//

		//* world
		if (SxImGui::ImageButton(
			"## gizmo world",
			modeTexture_[SxImGuizmo::World].WaitGet()->GetGPUHandleSRV().ptr,
			{ 16, 16 },
			gizmoMode_ == SxImGuizmo::World ? kSelectedColor : kNonSelectedColor)) {
			gizmoMode_ = SxImGuizmo::World;
		}

		//* local
		if (SxImGui::ImageButton(
			"## gizmo local",
			modeTexture_[SxImGuizmo::Local].WaitGet()->GetGPUHandleSRV().ptr,
			{ 16, 16 },
			gizmoMode_ == SxImGuizmo::Local ? kSelectedColor : kNonSelectedColor)) {
			gizmoMode_ = SxImGuizmo::Local;
		}

		ImGui::Dummy({ 8, 0 });
		ImGui::Separator();
		ImGui::Dummy({ 8, 0 });

		//* grid menu *//

		if (SxImGui::ImageButton(
			"## grid",
			gridTexture_.WaitGet()->GetGPUHandleSRV().ptr,
			{ 16, 16 },
			isRenderGrid_ ? kSelectedColor : kNonSelectedColor)) {
			isRenderGrid_ = !isRenderGrid_;
		}

		ImGui::PopStyleVar();
		ImGui::EndMenuBar();
	}

	sceneWindow_ = ImGui::GetWindowDrawList();

	sceneRect_ = SetImGuiImageFullWindow(
		checkerboard_.WaitGet()->GetGPUHandleSRV(),
		textures_->GetSize()
	);

	DisplayGBufferTexture(buffer_);

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

	//* render scene information *//

	ShowIconScene();
	ShowInfoTextScene();
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
		FMainRender::GetInstance()->GetTextures()->GetGBuffer(FMainGBuffer::Layout::Scene)->GetGPUHandleSRV(),
		kMainWindowSize
	);

	SetImGuiImageFullWindow(
		FMainRender::GetInstance()->GetTextures()->GetGBuffer(FMainGBuffer::Layout::UI)->GetGPUHandleSRV(),
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

	canvasRect_ = SetImGuiImageFullWindow(
		checkerboard_.WaitGet()->GetGPUHandleSRV(),
		kMainWindowSize
	);

	SetImGuiImageFullWindow(
		FMainRender::GetInstance()->GetTextures()->GetGBuffer(FMainGBuffer::Layout::UI)->GetGPUHandleSRV(),
		kMainWindowSize
	);

	ImGui::End();
	ImGui::PopStyleVar();
}

void RenderSceneEditor::ShowIconScene() {
	if (sceneWindow_ == nullptr) {
		return;
	}

	// Post Process Layer
	sComponentStorage->ForEach<PostProcessLayerComponent>([&](PostProcessLayerComponent* component) {
		
		auto transform = component->GetTransform();

		if (transform == nullptr) {
			return;
		}

		Color4f color = component->IsActive()
			? Color4f{ 1.0f, 1.0f, 1.0f, 1.0f }
			: Color4f{ 0.2f, 0.2f, 0.2f, 1.0f };

		RenderIcon(component->GetBehaviour(), Icon::Volume, transform->GetPosition(), color);
	});

	// Directional Light
	sComponentStorage->ForEach<DirectionalLightComponent>([&](DirectionalLightComponent* component) {

		Color4f color = component->IsActive()
			? Color4f(component->GetParameter().color, 1.0f)
			: Color4f{ 0.2f, 0.2f, 0.2f, 1.0f };

		RenderIcon(component->GetBehaviour(), Icon::DirectionalLight, component->RequireTransform()->GetPosition(), color);
	});

	// Point Light
	sComponentStorage->ForEach<PointLightComponent>([&](PointLightComponent* component) {

		Color4f color = component->IsActive()
			? Color4f(component->GetParameter().color, 1.0f)
			: Color4f{ 0.2f, 0.2f, 0.2f, 1.0f };

		RenderIcon(component->GetBehaviour(), Icon::PointLight, component->RequireTransform()->GetPosition(), color);
	});

	// Camera
	sComponentStorage->ForEach<CameraComponent>([&](CameraComponent* component) {
		if (component == camera_->GetComponent<CameraComponent>()) {
			return; //!< editor cameraは無視
		}

		Color4f color = component->IsActive()
			? Color4f{ 1.0f, 1.0f, 1.0f, 1.0f }
		: Color4f{ 0.2f, 0.2f, 0.2f, 1.0f };

		RenderIcon(component->GetBehaviour(), Icon::Camera, Matrix4x4::GetTranslation(component->GetCamera().world), color);
	});

}

void RenderSceneEditor::ShowInfoTextScene() {
	if (sceneWindow_ == nullptr) {
		return;
	}

	static const ImVec2 kPadding = { 4.0f, 4.0f };

	ImVec2 position = { sceneRect_.pos.x + kPadding.x, sceneRect_.pos.y + sceneRect_.size.y - kPadding.y };

	switch (config_.technique) {
		case FSceneRenderer::GraphicsTechnique::Deferred: //!< deferred rendering
			RenderTextSceneWindow(position, std::format("GBuffer | {}", magic_enum::enum_name(buffer_)));
			RenderTextSceneWindow(position, "> Deferred Rendering");
			break;

		case FSceneRenderer::GraphicsTechnique::Pathtracing: //!< path tracing
			//RenderTextSceneWindow(position, std::format("sample count: {}", renderer_->GetCurrentSampleCount()));
			RenderTextSceneWindow(position, "> Path Tracing (Preview)");
			break;
	}
	
}

void RenderSceneEditor::UpdateKeyShortcut() {

	// bufferの切り替え
	if (SxavengerSystem::IsPressKey(KeyId::KEY_LALT) && SxavengerSystem::IsTriggerKey(KeyId::KEY_UP)) { //!< left alt + Up
		if (buffer_ > GBuffer::Scene) {
			buffer_ = static_cast<GBuffer>(static_cast<uint32_t>(buffer_) - 1);
		}
	}

	if (SxavengerSystem::IsPressKey(KeyId::KEY_LALT) && SxavengerSystem::IsTriggerKey(KeyId::KEY_DOWN)) { //!< left alt + Down
		if (buffer_ < GBuffer::Indirect_Reservoir) {
			buffer_ = static_cast<GBuffer>(static_cast<uint32_t>(buffer_) + 1);
		}

	}

	if (SxavengerSystem::IsPressKey(KeyId::KEY_LALT) && SxavengerSystem::IsTriggerKey(KeyId::KEY_1)) { //!< left alt + 1
		buffer_ = GBuffer::Scene;
	}

	if (SxavengerSystem::IsPressKey(KeyId::KEY_LALT) && SxavengerSystem::IsTriggerKey(KeyId::KEY_2)) { //!< left alt + 2
		buffer_ = GBuffer::Deferred_GBuffer;
	}

	if (SxavengerSystem::IsPressKey(KeyId::KEY_LALT) && SxavengerSystem::IsTriggerKey(KeyId::KEY_3)) { //!< left alt + 3
		buffer_ = GBuffer::Lighting_GBuffer;
	}

}

RenderSceneEditor::WindowRect RenderSceneEditor::SetImGuiImageFullWindow(const D3D12_GPU_DESCRIPTOR_HANDLE& handle, const Vector2ui& size) const {

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

void RenderSceneEditor::SetImGuiImageFullWindowEnable(const D3D12_GPU_DESCRIPTOR_HANDLE& handle, const Vector2ui& size, bool isEnable) {

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

	ImVec4 tint = isEnable ? ImVec4(1.0f, 1.0f, 1.0f, 1.0f) : ImVec4(0.2f, 0.2f, 0.2f, 1.0f);

	// 画像の描画
	ImGui::SetCursorPos(leftTop);
	ImGui::ImageWithBg(
		handle.ptr, displayTextureSize,
		{ 0.0f, 0.0f }, { 1.0f, 1.0f },
		{ 0.0f, 0.0f, 0.0f, 0.0f },
		tint
	);

}

void RenderSceneEditor::SetImGuiImagesFullWindowEnable(const std::vector<std::pair<D3D12_GPU_DESCRIPTOR_HANDLE, GBuffer>>& handles, const Vector2ui& size, bool isEnable) {

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

	// x軸でhandlesの数だけ分割
	const float kWidthEvery = displayTextureSize.x / static_cast<float>(handles.size());
	const float kTexcoordEvery    = 1.0f / static_cast<float>(handles.size());

	ImVec4 tint = isEnable ? ImVec4(1.0f, 1.0f, 1.0f, 1.0f) : ImVec4(0.2f, 0.2f, 0.2f, 1.0f);

	for (size_t i = 0; i < handles.size(); ++i) {

		// 出力場所の調整
		ImVec2 leftTop = {
			(windowSize.x - displayTextureSize.x) * 0.5f + regionMin.x + kWidthEvery * i,
			(windowSize.y - displayTextureSize.y) * 0.5f + regionMin.y,
		};

		// imageの描画
		ImGui::SetCursorPos(leftTop);
		ImGui::ImageWithBg(
			handles[i].first.ptr, { kWidthEvery, displayTextureSize.y },
			{ kTexcoordEvery * i, 0.0f }, { kTexcoordEvery * (i + 1), 1.0f },
			{ 0.0f, 0.0f, 0.0f, 0.0f },
			tint
		);

		// textの描画
		ImGui::SetCursorPos(leftTop);
		SxImGui::TextClipped(magic_enum::enum_name(handles[i].second).data(), kWidthEvery);

		// 選択されたら切り替え
		ImGui::SetCursorPos(leftTop);
		ImGui::InvisibleButton(magic_enum::enum_name(handles[i].second).data(), { kWidthEvery, displayTextureSize.y });

		if (SxImGui::IsDoubleClickItem()) {
			buffer_ = handles[i].second;
		}
	}
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

void RenderSceneEditor::ShowCameraInfomation(const WindowRect& rect) {

	ImVec2 cursol = ImGui::GetCursorPos();

	ImGui::SetCursorPos({ rect.pos.x + rect.size.x * 0.5f, rect.pos.y + rect.size.y * 0.5f });
	ImGui::Text("TestA");
	ImGui::Text("TestB");

	ImGui::SetCursorPos(cursol);
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

void RenderSceneEditor::DisplayGBufferTexture(GBuffer buffer) {
	switch (buffer) {
		case GBuffer::Scene:
			SetImGuiImageFullWindowEnable(
				textures_->GetGBuffer(FMainGBuffer::Layout::Scene)->GetGPUHandleSRV(),
				textures_->GetSize(),
				isRender_
			);
			break;

		case GBuffer::Deferred_GBuffer:
			SetImGuiImagesFullWindowEnable(
				{
					{ textures_->GetGBuffer(FDeferredGBuffer::Layout::Albedo)->GetGPUHandleSRV(),      GBuffer::Albedo },
					{ textures_->GetGBuffer(FDeferredGBuffer::Layout::Normal)->GetGPUHandleSRV(),      GBuffer::Normal },
					{ textures_->GetGBuffer(FDeferredGBuffer::Layout::MaterialARM)->GetGPUHandleSRV(), GBuffer::MaterialARM },
					{ textures_->GetGBuffer(FDeferredGBuffer::Layout::Position)->GetGPUHandleSRV(),    GBuffer::Position },
				},
				textures_->GetSize(),
				isRender_
			);
			break;

		case GBuffer::Lighting_GBuffer:
			SetImGuiImagesFullWindowEnable(
				{
					{ textures_->GetGBuffer(FLightingGBuffer::Layout::Direct)->GetGPUHandleSRV(),   GBuffer::Direct },
					{ textures_->GetGBuffer(FLightingGBuffer::Layout::Indirect)->GetGPUHandleSRV(), GBuffer::Indirect },
				},
				textures_->GetSize(),
				isRender_
			);
			break;

		case GBuffer::Albedo:
			SetImGuiImageFullWindowEnable(
				textures_->GetGBuffer(FDeferredGBuffer::Layout::Albedo)->GetGPUHandleSRV(),
				textures_->GetSize(),
				isRender_
			);
			break;

		case GBuffer::Normal:
			SetImGuiImageFullWindowEnable(
				textures_->GetGBuffer(FDeferredGBuffer::Layout::Normal)->GetGPUHandleSRV(),
				textures_->GetSize(),
				isRender_
			);
			break;

		case GBuffer::MaterialARM:
			SetImGuiImageFullWindowEnable(
				textures_->GetGBuffer(FDeferredGBuffer::Layout::MaterialARM)->GetGPUHandleSRV(),
				textures_->GetSize(),
				isRender_
			);
			break;

		case GBuffer::Position:
			SetImGuiImageFullWindowEnable(
				textures_->GetGBuffer(FDeferredGBuffer::Layout::Position)->GetGPUHandleSRV(),
				textures_->GetSize(),
				isRender_
			);
			break;

		case GBuffer::Direct:
			SetImGuiImageFullWindowEnable(
				textures_->GetGBuffer(FLightingGBuffer::Layout::Direct)->GetGPUHandleSRV(),
				textures_->GetSize(),
				isRender_
			);
			break;

		case GBuffer::Indirect:
			SetImGuiImageFullWindowEnable(
				textures_->GetGBuffer(FLightingGBuffer::Layout::Indirect)->GetGPUHandleSRV(),
				textures_->GetSize(),
				isRender_
			);
			break;

		case GBuffer::Indirect_Reservoir:
			SetImGuiImageFullWindowEnable(
				textures_->GetGBuffer(FLightingGBuffer::Layout::Indirect_Reservoir)->GetGPUHandleSRV(),
				textures_->GetSize(),
				isRender_
			);
			break;

	}
}

void RenderSceneEditor::RenderIcon(BaseInspector* inspector, Icon icon, const Vector3f& position, const Color4f& color) {
	if (sceneWindow_ == nullptr) {
		return;
	}

	Vector3f ndc = camera_->GetComponent<CameraComponent>()->CalculateNDCPosition(position);

	if (Any(ndc < Vector3(-1.0f, -1.0f, 0.0f)) || Any(ndc > Vector3(1.0f, 1.0f, 1.0f))) {
		return;
		// FIXME: size込みの判定に変更する
	}

	Vector2f screen = {
		(ndc.x + 1.0f) * 0.5f * sceneRect_.size.x,
		(1.0f - ndc.y) * 0.5f * sceneRect_.size.y
	};

	const Vector2f kOffset = iconSize_ / 2.0f;

	ScreenRect rect = {};
	rect.min = { sceneRect_.pos.x + screen.x - kOffset.x, sceneRect_.pos.y + screen.y - kOffset.y };
	rect.max = { sceneRect_.pos.x + screen.x + kOffset.x, sceneRect_.pos.y + screen.y + kOffset.y };

	// icon shadow
	sceneWindow_->AddImage(
		icons_[static_cast<uint32_t>(icon)].WaitGet()->GetGPUHandleSRV().ptr,
		{ rect.min.x, rect.min.y + 1.0f },
		{ rect.max.x, rect.max.y + 1.0f },
		ImVec2{ 0.0f, 0.0f },
		ImVec2{ 1.0f, 1.0f },
		ImColor{ 0.01f, 0.01f, 0.01f, color.a * 0.8f }
	);

	// main icon
	sceneWindow_->AddImage(
		icons_[static_cast<uint32_t>(icon)].WaitGet()->GetGPUHandleSRV().ptr,
		{ rect.min.x, rect.min.y },
		{ rect.max.x, rect.max.y },
		ImVec2{ 0.0f, 0.0f },
		ImVec2{ 1.0f, 1.0f },
		ImColor{ color.r, color.g, color.b, color.a }
	);

	auto editor = BaseEditor::GetEditorEngine()->GetEditor<InspectorEditor>();

	if (inspector == nullptr || editor == nullptr) {
		return; //!< 選択不可能(inspectorが未設定, editorが存在しない)
	}

	// rectの調整
	rect.min = Vector2f::Max(rect.min, sceneRect_.pos);
	rect.max = Vector2f::Min(rect.max, sceneRect_.pos + sceneRect_.size);

	// mouseクリックでInspectorに設定
	if (SxImGui::IsMouseClickedRect({ rect.min.x, rect.min.y }, { rect.max.x, rect.max.y }, ImGuiMouseButton_Left)) {
		editor->SetInspector(inspector);
	}
}

void RenderSceneEditor::RenderTextSceneWindow(ImVec2& position, const std::string& text, ImU32 color) {

	ImVec2 size = ImGui::CalcTextSize(text.c_str());
	position.y -= size.y;

	sceneWindow_->AddText(ImVec2(position.x, position.y), color, text.c_str());
}
