#include "MainGameLoop.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/SxavengerSystem.h>
#include <Engine/Editor/EditorEngine.h>
#include <Engine/Editor/Editors/EngineDeveloperEditor.h>
#include <Engine/Editor/Editors/RenderSceneEditor.h>

#include "Engine/!Render/FRenderCore.h"

////////////////////////////////////////////////////////////////////////////////////////////
// MainGameLoop class methods
////////////////////////////////////////////////////////////////////////////////////////////

void MainGameLoop::Init(GameLoop::Context* context) {
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

void MainGameLoop::Term() {
}

void MainGameLoop::InitSystem() {

	FRenderCore::GetInstance()->Init();

	main_ = SxavengerSystem::CreateMainWindow(kMainWindowSize, L"CG3_Advance2").lock();
	main_->SetIcon("packages/icon/SxavengerEngineSubIcon.ico", { 32, 32 });

	//* render *//

	textures_ = std::make_unique<FSceneTextures>();
	textures_->Create(main_->GetSize());

	scene_ = std::make_unique<FScene>();

	renderer_ = std::make_unique<FSceneRenderer>();
	renderer_->SetTextures(textures_.get());
	renderer_->SetScene(scene_.get());

	//* camera *//

	camera_ = std::make_unique<ACineCameraActor>();
	camera_->Init();
	renderer_->SetCamera(camera_.get());

	//* geometries *//

	sphere_ = std::make_unique<AModelActor>();
	sphere_->Init();
	sphere_->SetName("sphere");
	sphere_->SetModel(SxavengerAsset::TryImport<AssetModel>("asset/model/primitive/sphere.obj"));
	sphere_->SetRenderWait(false);
	scene_->AddGeometry(sphere_.get());

	plane1_ = std::make_unique<AModelActor>();
	plane1_->Init();
	plane1_->SetName("plane1");
	plane1_->SetModel(SxavengerAsset::TryImport<AssetModel>("asset/model/primitive/plane.obj"));
	scene_->AddGeometry(plane1_.get());

	plane2_ = std::make_unique<AModelActor>();
	plane2_->Init();
	plane2_->SetName("plane2");
	plane2_->SetModel(SxavengerAsset::TryImport<AssetModel>("asset/model/primitive/plane.gltf"));
	scene_->AddGeometry(plane2_.get());



	//* light *//

	directionals_[0] = std::make_unique<ADirectionalLightActor>();
	directionals_[0]->Init();
	directionals_[0]->GetIsActive() = false;
	scene_->AddLight(directionals_[0].get());

	directionals_[1] = std::make_unique<ADirectionalLightActor>();
	directionals_[1]->Init();
	directionals_[1]->GetIsActive() = false;
	scene_->AddLight(directionals_[1].get());

	points_[0] = std::make_unique<APointLightActor>();
	points_[0]->Init();
	points_[0]->GetIsActive() = true;
	scene_->AddLight(points_[0].get());

	points_[1] = std::make_unique<APointLightActor>();
	points_[1]->Init();
	points_[1]->GetIsActive() = false;
	scene_->AddLight(points_[1].get());

	spots_[0] = std::make_unique<ASpotLightActor>();
	spots_[0]->Init();
	spots_[0]->GetIsActive() = false;
	scene_->AddLight(spots_[0].get());

	spots_[1] = std::make_unique<ASpotLightActor>();
	spots_[1]->Init();
	spots_[1]->GetIsActive() = false;
	scene_->AddLight(spots_[1].get());

	chessboard_ = std::make_unique<AModelActor>();
	chessboard_->Init();
	chessboard_->SetName("chessboard");
	chessboard_->SetModel(SxavengerAsset::TryImport<AssetModel>("asset/model/chessboard/chessboard.gltf"));
	chessboard_->SetRenderWait(false);
	scene_->AddGeometry(chessboard_.get());

	//* editors *//

	sEditorEngine->ExecuteEditorFunction<RenderSceneEditor>([this](RenderSceneEditor* editor) {
		editor->SetGameRenderer(renderer_.get());
	});
}

void MainGameLoop::TermSystem() {
}

void MainGameLoop::UpdateSystem() {
}

void MainGameLoop::DrawSystem() {

	renderer_->Render(SxavengerSystem::GetMainThreadContext());

	sEditorEngine->ExecuteEditorFunction<RenderSceneEditor>([](RenderSceneEditor* editor) {
		editor->Draw();
	});

	main_->BeginRendering();
	main_->ClearWindow();

	//presenter_.Present(SxavengerSystem::GetMainThreadContext(), textures_->GetSize(), textures_->GetGBuffer(FSceneTextures::GBufferLayout::Result)->GetGPUHandleSRV());

	SxavengerSystem::RenderImGui();

	main_->EndRendering();

}
