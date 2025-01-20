#include "APivotCameraActor.h"

////////////////////////////////////////////////////////////////////////////////////////////
// ACineCameraActor class methods
////////////////////////////////////////////////////////////////////////////////////////////

void APivotCameraActor::Init() {
	ACameraActor::Init();
	UpdateView();
}

void APivotCameraActor::UpdateView() {

	Quaternion r
		= MakeAxisAngle({ 0.0f, 1.0f, 0.0f }, parameter_.angle.x) * MakeAxisAngle({ 1.0f, 0.0f, 0.0f }, parameter_.angle.y);

	Vector3f direciton = RotateVector({ 0.0f, 0.0f, -1.0f }, r);

	ACameraActor::GetTransform().translate = parameter_.point + direciton * parameter_.distance;
	ACameraActor::GetTransform().rotate    = r;
	ACameraActor::UpdateView();
}
