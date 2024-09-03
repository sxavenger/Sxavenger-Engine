#include "Light.h"
_DXOBJECT_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* enigne
#include <Sxavenger.h>

//=========================================================================================
// static variables
//=========================================================================================
static const char* items[kCountOfLightType] = { "None", "Directional", "Point", "Spot"};

////////////////////////////////////////////////////////////////////////////////////////////
// Light structure methods
////////////////////////////////////////////////////////////////////////////////////////////

void Light::SetImGuiCommand() {

	ImGui::ColorEdit3("color",    &color.r);
	ImGui::DragFloat("intensity", &color.a, 0.01f);
	ImGui::DragFloat3("position", &position.x, 0.01f);

	if (ImGui::BeginCombo("type", items[type])) {
		for (int i = 0; i < kCountOfLightType; ++i) {
			if (ImGui::Selectable(items[i], i == type)) {
				type = static_cast<LightType>(i);
			}
		}
		ImGui::EndCombo();
	}

	ImGui::Separator();
	ImGui::Spacing();

	switch (type) {
		case LightType::LIGHT_DIRECTIONAL:

			ImGui::DragFloat3("direction", &direction.x, 0.01f);
			if (ImGui::IsItemHovered()) {
				direction = Normalize(direction);
			}

			break;

		case LightType::LIGHT_POINT:

			ImGui::DragFloat("range", &range, 0.1f);
			ImGui::DragFloat("decay", &decay, 0.1f);

			break;

		case LightType::LIGHT_SPOT:

			ImGui::DragFloat3("direction", &direction.x, 0.01f);
			if (ImGui::IsItemHovered()) {
				direction = Normalize(direction);
			}

			ImGui::DragFloat("range", &range, 0.1f);
			ImGui::DragFloat("decay", &decay, 0.1f);

			ImGui::SliderFloat("angle", &angle, -1.0f, 1.0f);

			if (falloffAngle < angle) {
				falloffAngle = angle;
			}

			ImGui::SliderFloat("falloffStartAngle", &falloffAngle, angle, 1.0f);

			break;

		default:
			break;
	}

}


////////////////////////////////////////////////////////////////////////////////////////////
// LightBuffer class methods
////////////////////////////////////////////////////////////////////////////////////////////

void LightBuffer::Init() {

	buffer_
		= std::make_unique<BufferResource<Light>>(Sxavenger::GetDevicesObj(), kMaxLightNum_);

	(*buffer_)[0].type      = LIGHT_DIRECTIONAL;
	(*buffer_)[0].color     = {1.0f, 1.0f, 1.0f, 1.0f};
	(*buffer_)[0].direction = { 0.0f, -1.0f, 0.0f };

}

void LightBuffer::Term() {
}

void LightBuffer::SetAttributeImGui() {

	for (int i = 0; i < kMaxLightNum_; ++i) {

		std::string label = std::format("[{}]", i);

		if (ImGui::TreeNode(label.c_str())) {
			(*buffer_)[0].SetImGuiCommand();
			ImGui::TreePop();
		}
	}
}