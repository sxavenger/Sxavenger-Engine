#include "Camera2D.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "Environment.h"

////////////////////////////////////////////////////////////////////////////////////////////
// Camera2D methods
////////////////////////////////////////////////////////////////////////////////////////////

Camera2D::Camera2D() {
	Init();
}

Camera2D::~Camera2D() {
}

void Camera2D::Init() {
	SetProjection(0.0f, 0.0f, static_cast<float>(kWindowWidth), static_cast<float>(kWindowHeight), 0.0f, 100.0f);
}

void Camera2D::SetProjection(float left, float top, float right, float bottom, float nearClip, float farClip) {
	projectionMantrix_ = Matrix::MakeOrthographic(left, top, right, bottom, nearClip, farClip);
}
