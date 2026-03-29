#include "RenderSceneEditor.h"
SXAVENGER_ENGINE_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* editor
#include "../EditorEngine.h"

//* engine [system]
#include <Engine/System/Configuration/Configuration.h>
#include <Engine/System/UI/SxImGui.h>
#include <Engine/System/UI/SxImGuizmo.h>
#include <Engine/System/DirectX/DirectXPixEvent.h>

//* engine [graphics]
#include <Engine/Graphics/Graphics.h>

//* engine [assets]
#include <Engine/Assets/Asset/AssetStorage.h>
#include <Engine/Assets/Content/ContentStorage.h>

//* engine [components]
#include <Engine/Components/Component/Transform/TransformComponent.h> 
#include <Engine/Components/Component/Transform/RectTransformComponent.h>
#include <Engine/Components/Component/Camera/CameraComponent.h>
#include <Engine/Components/Component/Light/Punctual/DirectionalLightComponent.h>
#include <Engine/Components/Component/Light/Punctual/PointLightComponent.h>
#include <Engine/Components/Component/Light/Punctual/SpotLightComponent.h>
#include <Engine/Components/Component/Light/Rect/RectLightComponent.h>
#include <Engine/Components/Component/PostProcessLayer/PostProcessLayerComponent.h>
#include <Engine/Components/Component/ComponentHelper.h>
#include <Engine/Components/Entity/EntityBehaviour.h>
#include <Engine/Components/Entity/BehaviourHelper.h>

//* engine [module]
#include <Engine/Module/Exporter/TextureExporter.h>

//* engine [render]
#include <Engine/Render/Buffer/FMainBuffer.h>
#include <Engine/Render/Buffer/FGBuffer.h>
#include <Engine/Render/Buffer/FLightAccumulationBuffer.h>
#include <Engine/Render/Buffer/FTransparentBuffer.h>
#include <Engine/Render/Core/FRenderCore.h>
#include <Engine/Render/FMainRender.h>
#include <Engine/Render/FPresenter.h>

//* lib
#include <Lib/Geometry/VectorComparison.h>

//* externals
#include <magic_enum.hpp>

////////////////////////////////////////////////////////////////////////////////////////////
// RenderSceneEditor class methods
////////////////////////////////////////////////////////////////////////////////////////////

void RenderSceneEditor::Init() {

	checkerboard_
		= sContentStorage->Import<ContentTexture>(
			"packages/textures/checker_black.png",
			ContentTexture::Option{ .isGenerateMipmap = false, .isCompress = false }
		)->GetId();

	operationTexture_[static_cast<uint32_t>(GuizmoOperation::Translate)]
		= sContentStorage->Import<ContentTexture>(
			"packages/textures/icon/operation_translate.png",
			ContentTexture::Option{ .isGenerateMipmap = false, .isCompress = false }
		)->GetId();

	operationTexture_[static_cast<uint32_t>(GuizmoOperation::Rotate)]
		= sContentStorage->Import<ContentTexture>(
			"packages/textures/icon/operation_rotate.png",
			ContentTexture::Option{ .isGenerateMipmap = false, .isCompress = false }
		)->GetId();

	operationTexture_[static_cast<uint32_t>(GuizmoOperation::Scale)]
		= sContentStorage->Import<ContentTexture>(
			"packages/textures/icon/operation_scale.png",
			ContentTexture::Option{ .isGenerateMipmap = false, .isCompress = false }
		)->GetId();

	modeTexture_[SxImGuizmo::World]
		= sContentStorage->Import<ContentTexture>(
			"packages/textures/icon/mode_world.png",
			ContentTexture::Option{ .isGenerateMipmap = false, .isCompress = false }
		)->GetId();

	modeTexture_[SxImGuizmo::Local]
		= sContentStorage->Import<ContentTexture>(
			"packages/textures/icon/mode_local.png",
			ContentTexture::Option{ .isGenerateMipmap = false, .isCompress = false }
		)->GetId();

	gridTexture_
		= sContentStorage->Import<ContentTexture>(
			"packages/textures/icon/grid.png",
			ContentTexture::Option{ .isGenerateMipmap = false, .isCompress = false }
		)->GetId();

	camera_ = std::make_unique<PerspectiveCameraActor>();
	camera_->SetPerspective(PerspectiveCameraActor::Perspective::ThirdPerson);
	camera_->SetPoint(kOrigin3<float>);
	camera_->SetDistance(12.0f);
	camera_->SetAngle({ 0.0f, kPi / 16.0f });
	camera_->Update();

	(*camera_)->SetName("editor camera");

	auto camera = (*camera_)->GetComponent<CameraComponent>();
	camera->SetTag(CameraComponent::Tag::Editor);
	camera->GetProjection().focal = 16.0f;
	camera->UpdateProj();
	UpdateView();

	buffer_ = std::make_unique<FRenderTargetBuffer>();
	buffer_->Init(Configuration::GetConfig().resolution);

	config_ = {};
	config_.name   = "Editor";
	config_.buffer = buffer_.get();
	config_.tag    = CameraComponent::Tag::Editor;
	config_.option = FRenderConfig::OptionFlag::Tonemap;

	icons_[static_cast<uint32_t>(Icon::Volume)]           = sContentStorage->Import<ContentTexture>("packages/textures/icon/scene_volume.png")->GetId();
	icons_[static_cast<uint32_t>(Icon::DirectionalLight)] = sContentStorage->Import<ContentTexture>("packages/textures/icon/scene_directionalLight.png")->GetId();
	icons_[static_cast<uint32_t>(Icon::PointLight)]       = sContentStorage->Import<ContentTexture>("packages/textures/icon/scene_pointLight.png")->GetId();
	icons_[static_cast<uint32_t>(Icon::SpotLight)]        = sContentStorage->Import<ContentTexture>("packages/textures/icon/scene_spotLight.png")->GetId();
	icons_[static_cast<uint32_t>(Icon::RectLight)]        = sContentStorage->Import<ContentTexture>("packages/textures/icon/scene_rectLight.png")->GetId();
	icons_[static_cast<uint32_t>(Icon::Camera)]           = sContentStorage->Import<ContentTexture>("packages/textures/icon/scene_camera.png")->GetId();
	
	{
		selectLine_.CreateBlob(kPackagesDirectory / L"shaders/render/geometry/line/Line.vs.hlsl", DxObject::GraphicsShaderType::Vertex);
		selectLine_.CreateBlob(kPackagesDirectory / L"shaders/render/geometry/line/Line.gs.hlsl", DxObject::GraphicsShaderType::Geometry);
		selectLine_.CreateBlob(kPackagesDirectory / L"shaders/render/geometry/line/Line.ps.hlsl", DxObject::GraphicsShaderType::Pixel);
		selectLine_.ReflectionRootSignature(System::GetDxDevice());

		DxObject::GraphicsPipelineDesc desc = {};
		desc.CreateDefaultDesc();

		desc.SetDepthStencil(true, D3D12_DEPTH_WRITE_MASK_ZERO, D3D12_COMPARISON_FUNC_LESS_EQUAL);

		desc.SetRTVFormat(0, FBaseBuffer::kColorFormat);
		desc.SetBlendMode(0, BlendMode::Normal_AlphaMax);

		selectLine_.CreatePipeline(System::GetDxDevice(), desc);

		picker_.CreateBlob(kPackagesDirectory / L"shaders/editor/Picker.cs.hlsl");
		picker_.ReflectionPipeline(System::GetDxDevice());
	}
}

