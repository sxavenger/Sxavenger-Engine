#include "Light.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include <externals/imgui/imgui.h>

//=========================================================================================
// static variables
//=========================================================================================
const char* Light::item[kLightTypeCount] = { "Directional", "Point" };

////////////////////////////////////////////////////////////////////////////////////////////
// Light methods
////////////////////////////////////////////////////////////////////////////////////////////

void Light::Init(DxObject::Devices* device) {
	// resource
	resource_ = std::make_unique<DxObject::BufferPtrResource<LightData>>(device, 1);
	resource_->Set(0, &lightData_);
}

void Light::UpdateImGui(const char* windowName, const char* treeName) {
	ImGui::Begin(windowName);

	if (ImGui::TreeNode(treeName)) {

		ImGui::ColorEdit4("color", &lightData_.color.x);
		ImGui::DragFloat3("position", &lightData_.position.x, 0.01f);
		ImGui::DragFloat3("rotate", &rotate_.x, 0.01f);

		// directionの計算
		lightData_.direction = Matrix::Transform({ 0.0f, 0.0f, 1.0f }, Matrix::MakeRotate(rotate_));

		ImGui::Text("> [direction] x: %.3f y: %.3f z: %.3f", lightData_.direction.x, lightData_.direction.y, lightData_.direction.z);

		ImGui::SliderFloat("shininess", &lightData_.shininess, 0.0f, 1.0f);

		if (ImGui::BeginCombo("lightType", item[lightData_.lightType])) {

			for (int i = 0; i < kLightTypeCount; ++i) {
				// item[i]が選択されてるかどうか
				bool isSelected = (i == lightData_.lightType);

				if (ImGui::Selectable(item[i], isSelected)) {
					lightData_.lightType = i;
				}
			}

			ImGui::EndCombo();
		}

		if (lightData_.lightType == TYPE_POINT) {
			ImGui::DragFloat("range", &lightData_.range, 1.0f, 0.0f, 100.0f);
		}

		ImGui::TreePop();
	}

	ImGui::End();
}

void Light::Term() {
	resource_.reset();
}