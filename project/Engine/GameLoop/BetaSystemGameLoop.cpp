#include "BetaSystemGameLoop.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/SxavengerSystem.h>
#include <Engine/Content/SxavengerContent.h>
#include <Engine/Editor/EditorEngine.h>
#include <Engine/Editor/Editors/DevelopEditor.h>
#include <Engine/Render/FMainRender.h>
#include <Engine/Component/Components/Collider/ColliderComponent.h>
#include <Engine/Component/Components/Collider/CollisionManager.h>
#include <Engine/Component/Components/SpriteRenderer/SpriteRendererComponent.h>
#include <Engine/Component/Components/Particle/ParticleComponent.h>
#include <Engine/Component/Components/PostProcessLayer/PostProcessLayerComponent.h>
#include <Engine/Component/Components/Transform/RectTransformComponent.h>
#include <Engine/Component/Components/TextRenderer/TextRendererComponent.h>
#include <Engine/Component/Components/Audio/AudioSourceComponent.h>
#include <Engine/Component/ComponentHelper.h>
#include <Engine/System/Runtime/Performance/DeltaTimePoint.h>
#include <Engine/Render/FRenderCore.h>
#include <Engine/Content/InputGeometry/InputPrimitiveHelper.h>

#include "Engine/Component/Components/Light/Environment/SkyLightComponent.h"

#include "Engine/Preview/Content/UContentStorage.h"

////////////////////////////////////////////////////////////////////////////////////////////
// BetaSystemGameLoop class methods
////////////////////////////////////////////////////////////////////////////////////////////

void BetaSystemGameLoop::Init(GameLoop::Context* context) {
	context->SetProcess(GameLoop::Process::Init, std::nullopt, [this]() { InitSystem(); });
	context->SetProcess(GameLoop::Process::Term, std::nullopt, [this]() { TermSystem(); });
	context->SetProcess(GameLoop::Process::Update, std::nullopt, [this]() {
		if (auto develop = sEditorEngine->GetEditor<DevelopEditor>()) {
			if (!develop->IsProcessRequired()) {
				return;
			}
		}

		UpdateSystem();
	});
	context->SetProcess(GameLoop::Process::Render, std::nullopt, [this]() { DrawSystem(); });

	context->SetCondition(
		[this]() { return !SxavengerSystem::ProcessMessage(); }
	);
}

void BetaSystemGameLoop::Term() {
}

void BetaSystemGameLoop::InitSystem() {

	main_ = SxavengerSystem::CreateMainWindow(kMainWindowSize, L"sxavenger engine beta window").lock();
	main_->SetIcon("packages/icon/SxavengerEngineIcon.ico", { 32, 32 });

	camera_ = std::make_unique<ControllableCameraActor>();
	camera_->Init();
	camera_->GetComponent<CameraComponent>()->SetTag(CameraComponent::Tag::Game);

	offlineSkylight_ = std::make_unique<MonoBehaviour>();
	auto light = offlineSkylight_->AddComponent<SkyLightComponent>();
	light->SetIrradiance(sUContentStorage->Import<UContentTexture>("assets/textures/textureCube/sky_irradiance.dds")->GetId());
	light->SetRadiance(sUContentStorage->Import<UContentTexture>("assets/textures/textureCube/sky_radiance.dds")->GetId());
	light->SetEnvironment(sUContentStorage->Import<UContentTexture>("assets/textures/textureCube/sky_environment.dds")->GetId());

	behaviour_ = std::make_unique<MonoBehaviour>();
	behaviour_->AddComponent<TransformComponent>();
	behaviour_->AddComponent<PostProcessLayerComponent>();
	behaviour_->GetComponent<PostProcessLayerComponent>()->SetTag(PostProcessLayerComponent::Tag::None);
	behaviour_->GetComponent<PostProcessLayerComponent>()->AddPostProcess<PostProcessAutoExposure>();
	behaviour_->GetComponent<PostProcessLayerComponent>()->AddPostProcess<PostProcessLocalExposure>(false);
	behaviour_->GetComponent<PostProcessLayerComponent>()->AddPostProcess<PostProcessChromaticAberration>();
	behaviour_->GetComponent<PostProcessLayerComponent>()->AddPostProcess<PostProcessRadialBlur>();
	behaviour_->GetComponent<PostProcessLayerComponent>()->AddPostProcess<PostProcessMotionBlur>();

	sUContentStorage->Import<UContentTexture>("assets/textures/LUT/lut_greenish.png", UContentTexture::Option{ UContentTexture::Encoding::Intensity, false });
	sUContentStorage->Import<UContentTexture>("assets/textures/LUT/lut_reddish.png", UContentTexture::Option{ UContentTexture::Encoding::Intensity, false });
	sUContentStorage->Import<UContentTexture>("assets/textures/LUT/lut_sepia.png", UContentTexture::Option{ UContentTexture::Encoding::Intensity, false });

	const auto& texture = sUContentStorage->Import<UContentTexture>("assets/textures/LUT/lut_reddish.png", UContentTexture::Option{ UContentTexture::Encoding::Intensity, false })->GetId();
	auto lut = behaviour_->GetComponent<PostProcessLayerComponent>()->AddPostProcess<PostProcessLUT>();
	lut->CreateTexture(SxavengerSystem::GetDirectQueueContext(), texture, { 16, 16 });

	auto t = behaviour_->AddComponent<RectTransformComponent>();
	auto text = behaviour_->AddComponent<TextRendererComponent>();

	text->SetFont(sUContentStorage->Import<UContentFont>("assets/font/MPLUSRounded1c-Regular.ttf")->GetId());
	text->SetText(L"Sxavenger Engine");

	t->GetTransform().translate = { 200.0f, 200.0f };

	performance_ = std::make_unique<PerformanceActor>();
	performance_->Init();

	//* test *//

	texture_ = sUContentStorage->Import<UContentTexture>("assets/textures/uvchecker.png")->GetId();

	Vector2ui size     = texture_.WaitRequire()->GetMetadata().size;
	DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM;

	downsample_.Create(size / 4u, format, FBaseTexture::Flag::All);
	upscale_.Create(size, format, FBaseTexture::Flag::All);

	pipelineA_.CreateBlob("assets/shaders/downsample.cs.hlsl");
	pipelineA_.ReflectionPipeline(SxavengerSystem::GetDxDevice());

	pipelineB_.CreateBlob("assets/shaders/upscale.cs.hlsl");
	pipelineB_.ReflectionPipeline(SxavengerSystem::GetDxDevice());

}

