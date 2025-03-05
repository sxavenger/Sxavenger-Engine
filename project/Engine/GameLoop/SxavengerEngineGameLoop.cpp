#include "SxavengerEngineGameLoop.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/SxavengerSystem.h>
#include <Engine/Content/SxavengerContent.h>
#include <Engine/Asset/SxavengerAsset.h>
#include <Engine/Render/FRenderCore.h>

////////////////////////////////////////////////////////////////////////////////////////////
// SxavengerEngineGameLoop class methods
////////////////////////////////////////////////////////////////////////////////////////////

void SxavengerEngineGameLoop::Init(GameLoop::Context* context) {
	context->SetProcess(GameLoop::Process::Init, std::nullopt, [this]() {
		FRenderCore::GetInstance()->Init();
	});

	context->SetProcess(GameLoop::Process::Init, UINT32_MAX, [this]() {
		SxavengerSystem::ExecuteAllAllocator();
	});

	context->SetProcess(GameLoop::Process::Term, std::nullopt, [this]() {
		FRenderCore::GetInstance()->Term();
	});

	context->SetProcess(GameLoop::Process::Begin, 0, [this]() {
		SxavengerSystem::BeginPerformace();
		SxavengerSystem::GetInput()->Update();
		SxavengerSystem::BeginImGuiFrame();
	});

	context->SetProcess(GameLoop::Process::Update, UINT32_MAX, [this]() {
		SxavengerSystem::RecordLap("update [game logic]");
		SxavengerSystem::EndImGuiFrame();
		SxavengerSystem::TransitionAllocator();
		SxavengerSystem::RecordLap("update [gpu transtion]");
	});

	context->SetProcess(GameLoop::Process::End, 0, [this]() {
		SxavengerSystem::RecordLap("draw [draw logic]");
		SxavengerSystem::PresentAllWindow();
		SxavengerSystem::TransitionAllocator();
		SxavengerSystem::RecordLap("draw [gpu execution]");
		SxavengerSystem::EndPerformace();
	});

	SxavengerLogger::Init();
	SxavengerSystem::Init();
	SxavengerContent::Init();
	CreateWhite1x1();
	SxavengerAsset::Init();
}

void SxavengerEngineGameLoop::Term() {
	SxavengerAsset::Term();
	SxavengerContent::Term();
	SxavengerSystem::Term();
}

void SxavengerEngineGameLoop::CreateWhite1x1() {
	std::unique_ptr<DxObject::ReflectionComputePipelineState> compute = std::make_unique<DxObject::ReflectionComputePipelineState>();
	compute->CreateBlob(kPackagesShaderDirectory / "common/white1x1.cs.hlsl");
	compute->ReflectionPipeline(SxavengerSystem::GetDxDevice());

	std::unique_ptr<UnorderedTexture> white1x1 = std::make_unique<UnorderedTexture>();
	white1x1->Create({ 1, 1 }, DXGI_FORMAT_R8G8B8A8_UNORM);
	white1x1->TransitionBeginUnordered(SxavengerSystem::GetMainThreadContext());
	compute->SetPipeline(SxavengerSystem::GetMainThreadContext()->GetDxCommand());

	DxObject::BindBufferDesc bind = {};
	bind.SetHandle("gOutput", white1x1->GetGPUHandleUAV());
	compute->BindComputeBuffer(SxavengerSystem::GetMainThreadContext()->GetDxCommand(), bind);

	compute->Dispatch(SxavengerSystem::GetMainThreadContext()->GetDxCommand(), { 1, 1, 1 });

	white1x1->TransitionEndUnordered(SxavengerSystem::GetMainThreadContext());
	SxavengerSystem::TransitionAllocator();

	SxavengerContent::RegisterTexture("white1x1", std::move(white1x1));
}
