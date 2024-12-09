#include "DebugCamera3d.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/SxavengerSystem.h>

////////////////////////////////////////////////////////////////////////////////////////////
// BlenderDebugCamera3d class methods
////////////////////////////////////////////////////////////////////////////////////////////

void BlenderDebugCamera3d::Create() {
	Camera3d::Create();
	Reset();
}

void BlenderDebugCamera3d::Reset() {
	distance_         = 12.0f;
	lon_              = 0.0f;
	lat_              = 0.0f;
	point_            = kOrigin3<float>;
	CalculateView();
}

void BlenderDebugCamera3d::Update() {

	auto input = SxavengerSystem::GetInput()->GetMouseInput();

	if (input->IsPress(MouseId::MOUSE_MIDDLE)) {
		Vector2f delta = input->GetDeltaPosition();

		lon_ = std::fmod(lon_ + delta.x * rotateDelta_, pi_v * 2.0f);
		lat_ = std::fmod(lat_ + delta.y * rotateDelta_, pi_v * 2.0f);
	}

	if (input->IsPress(MouseId::MOUSE_RIGHT)) {
		Vector2f delta = input->GetDeltaPosition();

		Vector3f right = RotateVector({ 1.0f, 0.0f, 0.0f }, Camera3d::GetTransform().rotate);
		Vector3f up    = RotateVector({ 0.0f, 1.0f, 0.0f }, Camera3d::GetTransform().rotate);

		point_ += -right * delta.x * moveDelta_;
		point_ += up * delta.y * moveDelta_;
	}

	distance_ = std::max(distance_ - input->GetDeltaWheel(), 0.0f);

	CalculateView();
}

void BlenderDebugCamera3d::CalculateView() {

	Quaternion rotate = MakeAxisAngle({0.0f, 1.0f, 0.0f}, lon_) * MakeAxisAngle({ 1.0f, 0.0f, 0.0f }, lat_);

	static const Vector3f direction = { 0.0f, 0.0f, -1.0f };
	Vector3f rotatedDirection = RotateVector(direction, rotate);

	Camera3d::GetTransform().translate = point_ + rotatedDirection * distance_;
	Camera3d::GetTransform().rotate    = rotate;
	Camera3d::UpdateMatrix();

}
