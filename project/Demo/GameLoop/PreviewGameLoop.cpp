#include "PreviewGameLoop.h"
_DXOBJECT_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/SxavengerSystem.h>
#include <Engine/Component/Components/Camera/CameraComponent.h>
#include <Engine/Component/Components/Light/Punctual/PointLightComponent.h>
#include <Engine/Component/ComponentHelper.h>
#include <Engine/Editor/EditorEngine.h>
#include <Engine/Editor/Editors/DevelopEditor.h>

#include <Engine/Content/Exporter/TextureExporter.h>
#include <Engine/System/DirectX/DxObject/DxResourceStorage.h>

////////////////////////////////////////////////////////////////////////////////////////////
// PreviewGameLoop class methods
////////////////////////////////////////////////////////////////////////////////////////////

void PreviewGameLoop::Init(GameLoop::Context* context) {
	context->SetProcess(GameLoop::Process::Init, std::nullopt, [this]() { InitGame(); });
	context->SetProcess(GameLoop::Process::Term, std::nullopt, [this]() { TermGame(); });
	context->SetProcess(GameLoop::Process::Update, std::nullopt, [this]() {
		if (auto develop = sEditorEngine->GetEditor<DevelopEditor>()) {
			if (!develop->IsProcessRequired()) {
				return;
			}
		}

		UpdateGame();
	});
	context->SetProcess(GameLoop::Process::Render, std::nullopt, [this]() { DrawGame(); });

	context->SetCondition(
		[this]() { return !SxavengerSystem::ProcessMessage(); }
	);
}

void PreviewGameLoop::Term() {
}

void PreviewGameLoop::InitGame() {

	main_ = SxavengerSystem::CreateMainWindow(kMainWindowSize, L"sxavenger engine preview window", { 0.14f, 0.2f, 0.24f, 1.f }).lock();
	main_->SetIcon("packages/icon/SxavengerEngineIcon.ico", { 32, 32 });

	DxObject::ResourceStorage storage;
	auto buffer = storage.CreateBuffer();
	
}

void PreviewGameLoop::TermGame() {
}

void PreviewGameLoop::UpdateGame() {

	ComponentHelper::UpdateTransform();
	
}

void PreviewGameLoop::DrawGame() {

	main_->BeginRenderWindow(SxavengerSystem::GetDirectQueueContext());
	main_->ClearWindow(SxavengerSystem::GetDirectQueueContext());

	SxavengerSystem::RenderImGui();

	main_->EndRenderWindow(SxavengerSystem::GetDirectQueueContext());
}
