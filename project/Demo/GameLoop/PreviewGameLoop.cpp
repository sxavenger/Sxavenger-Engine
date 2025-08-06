#include "PreviewGameLoop.h"
_DXOBJECT_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/SxavengerSystem.h>
#include <Engine/Asset/SxavengerAsset.h>
#include <Engine/Component/Components/Camera/CameraComponent.h>
#include <Engine/Component/Components/Light/Punctual/PointLightComponent.h>
#include <Engine/Component/ComponentHelper.h>
#include <Engine/Editor/EditorEngine.h>
#include <Engine/Editor/Editors/DevelopEditor.h>

#include <Engine/Content/Exporter/TextureExporter.h>

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

	texture_.Create({ 16, 16 });

	pipeline_.CreateAsset("assets/shaders/test.cs.hlsl");
	pipeline_.RegisterBlob();
	pipeline_.ReflectionPipeline(SxavengerSystem::GetDxDevice());
}

void PreviewGameLoop::TermGame() {
}

void PreviewGameLoop::UpdateGame() {

	pipeline_.SetPipeline(SxavengerSystem::GetDirectQueueContext()->GetDxCommand());

	texture_.TransitionBeginUnordered(SxavengerSystem::GetDirectQueueContext());

	DxObject::BindBufferDesc desc = {};
	desc.SetHandle("gOutput", texture_.GetGPUHandleUAV());
	desc.SetHandle("gDummy",  D3D12_GPU_DESCRIPTOR_HANDLE{});

	pipeline_.BindComputeBuffer(SxavengerSystem::GetDirectQueueContext()->GetDxCommand(), desc);
	pipeline_.Dispatch(SxavengerSystem::GetDirectQueueContext()->GetDxCommand(), { DxObject::RoundUp(texture_.GetSize().x, 16), DxObject::RoundUp(texture_.GetSize().y, 16), 1 });

	texture_.TransitionEndUnordered(SxavengerSystem::GetDirectQueueContext());
}

void PreviewGameLoop::DrawGame() {

	main_->BeginRenderWindow(SxavengerSystem::GetDirectQueueContext());
	main_->ClearWindow(SxavengerSystem::GetDirectQueueContext());

	FPresenter::Present(SxavengerSystem::GetDirectQueueContext(), main_->GetSize(), texture_.GetGPUHandleSRV());
	SxavengerSystem::RenderImGui();

	main_->EndRenderWindow(SxavengerSystem::GetDirectQueueContext());
}
