#include "BetaSystemGameLoop.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/SxavengerSystem.h>
#include <Engine/Asset/SxavengerAsset.h>
#include <Engine/Editor/EditorEngine.h>
#include <Engine/Editor/Editors/EngineDeveloperEditor.h>
#include <Engine/Editor/Editors/RenderSceneEditor.h>
#include <Engine/Module/SxavengerModule.h>

#include <Lib/Sxl/Optional.h>
#include <Engine/System/Config/SxavengerConfig.h>

////////////////////////////////////////////////////////////////////////////////////////////
// BetaSystemGameLoop class methods
////////////////////////////////////////////////////////////////////////////////////////////

void BetaSystemGameLoop::Init(GameLoop::Context* context) {
	context->SetState(GameLoop::State::Init, std::nullopt, [this]() { InitSystem(); });
	context->SetState(GameLoop::State::Term, std::nullopt, [this]() { TermSystem(); });
	context->SetState(GameLoop::State::Update, std::nullopt, [this]() {
		if (auto developer = sEditorEngine->TryGetEditor<EngineDeveloperEditor>()) {
			if (!developer->IsProcessRequired()) {
				return;
			}
		}

		UpdateSystem();
	});
	context->SetState(GameLoop::State::Draw, std::nullopt, [this]() { DrawSystem(); });

	context->SetCondition(
		[this]() { return !SxavengerSystem::ProcessMessage(); }
	);
}

void BetaSystemGameLoop::Term() {
}

void BetaSystemGameLoop::InitSystem() {

	main_ = SxavengerSystem::CreateMainWindow(kMainWindowSize, L"beta system window").lock();
	main_->SetIcon("packages/icon/SxavengerEngineSubIcon.ico", { 32, 32 });

	pipeline_ = std::make_unique<CustomReflectionComputePipeline>();
	pipeline_->CreateAsset(kAssetsDirectory / "shaders/test.cs.hlsl");
	pipeline_->RegisterBlob();
	pipeline_->ReflectionPipeline(SxavengerSystem::GetDxDevice());

	buffer_ = std::make_unique<DxObject::DimensionBuffer<std::pair<Color4f, Color4f>>>();
	buffer_->Create(SxavengerSystem::GetDxDevice(), 1);
	buffer_->At(0).first = { 1.0f, 0.0f, 0.0f, 1.0f };
	buffer_->At(0).second = { 1.0f, 1.0f, 0.0f, 1.0f };

	texture_ = std::make_unique<UnorderedTexture>();
	texture_->Create({ 1, 1 });
	texture_->TransitionBeginUnordered(SxavengerSystem::GetMainThreadContext());

}

void BetaSystemGameLoop::TermSystem() {
}

void BetaSystemGameLoop::UpdateSystem() {
}

void BetaSystemGameLoop::DrawSystem() {

	pipeline_->SetPipeline(SxavengerSystem::GetMainThreadContext()->GetDxCommand());

	DxObject::BindBufferDesc desc = {};
	desc.SetAddress("gParameter", buffer_->GetGPUVirtualAddress());
	desc.SetHandle("gOutput", texture_->GetGPUHandleUAV());

	pipeline_->BindComputeBuffer(SxavengerSystem::GetMainThreadContext()->GetDxCommand(), desc);
	pipeline_->Dispatch(SxavengerSystem::GetMainThreadContext()->GetDxCommand(), { 1, 1, 1 });

	main_->BeginRendering();
	main_->ClearWindow();

	SxavengerSystem::RenderImGui();

	main_->EndRendering();
}
