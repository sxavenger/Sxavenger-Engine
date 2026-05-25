#include "ExampleGameLoop.h"
SXAVENGER_ENGINE_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/Configuration/Configuration.h>
#include <Engine/System/System.h>
#include <Engine/System/Runtime/Performance/DeltaTimePoint.h>
#include <Engine/Assets/Content/ContentStorage.h>
#include <Engine/Components/Component/Transform/TransformComponent.h>
#include <Engine/Components/Component/Light/Environment/SkyAtmosphereComponent.h>
#include <Engine/Components/Component/Light/Environment/SkyLightComponent.h>
#include <Engine/Components/Component/Light/Rect/RectLightComponent.h>
#include <Engine/Components/Component/Transform/RectTransformComponent.h>
#include <Engine/Components/Component/CanvasRenderer/TextRendererComponent.h>
#include <Engine/Components/Component/PostProcessLayer/PostProcessLayerComponent.h>
#include <Engine/Components/Component/Collider/ColliderComponent.h>
#include <Engine/Components/Component/Collider/CollisionManager.h>
#include <Engine/Components/Component/ComponentHelper.h>
#include <Engine/Components/Entity/BehaviourHelper.h>
#include <Engine/Components/Entity/EntityBehaviourStorage.h>
#include <Engine/Render/FMainRender.h>
#include <Engine/Editors/EditorEngine.h>
#include <Engine/Editors/Editor/DevelopEditor.h>

#include <Engine/System/UI/SxGui.h>
#include <Engine/Graphics/Graphics.h>

//* lib
#include <Lib/Adapter/Random/Random.h>

////////////////////////////////////////////////////////////////////////////////////////////
// ExampleGameLoop class methods
////////////////////////////////////////////////////////////////////////////////////////////

void ExampleGameLoop::Init(Execution::Context* context) {
	context->SetProcess(Execution::Process::Init, std::nullopt, [this]() { InitSystem(); });
	context->SetProcess(Execution::Process::Term, std::nullopt, [this]() { TermSystem(); });
	context->SetProcess(Execution::Process::Update, std::nullopt, [this]() {
		if (auto develop = sEditorEngine->GetEditor<DevelopEditor>()) {
			if (!develop->IsProcessRequired()) {
				return;
			}
		}

		UpdateSystem();
	});
	context->SetProcess(Execution::Process::Render, std::nullopt, [this]() { RenderSystem(); });

	context->SetCondition([]() { return !System::ProcessMessage(); });             //!< windowが閉じられた場合
	context->SetCondition([]() { return System::IsPressKey(KeyId::KEY_ESCAPE); }); //!< ESCキーが押された
}

void ExampleGameLoop::Term() {
}

