#include "RayTracingAO.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include <MyEngine.h>

////////////////////////////////////////////////////////////////////////////////////////////
// RayTracingAO class methods
////////////////////////////////////////////////////////////////////////////////////////////

void RayTracingAO::Init() {

	SetThisAttribute("AmbientOcclusion");

	resource_ = std::make_unique<DxObject::BufferPtrResource<AOParameter>>(MyEngine::GetDevicesObj(), 1);
	resource_->SetPtr(0, &parameter_);

}

void RayTracingAO::Term() {
	resource_.reset();
}

void RayTracingAO::SetAttributeImGui() {

	static bool isAmbientOcclusionEnable = parameter_.isAmbientOcclusionEnable;
	ImGui::Checkbox("isAmbientOcclusionEnable", &isAmbientOcclusionEnable);
	parameter_.isAmbientOcclusionEnable = isAmbientOcclusionEnable;

	ImGui::DragFloat("aoRadius", &parameter_.aoRadius, 0.01f);

	static int subdivision = parameter_.subdivision;
	ImGui::SliderInt("subdivision", &subdivision, 1, 10);
	parameter_.subdivision = subdivision;
}