void RenderSceneEditor::ShowMainMenu() {
	if (ImGui::BeginMenu("render")) {
		MenuPadding();
		ImGui::SeparatorText("render");

		ShowSceneMenu();
		ShowGameMenu();
		ShowGizmoMenu();
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

	auto context = System::GetDirectQueueContext();

	config_.colorSpace = System::GetMainWindow()->GetColorSpace();
	config_.cullCamera = isDebugCulling_ ? ComponentHelper::GetCameraComponent(CameraComponent::Tag::Game) : nullptr;

	FMainRender::GetInstance()->GetContext().Render(context, config_);

	//* Debug Render Scene *//
	context->BeginEvent(L"RenderSceneEditor | Debug Render");

	{ //!< Debug Render Scene

		buffer_->BeginRenderTargetMainScene(context);

		CameraComponent* camera = ComponentHelper::GetCameraComponent(CameraComponent::Tag::Editor);

		if (isRenderGrid_) {
			Graphics::PushGrid(camera->GetCamera().world, camera->GetCamera().projInv, { 64, 64 }, 64);
			//!< Gridの描画
			// TODO: Shader側で無限Gridにする. (Unreal Engineを参考にして)
		}

		if (camera == camera_->GetBehaviour()->GetComponent<CameraComponent>()) {
			if (isMoveCamera_ && camera_->GetPerspective() == PerspectiveCameraActor::Perspective::ThirdPerson && camera_->GetDistance() > 0.0f) {
				Graphics::PushAxis(camera_->GetPoint(), 1.0f);
				//!< カメラが3人称視点で移動している場合は、カメラの位置に軸を描画.
			}
		}

		

		Graphics::GetDebugPrimitive()->DrawToScene(context, camera->GetGPUVirtualAddress());
		//!< Debug Primitiveの描画

		selectLine_.SetPipeline(context->GetDxCommand());
		RenderInspector(context, ComponentHelper::GetCameraComponent(CameraComponent::Tag::Editor));
		//!< 選択中のオブジェクトのInspectorを描画

		buffer_->EndRenderTargetMainScene(context);
	}

	context->EndEvent();

	if (!sceneWindow_.expired()) { //!< Scene Windowが存在している場合はScene Windowに描画
		context->BeginEvent(L"RenderSceneEditor | Present Scene Window");

		FMainBuffer* main = buffer_->GetBuffer<FMainBuffer>();
		auto window       = sceneWindow_.lock();

		window->BeginRenderWindow(context);
		FPresenter::Present(context, window->GetClient(), main->GetBuffer(FMainBuffer::Layout::Scene).GetGPUHandleSRV());
		window->EndRenderWindow(context);

		context->EndEvent();
	}
}

void RenderSceneEditor::Manipulate(EntityBehaviour* behaviour) {
	if (gizmoUsed_.has_value() && gizmoUsed_.value() != GuizmoUsed::Scene) {
		return;
	}

	SxImGuizmo::SetDrawlist(sceneWindowDrawer_);
	SxImGuizmo::SetOrthographic(false);

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

	SxImGuizmo::Enable(!component->HasParent());

	SxImGuizmo::GizmoOutput output = {};

	bool isEdit = SxImGuizmo::Manipulate(
		reinterpret_cast<const float*>((*camera_)->GetComponent<CameraComponent>()->GetCamera().view.m.data()),
		reinterpret_cast<const float*>((*camera_)->GetComponent<CameraComponent>()->GetCamera().proj.m.data()),
		reinterpret_cast<float*>(m.m.data()),
		output,
		operation,
		gizmoMode_
	);

	SxImGuizmo::Enable(true);

	gizmoUsed_ = SxImGuizmo::IsUsing() ? std::make_optional(GuizmoUsed::Scene) : std::nullopt;

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

void RenderSceneEditor::ManipulateCanvas(EntityBehaviour* behaviour) {
	if (gizmoUsed_.has_value() && gizmoUsed_.value() != GuizmoUsed::Canvas) {
		return;
	}

	SxImGuizmo::SetDrawlist(canvasWindowDrawer_);
	SxImGuizmo::SetOrthographic(true);

	SxImGuizmo::Operation operation = SxImGuizmo::NONE;

	// todo: flagに変更
	if (gizmoOperation_ == GuizmoOperation::Scale) {
		operation = SxImGuizmo::SCALE_X | SxImGuizmo::SCALE_Y;
	}

	if (gizmoOperation_ == GuizmoOperation::Translate) {
		operation = SxImGuizmo::TRANSLATE_X | SxImGuizmo::TRANSLATE_Y;
	}

	if (gizmoOperation_ == GuizmoOperation::Rotate) {
		operation = SxImGuizmo::ROTATE_Z;
	}

	// rect transform component の取得
	auto component = behaviour->GetComponent<RectTransformComponent>();

	if (component == nullptr) {
		return;
	}

	SxImGuizmo::SetRect({ canvasRect_.pos.x, canvasRect_.pos.y }, { canvasRect_.size.x, canvasRect_.size.y });

	Matrix4x4 m = component->GetTransform().ToMatrix();

	SxImGuizmo::Enable(!component->HasParent());

	SxImGuizmo::GizmoOutput output = {};

	static const Matrix4x4 view = Matrix4x4::Identity();
	static const Matrix4x4 proj = Matrix4x4::Orthographic(0.0f, 0.0f, static_cast<float>(buffer_->GetResolution().x), static_cast<float>(buffer_->GetResolution().y), 0.0f, 128.0f);

	bool isEdit = SxImGuizmo::Manipulate(
		reinterpret_cast<const float*>(view.m.data()),
		reinterpret_cast<const float*>(proj.m.data()),
		reinterpret_cast<float*>(m.m.data()),
		output,
		operation,
		SxImGuizmo::Mode::World
	);

	SxImGuizmo::Enable(true);

	gizmoUsed_ = SxImGuizmo::IsUsing() ? std::make_optional(GuizmoUsed::Canvas) : std::nullopt;

	if (component->HasParent()) {
		return;
	}

	if (!isEdit) {
		return;
	}

	switch (output.type) {
		case SxImGuizmo::GizmoOutput::OutputType::Translation:
			component->GetTransform().translate += { output.value.x, output.value.y };
			break;

		case SxImGuizmo::GizmoOutput::OutputType::Scale:
			component->GetTransform().scale = { output.value.x, output.value.y };
			break;

		// todo: rotate
	}

	component->UpdateMatrix();
}

void RenderSceneEditor::SetCameraPoint(const Vector3f& point) {
	camera_->SetPoint(point);
	camera_->UpdateTransform();
	UpdateView();
}

void RenderSceneEditor::ShowSceneMenu() {
	if (ImGui::BeginMenu("scene")) {
		BaseEditor::MenuPadding();
		ImGui::SeparatorText("scene");

		// render
		ImGui::Checkbox("render scene", &isRender_);

		ImGui::BeginDisabled(!isRender_);

		// layout display
		ImGui::Text("layout");
		ImGui::Separator();

		SxGui::ComboEnum("Display Buffer", &displayBuffer_);

		SxImGui::HelpMarker("(!)", "[alt] + [up] || [down]");

		ImGui::EndDisabled();

		// process
		ImGui::Text("process option");
		ImGui::Separator();

		SxGui::ComboEnum("anti-aliasing",       &config_.antiAliasing);
		SxGui::ComboEnum("ambient-occlusion",   &config_.ambientOcclusion);
		SxGui::ComboEnum("global-illumination", &config_.globalIllumination);

		for (const auto& [value, name] : magic_enum::enum_entries<FRenderConfig::OptionFlag>()) {
			if (value == FRenderConfig::OptionFlag::Default) {
				continue;
			}
			SxImGui::CheckBoxFlags(name.data(), &config_.option.Get(), static_cast<size_t>(value));
		}

		ImGui::Text("debug option");
		ImGui::Separator();

		ImGui::Checkbox("cull camera",  &isDebugCulling_);
		ImGui::Checkbox("render grid",  &isRenderGrid_);
		ImGui::Checkbox("render probe", &isRenderProbe_);

		// window
		ImGui::Text("window");
		ImGui::Separator();

		if (sceneWindow_.expired()) { //!< windowが表示されていない場合.
			if (ImGui::Button("open")) {
				sceneWindow_ = System::CreateSubWindow(Configuration::GetConfig().resolution, L"Scene Window (Editor)", DirectXWindowContext::ProcessCategory::Window);
			}

		} else { //!< windowが表示されている場合
			if (ImGui::Button("close")) {
				sceneWindow_.lock()->Close();
				sceneWindow_.reset();
			}
			
		}
		
		ImGui::EndMenu();
	}
}

void RenderSceneEditor::ShowGameMenu() {
	if (ImGui::BeginMenu("game")) {
		BaseEditor::MenuPadding();
		ImGui::SeparatorText("game");

		FRenderConfig& config = FMainRender::GetInstance()->GetConfig();

		// process
		ImGui::Text("process");
		ImGui::Separator();

		SxGui::ComboEnum("anti-aliasing",       &config.antiAliasing);
		SxGui::ComboEnum("ambient-occlusion",   &config.ambientOcclusion);
		SxGui::ComboEnum("global-illumination", &config.globalIllumination);

		for (const auto& [value, name] : magic_enum::enum_entries<FRenderConfig::OptionFlag>()) {
			if (value == FRenderConfig::OptionFlag::Default) {
				continue;
			}
			SxImGui::CheckBoxFlags(name.data(), &config.option.Get(), static_cast<size_t>(value));
		}

		ImGui::EndMenu();
	}
}

void RenderSceneEditor::ShowGizmoMenu() {
	if (ImGui::BeginMenu("gizmo")) {
		BaseEditor::MenuPadding();
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

void RenderSceneEditor::ShowCaptureMenu() {
	if (ImGui::BeginMenu("capture")) {
		BaseEditor::MenuPadding();
		ImGui::SeparatorText("capture");

		const ImVec2 region = { BaseEditor::GetMenuPadding().x, ImGui::GetContentRegionAvail().y };

		ImGui::Text(std::format("{} Capture Window", SxGui::Icon::Capture).c_str());

		ImGui::BeginTable("## Capture Window", 2, ImGuiTableFlags_BordersInnerV);
		ImGui::TableNextRow();

		ImGui::TableSetColumnIndex(0); //!< Scene Window Capture
		if (buffer_->HasBuffer<FMainBuffer>()) {

			FMainBuffer* main   = buffer_->GetBuffer<FMainBuffer>();
			Vector2f resolution = main->GetBuffer(FMainBuffer::Layout::Scene).GetResolution();

			SxGui::Image(main->GetBuffer(FMainBuffer::Layout::Scene).GetGPUHandleSRV().ptr, { resolution.x, resolution.y }, { region.x * 0.5f, region.y });

			if (ImGui::Button("Scene Window Capture", { region.x * 0.5f, 0.0f })) {

				auto filepath = WinApp::GetSaveFilepath(L"画像(Scene-Capture)の保存先", std::filesystem::current_path(), { L"画像ファイル", L"*.png; *.jpg; *.hdr; *.tga; *.dds;" }, L".png");

				if (filepath.has_value()) {
					TextureExporter::Export(
						System::GetDirectQueueContext(),
						TextureExporter::TextureDimension::Texture2D,
						main->GetBuffer(FMainBuffer::Layout::Scene).GetResource(),
						DxObject::kDefaultScreenViewFormat,
						filepath.value()
					);
				}
			}

		} else {
			ImGui::Dummy({ region.x * 0.5f, region.y });
		}
		

		ImGui::TableSetColumnIndex(1); //!< Game Window Capture
		if (FMainRender::GetInstance()->GetBuffer()->HasBuffer<FMainBuffer>()) {

			FMainBuffer* main   = FMainRender::GetInstance()->GetBuffer()->GetBuffer<FMainBuffer>();
			Vector2f resolution = main->GetBuffer(FMainBuffer::Layout::Scene).GetResolution();

			SxGui::Image(main->GetBuffer(FMainBuffer::Layout::Scene).GetGPUHandleSRV().ptr, { resolution.x, resolution.y }, { region.x * 0.5f, region.y });

			if (ImGui::Button("Game Window Capture", { region.x * 0.5f, 0.0f })) {

				auto filepath = WinApp::GetSaveFilepath(L"画像(Game-Capture)の保存先", std::filesystem::current_path(), { L"画像ファイル", L"*.png; *.jpg; *.hdr; *.tga; *.dds;" }, L".png");

				if (filepath.has_value()) {
					TextureExporter::Export(
						System::GetDirectQueueContext(),
						TextureExporter::TextureDimension::Texture2D,
						main->GetBuffer(FMainBuffer::Layout::Scene).GetResource(),
						DxObject::kDefaultScreenViewFormat,
						filepath.value()
					);
				}
			}

		} else {
			ImGui::Dummy({ region.x * 0.5f, region.y });
		}
		

		ImGui::EndTable();

		ImGui::Dummy({ 0, ImGui::GetStyle().ItemSpacing.y });
		ImGui::Text(std::format("{} Capture Pix", SxGui::Icon::ControlCamera).c_str());

		if (ImGui::Button("Capture Next Frames", { ImGui::GetContentRegionAvail().x, 0.0f })) {

			auto filepath = WinApp::GetSaveFilepath(L"PIXの保存先", std::filesystem::current_path(), { L"PIXファイル", L"*.wpix;" }, L".wpix");

			if (filepath.has_value()) {
				DirectXPixEvent::CaptureNextFrames(filepath.value().filename(), 1);
			}
		}

		ImGui::EndMenu();
	}
}

void RenderSceneEditor::ShowSceneWindow() {

	std::string label = std::format("{} Scene ## Render Scene Editor", SxGui::Icon::Window);

	BaseEditor::SetNextWindowDocking();

	//* fix window style
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0, 0 });
	ImGui::Begin(label.c_str(), nullptr, BaseEditor::GetWindowFlag() | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_MenuBar);

	//* menu bar
	if (ImGui::BeginMenuBar()) {
		ImGui::PushStyleVarX(ImGuiStyleVar_ItemSpacing, 0);

		static ImVec4 kSelectedColor = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
		static ImVec4 kNonSelectedColor = ImVec4(0.4f, 0.4f, 0.4f, 1.0f);

		//* gizmo operation menu *//
		
		//* translate
		if (SxImGui::ImageButton(
			"## gizmo translate",
			operationTexture_[static_cast<uint32_t>(GuizmoOperation::Translate)].Get()->GetGPUHandleSRV().ptr,
			{ 16, 16 },
			gizmoOperation_ == GuizmoOperation::Translate ? kSelectedColor : kNonSelectedColor)) {
			gizmoOperation_ = GuizmoOperation::Translate;
		}

		//* rotate
		if (SxImGui::ImageButton(
			"## gizmo rotate",
			operationTexture_[static_cast<uint32_t>(GuizmoOperation::Rotate)].Get()->GetGPUHandleSRV().ptr,
			{ 16, 16 },
			gizmoOperation_ == GuizmoOperation::Rotate ? kSelectedColor : kNonSelectedColor)) {
			gizmoOperation_ = GuizmoOperation::Rotate;
		}

		//* scale
		if (SxImGui::ImageButton(
			"## gizmo scale",
			operationTexture_[static_cast<uint32_t>(GuizmoOperation::Scale)].Get()->GetGPUHandleSRV().ptr,
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
			modeTexture_[SxImGuizmo::World].Get()->GetGPUHandleSRV().ptr,
			{ 16, 16 },
			gizmoMode_ == SxImGuizmo::World ? kSelectedColor : kNonSelectedColor)) {
			gizmoMode_ = SxImGuizmo::World;
		}

		//* local
		if (SxImGui::ImageButton(
			"## gizmo local",
			modeTexture_[SxImGuizmo::Local].Get()->GetGPUHandleSRV().ptr,
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
			gridTexture_.Get()->GetGPUHandleSRV().ptr,
			{ 16, 16 },
			isRenderGrid_ ? kSelectedColor : kNonSelectedColor)) {
			isRenderGrid_ = !isRenderGrid_;
		}

		ImGui::PopStyleVar();
		ImGui::EndMenuBar();
	}

	sceneWindowDrawer_ = ImGui::GetWindowDrawList();

	sceneRect_ = SetImGuiImageFullWindow(
		checkerboard_.Get()->GetGPUHandleSRV(),
		buffer_->GetResolution()
	);

	ShowDisplayBuffer(displayBuffer_);

	if (ImGui::IsWindowHovered() && (ImGui::IsMouseClicked(ImGuiMouseButton_Middle) || ImGui::IsMouseClicked(ImGuiMouseButton_Right))) {
		//!< window hovered 状態で mouse middle click が押された場合, camera操作(focus)を許可.
		ImGui::SetWindowFocus();
	}

	isFocusSceneWindow_ = ImGui::IsWindowFocused();

	if (isFocusSceneWindow_) {
		UpdateCamera();
	}

	ImGui::End();
	ImGui::PopStyleVar();

	if (!SxImGuizmo::IsOver() && isFocusSceneWindow_) {
		PickMesh(System::GetDirectQueueContext(), sceneRect_);
	}

	//* render scene information *//

	ShowIconScene();
	ShowInfoTextScene();
}

void RenderSceneEditor::ShowGameWindow() {

	std::string label = std::format("{} Game ## Render Scene Editor", SxGui::Icon::Window);

	BaseEditor::SetNextWindowDocking();

	//* fix window style
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0, 0 });
	ImGui::Begin(label.c_str(), nullptr, BaseEditor::GetWindowFlag() | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

	SetImGuiImageFullWindow(
		checkerboard_.Get()->GetGPUHandleSRV(),
		Configuration::GetConfig().resolution
	);

	FMainBuffer* main = FMainRender::GetInstance()->GetBuffer()->GetBuffer<FMainBuffer>();

	SetImGuiImageFullWindow( //!< Scene Buffer の描画
		main->GetBuffer(FMainBuffer::Layout::Scene).GetGPUHandleSRV(),
		Configuration::GetConfig().resolution
	);

	SetImGuiImageFullWindow( //!< Canvas Buffer の描画
		main->GetBuffer(FMainBuffer::Layout::Canvas).GetGPUHandleSRV(),
		Configuration::GetConfig().resolution
	);

	isFocusGameWindow_ = ImGui::IsWindowFocused();

	ImGui::End();
	ImGui::PopStyleVar();
}

void RenderSceneEditor::ShowCanvasWindow() {

	std::string label = std::format("{} Canvas ## Render Scene Editor", SxGui::Icon::Layers);

	BaseEditor::SetNextWindowDocking();

	//* fix window style
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0, 0 });
	ImGui::Begin(label.c_str(), nullptr, BaseEditor::GetWindowFlag() | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

	canvasWindowDrawer_ = ImGui::GetWindowDrawList();

	canvasRect_ = SetImGuiImageFullWindow(
		checkerboard_.Get()->GetGPUHandleSRV(),
		Configuration::GetConfig().resolution
	);

	FMainBuffer* main = FMainRender::GetInstance()->GetBuffer()->GetBuffer<FMainBuffer>();

	SetImGuiImageFullWindow( //!< Canvas Buffer の描画
		main->GetBuffer(FMainBuffer::Layout::Canvas).GetGPUHandleSRV(),
		Configuration::GetConfig().resolution
	);

	ImGui::End();
	ImGui::PopStyleVar();
}

