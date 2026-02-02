#include "PerspectiveCameraActor.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/Configuration/Configuration.h>
#include <Engine/System/System.h>
#include <Engine/Components/Component/Camera/CameraComponent.h>
#include <Engine/Components/Component/Transform/TransformComponent.h>
#include <Engine/Editors/EditorEngine.h>
#include <Engine/Editors/Editor/RenderSceneEditor.h>

////////////////////////////////////////////////////////////////////////////////////////////
// PerspectiveCameraActor class methods
////////////////////////////////////////////////////////////////////////////////////////////

PerspectiveCameraActor::PerspectiveCameraActor()
	: GameObject("perspective camera") {

	//* behaviourの設定
	address_->AddComponent<SXAVENGER_ENGINE TransformComponent>();
	address_->AddComponent<SXAVENGER_ENGINE CameraComponent>()->SetTag(SXAVENGER_ENGINE CameraComponent::Tag::Game);

}

void PerspectiveCameraActor::Inspectable() {
	if (SxImGui::ComboEnum("perspective", &perspective_)) {
		SetPerspective(perspective_);
	}

	SxImGui::DragScalar<float>("sensitivity", &sensitivity_, 0.0001f, 0.0f, std::nullopt);
	SxImGui::DragScalar<float>("speed", &speed_, 0.01f, 0.1f, std::nullopt);

	switch (perspective_) {
		case Perspective::FirstPerson:
			ImGui::Text(std::format("angle: {}", angle_).c_str());
			break;

		case Perspective::ThirdPerson:
			ImGui::Text(std::format("angle: {}", angle_).c_str());
			ImGui::Text(std::format("point: {}", point_).c_str());
			ImGui::Text(std::format("distance: {}", distance_).c_str());
			break;
	}
}

bool PerspectiveCameraActor::Update() {

	if (!ShouldUpdate()) {
		return false;
	}

	bool isControl = false;

	const SXAVENGER_ENGINE KeyboardInput* keyboard = SXAVENGER_ENGINE System::GetKeyboardInput();
	const SXAVENGER_ENGINE MouseInput* mouse       = SXAVENGER_ENGINE System::GetMouseInput();

	if (keyboard != nullptr && mouse != nullptr) {
		switch (perspective_) {
			case Perspective::FirstPerson:
				if (mouse->IsPress(MouseId::MOUSE_RIGHT)) {
					AroundFirstPerson(keyboard, mouse);
					MoveFirstPerson(keyboard, mouse);
					FixMouseFirstPerson(keyboard, mouse);

					isControl |= true;
				}
				break;

			case Perspective::ThirdPerson:
				isControl |= AroundThirdPerson(keyboard, mouse);
				isControl |= MoveThirdPerson(keyboard, mouse);
				break;
		}
	}

	UpdateTransform();

	return isControl;
}

void PerspectiveCameraActor::UpdateTransform() {

	auto transform = address_->GetComponent<SXAVENGER_ENGINE TransformComponent>();

	rotation_ = Quaternion::AxisAngle(kUnitY3<float>, angle_.x) * Quaternion::AxisAngle(kUnitX3<float>, angle_.y);

	Vector3f direction = Quaternion::RotateVector(kBackward3<float>, rotation_);

	(*transform)->translate = point_ + direction * distance_;
	(*transform)->rotate    = rotation_;
}


void PerspectiveCameraActor::SetPerspective(Perspective perspective) {
	perspective_ = perspective;

	auto transform = address_->GetComponent<SXAVENGER_ENGINE TransformComponent>();

	distance_ = 0.0f;
	point_    = transform->GetPosition();
}

void PerspectiveCameraActor::AroundFirstPerson(MAYBE_UNUSED const SXAVENGER_ENGINE KeyboardInput* keyboard, MAYBE_UNUSED const SXAVENGER_ENGINE MouseInput* mouse) {
	//!< マウス移動による視点変更
	angle_ += Vector2f{ mouse->GetDeltaPosition() } * sensitivity_;

	angle_.y = std::clamp(angle_.y, -kPi / 2.0f, kPi / 2.0f);
	angle_.x = std::fmod(angle_.x, kTau);

	rotation_ = Quaternion::AxisAngle(kUnitY3<float>, angle_.x) * Quaternion::AxisAngle(kUnitX3<float>, angle_.y);
}

void PerspectiveCameraActor::MoveFirstPerson(MAYBE_UNUSED const SXAVENGER_ENGINE KeyboardInput* keyboard, MAYBE_UNUSED const SXAVENGER_ENGINE MouseInput* mouse) {
	//!< キーボード入力による移動

	Vector2f direction = {};
	Vector3f velocity  = {};

	if (keyboard->IsPress(KeyId::KEY_W)) {
		direction.y += 1.0f;
	}

	if (keyboard->IsPress(KeyId::KEY_S)) {
		direction.y -= 1.0f;
	}

	if (keyboard->IsPress(KeyId::KEY_A)) {
		direction.x -= 1.0f;
	}

	if (keyboard->IsPress(KeyId::KEY_D)) {
		direction.x += 1.0f;
	}

	if (keyboard->IsPress(KeyId::KEY_E)) {
		velocity.y += 1.0f;
	}

	if (keyboard->IsPress(KeyId::KEY_Q)) {
		velocity.y -= 1.0f;
	}

	if (keyboard->IsPress(KeyId::KEY_LSHIFT)) {
		direction *= 2.0f;
		velocity  *= 2.0f;
	}

	speed_ += mouse->GetDeltaWheelNormalized() * 0.1f; //!< fixme: 定数化
	speed_ = std::max(speed_, 0.1f);

	direction *= speed_ * SXAVENGER_ENGINE System::GetDeltaTimef().time;
	velocity  *= speed_ * SXAVENGER_ENGINE System::GetDeltaTimef().time;

	velocity += Quaternion::RotateVector({ direction.x, 0.0f, direction.y }, rotation_);

	point_ += velocity;

}

