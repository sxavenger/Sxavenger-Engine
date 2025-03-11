#include "BetaSystemGameLoop.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/SxavengerSystem.h>
#include <Engine/Asset/SxavengerAsset.h>
#include <Engine/Asset/Observer/AssetObserver.h>
#include <Engine/Editor/EditorEngine.h>
#include <Engine/Editor/Editors/DevelopEditor.h>
#include <Engine/Editor/Editors/HierarchyEditor.h>
#include <Engine/Render/FMainRender.h>
#include <Engine/Component/Components/Collider/ColliderComponent.h>
#include <Engine/Component/Components/Collider/CollisionManager.h>
#include <Engine/Component/MonoBehaviourContainer.h>

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
	mesh_ = observer.WaitGet()->CreateMonoBehavior("teapot");

	/*AssetObserver<AssetModel> observer = SxavengerAsset::TryImport<AssetModel>("assets/models/chessboard/chessboard.gltf");
	mesh_ = observer.WaitGet()->CreateMonoBehavior("chessboard");*/

	auto collider = mesh_->AddComponent<ColliderComponent>();
	collider->SetColliderBoundingSphere({ 1.0f });
	collider->SetTag("tag1");
	mesh_->AddComponent<TransformComponent>();
	mesh_->SetActive(false);
	mesh_->SetView(false);

	camera_ = std::make_unique<MonoBehaviour>();
	camera_->AddComponent<TransformComponent>();
	auto camera = camera_->AddComponent<CameraComponent>();
	camera->Init();
	camera->SetTag(CameraComponent::Tag::GameCamera);
	auto collider2 = camera_->AddComponent<ColliderComponent>();
	collider2->SetColliderBoundingSphere({ 1.0f });
	collider2->SetTag("tag2");

	//camera_->GetChild();


	light_ = std::make_unique<MonoBehaviour>();
	auto transform = light_->AddComponent<TransformComponent>();
	transform->GetTransform().rotate = AxisAngle({ 1.0f, 0.0f, 0.0f }, pi_v / 2.0f);
	auto light = light_->AddComponent<DirectionalLightComponent>();
	light->Init();
	light_->AddComponent<ColliderComponent>()->SetColliderBoundingSphere({ 1.0f });
	light_->GetComponent<ColliderComponent>()->SetTag("tag1");


	sCollisionManager->SetOnCollisionFunctions(
		"tag2", "tag1", {
			[](ColliderComponent*, ColliderComponent*) {
				Log("on collision enter");
			},
			[](ColliderComponent*, ColliderComponent*) {
				Log("on collision exit");
			}
		}
	);

	attribute_ = std::make_unique<Attribute>();
	attribute_->SetName("attribute yay");
	attribute_->SetAttributeFunc([this]() {
		ImGui::Text("attribute yay");
	});
}

void BetaSystemGameLoop::TermSystem() {
}

void BetaSystemGameLoop::UpdateSystem() {

	sMonoBehaviourContainer->ForEach(std::execution::par, [](MonoBehaviour* behaviour) {
		behaviour->UpdateComponent(); // todo: 遅延updateで何とかしたい.
	});
	// todo: engine側のgameloopに移動.

	sCollisionManager->CheckCollision();
}

void BetaSystemGameLoop::DrawSystem() {

	FMainRender::GetInstance()->GetRenderer()->Render(SxavengerSystem::GetMainThreadContext());

	main_->BeginRendering();
	main_->ClearWindow();

	FMainRender::GetInstance()->PresentMain(SxavengerSystem::GetMainThreadContext());
	SxavengerSystem::RenderImGui();

	main_->EndRendering();
}