void BetaSystemGameLoop::TermSystem() {
}

void BetaSystemGameLoop::UpdateSystem() {

	//-----------------------------------------------------------------------------------------
	// Update
	//-----------------------------------------------------------------------------------------

	//atmosphere_->Update();
	camera_->Update();

	performance_->Update();

	//leadParticle_->Update();

	//-----------------------------------------------------------------------------------------
	// SystemUpdate...?
	//-----------------------------------------------------------------------------------------

	ComponentHelper::UpdateTransform();
	// todo: engine側のgameloopに移動.

	sCollisionManager->CheckCollision();
	// todo: engine側のgameloopに移動.

	//-----------------------------------------------------------------------------------------
	// LateUpdate
	//-----------------------------------------------------------------------------------------

	//-----------------------------------------------------------------------------------------
	// final Update...?
	//-----------------------------------------------------------------------------------------

	ComponentHelper::UpdateSkinning();
	// todo: engine側のgameloopに移動.

	//* test *//
	

}

void BetaSystemGameLoop::DrawSystem() {

	auto context = SxavengerSystem::GetDirectQueueContext();

	context->GetCommandList()->ClearUnorderedAccessViewFloat(
		downsample_.GetGPUHandleUAV(),
		downsample_.GetCPUHandleUAV(),
		downsample_.GetResource(),
		&kOrigin4<float>.x,
		0, nullptr
	);

	{
		downsample_.TransitionBeginUnordered(context);

		pipelineA_.SetPipeline(context->GetDxCommand());

		DxObject::BindBufferDesc desc = {};
		desc.SetHandle("gOutput", downsample_.GetGPUHandleUAV());
		desc.SetHandle("gInput",  texture_.WaitRequire()->GetGPUHandleSRV());

		pipelineA_.BindComputeBuffer(context->GetDxCommand(), desc);
		pipelineA_.Dispatch(context->GetDxCommand(), { DxObject::RoundUp(downsample_.GetSize().x, 16), DxObject::RoundUp(downsample_.GetSize().y, 16), 1 });

		downsample_.TransitionEndUnordered(context);
	}

	{
		upscale_.TransitionBeginUnordered(context);

		pipelineB_.SetPipeline(context->GetDxCommand());

		DxObject::BindBufferDesc desc = {};
		desc.SetHandle("gOutput", upscale_.GetGPUHandleUAV());
		desc.SetHandle("gInput",  downsample_.GetGPUHandleSRV());

		pipelineB_.BindComputeBuffer(context->GetDxCommand(), desc);
		pipelineB_.Dispatch(context->GetDxCommand(), { DxObject::RoundUp(upscale_.GetSize().x, 16), DxObject::RoundUp(upscale_.GetSize().y, 16), 1 });

		upscale_.TransitionEndUnordered(context);

	}

	FMainRender::GetInstance()->Render(SxavengerSystem::GetDirectQueueContext(), main_.get());

	main_->BeginRenderWindow(SxavengerSystem::GetDirectQueueContext());
	main_->ClearWindow(SxavengerSystem::GetDirectQueueContext());
	 
	FMainRender::GetInstance()->PresentMain(SxavengerSystem::GetDirectQueueContext());
	SxavengerSystem::RenderImGui();

	main_->EndRenderWindow(SxavengerSystem::GetDirectQueueContext());

	//* test *//

}
