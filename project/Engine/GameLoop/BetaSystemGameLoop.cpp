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

#include "Engine/!Render/FRenderCore.h"
#include "Engine/System/Config/SxavengerDirectory.h"

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


	{
		raygeneration_ = std::make_unique<DxrObject::RaytracingBlob>();
		raygeneration_->Create("packages/shaders/raytracing/demo/raygenerationDemo.hlsl");

		raygenerationExpt_ = std::make_unique<DxrObject::ExportGroup>();
		raygenerationExpt_->ExportRaygeneration(L"mainRaygeneration");
		raygenerationExpt_->SetBlob(raygeneration_.get());
		//raygenerationExpt_->CreateRootSignature();
	}

	{
		miss_ = std::make_unique<DxrObject::RaytracingBlob>();
		miss_->Create("packages/shaders/raytracing/demo/missDemo.hlsl");

		missExpt_ = std::make_unique<DxrObject::ExportGroup>();
		missExpt_->ExportMiss(L"mainMiss");
		missExpt_->SetBlob(miss_.get());
		//missExpt_->CreateRootSignature();
	}

	{
		stateObject_ = std::make_unique<DxrObject::StateObjectContext>();

		DxrObject::GlobalRootSignatureDesc desc = {};
		desc.SetHandleUAV(0, 10); //!< gOutput
		//desc.SetHandleSRV(1, 10); //!< gScene

		stateObject_->CreateRootSignature(SxavengerSystem::GetDxDevice(), desc);

		DxrObject::StateObjectDesc stateDesc = {};

		stateDesc.AddExport(raygenerationExpt_.get());
		stateDesc.AddExport(missExpt_.get());

		stateDesc.SetPayloadStride(sizeof(Vector3f));
		stateDesc.SetAttributeStride(sizeof(Vector2f));
		stateDesc.SetMaxRecursionDepth(1);

		stateObject_->CreateStateObject(SxavengerSystem::GetDxDevice(), stateDesc);
	}

	model_ = SxavengerAsset::TryImport<AssetModel>("asset/model/primitive/teapot.obj").WaitGet();
	model_->CreateBottomLevelAS(SxavengerSystem::GetMainThreadContext());

	topLevelAS_ = std::make_unique<DxrObject::TopLevelAS>();

	texture_ = std::make_unique<UnorderedTexture>();
	texture_->Create({ 1280, 720 });
	

}

void BetaSystemGameLoop::TermSystem() {
}

void BetaSystemGameLoop::UpdateSystem() {

	for (uint32_t i = 0; i < model_->GetMeshSize(); ++i) {

		DxrObject::TopLevelAS::Instance instance = {};
		instance.bottomLevelAS = model_->GetInputMesh(i).GetBottomLevelAS();
		instance.mat           = Matrix4x4::Identity();
		
	}

}

void BetaSystemGameLoop::DrawSystem() {

	stateObject_->UpdateShaderTable(SxavengerSystem::GetDxDevice(), nullptr);

	auto command = SxavengerSystem::GetMainThreadContext()->GetDxCommand();

	stateObject_->SetStateObject(command);

	command->GetCommandList()->SetComputeRootDescriptorTable(0, texture_->GetGPUHandleUAV());

	stateObject_->DispatchRays(command, texture_->GetSize());

	main_->BeginRendering();
	main_->ClearWindow();

	SxavengerSystem::RenderImGui();

	main_->EndRendering();
}
