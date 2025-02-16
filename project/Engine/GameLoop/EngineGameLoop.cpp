#include "EngineGameLoop.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/SxavengerSystem.h>
#include <Engine/Asset/SxavengerAsset.h>
#include <Engine/Content/SxavengerContent.h>
#include <Engine/Module/SxavengerModule.h>
#include <Engine/System/Config/SxavengerConfig.h>
#include <Engine/Render/FRenderCore.h>

////////////////////////////////////////////////////////////////////////////////////////////
// EngineGameLoop class methods
////////////////////////////////////////////////////////////////////////////////////////////

void EngineGameLoop::Init(GameLoop::Context* context) {
	context->SetState(
		GameLoop::State::Init, std::nullopt, [this]() {
			SxavengerLogger::Init();
			SxavengerSystem::Init();
			SxavengerAsset::Init();
			SxavengerContent::Init();
			SxavengerModule::Init();

			{ //!< white1x1の生成
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

			FRenderCore::GetInstance()->Init();
		}
	);

	context->SetState(
		GameLoop::State::Init, UINT32_MAX, [this]() {
			SxavengerSystem::ExecuteAllAllocator();
		}
	);

	context->SetState(
		GameLoop::State::Term, std::nullopt, [this]() {
			FRenderCore::GetInstance()->Term();

			SxavengerModule::Term();
			SxavengerContent::Term();
			SxavengerAsset::Term();
			SxavengerSystem::Term();
		}
	);

	context->SetState(
		GameLoop::State::Begin, 0, [this]() {
			SxavengerSystem::BeginPerformace();
			SxavengerSystem::GetInput()->Update();
			SxavengerSystem::BeginImGuiFrame();
		}
	);

	context->SetState(
		GameLoop::State::Update, UINT32_MAX, [this]() {
			SxavengerSystem::EndImGuiFrame();
			SxavengerSystem::TransitionAllocator();
		}
	);

	context->SetState(
		GameLoop::State::End, 0, [this]() {
			SxavengerSystem::PresentAllWindow();
			SxavengerSystem::ExecuteAllAllocator();
			SxavengerModule::ResetPrimtive();
			SxavengerSystem::EndPerformace();
		}
	);
}

void EngineGameLoop::Term() {
}