void PerspectiveCameraActor::FixMouseFirstPerson(MAYBE_UNUSED const SXAVENGER_ENGINE KeyboardInput* keyboard, MAYBE_UNUSED const SXAVENGER_ENGINE MouseInput* mouse) {
	// マウス座標の固定
	Vector2i point = static_cast<Vector2i>(SXAVENGER_ENGINE Configuration::GetConfig().resolution) / 2;
	mouse->SetPosition(point);

	// マウスの非表示
	mouse->SetShowCursor(false);
}

bool PerspectiveCameraActor::AroundThirdPerson(MAYBE_UNUSED const SXAVENGER_ENGINE KeyboardInput* keyboard, MAYBE_UNUSED const SXAVENGER_ENGINE MouseInput* mouse) {

	bool isControl = false;

	//!< マウス移動による視点変更
	if (mouse->IsPress(MouseId::MOUSE_MIDDLE)) {
		angle_ += Vector2f{ mouse->GetDeltaPosition() } * sensitivity_;

		angle_.y = std::clamp(angle_.y, -kPi / 2.0f, kPi / 2.0f);
		angle_.x = std::fmod(angle_.x, kTau);

		rotation_ = Quaternion::AxisAngle(kUnitY3<float>, angle_.x) * Quaternion::AxisAngle(kUnitX3<float>, angle_.y);

		isControl = true;
	}

	return isControl;
}

bool PerspectiveCameraActor::MoveThirdPerson(MAYBE_UNUSED const SXAVENGER_ENGINE KeyboardInput* keyboard, MAYBE_UNUSED const SXAVENGER_ENGINE MouseInput* mouse) {

	bool isControl = false;

	if (mouse->IsPress(MouseId::MOUSE_RIGHT)) {

		//!< マウス移動による移動
		{
			Vector2f delta = Vector2f{ mouse->GetDeltaPosition() } * sensitivity_;

			Vector3f right = Quaternion::RotateVector(kUnitX3<float>, rotation_);
			Vector3f up    = Quaternion::RotateVector(kUnitY3<float>, rotation_);

			Vector3f velocity = {};

			velocity -= right * delta.x * speed_;
			velocity += up * delta.y * speed_;

			point_ += velocity;
		}
		
		//!< キーボード入力による移動
		{

			Vector2f direction = {};
			Vector3f velocity  = {};

			if (keyboard->IsPress(KeyId::KEY_W)) {
				direction.y += 1.0f;
			}

			if (keyboard->IsPress(KeyId::KEY_S)) {
				direction.y -= 1.0f;
			}

			if (keyboard->IsPress(KeyId::KEY_A)) {
				direction.x -= 1.0f;
			}

			if (keyboard->IsPress(KeyId::KEY_D)) {
				direction.x += 1.0f;
			}

			if (keyboard->IsPress(KeyId::KEY_E)) {
				velocity.y += 1.0f;
			}

			if (keyboard->IsPress(KeyId::KEY_Q)) {
				velocity.y -= 1.0f;
			}

			if (keyboard->IsPress(KeyId::KEY_LSHIFT)) {
				direction *= 2.0f;
				velocity  *= 2.0f;
			}

			speed_ += mouse->GetDeltaWheelNormalized() * 0.1f; //!< fixme: 定数化
			speed_ = std::max(speed_, 0.1f);
			
			direction *= speed_ * SXAVENGER_ENGINE System::GetDeltaTimef().time;
			velocity  *= speed_ * SXAVENGER_ENGINE System::GetDeltaTimef().time;

			Vector3f forward = Quaternion::RotateVector(kUnitZ3<float>, rotation_);
			Vector3f right   = Quaternion::RotateVector(kUnitX3<float>, rotation_);

			point_ += (forward * direction.y + right * direction.x) + velocity;

		}

		isControl = true;

	} else {
		//!< マウスホイールによる距離変更
		if (mouse->IsWheel()) {
			distance_ = std::max(distance_ - mouse->GetDeltaWheelNormalized(), 0.0f);
			isControl = true;
		}
	}

	return isControl;
}

bool PerspectiveCameraActor::ShouldUpdate() const {

	bool result = false;

	switch (address_->GetComponent<SXAVENGER_ENGINE CameraComponent>()->GetTag()) {
		case SXAVENGER_ENGINE CameraComponent::Tag::Game:
			result = true;

#ifdef _DEVELOPMENT
			if (SXAVENGER_ENGINE sEditorEngine->IsEditorDisplay()) {
				SXAVENGER_ENGINE sEditorEngine->ExecuteEditorFunction<SXAVENGER_ENGINE RenderSceneEditor>([&](SXAVENGER_ENGINE RenderSceneEditor* editor) {
					result = editor->IsFocusGameWindow();
				});
			}
#endif
			break;

		case SXAVENGER_ENGINE CameraComponent::Tag::Editor:
#ifdef _DEVELOPMENT
			if (SXAVENGER_ENGINE sEditorEngine->IsEditorDisplay()) {
				SXAVENGER_ENGINE sEditorEngine->ExecuteEditorFunction<SXAVENGER_ENGINE RenderSceneEditor>([&](SXAVENGER_ENGINE RenderSceneEditor* editor) {
					result = editor->IsFocusSceneWindow();
				});
			}
#endif
			break;
	}

	return result;
}