void RenderSceneEditor::ShowInfoTextScene() {
	if (sceneWindowDrawer_ == nullptr) {
		return;
	}

	static const ImVec2 kPadding = { 4.0f, 4.0f };

	ImVec2 position = { sceneRect_.pos.x + kPadding.x, sceneRect_.pos.y + sceneRect_.size.y - kPadding.y };

	RenderTextSceneWindow(position, std::format(" Tonemap:            {}", config_.option.Test(FRenderConfig::OptionFlag::Tonemap)));
	RenderTextSceneWindow(position, std::format(" PostProcess:        {}", config_.option.Test(FRenderConfig::OptionFlag::PostProcess)));
	RenderTextSceneWindow(position, std::format(" Anti-Aliasing:      {}", magic_enum::enum_name(config_.antiAliasing)));
	RenderTextSceneWindow(position, std::format(" GlobalIllumination: {}", magic_enum::enum_name(config_.globalIllumination)));
	RenderTextSceneWindow(position, std::format(" Ambient-Occlusion:  {}", magic_enum::enum_name(config_.ambientOcclusion)));
	RenderTextSceneWindow(position, std::format("> Config"));
	RenderTextSceneWindow(position, std::format("Display Buffer | {}", magic_enum::enum_name(displayBuffer_)));

}

