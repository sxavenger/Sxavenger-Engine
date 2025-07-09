#include "PivotCameraActor.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/UI/SxImGui.h>

////////////////////////////////////////////////////////////////////////////////////////////
// PivotCameraActor class methods
////////////////////////////////////////////////////////////////////////////////////////////

void PivotCameraActor::Init() {
	MonoBehaviour::SetName("pivot camera actor");
	MonoBehaviour::AddComponent<TransformComponent>();
	MonoBehaviour::AddComponent<CameraComponent>();
}

void PivotCameraActor::Update() {
	auto transform = MonoBehaviour::AddComponent<TransformComponent>();

	Vector3f direction = Quaternion::RotateVector(kBackward3<float>, rotation_);
	transform->GetTransform().translate = point_ + direction * distance_;
	transform->GetTransform().rotate    = rotation_;
}

void PivotCameraActor::Inspectable() {
	SxImGui::DragFloat("distance", &distance_, 0.01f, 0.0f);
}
