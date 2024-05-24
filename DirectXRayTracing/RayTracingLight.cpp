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
	ImGui::DragFloat3("direction", &data_.direction.x, 0.02f);

	data_.direction = Vector::Normalize(data_.direction);

}


