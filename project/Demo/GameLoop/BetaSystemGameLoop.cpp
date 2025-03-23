#include "BetaSystemGameLoop.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/SxavengerSystem.h>
#include <Engine/Asset/SxavengerAsset.h>
#include <Engine/Editor/EditorEngine.h>
#include <Engine/Editor/Editors/DevelopEditor.h>
#include <Engine/Editor/Editors/HierarchyEditor.h>
#include <Engine/Render/FMainRender.h>
#include <Engine/Component/Components/Collider/ColliderComponent.h>
#include <Engine/Component/Components/Collider/CollisionManager.h>
#include <Engine/Component/Components/SpriteRenderer/SpriteRendererComponent.h>
#include <Engine/Component/ComponentHelper.h>
#include <Engine/System/Runtime/Performance/DeltaTimePoint.h>

//* c++
#include <execution>

////////////////////////////////////////////////////////////////////////////////////////////
// BetaSystemGameLoop class methods
////////////////////////////////////////////////////////////////////////////////////////////

void BetaSystemGameLoop::Init(GameLoop::Context* context) {
	context->SetProcess(GameLoop::Process::Init, std::nullopt, [this]() { InitSystem(); });
	context->SetProcess(GameLoop::Process::Term, std::nullopt, [this]() { TermSystem(); });
	context->SetProcess(GameLoop::Process::Update, std::nullopt, [this]() {
		if (auto develop = sEditorEngine->TryGetEditor<DevelopEditor>()) {
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
	main_->SetIcon("packages/icon/SxavengerEngineSubIcon.ico", { 32, 32 });

	AssetObserver<AssetModel> observer = SxavengerAsset::TryImport<AssetModel>("assets/models/primitive/teapot.obj");
	mesh_ = observer.WaitGet()->CreateStaticMeshBehaviour("teapot");

	/*AssetObserver<AssetModel> observer = SxavengerAsset::TryImport<AssetModel>("assets/models/chessboard/chessboard.gltf");
	mesh_ = observer.WaitGet()->CreateMonoBehavior("chessboard");*/

	mesh_->AddComponent<TransformComponent>();
	mesh_->SetActive(false);
	mesh_->SetView(false);

	camera_ = std::make_unique<MonoBehaviour>();
	camera_->SetName("game camera");
	camera_->AddComponent<TransformComponent>();
	auto camera = camera_->AddComponent<CameraComponent>();
	camera->Init();
	camera->SetTag(CameraComponent::Tag::GameCamera);

	auto sprite = camera_->AddComponent<SpriteRendererComponent>();
	sprite->SetTexture(SxavengerAsset::TryImport<AssetTexture>("assets/textures/uvChecker.png"));
	sprite->SetSize({ 120.0f, 120.0f });

	light_ = std::make_unique<MonoBehaviour>();
	light_->SetName("directional light");
	auto transform = light_->AddComponent<TransformComponent>();
	transform->GetTransform().rotate = Quaternion::AxisAngle({ 1.0f, 0.0f, 0.0f }, kPi / 2.0f);
	auto light = light_->AddComponent<DirectionalLightComponent>();
	light->Init();

	{
		AssetObserver<AssetModel> tree = SxavengerAsset::TryImport<AssetModel>("assets/models/objects/tree_low.obj");

		for (uint32_t i = 0; i < kTreeCount; ++i) {
			trees_[i] = std::make_unique<MonoBehaviour>();
			tree.WaitGet()->CreateStaticMeshBehaviour(trees_[i].get());
			trees_[i]->AddComponent<TransformComponent>();
			trees_[i]->SetName("tree[" + std::to_string(i) + "]");
		}
	}

	{
		AssetObserver<AssetModel> tile = SxavengerAsset::TryImport<AssetModel>("assets/models/objects/tile.obj");
		tile_ = tile.WaitGet()->CreateStaticMeshBehaviour("tile");
		tile_->AddComponent<TransformComponent>();
	}

	{
		AssetObserver<AssetModel> cube = SxavengerAsset::TryImport<AssetModel>("assets/models/human/idle.gltf");
		human_ = cube.WaitGet()->CreateSkinnedMeshBehaviour("animatedCube");
		human_->AddComponent<TransformComponent>();

		animator_ = SxavengerAsset::TryImport<AssetAnimator>("assets/models/human/idle.gltf");
		/*auto child = cube_->GetChild("AnimatedCube");
		child->GetComponent<SkinnedMeshRendererComponent>()->UpdateAnimation(animator_.WaitGet()->GetAnimation(0), 0.0f, true);*/
	}
}

void BetaSystemGameLoop::TermSystem() {
}

void BetaSystemGameLoop::UpdateSystem() {

	static DeltaTimePointf<TimeUnit::second> time = 0.0f;
	time.AddDeltaTime();

	ComponentHelper::ApplyAnimation(human_.get(), animator_.WaitGet()->GetAnimation(0), time, true);

	ComponentHelper::UpdateTransform();
	// todo: engine側のgameloopに移動.

	ComponentHelper::UpdateSkinning();
	// todo: engine側のgameloopに移動.

	sCollisionManager->CheckCollision();

}

void BetaSystemGameLoop::DrawSystem() {

	FMainRender::GetInstance()->Render(SxavengerSystem::GetMainThreadContext());

	main_->BeginRendering();
	main_->ClearWindow();

	FMainRender::GetInstance()->PresentMain(SxavengerSystem::GetMainThreadContext());
	SxavengerSystem::RenderImGui();

	main_->EndRendering();
}
