#include "APivotCameraActor.h"

////////////////////////////////////////////////////////////////////////////////////////////
// ACineCameraActor class methods
////////////////////////////////////////////////////////////////////////////////////////////

void APivotCameraActor::Init() {
	ACameraActor::Init();
	UpdateView();

	AActor::name_ = "pivot camera actor";
}

void APivotCameraActor::UpdateView() {

	Quaternion r
		= AxisAngle({ 0.0f, 1.0f, 0.0f }, parameter_.angle.x) * AxisAngle({ 1.0f, 0.0f, 0.0f }, parameter_.angle.y);

	Vector3f direciton = RotateVector({ 0.0f, 0.0f, -1.0f }, r);

	ACameraActor::transform_.translate = parameter_.point + direciton * parameter_.distance;
	ACameraActor::transform_.rotate    = r;
	ACameraActor::UpdateView();
}
