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

	SetThisAttribute("light");

}

void RayTracingLight::Term() {
	resource_.reset();
}

void RayTracingLight::SetAttributeImGui() {

	ImGui::ColorEdit4("color", &data_.color.r);
	ImGui::DragFloat3("position", &data_.position.x, 0.02f);
	ImGui::DragFloat3("direction", &data_.direction.x, 0.02f);
	data_.direction = Vector::Normalize(data_.direction);

	ImGui::DragFloat("range", &data_.range, 0.02f, 0.0f, 100.0f);
	ImGui::DragFloat("decay", &data_.decay, 0.02f, 0.0f, 100.0f);

	// bools
	static bool isLightingEnable = data_.isLightingEnable;
	static bool isShadowEnable = data_.isShadowEnable;

	ImGui::Checkbox("isLightingEnable", &isLightingEnable);
	data_.isLightingEnable = isLightingEnable;

	ImGui::Checkbox("isShadowEnable", &isShadowEnable);
	data_.isShadowEnable = isShadowEnable;

}


