#include "Material.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include <Sxavenger.h>

//-----------------------------------------------------------------------------------------
// using
//-----------------------------------------------------------------------------------------
_DXOBJECT_USING

////////////////////////////////////////////////////////////////////////////////////////////
// ObjectMaterial class methods
////////////////////////////////////////////////////////////////////////////////////////////

void ObjectMaterialBuffer::Init() {
	buffer_ = std::make_unique<BufferResource<ObjectMaterial>>(Sxavenger::GetDevicesObj(), 1);
	(*buffer_)[0].Init();
}

void ObjectMaterialBuffer::Transfer() {
	(*buffer_)[0].color       = color;
	(*buffer_)[0].uvTransform = uvTransform.ToMatrix();
	(*buffer_)[0].lambertType = lambertType;
	(*buffer_)[0].phongType   = phongType;
}

void ObjectMaterialBuffer::SetImGuiCommand() {

	ImGui::ColorEdit4("color", &color.r);
	uvTransform.SetImGuiCommand();

	if (ImGui::BeginCombo("lambert type", lambertItems_[lambertType])) {
		for (int i = 0; i < LambertType::kLambertTypeCount; ++i) {
			if (ImGui::Selectable(lambertItems_[i], i == lambertType)) {
				lambertType = static_cast<LambertType>(i);
			}
		}
		ImGui::EndCombo();
	}

	if (ImGui::BeginCombo("phong type", phongItems_[phongType])) {
		for (int i = 0; i < PhongType::kPhongTypeCount; ++i) {
			if (ImGui::Selectable(phongItems_[i], i == phongType)) {
				phongType = static_cast<PhongType>(i);
			}
		}
		ImGui::EndCombo();
	}

	Transfer();
}
