#include "ControllableCameraActor.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/Configuration/Configuration.h>
#include <Engine/System/System.h>
#include <Engine/Components/Entity/BehaviourHelper.h>
#include <Engine/Components/Component/Camera/CameraComponent.h>
#include <Engine/Components/Component/Transform/TransformComponent.h>
#include <Engine/Editors/EditorEngine.h>
#include <Engine/Editors/Editor/RenderSceneEditor.h>

////////////////////////////////////////////////////////////////////////////////////////////
// ControllableCameraActor class methods
////////////////////////////////////////////////////////////////////////////////////////////

ControllableCameraActor::ControllableCameraActor()
	: GameObject("controllable camera") {
	//* behaviourの設定
	address_->AddComponent<SXAVENGER_ENGINE TransformComponent>();
	address_->AddComponent<SXAVENGER_ENGINE CameraComponent>()->SetTag(SXAVENGER_ENGINE CameraComponent::Tag::Game);

	//* inputの取得
	keyboard_ = SXAVENGER_ENGINE System::GetKeyboardInput();
	mouse_    = SXAVENGER_ENGINE System::GetMouseInput();
}

void ControllableCameraActor::Update() {
	if (CheckNeedUpdate()) {
		Around();
		Move();
		FixMouse();
	}
}

bool ControllableCameraActor::CheckNeedUpdate() {
	bool result = false;

	if (mouse_->IsPress(MouseId::MOUSE_RIGHT)) {

		result = true;

		if (SXAVENGER_ENGINE sEditorEngine->IsEditorDisplay()) {
			SXAVENGER_ENGINE sEditorEngine->ExecuteEditorFunction<SXAVENGER_ENGINE RenderSceneEditor>([&](SXAVENGER_ENGINE RenderSceneEditor* editor) {
				result = editor->IsFocusGameWindow();
			});
		}
	}

	return result;
}

void ControllableCameraActor::Around() {
	Vector2f delta = mouse_->GetDeltaPosition();

	angle_ += delta * 0.002f;

	angle_.y = std::clamp(angle_.y, -kPi, kPi);
	angle_.x = std::fmod(angle_.x, kTau);

	auto transform = address_->GetComponent<SXAVENGER_ENGINE TransformComponent>();
	transform->rotate = Quaternion::AxisAngle({ 0.0f, 1.0f, 0.0f }, angle_.x) * Quaternion::AxisAngle({ 1.0f, 0.0f, 0.0f }, angle_.y);
}

void ControllableCameraActor::Move() {

	Vector2f direction = {};

	if (keyboard_->IsPress(KeyId::KEY_W)) {
		direction.y += 1.0f;
	}

	if (keyboard_->IsPress(KeyId::KEY_S)) {
		direction.y -= 1.0f;
	}

	if (keyboard_->IsPress(KeyId::KEY_A)) {
		direction.x -= 1.0f;
	}

	if (keyboard_->IsPress(KeyId::KEY_D)) {
		direction.x += 1.0f;
	}

	if (keyboard_->IsPress(KeyId::KEY_LSHIFT)) {
		direction *= 2.0f;
	}

	speed_ += mouse_->GetDeltaWheelNormalized() * 0.1f;
	speed_ = std::clamp(speed_, 0.1f, 10.0f);

	direction *= speed_ * SXAVENGER_ENGINE System::GetDeltaTimef().time;

	auto transform = address_->GetComponent<SXAVENGER_ENGINE TransformComponent>();
	transform->translate += Quaternion::RotateVector({ direction.x, 0.0f, direction.y }, transform->rotate);

}

void ControllableCameraActor::FixMouse() {
	// マウス座標の固定
	Vector2i point = static_cast<Vector2i>(SXAVENGER_ENGINE Configuration::GetConfig().resolution) / 2;
	mouse_->SetPosition(point);

	// マウスの非表示
	mouse_->ShowCousor(false);
}
