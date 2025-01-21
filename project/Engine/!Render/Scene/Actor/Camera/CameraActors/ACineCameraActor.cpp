#include "ACineCameraActor.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* external
#include <imgui.h>

////////////////////////////////////////////////////////////////////////////////////////////
// CineCameraParameter structure
////////////////////////////////////////////////////////////////////////////////////////////

Matrix4x4 ACineCameraActor::CineCameraParameter::GetProj() const {
	float aspect = sensor.x / sensor.y;
	float fovY   = 2.0f * atan(sensor.y / (2.0f * focal));

	return Matrix::MakePerspectiveFov(fovY, aspect, nearZ, farZ);
}

////////////////////////////////////////////////////////////////////////////////////////////
// ACineCameraActor class
////////////////////////////////////////////////////////////////////////////////////////////

void ACineCameraActor::Init() {
	ACameraActor::Init();

	//* 仮paraemter(transform)
	ACameraActor::GetTransform().translate = { 0.0f, 0.0f, -12.0f };
	ACameraActor::UpdateView();

	//* 仮paraemter(window size = 1280x720の場合)
	parameter_.sensor = { 16.0f, 9.0f };
	parameter_.focal  = 20.0f;
	parameter_.nearZ  = 0.01f;
	parameter_.farZ   = 1000.0f;

	UpdateProj();

	AActor::name_ = "cine camera actor";
}

void ACineCameraActor::UpdateProj() {
	ACameraActor::UpdateProj(parameter_.GetProj());
}

void ACineCameraActor::InspectorImGui() {
	ImGui::DragFloat2("sensor", &parameter_.sensor.x, 0.01f);
	ImGui::DragFloat("focal", &parameter_.focal, 0.01f);
	ImGui::DragFloat("nearZ", &parameter_.nearZ, 0.01f);
	ImGui::DragFloat("farZ", &parameter_.farZ, 0.01f);
	UpdateProj();
}
