#include "TitleScene.h"
SXAVENGER_ENGINE_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* scene
#include "GameScene.h"

//* engine
#include <Engine/System/System.h>
#include <Engine/System/Runtime/Input/Input.h>
#include <Engine/System/Utility/RuntimeLogger.h>
#include <Engine/Components/Component/Transform/TransformComponent.h>
#include <Engine/Components/Component/Light/Environment/SkyAtmosphereComponent.h>
#include <Engine/Components/Component/Light/Punctual/DirectionalLightComponent.h>

//* lib
#include <Lib/Math/Vector3.h>
#include <Lib/Math/Quaternion.h>
#include <Lib/Math/GeometryMath.h>

using namespace Demo;

////////////////////////////////////////////////////////////////////////////////////////////
// TitleScene class methods
////////////////////////////////////////////////////////////////////////////////////////////

void TitleScene::Init() {

	// 背景描画用のカメラを生成する.
	// FMainRenderはGameタグのCameraComponentを描画に使用するため, シーンごとにカメラを1つ用意する.
	camera_ = std::make_unique<PerspectiveCameraActor>();
	camera_->SetPoint({ 0.0f, 1.0f, -6.0f }); //!< タイトルの俯瞰位置

	// 空(大気散乱)を生成する. タイトルでは低い太陽角度で夕暮れの雰囲気にする.
	atmosphere_ = std::make_unique<GameObject>("title.atmosphere");
	(*atmosphere_)->AddComponent<TransformComponent>();
	(*atmosphere_)->AddComponent<SkyAtmosphereComponent>();
	(*atmosphere_)->AddComponent<DirectionalLightComponent>();

	(*(*atmosphere_)->GetComponent<TransformComponent>())->rotate
		= Quaternion::AxisAngle(Vector3f{ 1.0f, 0.0f, 0.0f }, kPi / 16.0f);
}

void TitleScene::Start() {
	RuntimeLogger::LogInformation("[Demo::TitleScene]", "enter title scene. press [SPACE] to start.");
}

void TitleScene::Update() {

	// 決定入力(SPACEのトリガ)でGameSceneへ遷移する.
	// Destruction::All で現在のシーン(Title)を破棄してから遷移するため, タイトルのオブジェクトは解放される.
	if (System::IsTriggerKey(KeyId::KEY_SPACE)) {
		SetTransition({ Transition::Destruction::All, { GameScene::kSceneName } });
	}
}

void TitleScene::LateUpdate() {
	// 大気散乱のLUT等はGPU更新が必要なため, 描画前に毎フレーム更新する.
	(*atmosphere_)->GetComponent<SkyAtmosphereComponent>()->Update(System::GetDirectQueueContext());
}
