#include "SampleRaytracingGameLoop.h"

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
// SampleRaytracingGameLoop class methods
////////////////////////////////////////////////////////////////////////////////////////////

void SampleRaytracingGameLoop::Init(GameLoop::Context* context) {
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

void SampleRaytracingGameLoop::Term() {
}

void SampleRaytracingGameLoop::InitSystem() {
	
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
		hitgroup_ = std::make_unique<DxrObject::RaytracingBlob>();
		hitgroup_->Create("packages/shaders/raytracing/demo/hitgroupDemo.hlsl");

		hitgroupExpt_ = std::make_unique<DxrObject::ExportGroup>();

		DxrObject::ExportGroup::Hitgroup hitgroup = {};
		hitgroup.closesthit = L"mainClosesthit";
		hitgroup.anyhit     = L"mainAnyhit";

		hitgroupExpt_->ExportHitgroup(L"HitgroupA", hitgroup);
		hitgroupExpt_->SetBlob(hitgroup_.get());

		DxrObject::LocalRootSignatureDesc desc = {};
		desc.SetVirtualSRV(0, 0); //!< gVertex
		desc.SetVirtualSRV(1, 1); //!< gIndex
		desc.SetHandleSRV(2, 2); //!< gAlbedo
		desc.SetSamplerLinear(DxObject::MODE_WRAP, DxObject::ShaderVisibility::VISIBILITY_ALL, 0); //!< gSampler

		hitgroupExpt_->CreateRootSignature(SxavengerSystem::GetDxDevice(), desc);
	}

	{
		stateObject_ = std::make_unique<DxrObject::StateObjectContext>();

		DxrObject::GlobalRootSignatureDesc desc = {};
		desc.SetHandleUAV(0, 10); //!< gOutput
		desc.SetVirtualSRV(1, 10); //!< gScene
		desc.SetVirtualCBV(2, 10); //!< gCamera

		stateObject_->CreateRootSignature(SxavengerSystem::GetDxDevice(), desc);

		DxrObject::StateObjectDesc stateDesc = {};

		stateDesc.AddExport(raygenerationExpt_.get());
		stateDesc.AddExport(missExpt_.get());
		stateDesc.AddExport(hitgroupExpt_.get());

		stateDesc.SetPayloadStride(sizeof(Vector3f));
		stateDesc.SetAttributeStride(sizeof(Vector2f));
		stateDesc.SetMaxRecursionDepth(4);

		stateObject_->CreateStateObject(SxavengerSystem::GetDxDevice(), stateDesc);
	}

	model_ = SxavengerAsset::TryImport<AssetModel>("asset/model/primitive/teapot.obj").WaitGet();
	model_->CreateBottomLevelAS(SxavengerSystem::GetMainThreadContext());

	floor_ = SxavengerAsset::TryImport<AssetModel>("asset/model/primitive/floor.obj").WaitGet();
	floor_->CreateBottomLevelAS(SxavengerSystem::GetMainThreadContext());

	topLevelAS_ = std::make_unique<DxrObject::TopLevelAS>();

	camera_ = std::make_unique<ACineCameraActor>();
	camera_->Init();

	texture_ = std::make_unique<UnorderedTexture>();
	texture_->Create({ 1280, 720 });

	camera2_ = std::make_unique<APivotCameraActor>();
	camera2_->Init();

	texture2_ = std::make_unique<UnorderedTexture>();
	texture2_->Create({ 1280, 720 });
	
	presenter_.Init();

}

void SampleRaytracingGameLoop::TermSystem() {
}

void SampleRaytracingGameLoop::UpdateSystem() {

	if (SxavengerSystem::IsTriggerKey(KeyId::KEY_1)) {
		isView1_ = true;
	}

	if (SxavengerSystem::IsTriggerKey(KeyId::KEY_2)) {
		isView1_ = false;
	}

	topLevelAS_->BeginSetupInstance();

	for (uint32_t i = 0; i < model_->GetMeshSize(); ++i) {

		DxrObject::TopLevelAS::Instance instance = {};
		instance.bottomLevelAS = model_->GetInputMesh(i).GetBottomLevelAS();
		instance.mat           = Matrix::MakeTranslate({ 0.0f, 0.0f, 8.0f});
		instance.expt          = hitgroupExpt_.get();

		DxrObject::WriteBindBufferDesc desc = {};
		desc.SetAddress(0, model_->GetInputMesh(i).GetVertex()->GetGPUVirtualAddress());
		desc.SetAddress(1, model_->GetInputMesh(i).GetIndex()->GetGPUVirtualAddress());
		desc.SetHandle(2, model_->GetTextureHandle(i));

		instance.parameter = std::move(desc);

		topLevelAS_->AddInstance(instance);
	}

	for (uint32_t i = 0; i < floor_->GetMeshSize(); ++i) {

		DxrObject::TopLevelAS::Instance instance = {};
		instance.bottomLevelAS = floor_->GetInputMesh(i).GetBottomLevelAS();
		instance.mat           = Matrix::MakeAffine({ 4.0f, 1.0f, 4.0f }, Vector3f{}, { 0.0f, -1.0f, 0.0f });
		instance.expt          = hitgroupExpt_.get();

		DxrObject::WriteBindBufferDesc desc = {};

		desc.SetAddress(0, floor_->GetInputMesh(i).GetVertex()->GetGPUVirtualAddress());
		desc.SetAddress(1, floor_->GetInputMesh(i).GetIndex()->GetGPUVirtualAddress());
		desc.SetHandle(2, floor_->GetTextureHandle(i));

		instance.parameter = std::move(desc);

		topLevelAS_->AddInstance(instance);
	}

	topLevelAS_->EndSetupInstance(SxavengerSystem::GetDxDevice(), SxavengerSystem::GetMainThreadContext()->GetDxCommand());

}

void SampleRaytracingGameLoop::DrawSystem() {
	stateObject_->UpdateShaderTable(SxavengerSystem::GetDxDevice(), topLevelAS_.get());

	auto command = SxavengerSystem::GetMainThreadContext()->GetDxCommand();

	{
		stateObject_->SetStateObject(command);

		command->GetCommandList()->SetComputeRootDescriptorTable(0, texture_->GetGPUHandleUAV());
		command->GetCommandList()->SetComputeRootShaderResourceView(1, topLevelAS_->GetGPUVirtualAddress());
		command->GetCommandList()->SetComputeRootConstantBufferView(2, camera_->GetGPUVirtualAddress());

		stateObject_->DispatchRays(command, texture_->GetSize());
	}

	{
		stateObject_->SetStateObject(command);

		command->GetCommandList()->SetComputeRootDescriptorTable(0, texture2_->GetGPUHandleUAV());
		command->GetCommandList()->SetComputeRootShaderResourceView(1, topLevelAS_->GetGPUVirtualAddress());
		command->GetCommandList()->SetComputeRootConstantBufferView(2, camera2_->GetGPUVirtualAddress());

		stateObject_->DispatchRays(command, texture_->GetSize());
	}

	main_->BeginRendering();
	main_->ClearWindow();

	if (isView1_) {
		presenter_.Present(SxavengerSystem::GetMainThreadContext(), texture_->GetSize(), texture_->GetGPUHandleSRV());

	} else {
		presenter_.Present(SxavengerSystem::GetMainThreadContext(), texture2_->GetSize(), texture2_->GetGPUHandleSRV());
	}
	//presenter_.Present(SxavengerSystem::GetMainThreadContext(), texture_->GetSize(), texture_->GetGPUHandleSRV());

	SxavengerSystem::RenderImGui();

	main_->EndRendering();
}
