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
			if (ImGui::IsItemFocused()) {
				direction = Normalize(direction);
			}

			break;

		case LightType::LIGHT_POINT:

			ImGui::DragFloat("range", &range, 0.1f);
			ImGui::DragFloat("decay", &decay, 0.1f);

			break;

		case LightType::LIGHT_SPOT:

			ImGui::DragFloat3("direction", &direction.x, 0.01f);
			if (ImGui::IsItemFocused()) {
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

	Attribute::SetAttributeName("light buffer");
}

void LightBuffer::Term() {
}

void LightBuffer::SetDirectionalLight(uint32_t index, const DirectionalLight& directional) {
	(*buffer_)[index].color     = directional.color;
	(*buffer_)[index].position  = directional.position;
	(*buffer_)[index].direction = directional.direction;
	(*buffer_)[index].type      = LIGHT_DIRECTIONAL;
}

void LightBuffer::SetPointLight(uint32_t index, const PointLight& point) {
	(*buffer_)[index].color     = point.color;
	(*buffer_)[index].position  = point.position;
	(*buffer_)[index].range     = point.range;
	(*buffer_)[index].decay     = point.decay;
	(*buffer_)[index].type      = LIGHT_POINT;
}

void LightBuffer::SetSpotLight(uint32_t index, const SpotLight& spot) {
	(*buffer_)[index].color        = spot.color;
	(*buffer_)[index].position     = spot.position;
	(*buffer_)[index].direction    = spot.direction;
	(*buffer_)[index].range        = spot.range;
	(*buffer_)[index].decay        = spot.decay;
	(*buffer_)[index].angle        = spot.angle;
	(*buffer_)[index].falloffAngle = spot.falloffAngle;
	(*buffer_)[index].type         = LIGHT_POINT;
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