#include "CineCamera.h"
_DXOBJECT_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
// Lib
#include <Lib/Environment.h>

// engine
#include <Engine/System/Sxavenger.h>

////////////////////////////////////////////////////////////////////////////////////////////
// CineCamera class methods
////////////////////////////////////////////////////////////////////////////////////////////

void CineCamera::Init() {

	// bufferの設定
	buffer_ = std::make_unique<BufferResource<CameraForGPU>>(Sxavenger::GetDevicesObj(), 1);

	SetTransform(kUnit3, kOrigin3, { 0.0f, 0.0f, -10.0f });
	SetProjection({16.0f, 9.0f}, 20.0f, 0.01f, 1000.0f);
}

void CineCamera::Term() {
}

void CineCamera::SetProjection(const Vector2f& sensorSize, float focalLength, float nearClip, float farClip) {
	sensorSize_  = sensorSize;
	focalLength_ = focalLength;
	nearClip_    = nearClip;
	farClip_     = farClip;

	CalculateProjection();
}

void CineCamera::SetAttributeImGui() {
	Camera3D::SetAttributeImGui();

	ImGui::DragFloat2("sensorSize", &sensorSize_.x, 0.01f, 0.0f, 0.0f, "%.3fmm");
	ImGui::DragFloat("focalLength", &focalLength_,  0.01f, 0.0f, 0.0f, "%.3fmm");
	ImGui::DragFloat("nearClip",    &nearClip_, 1.0f);
	ImGui::DragFloat("farClip",     &farClip_,  1.0f);

	CalculateProjection();
}

void CineCamera::CalculateProjection() {

	float aspectRaito = sensorSize_.x / sensorSize_.y;
	float fovY        = 2.0f * std::atan(sensorSize_.y / (2.0f * focalLength_));

	Camera3D::SetProjection(fovY, aspectRaito, nearClip_, farClip_);
}
