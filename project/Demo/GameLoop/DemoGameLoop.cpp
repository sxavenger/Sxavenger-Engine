//[DemoGameLoop.cpp]
#include "DemoGameLoop.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/SxavengerSystem.h>
#include <Engine/Asset/SxavengerAsset.h>
#include <Engine/Editor/EditorEngine.h>
#include <Engine/Editor/Editors/DevelopEditor.h>
#include <Engine/Render/FMainRender.h>
#include <Engine/Content/InputGeometry/InputPrimitiveHelper.h>
#include <Engine/Component/Components/Collider/ColliderComponent.h>
#include <Engine/Component/Components/Collider/CollisionManager.h>
#include <Engine/Component/Components/SpriteRenderer/SpriteRendererComponent.h>
#include <Engine/Component/Components/PostProcessLayer/PostProcessLayerComponent.h>
#include <Engine/Component/Components/Particle/EmitterComponent.h>
#include <Engine/Component/Components/Particle/GPUParticleComponent.h>
#include <Engine/Component/ComponentHelper.h>
#include <Engine/Module/Scene/SceneObjects.h>

//* lib
#include <Lib/Adapter/Random/Random.h>

////////////////////////////////////////////////////////////////////////////////////////////
// DemoGameLoop class methods
////////////////////////////////////////////////////////////////////////////////////////////

void DemoGameLoop::Init(GameLoop::Context* context) {
	context->SetProcess(GameLoop::Process::Init, std::nullopt, [this]() { InitGame(); });
	context->SetProcess(GameLoop::Process::Term, std::nullopt, [this]() { TermGame(); });
	context->SetProcess(GameLoop::Process::Update, std::nullopt, [this]() {
		if (auto developer = sEditorEngine->GetEditor<DevelopEditor>()) {
			if (!developer->IsProcessRequired()) {
				return;
			}
		}

		UpdateGame();
	});
	context->SetProcess(GameLoop::Process::Render, std::nullopt, [this]() { DrawGame(); });

	context->SetCondition([this]() { return !SxavengerSystem::ProcessMessage(); });
}

void DemoGameLoop::Term() {
}

void DemoGameLoop::InitGame() {

	main_ = SxavengerSystem::CreateMainWindow(kMainWindowSize, L"Sxavenger Engine Demo").lock();
	main_->SetIcon("packages/icon/SxavengerEngineIcon.ico", { 32, 32 });

	player_ = std::make_unique<Player>();
	player_->Load();
	player_->Awake();
	player_->Start();

	AssetObserver<AssetScene> scene = SxavengerAsset::TryImport<AssetScene>("assets/scene/sponza.scene");
	sSceneObjects->InputJson(scene.Acquire()->GetData());

	SetCollisionCallback();

	skylight_ = std::make_unique<MonoBehaviour>();
	skylight_->SetName("skylight");
	auto light = skylight_->AddComponent<SkyLightComponent>();
	light->GetDiffuseParameter().SetTexture(SxavengerAsset::Import<AssetTexture>("assets/textures/textureCube/sky_irradiance.dds"));
	light->GetSpecularParameter().SetTexture(SxavengerAsset::Import<AssetTexture>("assets/textures/textureCube/sky_radiance.dds"));
	light->SetEnvironment(SxavengerAsset::Import<AssetTexture>("assets/textures/textureCube/sky_environment.dds").WaitAcquire()->GetGPUHandleSRV());
	light->GetParameter().intensity = 0.3f;

	volume_ = std::make_unique<MonoBehaviour>();
	volume_->SetName("volume");
	volume_->AddComponent<TransformComponent>();
	volume_->GetComponent<TransformComponent>()->GetTransform().scale = { 14, 14, 14 };
	volume_->AddComponent<PostProcessLayerComponent>();
	volume_->GetComponent<PostProcessLayerComponent>()->SetTag(PostProcessLayerComponent::Tag::Volume);

	volume_->GetComponent<PostProcessLayerComponent>()->AddPostProcess<PostProcessChromaticAberration>();
	volume_->GetComponent<PostProcessLayerComponent>()->AddPostProcess<PostProcessRadialBlur>();

	auto texture = SxavengerAsset::TryImport<AssetTexture>("assets/textures/LUT/lut_reddish.png", Texture::Option{ Texture::Encoding::Intensity, false });
	auto lut = volume_->GetComponent<PostProcessLayerComponent>()->AddPostProcess<PostProcessLUT>();
	lut->CreateTexture(SxavengerSystem::GetDirectQueueContext(), texture, { 16, 16 });

	// test
	auto collider = volume_->AddComponent<ColliderComponent>();
	collider->SetTag("Wall");
	collider->SetColliderBoundingAABB(
		CollisionBoundings::AABB{
			.min = { -7.0f, -7.0f, -7.0f },
			.max = { 7.0f, 7.0f, 7.0f }
		}
	);
}

void DemoGameLoop::TermGame() {
}

void DemoGameLoop::UpdateGame() {

	//-----------------------------------------------------------------------------------------
	// GameLogic Update
	//-----------------------------------------------------------------------------------------

	player_->Update();

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

void DemoGameLoop::DrawGame() {

	FMainRender::GetInstance()->Render(SxavengerSystem::GetDirectQueueContext(), main_.get());

	main_->BeginRenderWindow(SxavengerSystem::GetDirectQueueContext());
	main_->ClearWindow(SxavengerSystem::GetDirectQueueContext());


	FMainRender::GetInstance()->PresentMain(SxavengerSystem::GetDirectQueueContext());
	SxavengerSystem::RenderImGui();
	
	main_->EndRenderWindow(SxavengerSystem::GetDirectQueueContext());
}

void DemoGameLoop::SetCollisionCallback() {

	sCollisionManager->SetOnCollisionFunctionStay(
		"Player", "Wall",
		[](_MAYBE_UNUSED ColliderComponent* const player, _MAYBE_UNUSED ColliderComponent* const wall) {
		CollisionHelper::PushBackAABB(player, wall);
	});

}
