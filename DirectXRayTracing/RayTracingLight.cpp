#include "RayTracingLight.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include <MyEngine.h>
#include <imgui.h>
#include <MathLib.h>

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

	ImGui::ColorEdit4("color",      &data_.color.r);
	ImGui::SliderFloat("intensity", &data_.intensity, 0.0f, 1.0f);

	ImGui::RadioButton("directionLight", &data_.type, LIGHT_DIRECTION);
	ImGui::SameLine();
	ImGui::RadioButton("pointLight", &data_.type, LIGHT_POINT);
	ImGui::SameLine();
	ImGui::RadioButton("spotLight", &data_.type, LIGHT_SPOT);

	if (data_.type == LIGHT_DIRECTION) {
		ImGui::DragFloat3("direction", &data_.direction.x, 0.02f);
		data_.direction = Normalize(data_.direction);

	} else if (data_.type == LIGHT_POINT) {
		ImGui::DragFloat3("position", &data_.position.x, 0.02f);
		ImGui::DragFloat("range",     &data_.range, 0.02f, 0.0f, 100.0f);
		ImGui::DragFloat("decay",     &data_.decay, 0.02f, 0.0f, 100.0f);

	} else if (data_.type == LIGHT_SPOT) {
		ImGui::DragFloat3("direction", &data_.direction.x, 0.02f);
		data_.direction = Normalize(data_.direction);

		ImGui::DragFloat3("position", &data_.position.x, 0.02f);
		ImGui::DragFloat("range", &data_.range, 0.02f, 0.0f, 100.0f);
		ImGui::DragFloat("decay", &data_.decay, 0.02f, 0.0f, 100.0f);

		ImGui::DragFloat("angle", &data_.angle, 0.01f, -1.0f, 1.0f);
	}
}


