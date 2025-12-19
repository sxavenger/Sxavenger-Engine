#include "EngineExecution.h"
SXAVENGER_ENGINE_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/Utility/StreamLogger.h>
#include <Engine/System/Configuration/Configuration.h>
#include <Engine/System/System.h>
#include <Engine/Graphics/Graphics.h>
#include <Engine/Assets/Asset/AssetStorage.h>
#include <Engine/Components/Component/Audio/AudioController.h>
#include <Engine/Components/Component/ComponentHelper.h>
#include <Engine/Components/Entity/MonoBehaviourStorage.h>
#include <Engine/Module/Scene/SceneObjects.h>
#include <Engine/Render/FRenderCore.h>
#include <Engine/Render/FMainRender.h>

//* c++
#include <limits>

////////////////////////////////////////////////////////////////////////////////////////////
// EngineExecution class methods
////////////////////////////////////////////////////////////////////////////////////////////

void EngineExecution::Init(Execution::Context* context) {
	SetProcess(context);

	StreamLogger::Init();
	Configuration::Load();
	System::Init();
	Graphics::Init();
}

void EngineExecution::Term() {
	sMonoBehaviourStorage->Term();
	sAudioController->Term();
	sAssetStorage->Term();
	sContentStorage->Term();
	Graphics::Term();
	System::Term();
}

void EngineExecution::SetProcess(Execution::Context* context) {
	context->SetProcess(Execution::Process::Init, std::nullopt, [this]() {

		sAssetStorage->Deserialize();

		ComponentHelper::RegisterComponents();
		sAudioController->Init();

		FRenderCore::GetInstance()->Init();
		FMainRender::GetInstance()->Init();

		CreateWhite1x1();
		CreateCheckerboard();

		
	});

	context->SetProcess(Execution::Process::Init, std::numeric_limits<uint32_t>::max(), [this]() {
		System::ExecuteAllAllocator();
	});

	context->SetProcess(Execution::Process::Term, std::nullopt, [this]() {
		FMainRender::GetInstance()->Term();
		FRenderCore::GetInstance()->Term();

		sSceneObjects->Clear();

		sAssetStorage->Serialize();

		System::Shutdown();
		System::ExecuteAllAllocator();
	});

	context->SetProcess(Execution::Process::Begin, 0, [this]() {
		System::GetDxDevice()->CheckDeviceStatus();
		System::BeginPerformace();
		System::GetInput()->Update();
		System::BeginImGuiFrame();
		ComponentHelper::BeginFrame();

		System::Record("begin [engine]");
	});

	context->SetProcess(Execution::Process::Update, std::numeric_limits<uint32_t>::max(), [this]() {
		System::Record("update [game logic]");

		System::EndImGuiFrame();

		FMainRender::GetInstance()->GetScene()->SetupTopLevelAS(System::GetDirectQueueContext());
		FMainRender::GetInstance()->GetScene()->SetupStateObject();
		FMainRender::GetInstance()->GetScene()->SetupLightContainer();

		UpdateAsset();

		System::Record("update [engine]");
	});

	context->SetProcess(Execution::Process::End, 0, [this]() {
		System::Record("render [draw logic]");

		//System::TransitionAllocator();
		System::ExecuteAllAllocator();
		//!< ダブルバッファ変更時, TransitionAllocatorに設定
		//! 
		System::Record("render [gpu execution]");

		System::PresentWindows();

		Graphics::ResetPrimtive();

		sMonoBehaviourStorage->UnregisterBehaviour();

		System::Record("end [engine]");
		System::EndPerformace();
	});
}

void EngineExecution::CreateWhite1x1() {
	std::unique_ptr<DxObject::ReflectionComputePipelineState> compute = std::make_unique<DxObject::ReflectionComputePipelineState>();
	compute->CreateBlob(kPackagesDirectory / "shaders/common/white1x1.cs.hlsl");
	compute->ReflectionPipeline(System::GetDxDevice());

	std::unique_ptr<UnorderedTexture> white1x1 = std::make_unique<UnorderedTexture>();
	white1x1->Create({ 1, 1 }, DXGI_FORMAT_R8G8B8A8_UNORM);
	white1x1->TransitionBeginUnordered(System::GetDirectQueueContext());
	compute->SetPipeline(System::GetDirectQueueContext()->GetDxCommand());

	DxObject::BindBufferDesc bind = {};
	bind.SetHandle("gOutput", white1x1->GetGPUHandleUAV());
	compute->BindComputeBuffer(System::GetDirectQueueContext()->GetDxCommand(), bind);

	compute->Dispatch(System::GetDirectQueueContext()->GetDxCommand(), { 1, 1, 1 });

	white1x1->TransitionEndUnordered(System::GetDirectQueueContext());
	System::ExecuteAllAllocator();

	Graphics::RegisterTexture("white1x1", std::move(white1x1));
}

void EngineExecution::CreateCheckerboard() {
	std::unique_ptr<DxObject::ReflectionComputePipelineState> compute = std::make_unique<DxObject::ReflectionComputePipelineState>();
	compute->CreateBlob(kPackagesDirectory / "shaders/common/checker.cs.hlsl");
	compute->ReflectionPipeline(System::GetDxDevice());

	std::unique_ptr<UnorderedTexture> checker = std::make_unique<UnorderedTexture>();
	checker->Create({ 256, 256 }, DXGI_FORMAT_R8G8B8A8_UNORM);
	checker->TransitionBeginUnordered(System::GetDirectQueueContext());
	compute->SetPipeline(System::GetDirectQueueContext()->GetDxCommand());

	DxObject::BindBufferDesc bind = {};
	bind.SetHandle("gOutput", checker->GetGPUHandleUAV());
	compute->BindComputeBuffer(System::GetDirectQueueContext()->GetDxCommand(), bind);

	compute->Dispatch(System::GetDirectQueueContext()->GetDxCommand(), { DxObject::RoundUp(checker->GetSize().x, 16), DxObject::RoundUp(checker->GetSize().y, 16), 1 });

	checker->TransitionEndUnordered(System::GetDirectQueueContext());
	System::ExecuteAllAllocator();

	Graphics::RegisterTexture("checkerboard", std::move(checker));
}

void EngineExecution::UpdateAsset() {
	sAssetStorage->ForEach<AssetMaterial>([](AssetMaterial* asset) {
		asset->Update();
	});

	sAssetStorage->ForEach<AssetTexture>([](AssetTexture* asset) {
		asset->Update(System::GetDirectQueueContext());
	});
}
