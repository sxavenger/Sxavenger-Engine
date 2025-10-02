#include "ControllableCameraActor.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/SxavengerSystem.h>
#include <Engine/Editor/EditorEngine.h>
#include <Engine/Editor/Editors/RenderSceneEditor.h>

////////////////////////////////////////////////////////////////////////////////////////////
// ControllableCameraActor class methods
////////////////////////////////////////////////////////////////////////////////////////////

void ControllableCameraActor::Init() {
	// components
	MonoBehaviour::SetName("controllable camera actor");
	MonoBehaviour::AddComponent<TransformComponent>();
	MonoBehaviour::AddComponent<CameraComponent>();

	// input
	keyboard_ = SxavengerSystem::GetInput()->GetKeyboardInput();
	mouse_    = SxavengerSystem::GetInput()->GetMouseInput();
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

		if (sEditorEngine->IsEditorDisplay()) {
			sEditorEngine->ExecuteEditorFunction<RenderSceneEditor>([&](RenderSceneEditor* editor) {
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

	auto transform = MonoBehaviour::GetComponent<TransformComponent>();
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

	speed_ += mouse_->GetDeltaWheel() * 0.1f;
	speed_ = std::clamp(speed_, 0.1f, 10.0f);

	direction *= speed_ * static_cast<float>(SxavengerSystem::GetDeltaTime().time);

	auto transform = MonoBehaviour::GetComponent<TransformComponent>();
	transform->translate += Quaternion::RotateVector({ direction.x, 0.0f, direction.y }, transform->rotate);
}

void ControllableCameraActor::FixMouse() {
	// マウス座標の固定
	Vector2i point = static_cast<Vector2i>(kMainWindowSize) / 2;
	mouse_->SetPosition(point);

	// マウスの非表示
	mouse_->ShowCousor(false);
}
