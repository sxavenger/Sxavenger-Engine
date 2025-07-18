#include "PreviewGameLoop.h"
_DXOBJECT_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/SxavengerSystem.h>
#include <Engine/Asset/SxavengerAsset.h>
#include <Engine/Component/Components/Camera/CameraComponent.h>
#include <Engine/Component/Components/Light/Punctual/PointLightComponent.h>
#include <Engine/Component/ComponentHelper.h>
#include <Engine/Editor/EditorEngine.h>
#include <Engine/Editor/Editors/DevelopEditor.h>

#include <Engine/Content/Exporter/TextureExporter.h>

////////////////////////////////////////////////////////////////////////////////////////////
// PreviewGameLoop class methods
////////////////////////////////////////////////////////////////////////////////////////////

void PreviewGameLoop::Init(GameLoop::Context* context) {
	context->SetProcess(GameLoop::Process::Init, std::nullopt, [this]() { InitGame(); });
	context->SetProcess(GameLoop::Process::Term, std::nullopt, [this]() { TermGame(); });
	context->SetProcess(GameLoop::Process::Update, std::nullopt, [this]() {
		if (auto develop = sEditorEngine->GetEditor<DevelopEditor>()) {
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

	main_ = SxavengerSystem::CreateMainWindow(kMainWindowSize, L"sxavenger engine preview window", { 0.14f, 0.2f, 0.24f, 1.f }).lock();
	main_->SetIcon("packages/icon/SxavengerEngineIcon.ico", { 32, 32 });

	presenter_.Init();
	texture_.Create(main_->GetSize(), DXGI_FORMAT_R32G32B32A32_FLOAT);

	environment_ = SxavengerAsset::TryImport<AssetTexture>(
		"assets/textures/textureCube/sky_environment.dds",
		Texture::Option{ Texture::Encoding::Intensity, false }
	);

	pipeline_ = std::make_unique<CustomReflectionComputePipeline>();
	pipeline_->CreateAsset("assets/shaders/test.cs.hlsl");
	pipeline_->RegisterBlob();
	pipeline_->ReflectionPipeline(SxavengerSystem::GetDxDevice());

	actor_.Init();

	parameter_ = std::make_unique<DxObject::DimensionBuffer<std::pair<uint32_t, float>>>();
	parameter_->Create(SxavengerSystem::GetDxDevice(), 1);
	parameter_->At(0).first  = 0;
	parameter_->At(0).second = 0.0f;
}

void PreviewGameLoop::TermGame() {
}

void PreviewGameLoop::UpdateGame() {

	if (SxavengerSystem::IsTriggerKey(KeyId::KEY_SPACE)) {
		parameter_->At(0).first++;
		parameter_->At(0).first %= 2;
	}

	if (SxavengerSystem::IsPressKey(KeyId::KEY_UP)) {
		parameter_->At(0).second += 0.1f;
	}

	if (SxavengerSystem::IsPressKey(KeyId::KEY_DOWN)) {
		parameter_->At(0).second -= 0.1f;
	}

	actor_.Update();
	ComponentHelper::UpdateTransform();

	texture_.TransitionBeginUnordered(SxavengerSystem::GetMainThreadContext());

	pipeline_->SetPipeline(SxavengerSystem::GetMainThreadContext()->GetDxCommand());

	DxObject::BindBufferDesc desc = {};
	desc.SetHandle("gOutput",      texture_.GetGPUHandleUAV());
	desc.SetHandle("gEnvironment", environment_.WaitAcquire()->GetGPUHandleSRV());
	desc.SetAddress("gCamera",     actor_.GetComponent<CameraComponent>()->GetGPUVirtualAddress());
	desc.SetAddress("gParameter",  parameter_->GetGPUVirtualAddress());
	pipeline_->BindComputeBuffer(SxavengerSystem::GetMainThreadContext()->GetDxCommand(), desc);

	pipeline_->Dispatch(SxavengerSystem::GetMainThreadContext()->GetDxCommand(), Vector3ui{ DxObject::RoundUp(texture_.GetSize().x, 16), DxObject::RoundUp(texture_.GetSize().y, 16), 1 });

	texture_.TransitionEndUnordered(SxavengerSystem::GetMainThreadContext());
}

void PreviewGameLoop::DrawGame() {

	main_->BeginRendering();
	main_->ClearWindow();

	presenter_.Present(SxavengerSystem::GetMainThreadContext(), main_->GetSize(), texture_.GetGPUHandleSRV());
	SxavengerSystem::RenderImGui();

	main_->EndRendering();
}

