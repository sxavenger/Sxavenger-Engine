#include "PerspectiveCamera.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/System.h>
#include <Engine/Components/Component/Transform/TransformComponent.h>
#include <Engine/Components/Component/Camera/CameraComponent.h>
#include <Engine/Module/Parameter/SerializeGui.h>

SXAVENGER_ENGINE_USING

////////////////////////////////////////////////////////////////////////////////////////////
// PerspectiveCamera class methods
////////////////////////////////////////////////////////////////////////////////////////////

void PerspectiveCamera::Awake() {
	(*this)->SetName("PerspectiveCamera");
	(*this)->AddComponent<TransformComponent>();

	auto camera = (*this)->AddComponent<CameraComponent>();
	camera->SetTag(CameraComponent::Tag::Game);


}

void PerspectiveCamera::Update() {

	switch (person_) {
		case Person::First:
			UpdateFirstPerson();
			break;
	}

	UpdateView();
}

void PerspectiveCamera::Inspectable() {
	SerializeGuiFormatter<float>::DragScalar(sensitivity_, 0.0001f, 0.0f, std::nullopt);
	SerializeGuiFormatter<float>::DragScalar3(offset_, 0.1f, std::nullopt, std::nullopt);
}

Vector3f PerspectiveCamera::CalculateMoveFront(const Vector3f& vector) const {

	Vector3f v = Quaternion::RotateVector(vector, rotation_);

	float length = v.Length();

	Vector3f front = { v.x, 0.0f, v.z };
	front = front.Normalize() * length;

	return front;
}

Quaternion PerspectiveCamera::CalculateForward() const {
	Vector3f direction = Quaternion::RotateVector(kForward3<float>, rotation_);
	direction = Vector3f{ direction.x, 0.0f, direction.z }.Normalize();

	Vector3f forward = {};

	// ロール, ピッチ, ロー回転
	forward.y = std::atan2(direction.x, direction.z);

	float length = Vector3f{ direction.x, 0.0f, direction.z }.Length();
	forward.x = std::atan2(-direction.y, length);

	return Quaternion::ToQuaternion(forward);
}

void PerspectiveCamera::UpdateFirstPerson() {

	angle_ += around_ * (*sensitivity_) * Vector2f { 1.0f, -1.0f };

	// 回転の制限
	angle_.y = std::clamp(angle_.y, -kPi / 2.5f, kPi / 2.5f);
	angle_.x = std::fmod(angle_.x, kTau);
	rotation_ = Quaternion::AxisAngle(kUnitY3<float>, angle_.x) * Quaternion::AxisAngle(kUnitX3<float>, angle_.y);

	distance_ = 0.0f;
	point_ = {};

	if (target_ != nullptr) {
		if (auto transform = target_->GetComponent<TransformComponent>()) {
			point_ = transform->GetPosition() + Quaternion::RotateVector((*offset_), transform->rotate);
		}
	}
}

void PerspectiveCamera::UpdateView() {

	auto transform = (*this)->GetComponent<TransformComponent>();

	Vector3f direction = Quaternion::RotateVector(kBackward3<float>, rotation_);
	transform->GetTransform().translate = point_ + direction * distance_;
	transform->GetTransform().rotate    = rotation_;
}
