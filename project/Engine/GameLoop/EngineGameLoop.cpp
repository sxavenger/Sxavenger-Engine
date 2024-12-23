#include "EngineGameLoop.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/SxavengerSystem.h>
#include <Engine/System/Runtime/Performance/Performance.h>
#include <Engine/Asset/SxavengerAsset.h>
#include <Engine/Content/SxavengerContent.h>
#include <Engine/Module/SxavengerModule.h>

////////////////////////////////////////////////////////////////////////////////////////////
// EngineGameLoop class methods
////////////////////////////////////////////////////////////////////////////////////////////

void EngineGameLoop::Init(GameLoop::Context* context) {
	context->SetState(
		GameLoop::State::Init, std::nullopt, [this]() {
			SxavengerSystem::Init();
			SxavengerAsset::Init();
			SxavengerContent::Init();
			SxavengerModule::Init();

			{ //!< white1x1の生成
				std::unique_ptr<DxObject::ReflectionComputePipelineState> compute = std::make_unique<DxObject::ReflectionComputePipelineState>();
				compute->CreateBlob("common/white1x1.cs.hlsl");
				compute->ReflectionPipeline(SxavengerSystem::GetDxDevice());

				std::shared_ptr<UnorderedTexture> white1x1 = SxavengerContent::TryCreateUnorderedTexture("white1x1", { 1, 1 });
				white1x1->TransitionBeginUnordered(SxavengerSystem::GetMainThreadContext());
				compute->SetPipeline(SxavengerSystem::GetMainThreadContext()->GetDxCommand());

				DxObject::BindBufferDesc bind = {};
				bind.SetHandle("gOutput", white1x1->GetGPUHandleUAV());
				compute->BindComputeBuffer(SxavengerSystem::GetMainThreadContext()->GetDxCommand(), bind);

				compute->Dispatch(SxavengerSystem::GetMainThreadContext()->GetDxCommand(), { 1, 1, 1 });

				white1x1->TransitionEndUnordered(SxavengerSystem::GetMainThreadContext());
				SxavengerSystem::TransitionAllocator();
			}
		}
	);

	context->SetState(
		GameLoop::State::Init, UINT32_MAX, [this]() {
			SxavengerSystem::ExecuteAllAllocator();
		}
	);

	context->SetState(
		GameLoop::State::Term, std::nullopt, [this]() {
			SxavengerSystem::TermThreadCollection();
			SxavengerModule::Term();
			SxavengerContent::Term();
			SxavengerAsset::Term();
			SxavengerSystem::Term();
		}
	);

	context->SetState(
		GameLoop::State::Begin, 0, [this]() {
			Performance::BeginFrame();
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
			Performance::EndFrame();
		}
	);
}

void EngineGameLoop::Term() {
}
