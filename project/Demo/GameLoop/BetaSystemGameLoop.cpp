#include "BetaSystemGameLoop.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/SxavengerSystem.h>
#include <Engine/Asset/SxavengerAsset.h>
#include <Engine/Editor/EditorEngine.h>
#include <Engine/Editor/Editors/DevelopEditor.h>
#include <Engine/Render/FMainRender.h>
#include <Engine/Component/Components/Collider/ColliderComponent.h>
#include <Engine/Component/Components/Collider/CollisionManager.h>
#include <Engine/Component/Components/SpriteRenderer/SpriteRendererComponent.h>
#include <Engine/Component/Components/Particle/ParticleComponent.h>
#include <Engine/Component/ComponentHelper.h>
#include <Engine/System/Runtime/Performance/DeltaTimePoint.h>
#include <Engine/Render/FRenderCore.h>
#include <Engine/Content/InputGeometry/InputPrimitiveHelper.h>

#include "Engine/Component/Components/Light/Environment/SkyLightComponent.h"

#include "Engine/System/Runtime/Thread/AsyncThread.h"

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

	SxavengerAsset::TryImport<AssetModel>("assets/models/scene/scene.gltf");
	SxavengerAsset::TryImport<AssetModel>("assets/models/PBR_Sphere_Test/model/PBR_Sphere.gltf");
	//SxavengerAsset::TryImport<AssetModel>("assets/models/chessboard/ABeautifulGame.gltf");
	//SxavengerAsset::TryImport<AssetModel>("assets/models/sponza/NewSponza_Main_glTF_003.gltf");
	//SxavengerAsset::TryImport<AssetModel>("assets/models/sponza_curtains/NewSponza_Curtains_glTF.gltf");
	
	lightA_  = ComponentHelper::CreatePointLightMonoBehaviour();
	lightB_  = ComponentHelper::CreateDirectionalLightMonoBehaviour();

	camera_->AddComponent<PostProcessLayerComponent>();
	camera_->GetComponent<PostProcessLayerComponent>()->AddPostProcess<PostProcessAutoExposure>();
	camera_->GetComponent<PostProcessLayerComponent>()->AddPostProcess<PostProcessLocalExposure>();
	//camera_->GetComponent<PostProcessLayerComponent>()->AddPostProcess<PostProcessBloom>();
	//camera_->GetComponent<PostProcessLayerComponent>()->AddPostProcess<PostProcessDoF>();

	camera_->AddComponent<CompositeProcessLayerComponent>();

	//auto texture = SxavengerAsset::TryImport<AssetTexture>("assets/textures/LUT/lut_greenish.png");
	auto texture = SxavengerAsset::TryImport<AssetTexture>("assets/textures/LUT/LUT_2StripTable.tga");
	auto lut = camera_->GetComponent<CompositeProcessLayerComponent>()->AddPostProcess<CompositeProcessLUT>();
	lut->CreateTexture(SxavengerSystem::GetMainThreadContext(), texture, { 16, 16 });

	//atmosphere_ = std::make_unique<AtmosphereActor>();
	//atmosphere_->Init({ 1024, 1024 });

	skylight_ = std::make_unique<SkyLightActor>();
	skylight_->Init({ 1024, 1024 });
	skylight_->SetTexture("assets/textures/textureCube/studio_small_09_4k.dds");
	skylight_->Update(true);
}

void BetaSystemGameLoop::TermSystem() {
}

void BetaSystemGameLoop::UpdateSystem() {

	//-----------------------------------------------------------------------------------------
	// Update
	//-----------------------------------------------------------------------------------------

	//atmosphere_->Update();
	camera_->Update();

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

	FMainRender::GetInstance()->Render(SxavengerSystem::GetMainThreadContext());

	main_->BeginRendering();
	main_->ClearWindow();

	FMainRender::GetInstance()->PresentMain(SxavengerSystem::GetMainThreadContext());
	SxavengerSystem::RenderImGui();

	main_->EndRendering();
}
