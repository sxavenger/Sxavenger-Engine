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
#include <Engine/Component/Components/Particle/GPUParticleComponent.h>
#include <Engine/Component/Components/Particle/EmitterComponent.h>
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

	//leadParticle_ = std::make_unique<LeadParticle>();
	//leadParticle_->Load();
	//leadParticle_->Awake();
	//leadParticle_->Start();

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

	texture_.Create(kMainWindowSize * 16u, DXGI_FORMAT_R32G32B32A32_FLOAT);
	pipeline_.CreateBlob(kAssetsDirectory / "shaders" / "test.cs.hlsl");
	pipeline_.ReflectionPipeline(SxavengerSystem::GetDxDevice());
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

	ComponentHelper::UpdateParticle();

}

void BetaSystemGameLoop::DrawSystem() {

	FMainRender::GetInstance()->Render(SxavengerSystem::GetDirectQueueContext(), main_.get());

	main_->BeginRenderWindow(SxavengerSystem::GetDirectQueueContext());
	main_->ClearWindow(SxavengerSystem::GetDirectQueueContext());
	 
	FMainRender::GetInstance()->PresentMain(SxavengerSystem::GetDirectQueueContext());
	SxavengerSystem::RenderImGui();

	main_->EndRenderWindow(SxavengerSystem::GetDirectQueueContext());

	//* test *//

	{

		FRenderTargetBuffer* buffer = FMainRender::GetInstance()->GetTextures();
		FScene*              scene  = FMainRender::GetInstance()->GetScene();

		texture_.TransitionBeginUnordered(SxavengerSystem::GetDirectQueueContext());

		pipeline_.SetPipeline(SxavengerSystem::GetDirectQueueContext()->GetDxCommand());

		DxObject::BindBufferDesc desc = {};
		desc.SetAddress("gScene", scene->GetTopLevelAS().GetGPUVirtualAddress());
		desc.SetHandle("gDirect", buffer->GetGBuffer(FLightingGBuffer::Layout::Direct)->GetGPUHandleSRV());
		desc.SetHandle("gOutput", texture_.GetGPUHandleUAV());
		desc.SetAddress("gDeferredBufferIndex", buffer->GetIndexBufferAddress());
		desc.SetAddress("gCamera", ComponentHelper::GetCameraComponent(CameraComponent::Tag::Game)->GetGPUVirtualAddress());

		pipeline_.BindComputeBuffer(SxavengerSystem::GetDirectQueueContext()->GetDxCommand(), desc);
		pipeline_.Dispatch(SxavengerSystem::GetDirectQueueContext()->GetDxCommand(), { DxObject::RoundUp(kMainWindowSize.x, 32), DxObject::RoundUp(kMainWindowSize.y, 32), 1});

		texture_.TransitionEndUnordered(SxavengerSystem::GetDirectQueueContext());
		
	}

}
