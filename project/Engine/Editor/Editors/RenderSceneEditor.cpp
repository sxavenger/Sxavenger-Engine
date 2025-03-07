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
	auto t = camera_->AddComponent<TransformComponent>();
	t->GetTransform().translate = { 0.0f, 0.0f, -8.0f };
	t->UpdateMatrix();
	auto v = camera_->AddComponent<CameraComponent>();
	v->UpdateView();
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

	textures_->EndTransparentBasePass(SxavengerSystem::GetMainThreadContext());
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