void RenderSceneEditor::ShowIconScene() {
	if (sceneWindowDrawer_ == nullptr) {
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

	// Spot Light
	sComponentStorage->ForEach<SpotLightComponent>([&](SpotLightComponent* component) {

		Color4f color = component->IsActive()
			? Color4f(component->GetParameter().color, 1.0f)
			: Color4f{ 0.2f, 0.2f, 0.2f, 1.0f };

		RenderIcon(component->GetBehaviour(), Icon::SpotLight, component->RequireTransform()->GetPosition(), color);
	});

	// Rect Light
	sComponentStorage->ForEach<RectLightComponent>([&](RectLightComponent* component) {
		Color4f color = component->IsActive()
			? Color4f(component->GetParameter().color, 1.0f)
			: Color4f{ 0.2f, 0.2f, 0.2f, 1.0f };
		RenderIcon(component->GetBehaviour(), Icon::RectLight, component->RequireTransform()->GetPosition(), color);
	});

	// Camera
	sComponentStorage->ForEach<CameraComponent>([&](CameraComponent* component) {
		if (component->GetTag() == CameraComponent::Tag::Editor) {
			return; //!< editor cameraは無視
		}

		Color4f color = component->IsActive()
			? Color4f{ 1.0f, 1.0f, 1.0f, 1.0f }
		: Color4f{ 0.2f, 0.2f, 0.2f, 1.0f };

		RenderIcon(component->GetBehaviour(), Icon::Camera, Matrix4x4::GetTranslation(component->GetCamera().world), color);
	});

}

void RenderSceneEditor::UpdateKeyShortcut() {

	//const KeyboardInput* keyboard = SxavengerSystem::GetKeyboardInput();

	// bufferの切り替え
	if (System::IsPressKey(KeyId::KEY_LALT) && System::IsTriggerKey(KeyId::KEY_UP)) { //!< left alt + Up
		if (displayBuffer_ > DisplayBuffer::Scene) {
			displayBuffer_ = static_cast<DisplayBuffer>(static_cast<uint32_t>(displayBuffer_) - 1);
		}
	}

	if (System::IsPressKey(KeyId::KEY_LALT) && System::IsTriggerKey(KeyId::KEY_DOWN)) { //!< left alt + Down
		if (static_cast<uint32_t>(displayBuffer_) < magic_enum::enum_count<DisplayBuffer>()) {
			displayBuffer_ = static_cast<DisplayBuffer>(static_cast<uint32_t>(displayBuffer_) + 1);
		}

	}

	if (System::IsPressKey(KeyId::KEY_LALT) && System::IsTriggerKey(KeyId::KEY_1)) { //!< [LALT] + [1]
		displayBuffer_ = DisplayBuffer::Scene;
	}

	if (System::IsPressKey(KeyId::KEY_LALT) && System::IsTriggerKey(KeyId::KEY_2)) { //!< [LALT] + [2]
		displayBuffer_ = DisplayBuffer::GBuffer;
	}

	if (System::IsPressKey(KeyId::KEY_LALT) && System::IsTriggerKey(KeyId::KEY_3)) { //!< [LALT] + [3]
		displayBuffer_ = DisplayBuffer::LightAccumulation;
	}

	if (System::IsPressKey(KeyId::KEY_LALT) && System::IsTriggerKey(KeyId::KEY_4)) { //!< [LALT] + [4]
		displayBuffer_ = DisplayBuffer::Transparent;
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

void RenderSceneEditor::SetImGuiImagesFullWindowEnable(const std::vector<std::pair<D3D12_GPU_DESCRIPTOR_HANDLE, DisplayBuffer>>& handles, const Vector2ui& size, bool isEnable) {

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
			displayBuffer_ = handles[i].second;
		}
	}
}

void RenderSceneEditor::UpdateCamera() {
	isMoveCamera_ = camera_->Update();

	if (isMoveCamera_) {
		UpdateView();
	}
}

void RenderSceneEditor::ShowCameraInformation(const WindowRect& rect) {

	ImVec2 cursor = ImGui::GetCursorPos();

	ImGui::SetCursorPos({ rect.pos.x + rect.size.x * 0.5f, rect.pos.y + rect.size.y * 0.5f });
	ImGui::Text("TestA");
	ImGui::Text("TestB");

	ImGui::SetCursorPos(cursor);
}

void RenderSceneEditor::UpdateView() {
	(*camera_)->GetComponent<TransformComponent>()->UpdateMatrix();
	(*camera_)->GetComponent<CameraComponent>()->UpdateView();
}

void RenderSceneEditor::RenderInspector(const DirectXQueueContext* context, const CameraComponent* camera) {

	//!< editorの取得
	InspectorEditor* editor = GetEditorEngine()->GetEditor<InspectorEditor>();

	if (editor == nullptr) {
		return; //!< InspectorEditorが存在しない場合は何もしない
	}

	EntityBehaviour* entity = dynamic_cast<EntityBehaviour*>(editor->GetInspector());

	if (entity == nullptr) {
		return; //!< Inspectorの対象がEntityBehaviourでない場合は何もしない
	}

	BehaviourHelper::ForEachBehaviour(entity, [&](EntityBehaviour* behaviour) {

		TransformComponent* transform = behaviour->GetComponent<TransformComponent>();

		if (transform == nullptr) {
			return; //!< TransformComponentを持たない場合は何もしない
		}

		ImColor c = ImGui::GetStyle().Colors[ImGuiCol_CheckMark]; //!< Editorのメインカラーとしてチェックマークの色を利用.
		float thickness = (entity == behaviour ? 0.8f : 0.1f);    //!< 対象のEntityBehaviourがInspectorで選択されている場合は線を太くする

		std::pair<Color4f, float> parameter = { Color4f{ c.Value.x, c.Value.y, c.Value.z, c.Value.w }, thickness };

		DxObject::BindBufferDesc desc = {};
		desc.Set32bitConstants("Dimension", 2, &buffer_->GetResolution());
		desc.Set32bitConstants("Parameter", 5, &parameter);
		desc.SetAddress("gCamera",    camera->GetGPUVirtualAddress());
		desc.SetAddress("gTransform", transform->GetGPUVirtualAddress());

		// rendererの取得
		if (MeshRendererComponent* renderer = behaviour->GetComponent<MeshRendererComponent>()) {
			renderer->GetMesh()->BindInputAssembler(context);
			selectLine_.BindGraphicsBuffer(context->GetDxCommand(), desc);
			renderer->GetMesh()->DrawCall(context);
		}

		if (SkinnedMeshRendererComponent* renderer = behaviour->GetComponent<SkinnedMeshRendererComponent>()) {
			renderer->BindInputAssembler(context);
			selectLine_.BindGraphicsBuffer(context->GetDxCommand(), desc);
			renderer->DrawCall(context);
		}

	});

	
}

void RenderSceneEditor::ShowDisplayBuffer(DisplayBuffer buffer) {
	switch (buffer) {
		//* FMainBufferの描画
		case DisplayBuffer::Scene:
			{
				FMainBuffer* main = buffer_->GetBuffer<FMainBuffer>();

				SetImGuiImageFullWindowEnable(
					main->GetBuffer(FMainBuffer::Layout::Scene).GetGPUHandleSRV(),
					buffer_->GetResolution(),
					isRender_
				);
			}
			return;

		case DisplayBuffer::GBuffer:
			{
				if (!buffer_->HasBuffer<FGBuffer>()) {
					return;
				}

				FGBuffer* gbuffer = buffer_->GetBuffer<FGBuffer>();

				SetImGuiImagesFullWindowEnable(
					{
						{ gbuffer->GetBuffer(FGBuffer::Layout::Albedo).GetGPUHandleSRV(),       DisplayBuffer::Albedo },
						{ gbuffer->GetBuffer(FGBuffer::Layout::Normal).GetGPUHandleSRV(),       DisplayBuffer::Normal },
						{ gbuffer->GetBuffer(FGBuffer::Layout::MaterialARM).GetGPUHandleSRV(),  DisplayBuffer::MaterialARM },
						{ gbuffer->GetBuffer(FGBuffer::Layout::Position).GetGPUHandleSRV(),     DisplayBuffer::Position },
						{ gbuffer->GetBuffer(FGBuffer::Layout::MotionVector).GetGPUHandleSRV(), DisplayBuffer::MotionVector }
					},
					buffer_->GetResolution(),
					isRender_
				);
			}
			return;

		case DisplayBuffer::Albedo:
			{
				if (!buffer_->HasBuffer<FGBuffer>()) {
					return;
				}

				FGBuffer* gbuffer = buffer_->GetBuffer<FGBuffer>();

				SetImGuiImageFullWindowEnable(
					gbuffer->GetBuffer(FGBuffer::Layout::Albedo).GetGPUHandleSRV(),
					buffer_->GetResolution(),
					isRender_
				);
			}
			return;

		case DisplayBuffer::Normal:
			{
				if (!buffer_->HasBuffer<FGBuffer>()) {
					return;
				}

				FGBuffer* gbuffer = buffer_->GetBuffer<FGBuffer>();

				SetImGuiImageFullWindowEnable(
					gbuffer->GetBuffer(FGBuffer::Layout::Normal).GetGPUHandleSRV(),
					buffer_->GetResolution(),
					isRender_
				);
			}
			return;

		case DisplayBuffer::MaterialARM:
			{
				if (!buffer_->HasBuffer<FGBuffer>()) {
					return;
				}

				FGBuffer* gbuffer = buffer_->GetBuffer<FGBuffer>();

				SetImGuiImageFullWindowEnable(
					gbuffer->GetBuffer(FGBuffer::Layout::MaterialARM).GetGPUHandleSRV(),
					buffer_->GetResolution(),
					isRender_
				);
			}
			return;

		case DisplayBuffer::Position:
			{
				if (!buffer_->HasBuffer<FGBuffer>()) {
					return;
				}

				FGBuffer* gbuffer = buffer_->GetBuffer<FGBuffer>();

				SetImGuiImageFullWindowEnable(
					gbuffer->GetBuffer(FGBuffer::Layout::Position).GetGPUHandleSRV(),
					buffer_->GetResolution(),
					isRender_
				);
			}
			return;

		case DisplayBuffer::MotionVector:
			{
				if (!buffer_->HasBuffer<FGBuffer>()) {
					return;
				}

				FGBuffer* gbuffer = buffer_->GetBuffer<FGBuffer>();

				SetImGuiImageFullWindowEnable(
					gbuffer->GetBuffer(FGBuffer::Layout::MotionVector).GetGPUHandleSRV(),
					buffer_->GetResolution(),
					isRender_
				);
			}
			return;

		case DisplayBuffer::LightAccumulation:
			{
				if (!buffer_->HasBuffer<FLightAccumulationBuffer>()) {
					return;
				}

				FLightAccumulationBuffer* lightAccumulationBuffer = buffer_->GetBuffer<FLightAccumulationBuffer>();

				SetImGuiImagesFullWindowEnable(
					{
						{ lightAccumulationBuffer->GetBuffer(FLightAccumulationBuffer::Layout::Direct).GetGPUHandleSRV(),   DisplayBuffer::Direct },
						{ lightAccumulationBuffer->GetBuffer(FLightAccumulationBuffer::Layout::Indirect).GetGPUHandleSRV(), DisplayBuffer::Indirect },
					},
					buffer_->GetResolution(),
					isRender_
				);
			}
			return;

		case DisplayBuffer::Direct:
			{
				if (!buffer_->HasBuffer<FLightAccumulationBuffer>()) {
					return;
				}

				FLightAccumulationBuffer* lightAccumulationBuffer = buffer_->GetBuffer<FLightAccumulationBuffer>();

				SetImGuiImageFullWindowEnable(
					lightAccumulationBuffer->GetBuffer(FLightAccumulationBuffer::Layout::Direct).GetGPUHandleSRV(),
					buffer_->GetResolution(),
					isRender_
				);
			}
			return;

		case DisplayBuffer::Indirect:
			{
				if (!buffer_->HasBuffer<FLightAccumulationBuffer>()) {
					return;
				}

				FLightAccumulationBuffer* lightAccumulationBuffer = buffer_->GetBuffer<FLightAccumulationBuffer>();

				SetImGuiImageFullWindowEnable(
					lightAccumulationBuffer->GetBuffer(FLightAccumulationBuffer::Layout::Indirect).GetGPUHandleSRV(),
					buffer_->GetResolution(),
					isRender_
				);
			}
			return;

		case DisplayBuffer::Transparent:
			{
				if (!buffer_->HasBuffer<FTransparentBuffer>()) {
					return;
				}

				FTransparentBuffer* transparent = buffer_->GetBuffer<FTransparentBuffer>();

				SetImGuiImagesFullWindowEnable(
					{
						{ transparent->GetBuffer(FTransparentBuffer::Layout::Accumulate).GetGPUHandleSRV(), DisplayBuffer::Accumulate },
						{ transparent->GetBuffer(FTransparentBuffer::Layout::Revealage).GetGPUHandleSRV(),  DisplayBuffer::Revealage },
					},
					buffer_->GetResolution(),
					isRender_
				);
			}
			return;

		case DisplayBuffer::Accumulate:
			{
				if (!buffer_->HasBuffer<FTransparentBuffer>()) {
					return;
				}

				FTransparentBuffer* transparent = buffer_->GetBuffer<FTransparentBuffer>();

				SetImGuiImageFullWindowEnable(
					transparent->GetBuffer(FTransparentBuffer::Layout::Accumulate).GetGPUHandleSRV(),
					buffer_->GetResolution(),
					isRender_
				);
			}
			return;

		case DisplayBuffer::Revealage:
			{
				if (!buffer_->HasBuffer<FTransparentBuffer>()) {
					return;
				}

				FTransparentBuffer* transparent = buffer_->GetBuffer<FTransparentBuffer>();

				SetImGuiImageFullWindowEnable(
					transparent->GetBuffer(FTransparentBuffer::Layout::Revealage).GetGPUHandleSRV(),
					buffer_->GetResolution(),
					isRender_
				);
			}
			return;
	}
}

void RenderSceneEditor::RenderIcon(BaseInspector* inspector, Icon icon, const Vector3f& position, const Color4f& color) {
	if (sceneWindowDrawer_ == nullptr) {
		return;
	}

	Vector3f ndc = (*camera_)->GetComponent<CameraComponent>()->CalculateNDCPosition(position);

	if (Comparison::Any(ndc < Vector3(-1.0f, -1.0f, 0.0f)) || Comparison::Any(ndc > Vector3(1.0f, 1.0f, 1.0f))) {
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
	sceneWindowDrawer_->AddImage(
		icons_[static_cast<uint32_t>(icon)].Get()->GetGPUHandleSRV().ptr,
		{ rect.min.x, rect.min.y + 1.0f },
		{ rect.max.x, rect.max.y + 1.0f },
		ImVec2{ 0.0f, 0.0f },
		ImVec2{ 1.0f, 1.0f },
		ImColor{ 0.01f, 0.01f, 0.01f, color.a * 0.8f }
	);

	// main icon
	sceneWindowDrawer_->AddImage(
		icons_[static_cast<uint32_t>(icon)].Get()->GetGPUHandleSRV().ptr,
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

	if (SxImGuizmo::IsOver()) {
		return; //!< gizmo操作中は選択不可
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

	sceneWindowDrawer_->AddText(ImVec2(position.x, position.y), color, text.c_str());
}

void RenderSceneEditor::PickMesh(const DirectXQueueContext* context, const WindowRect& rect) {

	if (!buffer_->HasBuffer<FGBuffer>()) {
		return; //!< ピックに必要なバッファが存在しない場合は何もしない
	}

	FGBuffer* buffer = buffer_->GetBuffer<FGBuffer>();

	//!< mouseの位置がrectの範囲内にあるか判定
	Vector2f min = rect.pos;
	Vector2f max = rect.pos + rect.size;

	if (!SxImGui::IsMouseClickedRect({ min.x, min.y }, { max.x, max.y }, ImGuiMouseButton_Left)) {
		return; //!< mouseがrectの範囲内にない場合は何もしない
	}

	Vector2f mouse = {
		ImGui::GetMousePos().x,
		ImGui::GetMousePos().y,
	};

	mouse = Clamp(mouse - rect.pos, Vector2f(0.0f, 0.0f), rect.size);
	mouse /= rect.size; //!< mouseの位置を0~1に正規化

	Vector2i pixel = Vector2i(static_cast<int32_t>(mouse.x * buffer_->GetResolution().x), static_cast<int32_t>(mouse.y * buffer_->GetResolution().y));

	static DxObject::UnorderedDimensionBuffer<uintptr_t> address;
	address.Create(System::GetDxDevice(), 1);

	picker_.SetPipeline(context->GetDxCommand());

	DxObject::BindBufferDesc desc = {};
	desc.SetHandle("gAddress", buffer->GetBuffer(FGBuffer::Layout::Address).GetGPUHandleSRV());
	desc.SetAddress("gPicker", address.GetGPUVirtualAddress());
	desc.Set32bitConstants("Pixel", 2, &pixel);
	picker_.BindComputeBuffer(context->GetDxCommand(), desc);

	picker_.Dispatch(context->GetDxCommand(), { 1, 1, 1 });

	static DxObject::ReadbackDimensionBuffer<uintptr_t> readback;
	readback.Readback(System::GetDxDevice(), context->GetDxCommand(), &address);

	context->ExecuteAllAllocators(); //!< readbackの結果を受け取るために、command listをflushする

	uintptr_t value = readback.At(0);

	if (value == 0) {
		return; //!< ピックした場所にオブジェクトが存在しない場合は何もしない
	}

	BehaviourAddress behaviour = { readback.At(0), BehaviourAddress::Ownership::Borrowed };

	BaseEditor::GetEditorEngine()->ExecuteEditorFunction<InspectorEditor>([&](InspectorEditor* editor) {
		editor->SetInspector(behaviour.Get());
	});
}
