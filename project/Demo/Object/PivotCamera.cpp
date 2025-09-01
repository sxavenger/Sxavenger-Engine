#include "PivotCamera.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/UI/SxImGui.h>
#include <Engine/System/SxavengerSystem.h>
#include <Engine/Component/Components/PostProcessLayer/PostProcessLayerComponent.h>
#include <Engine/Render/FMainRender.h>

//* lib
#include <Lib/Geometry/VectorComparision.h>

////////////////////////////////////////////////////////////////////////////////////////////
// PivotCamera class methods
////////////////////////////////////////////////////////////////////////////////////////////

void PivotCamera::Load() {
}

void PivotCamera::Awake() {
	MonoBehaviour::SetName("pivot camera");

	camera_ = std::make_unique<PivotCameraActor>();
	camera_->Init();
	camera_->SetName("camera");
	camera_->SetDistance(10.0f);

	MonoBehaviour::AddChild(camera_.get());

	keyboard_ = SxavengerSystem::GetInput()->GetKeyboardInput();
	gamepad_  = SxavengerSystem::GetInput()->GetGamepadInput(0);

	auto camera = camera_->GetComponent<CameraComponent>();
	camera->SetTag(CameraComponent::Tag::GameCamera);

	auto process = MonoBehaviour::AddComponent<PostProcessLayerComponent>();
	process->SetTag(PostProcessLayerComponent::Tag::Global);
	auto exposure = process->AddPostProcess<PostProcessAutoExposure>();
	exposure->GetParameter().compensation    = -2.0f;
	exposure->GetParameter().minLogLuminance = -1.0f;

	process->AddPostProcess<PostProcessBloom>();
}

void PivotCamera::Start() {
}

void PivotCamera::Update() {
	Rotation();
	camera_->Update();
}

void PivotCamera::Inspectable() {

	SxImGui::DragVector3("offset", &offset_.x, 0.01f);
	SxImGui::DragFloat("max speed", &maxSpeed_, 0.01f, 0.0f, 1.0f);

}

Vector2f PivotCamera::GetInputDirection() const {

	Vector2f direction = {};

	//* keyboard inputs *//

	if (keyboard_->IsPress(KeyId::KEY_UP)) {
		direction.y += 1.0f;
	}

	if (keyboard_->IsPress(KeyId::KEY_DOWN)) {
		direction.y -= 1.0f;
	}

	if (keyboard_->IsPress(KeyId::KEY_LEFT)) {
		direction.x -= 1.0f;
	}

	if (keyboard_->IsPress(KeyId::KEY_RIGHT)) {
		direction.x += 1.0f;
	}

	//* gamepad inputs *//

	if (gamepad_->IsConnect()) {
		Vector2f stick  = gamepad_->GetStickNormalized(GamepadStickId::STICK_RIGHT);
		float length    = stick.Length();
		float threshold = 0.2f;

		if (length >= threshold) {
			direction += stick;
		}
	}

	return direction.Normalize();

}

void PivotCamera::Rotation() {

	Vector2f input = GetInputDirection();

	if (All(input == kOrigin2<float>)) { //!< 入力がない場合

		Vector2f inverse = -speed_;

		if (inverse.Length() > 0.01f) {
			speed_ += inverse * 0.2f; // 徐々に減速

		} else {
			speed_ = kOrigin2<float>; // 完全に停止
		}

	} else {

		speed_ += input * 0.01f;

		speed_ = Vector2f::Clamp(
			speed_,
			-kUnit2<float> * maxSpeed_,
			kUnit2<float> * maxSpeed_
		);
	}

	angle_ += speed_;

	if (Any(speed_ != kOrigin2<float>)) {
		FMainRender::GetInstance()->GetRenderer()->ResetReservoir();
	}

	angle_.x = std::fmod(angle_.x, kPi * 2.0f);
	angle_.y = std::clamp(angle_.y, -kPi / 4.0f, kPi / 2.5f);

	camera_->SetRotation(
		Quaternion::AxisAngle(kUnitY3<float>, angle_.x) *
		Quaternion::AxisAngle(kUnitX3<float>, angle_.y)
	);

}
