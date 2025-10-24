#include "PreviewGameLoop.h"
_DXOBJECT_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/DirectX/DxObject/DxResourceStorage.h>
#include <Engine/Content/SxavengerContent.h>
#include <Engine/Preview/Content/UContentStorage.h>
#include <Engine/Preview/Asset/UAssetStorage.h>

#include <Engine/Render/FRenderCore.h>
#include <Engine/Render/FPresenter.h>

////////////////////////////////////////////////////////////////////////////////////////////
// PreviewGameLoop class methods
////////////////////////////////////////////////////////////////////////////////////////////

void PreviewGameLoop::Init(GameLoop::Context* context) {
	context->SetProcess(GameLoop::Process::Init, std::nullopt, [this]() {
		FRenderCore::GetInstance()->Init();
		InitGame();
	});

	context->SetProcess(GameLoop::Process::Init, std::numeric_limits<uint32_t>::max(), [this]() {
		SxavengerSystem::ExecuteAllAllocator();
	});

	context->SetProcess(GameLoop::Process::Term, std::nullopt, [this]() {
		TermGame();
		TermEngine();
	});

	context->SetProcess(GameLoop::Process::Begin, 0, [this]() {
		BeginEngine();
	});

	context->SetProcess(GameLoop::Process::Update, std::nullopt, [this]() {
		UpdateGame();
		UpdateEngine();
	});

	context->SetProcess(GameLoop::Process::Render, std::nullopt, [this]() {
		DrawGame();
	});

	context->SetProcess(GameLoop::Process::End, 0, [this]() {
		EndEngine();
	});

	context->SetCondition(
		[this]() { return !SxavengerSystem::ProcessMessage(); }
	);

	SxavengerLogger::Init();
	SxavengerConfig::Load();
	SxavengerSystem::Init();
	SxavengerContent::Init();
}

void PreviewGameLoop::Term() {
	sUAssetStorage->Term();
	sUContentStorage->Term();
	SxavengerContent::Term();
	SxavengerSystem::Term();
}

void PreviewGameLoop::InitGame() {

	main_ = SxavengerSystem::CreateMainWindow(kMainWindowSize, L"[Sxavenger Engine]: Preview Window", { 0.14f, 0.2f, 0.24f, 1.f }).lock();
	main_->SetIcon("packages/icon/SxavengerEngineIcon.ico", { 32, 32 });

	sub1_ = SxavengerSystem::CreateSubWindow(kMainWindowSize, L"[Sxavenger Engine]: Preview Window (sub window 1)", DirectXWindowContext::ProcessCategory::Window, { 0.14f, 0.2f, 0.24f, 1.f }).lock();

	compute_.CreateBlob(kAssetsDirectory / "shaders" / "Test.cs.hlsl");
	compute_.ReflectionPipeline(SxavengerSystem::GetDxDevice());

	texture_.Create({1, 1}, DXGI_FORMAT_R8G8B8A8_UNORM);

	constant_.Create(SxavengerSystem::GetDxDevice());
	constant_.At() = Color4f::Convert(0xFF00FFFF);
}

void PreviewGameLoop::TermEngine() {
	FRenderCore::GetInstance()->Term();

	SxavengerSystem::ExecuteAllAllocator();
	SxavengerSystem::Shutdown();
}

void PreviewGameLoop::TermGame() {
}

void PreviewGameLoop::BeginEngine() {
	SxavengerSystem::GetDxDevice()->CheckDeviceStatus();
	SxavengerSystem::BeginPerformace();
	SxavengerSystem::GetInput()->Update();
	SxavengerSystem::BeginImGuiFrame();
}

void PreviewGameLoop::UpdateGame() {

	static bool toggle = false;
	toggle = !toggle;

	if (toggle) {
		constant_.At() = Color4f::Convert(0xFF0000FF);

	} else {
		constant_.At() = Color4f::Convert(0xFF00FFFF);
	}

	texture_.TransitionBeginUnordered(SxavengerSystem::GetDirectQueueContext());

	compute_.SetPipeline(SxavengerSystem::GetDirectQueueContext()->GetDxCommand());

	DxObject::BindBufferDesc desc = {};
	desc.SetHandle("gTexture", texture_.GetGPUHandleUAV());
	desc.SetAddress("gParameter", constant_.GetGPUVirtualAddress());

	compute_.BindComputeBuffer(SxavengerSystem::GetDirectQueueContext()->GetDxCommand(), desc);
	compute_.Dispatch(SxavengerSystem::GetDirectQueueContext()->GetDxCommand(), DxObject::RoundUp({ 1, 1, 1 }, { 8, 8, 1 }));

	texture_.TransitionEndUnordered(SxavengerSystem::GetDirectQueueContext());
}

void PreviewGameLoop::UpdateEngine() {
	SxavengerSystem::RecordLap("update [game logic]");
	SxavengerSystem::EndImGuiFrame();
	SxavengerSystem::RecordLap("update [engine]");
}

void PreviewGameLoop::DrawGame() {

	main_->BeginRenderWindow(SxavengerSystem::GetDirectQueueContext());
	main_->ClearWindow(SxavengerSystem::GetDirectQueueContext());

	FPresenter::Present(
		SxavengerSystem::GetDirectQueueContext(),
		main_->GetSize(),
		texture_.GetGPUHandleSRV()
	);

	main_->EndRenderWindow(SxavengerSystem::GetDirectQueueContext());

	sub1_->BeginRenderWindow(SxavengerSystem::GetDirectQueueContext());
	sub1_->ClearWindow(SxavengerSystem::GetDirectQueueContext());

	sub1_->EndRenderWindow(SxavengerSystem::GetDirectQueueContext());
}

void PreviewGameLoop::EndEngine() {
	SxavengerSystem::RecordLap("render [draw logic]");
	SxavengerSystem::TransitionAllocator();
	SxavengerSystem::PresentWindows();
	DxObject::ResourceStorage::GetInstance()->Destroy();
	DxObject::ResourceStorage::GetInstance()->SwapBuffer();
	SxavengerSystem::RecordLap("render [gpu execution]");
	SxavengerSystem::EndPerformace();
}
