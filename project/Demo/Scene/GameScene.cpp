#include "GameScene.h"
SXAVENGER_ENGINE_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* scene
#include "TitleScene.h"

//* engine
#include <Engine/System/System.h>
#include <Engine/System/Runtime/Input/Input.h>
#include <Engine/System/Utility/RuntimeLogger.h>
#include <Engine/Assets/Content/ContentStorage.h>
#include <Engine/Components/Component/Transform/TransformComponent.h>
#include <Engine/Components/Component/Light/Environment/SkyAtmosphereComponent.h>
#include <Engine/Components/Component/Light/Punctual/DirectionalLightComponent.h>
#include <Engine/Components/Component/Collider/ColliderComponent.h>
#include <Engine/Components/Component/Collider/CollisionManager.h>
#include <Engine/Components/Entity/BehaviourHelper.h>

//* lib
#include <Lib/Math/Vector3.h>
#include <Lib/Math/Quaternion.h>
#include <Lib/Math/GeometryMath.h>

using namespace Demo;

////////////////////////////////////////////////////////////////////////////////////////////
// GameScene class methods
////////////////////////////////////////////////////////////////////////////////////////////

void GameScene::Init() {

	// 操作可能なカメラを生成する (Gameタグ付きのCameraComponentを内部で持つ).
	camera_ = std::make_unique<PerspectiveCameraActor>();

	// カメラに衝突判定を持たせ, "cube"との接触時に押し戻す.
	auto collider = (*camera_)->AddComponent<ColliderComponent>();
	collider->SetTag("camera");

	// 空(大気散乱)を生成する. ゲーム中は太陽を高めにして昼の雰囲気にする.
	atmosphere_ = std::make_unique<GameObject>("game.atmosphere");
	(*atmosphere_)->AddComponent<TransformComponent>();
	(*atmosphere_)->AddComponent<SkyAtmosphereComponent>();
	(*atmosphere_)->AddComponent<DirectionalLightComponent>();

	(*(*atmosphere_)->GetComponent<TransformComponent>())->rotate
		= Quaternion::AxisAngle(Vector3f{ 12.0f, -1.0f, 0.0f }.Normalize(), kPi / 4.0f);

	// アニメーションするキャラクターを生成する.
	// スキニングメッシュのBehaviourを構築し, 同じモデルからアニメーションクリップを取得する.
	human_ = std::make_unique<GameObject>("human");
	BehaviourHelper::CreateSkinnedMeshBehaviour(
		human_->GetAddress(),
		sContentStorage->Import<ContentModel>("assets/models/human/walking.gltf")
	);
	clip_ = sContentStorage->Import<ContentAnimation>("assets/models/human/walking.gltf")->GetAnimation(0);

	// カメラ("camera")とステージ("cube")の接触時のコールバックを登録する.
	// めり込み量分だけカメラを押し戻し, カメラがステージをすり抜けないようにする.
	sCollisionManager->SetOnCollisionFunctionEnter("camera", "cube",
		[](MAYBE_UNUSED ColliderComponent* const camera, MAYBE_UNUSED ColliderComponent* const cube, const CollisionDetection::Penetration& penetration) {
			camera->GetBehaviour()->GetComponent<TransformComponent>()->GetTransform().translate += penetration.direction * penetration.distance;
			camera->SetCollisionState(cube, ColliderComponent::History::Current, std::nullopt);
		});
}

void GameScene::Start() {
	RuntimeLogger::LogInformation("[Demo::GameScene]", "enter game scene. press [BACKSPACE] to return to title.");
}

void GameScene::Update() {

	// カメラの操作更新 (マウス/キーボードによる移動・回転).
	camera_->Update();

	// 左右キーで太陽(DirectionalLight)の向きを回転させ, 時間帯を変化させる.
	auto keyboard = System::GetKeyboardInput();

	if (keyboard->IsPress(KeyId::KEY_LEFT)) {
		(*(*atmosphere_)->GetComponent<TransformComponent>())->rotate *= Quaternion::AxisAngle(Vector3f{ 1.0f, 1.0f, 0.0f }.Normalize(), 0.01f);
	}

	if (keyboard->IsPress(KeyId::KEY_RIGHT)) {
		(*(*atmosphere_)->GetComponent<TransformComponent>())->rotate *= Quaternion::AxisAngle(Vector3f{ 1.0f, 1.0f, 0.0f }.Normalize(), -0.01f);
	}

	// アニメーション時刻を進め, キャラクターへ適用する.
	time_.AddDeltaTime();
	BehaviourHelper::ApplyAnimation(human_->GetAddress(), clip_.WaitGet()->GetAnimation(), time_, true);

	// 戻る入力(BACKSPACEのトリガ)でTitleSceneへ遷移する.
	if (System::IsTriggerKey(KeyId::KEY_BACKSPACE)) {
		SetTransition({ Transition::Destruction::All, { TitleScene::kSceneName } });
	}
}

void GameScene::LateUpdate() {
	// 大気散乱のLUT等はGPU更新が必要なため, 描画前に毎フレーム更新する.
	(*atmosphere_)->GetComponent<SkyAtmosphereComponent>()->Update(System::GetDirectQueueContext());
}
