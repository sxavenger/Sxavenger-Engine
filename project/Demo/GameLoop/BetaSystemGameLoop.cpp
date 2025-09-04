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
	camera_->GetComponent<CameraComponent>()->SetTag(CameraComponent::Tag::GameCamera);

	//SxavengerAsset::TryImport<AssetModel>("assets/models/PBR_Sphere_Test/model/PBR_Sphere.gltf");

	//atmosphere_ = std::make_unique<AtmosphereActor>();
	//atmosphere_->Init({ 1024, 1024 });

	//skylight_ = std::make_unique<SkyLightActor>();
	//skylight_->Init({ 1024, 1024 });
	//skylight_->SetTexture("assets/textures/textureCube/studio_small_09_4k.dds");
	//skylight_->Update(true);

	offlineSkylight_ = std::make_unique<MonoBehaviour>();
	auto light = offlineSkylight_->AddComponent<SkyLightComponent>();
	light->SetIrradiance(sUContentStorage->Import<UContentTexture>("assets/textures/textureCube/sky_irradiance.dds")->GetId());
	light->SetRadiance(sUContentStorage->Import<UContentTexture>("assets/textures/textureCube/sky_radiance.dds")->GetId());
	light->SetEnvironment(sUContentStorage->Import<UContentTexture>("assets/textures/textureCube/sky_environment.dds")->GetId());

	player_ = std::make_unique<Player>();
	player_->Load();
	player_->Awake();
	player_->Start();

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
	//behaviour_->GetComponent<PostProcessLayerComponent>()->AddPostProcess<PostProcessBloom>();
	//behaviour_->GetComponent<PostProcessLayerComponent>()->AddPostProcess<PostProcessDoF>();
	//behaviour_->GetComponent<PostProcessLayerComponent>()->AddPostProcess<PostProcessGrayScale>();
	behaviour_->GetComponent<PostProcessLayerComponent>()->AddPostProcess<PostProcessChromaticAberration>();
	behaviour_->GetComponent<PostProcessLayerComponent>()->AddPostProcess<PostProcessRadialBlur>();

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

	parameter_ = std::make_unique<ParameterActor>();
}

void BetaSystemGameLoop::TermSystem() {
}

void BetaSystemGameLoop::UpdateSystem() {

	//-----------------------------------------------------------------------------------------
	// Update
	//-----------------------------------------------------------------------------------------

	//atmosphere_->Update();
	camera_->Update();

	player_->Update();

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

}
