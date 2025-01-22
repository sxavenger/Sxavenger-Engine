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

	//* render system *//

	textures_ = std::make_unique<FSceneTextures>();
	textures_->Create(main_->GetSize());

	scene_ = std::make_unique<FScene>();

	renderer_ = std::make_unique<FSceneRenderer>();
	renderer_->SetTextures(textures_.get());
	renderer_->SetScene(scene_.get());
	renderer_->GetConfig().isUseRaytracing = false;

	camera_ = std::make_unique<ACineCameraActor>();
	camera_->Init();

	renderer_->SetCamera(camera_.get());

	model_ = std::make_unique<AModelActor>();
	model_->Init();
	model_->SetModel(SxavengerAsset::TryImport<AssetModel>("asset/model/player.obj"));
	//model_->SetModel(SxavengerAsset::TryImport<AssetModel>("asset/model/primitive/teapot.obj"));
	//model_->SetModel(SxavengerAsset::TryImport<AssetModel>("asset/model/chessBoard/chessBoard.gltf"));
	model_->SetRenderWait(false);

	floor_ = std::make_unique<AFloorActor>();
	floor_->Init();
	floor_->SetSize({ 12.0f, 12.0f });

	animation_ = std::make_unique<AModelAnimationActor>();
	animation_->SetModel(SxavengerAsset::TryImport<AssetModel>("asset/model/human/idle.gltf"));
	animation_->Init();

	animator_ = SxavengerAsset::TryImport<AssetAnimator>("asset/model/human/idle.gltf");

	scene_->AddGeometry(model_.get());
	scene_->AddGeometry(floor_.get());
	scene_->AddGeometry(animation_.get());

	light1_ = std::make_unique<APointLightActor>();
	light1_->Init();
	light1_->GetParameter().color_intensity = { 1.0f, 1.0f, 1.0f, 1.0f };

	light2_ = std::make_unique<APointLightActor>();
	light2_->Init();
	light2_->GetParameter().color_intensity = { 1.0f, 1.0f, 1.0f, 1.0f };

	light3_ = std::make_unique<ADirectionalLightActor>();
	light3_->Init();
	light3_->GetParameter().color_intensity = { 1.0f, 1.0f, 1.0f, 1.0f };

	light4_ = std::make_unique<ASpotLightActor>();
	light4_->Init();
	light4_->GetParameter().color_intensity = { 1.0f, 1.0f, 1.0f, 1.0f };

	scene_->AddLight(light1_.get());
	//scene_->AddLight(light2_.get());
	//scene_->AddLight(light3_.get());
	//scene_->AddLight(light4_.get());

	FRenderCore::GetInstance()->Init();

	canvas_ = std::make_unique<FCanvas>();
	canvas_->SetTextures(textures_.get());

	auto texture = SxavengerAsset::TryImport<AssetTexture>("asset/textures/uvChecker.png");

	sprite_ = std::make_unique<LSprite>();
	sprite_->Init();
	sprite_->SetTexture(texture.WaitGet()->GetGPUHandleSRV());
	sprite_->SetPosition({ 100.0f, 100.0f });
	sprite_->SetSize({ 400.0f, 400.0f });

	canvas_->AddLayer(sprite_.get());

	collider_ = std::make_unique<Collider>();
	collider_->SetColliderBoundingOBB();
	collider_->SetToCollection();

	sEditorEngine->ExecuteEditorFunction<RenderSceneEditor>([this](RenderSceneEditor* editor) {
		editor->SetGameRenderer(renderer_.get());
	});

	//* presenter *//

	presenter_.Init();
}

void BetaSystemGameLoop::TermSystem() {
	sEditorEngine->RemoveEditor<RenderSceneEditor>();
}

void BetaSystemGameLoop::UpdateSystem() {
	//model_->GetTransform().scale  = { 4.0f, 4.0f, 4.0f };
	model_->GetTransform().rotate *= MakeAxisAngle({ 0.0f, 1.0f, 0.0f }, 0.01f);
	model_->UpdateMatrix();

	time_ += SxavengerSystem::GetDeltaTime();
	animation_->GetSkeleton()->UpdateAnimation(animator_.WaitGet()->GetAnimation(0), time_, true);

	if (SxavengerSystem::IsTriggerKey(KeyId::KEY_SPACE)) {
		if (attribute_ == nullptr) {
			attribute_ = std::make_unique<AttributeComponent>();
			attribute_->GetName() = "new attribute";
			attribute_->SetToOutliner();

		} else {
			attribute_.reset();
		}
	}
}

void BetaSystemGameLoop::DrawSystem() {

	sEditorEngine->ExecuteEditorFunction<RenderSceneEditor>([](RenderSceneEditor* editor) {
		editor->Draw();
	});

	renderer_->Render(SxavengerSystem::GetMainThreadContext());
	canvas_->Render(SxavengerSystem::GetMainThreadContext());

	main_->BeginRendering();
	main_->ClearWindow();

	presenter_.Present(SxavengerSystem::GetMainThreadContext(), main_->GetSize(), textures_->GetGBuffer(FSceneTextures::GBufferLayout::Result)->GetGPUHandleSRV());

	SxavengerSystem::RenderImGui();

	main_->EndRendering();
}
