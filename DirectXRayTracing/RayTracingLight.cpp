#include "RayTracingLight.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include <MyEngine.h>
#include <imgui.h>

////////////////////////////////////////////////////////////////////////////////////////////
// RayTracingLight class methods
////////////////////////////////////////////////////////////////////////////////////////////

void RayTracingLight::Init() {

	resource_ = std::make_unique<DxObject::BufferPtrResource<DirectionalLight>>(MyEngine::GetDevicesObj(), 1);
	resource_->SetPtr(0, &data_);

	SetThisAttribute("directionalLight");

}

void RayTracingLight::Term() {
	resource_.reset();
}

void RayTracingLight::SetAttributeImGui() {

	ImGui::ColorEdit4("color", &data_.color.r);
	ImGui::DragFloat3("rotate", &rotate_.x, 0.02f);

	// rotateからdirectionを算出
	data_.direction = Matrix::Transform({ 0.0f, 0.0f, 1.0f }, Matrix::MakeRotate(rotate_));

	ImGui::Text("[direction] x: %.3f, y: %.3f, z: %.3f", data_.direction.x, data_.direction.y, data_.direction.z);

}