void ExampleGameLoop::InitSystem() {

	main_ = System::CreateMainWindow(
		Configuration::GetConfig().resolution,
		L"[Sxavenger Engine / Sxx Engine] Example Window"
	);
	main_->SetIcon(kPackagesDirectory / "icon" / "SxavengerEngineIcon.ico", { 32, 32 });

	{
		atmosphere_ = std::make_unique<GameObject>();
		(*atmosphere_)->AddComponent<TransformComponent>();
		(*atmosphere_)->AddComponent<SkyAtmosphereComponent>();
		(*atmosphere_)->AddComponent<DirectionalLightComponent>();

		(*(*atmosphere_)->GetComponent<TransformComponent>())->rotate = Quaternion::AxisAngle(Vector3f{ 12.0f, -1.0f, 0.0f }.Normalize(), kPi / 4.0f);
	}
	

	{
		camera_ = std::make_unique<PerspectiveCameraActor>();

		auto layer = (*camera_)->AddComponent<PostProcessLayerComponent>();
		layer->SetTag(PostProcessLayerComponent::Tag::Local);

		auto exposure = layer->AddPostProcess<PostProcessAutoExposure>();
		exposure->GetParameter().minLogLuminance = -8.0f;
		exposure->GetParameter().maxLogLuminance = 10.0f;
		exposure->GetParameter().compensation    = -5.0f;

		layer->AddPostProcess<PostProcessRadialBlur>(false);
		layer->AddPostProcess<PostProcessPostFx>();
		layer->AddPostProcess<PostProcessPosterize>(false);

		auto collider = (*camera_)->AddComponent<ColliderComponent>();
		collider->SetTag("camera");
	}
	

	performance_ = std::make_unique<PerformanceActor>();
	performance_->SetPosition({ 1190.0f, 0.0f });


#ifndef _DEVELOPMENT //!< デバッグビルドでは、シーンのロードに時間がかかるため、ロードしない.
	{
		json data;
		if (JsonHandler::LoadFromJson("assets/scene/sponza_lit.scene", data)) {
			sEntityBehaviourStorage->InputJson(data);
		}
	}
#endif

	sCollisionManager->SetOnCollisionFunctionEnter("camera", "cube",
		[](MAYBE_UNUSED ColliderComponent* const camera, MAYBE_UNUSED ColliderComponent* const cube, const CollisionDetection::Penetration& penetration) {

		camera->GetBehaviour()->GetComponent<TransformComponent>()->GetTransform().translate += penetration.direction * penetration.distance;
		camera->SetCollisionState(cube, ColliderComponent::History::Current, std::nullopt);
	});


	{
		human_ = std::make_unique<GameObject>();
		(*human_)->SetName("human");

		BehaviourHelper::CreateSkinnedMeshBehaviour(human_->GetAddress(), sContentStorage->Import<ContentModel>("assets/models/human/walking.gltf"));

		clip_ = sContentStorage->Import<ContentAnimation>("assets/models/human/walking.gltf")->GetAnimation(0);
	}
}

void ExampleGameLoop::TermSystem() {
}

void ExampleGameLoop::UpdateSystem() {

	//-----------------------------------------------------------------------------------------
	// Update
	//-----------------------------------------------------------------------------------------

	camera_->Update();

	auto keyboard = System::GetKeyboardInput();

	if (keyboard->IsPress(KeyId::KEY_LEFT)) {
		(*(*atmosphere_)->GetComponent<TransformComponent>())->rotate *= Quaternion::AxisAngle(Vector3f{ 1.0f, 1.0f, 0.0f }.Normalize(), 0.01f);
	}

	if (keyboard->IsPress(KeyId::KEY_RIGHT)) {
		(*(*atmosphere_)->GetComponent<TransformComponent>())->rotate *= Quaternion::AxisAngle(Vector3f{ 1.0f, 1.0f, 0.0f }.Normalize(), -0.01f);
	}

	static DeltaTimePointd<TimeUnit::second> time = 0.0f;
	time.AddDeltaTime();
	BehaviourHelper::ApplyAnimation(human_->GetAddress(), clip_.WaitGet()->GetAnimation(), time, true);

	performance_->Update();

	//-----------------------------------------------------------------------------------------
	// SystemUpdate
	//-----------------------------------------------------------------------------------------

	ComponentHelper::UpdateTransform();
	// todo: engine側のgameloopに移動.

	ComponentHelper::UpdateCollider();

	//-----------------------------------------------------------------------------------------
	// LateUpdate
	//-----------------------------------------------------------------------------------------

	(*atmosphere_)->GetComponent<SkyAtmosphereComponent>()->Update(System::GetDirectQueueContext());
	//!< TODO: ComponentHelperに移動予定

	//-----------------------------------------------------------------------------------------
	// final Update...?
	//-----------------------------------------------------------------------------------------

	ComponentHelper::UpdateSkinning();
	// todo: engine側のgameloopに移動.

	ComponentHelper::UpdateAudio3d();
}

void ExampleGameLoop::RenderSystem() {

	FMainRender::GetInstance()->Render(System::GetDirectQueueContext(), main_.get());

	main_->BeginRenderWindow(System::GetDirectQueueContext());
	main_->ClearWindow(System::GetDirectQueueContext());

	FMainRender::GetInstance()->Present(System::GetDirectQueueContext(), main_.get());
	System::RenderImGui(main_->GetClient());

	main_->EndRenderWindow(System::GetDirectQueueContext());

}
