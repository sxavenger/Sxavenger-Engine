#include "SxavengerEngineGameLoop.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/SxavengerSystem.h>
#include <Engine/Content/SxavengerContent.h>
#include <Engine/Render/FRenderCore.h>
#include <Engine/Render/FMainRender.h>
#include <Engine/Component/ComponentHelper.h>

//* c++
#include <limits>

////////////////////////////////////////////////////////////////////////////////////////////
// SxavengerEngineGameLoop class methods
////////////////////////////////////////////////////////////////////////////////////////////

void SxavengerEngineGameLoop::Init(GameLoop::Context* context) {
	context->SetProcess(GameLoop::Process::Init, std::nullopt, [this]() {
		FRenderCore::GetInstance()->Init();
		FMainRender::GetInstance()->Init();
		CreateWhite1x1();
		CreateCheckerboard();

		ComponentHelper::RegisterComponents();
		sUAssetStorage->Deserialize();
	});

	context->SetProcess(GameLoop::Process::Init, std::numeric_limits<uint32_t>::max(), [this]() {
		SxavengerSystem::ExecuteAllAllocator();
	});

	context->SetProcess(GameLoop::Process::Term, std::nullopt, [this]() {
		FMainRender::GetInstance()->Term();
		FRenderCore::GetInstance()->Term();

		SxavengerSystem::ShutdownAsyncThread();

		sUAssetStorage->Serialize();
	});

	context->SetProcess(GameLoop::Process::Begin, 0, [this]() {
		SxavengerSystem::BeginPerformace();
		SxavengerSystem::GetInput()->Update();
		SxavengerSystem::BeginImGuiFrame();
	});

	context->SetProcess(GameLoop::Process::Update, std::numeric_limits<uint32_t>::max(), [this]() {
		SxavengerSystem::RecordLap("update [game logic]");
		SxavengerSystem::EndImGuiFrame();
		FMainRender::GetInstance()->GetScene()->SetupTopLevelAS(SxavengerSystem::GetDirectQueueContext());
		FMainRender::GetInstance()->GetScene()->SetupStateObject();
		FMainRender::GetInstance()->GetScene()->SetupLightContainer();
		SxavengerSystem::TransitionAllocator();
		UpdateMaterials();
		SxavengerSystem::RecordLap("update [engine]");
	});

	context->SetProcess(GameLoop::Process::End, 0, [this]() {
		SxavengerSystem::RecordLap("render [draw logic]");
		SxavengerSystem::TransitionAllocator();
		SxavengerSystem::PresentWindows();
		SxavengerSystem::ExecuteAllAllocator();
		SxavengerContent::ResetPrimtive();
		SxavengerSystem::RecordLap("render [gpu execution]");
		SxavengerSystem::EndPerformace();
	});

	SxavengerLogger::Init();
	SxavengerConfig::Load();
	SxavengerSystem::Init();
	SxavengerContent::Init();
}

void SxavengerEngineGameLoop::Term() {
	sUAssetStorage->Term();
	sUContentStorage->Term();
	SxavengerContent::Term();
	SxavengerSystem::Term();
}

void SxavengerEngineGameLoop::CreateWhite1x1() {
	std::unique_ptr<DxObject::ReflectionComputePipelineState> compute = std::make_unique<DxObject::ReflectionComputePipelineState>();
	compute->CreateBlob(kPackagesShaderDirectory / "common/white1x1.cs.hlsl");
	compute->ReflectionPipeline(SxavengerSystem::GetDxDevice());

	std::unique_ptr<UnorderedTexture> white1x1 = std::make_unique<UnorderedTexture>();
	white1x1->Create({ 1, 1 }, DXGI_FORMAT_R8G8B8A8_UNORM);
	white1x1->TransitionBeginUnordered(SxavengerSystem::GetDirectQueueContext());
	compute->SetPipeline(SxavengerSystem::GetDirectQueueContext()->GetDxCommand());

	DxObject::BindBufferDesc bind = {};
	bind.SetHandle("gOutput", white1x1->GetGPUHandleUAV());
	compute->BindComputeBuffer(SxavengerSystem::GetDirectQueueContext()->GetDxCommand(), bind);

	compute->Dispatch(SxavengerSystem::GetDirectQueueContext()->GetDxCommand(), { 1, 1, 1 });

	white1x1->TransitionEndUnordered(SxavengerSystem::GetDirectQueueContext());
	SxavengerSystem::ExecuteAllAllocator();

	SxavengerContent::RegisterTexture("white1x1", std::move(white1x1));
}

void SxavengerEngineGameLoop::CreateCheckerboard() {
	std::unique_ptr<DxObject::ReflectionComputePipelineState> compute = std::make_unique<DxObject::ReflectionComputePipelineState>();
	compute->CreateBlob(kPackagesShaderDirectory / "common/checker.cs.hlsl");
	compute->ReflectionPipeline(SxavengerSystem::GetDxDevice());

	std::unique_ptr<UnorderedTexture> checker = std::make_unique<UnorderedTexture>();
	checker->Create({ 256, 256 }, DXGI_FORMAT_R8G8B8A8_UNORM);
	checker->TransitionBeginUnordered(SxavengerSystem::GetDirectQueueContext());
	compute->SetPipeline(SxavengerSystem::GetDirectQueueContext()->GetDxCommand());

	DxObject::BindBufferDesc bind = {};
	bind.SetHandle("gOutput", checker->GetGPUHandleUAV());
	compute->BindComputeBuffer(SxavengerSystem::GetDirectQueueContext()->GetDxCommand(), bind);

	compute->Dispatch(SxavengerSystem::GetDirectQueueContext()->GetDxCommand(), { DxObject::RoundUp(checker->GetSize().x, 16), DxObject::RoundUp(checker->GetSize().y, 16), 1 });

	checker->TransitionEndUnordered(SxavengerSystem::GetDirectQueueContext());
	SxavengerSystem::ExecuteAllAllocator();

	SxavengerContent::RegisterTexture("checkerboard", std::move(checker));
}

void SxavengerEngineGameLoop::UpdateMaterials() {
	sUAssetStorage->ForEach<UAssetMaterial>([](UAssetMaterial* asset) {
		asset->Update();
	});
}
