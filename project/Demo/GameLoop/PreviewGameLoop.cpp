#include "PreviewGameLoop.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/SxavengerSystem.h>
#include <Engine/Asset/SxavengerAsset.h>
#include <Engine/Editor/EditorEngine.h>
#include <Engine/Editor/Editors/DevelopEditor.h>

////////////////////////////////////////////////////////////////////////////////////////////
// PreviewGameLoop class methods
////////////////////////////////////////////////////////////////////////////////////////////

void PreviewGameLoop::Init(GameLoop::Context* context) {
	context->SetProcess(GameLoop::Process::Init, std::nullopt, [this]() { InitGame(); });
	context->SetProcess(GameLoop::Process::Term, std::nullopt, [this]() { TermGame(); });
	context->SetProcess(GameLoop::Process::Update, std::nullopt, [this]() {
		if (auto develop = sEditorEngine->TryGetEditor<DevelopEditor>()) {
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

	main_ = SxavengerSystem::CreateMainWindow(kMainWindowSize, L"sxavenger engine beta window").lock();
	main_->SetIcon("packages/icon/SxavengerEngineIcon.ico", { 32, 32 });

	pipeline_ = std::make_unique<CustomReflectionComputePipeline>();
	pipeline_->CreateAsset("assets/shaders/test.cs.hlsl");
	pipeline_->RegisterBlob();
	pipeline_->ReflectionPipeline(SxavengerSystem::GetDxDevice());

	pipeline2_ = std::make_unique<CustomReflectionComputePipeline>();
	pipeline2_->CreateAsset("assets/shaders/reset.cs.hlsl");
	pipeline2_->RegisterBlob();
	pipeline2_->ReflectionPipeline(SxavengerSystem::GetDxDevice());

	static const uint32_t kUnorderedCount = 12;

	{ //!< Append

		append_ = DxObject::CreateBufferResource(
			SxavengerSystem::GetDxDevice()->GetDevice(),
			D3D12_HEAP_TYPE_DEFAULT,
			sizeof(Data) * kUnorderedCount,
			D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS,
			D3D12_RESOURCE_STATE_UNORDERED_ACCESS
		);

		counter_ = DxObject::CreateBufferResource(
			SxavengerSystem::GetDxDevice()->GetDevice(),
			D3D12_HEAP_TYPE_DEFAULT,
			sizeof(uint32_t),
			D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS,
			D3D12_RESOURCE_STATE_UNORDERED_ACCESS
		);

		descriptor_ = SxavengerSystem::GetDescriptor(DxObject::kDescriptor_UAV);

		D3D12_UNORDERED_ACCESS_VIEW_DESC desc = {};
		desc.ViewDimension               = D3D12_UAV_DIMENSION_BUFFER;
		desc.Buffer.NumElements          = kUnorderedCount;
		desc.Buffer.StructureByteStride  = sizeof(Data);

		SxavengerSystem::GetDxDevice()->GetDevice()->CreateUnorderedAccessView(
			append_.Get(),
			counter_.Get(),
			&desc,
			descriptor_.GetCPUHandle()
		);
	}
}

void PreviewGameLoop::TermGame() {
}

void PreviewGameLoop::UpdateGame() {

	//!< reset
	{
		pipeline2_->SetPipeline(SxavengerSystem::GetMainThreadContext()->GetDxCommand());

		DxObject::BindBufferDesc desc = {};
		desc.SetAddress("gCounter", counter_->GetGPUVirtualAddress());
		pipeline2_->BindComputeBuffer(SxavengerSystem::GetMainThreadContext()->GetDxCommand(), desc);
		pipeline2_->Dispatch(SxavengerSystem::GetMainThreadContext()->GetDxCommand(), { 1, 1, 1 });
	}
	
	//!< append
	{
		pipeline_->SetPipeline(SxavengerSystem::GetMainThreadContext()->GetDxCommand());

		DxObject::BindBufferDesc desc = {};
		desc.SetHandle("gAppend",  descriptor_.GetGPUHandle());
		desc.SetHandle("gConsume", descriptor_.GetGPUHandle());
		pipeline_->BindComputeBuffer(SxavengerSystem::GetMainThreadContext()->GetDxCommand(), desc);
		pipeline_->Dispatch(SxavengerSystem::GetMainThreadContext()->GetDxCommand(), { 1, 1, 1 });
	}

}

void PreviewGameLoop::DrawGame() {

	main_->BeginRendering();
	main_->ClearWindow();

	SxavengerSystem::RenderImGui();

	main_->EndRendering();
}

