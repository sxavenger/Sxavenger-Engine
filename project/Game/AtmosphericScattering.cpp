#include "AtmosphericScattering.h"
_DXOBJECT_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/Console/SystemConsole.h>

////////////////////////////////////////////////////////////////////////////////////////////
// AtmosphericScattering class methods
////////////////////////////////////////////////////////////////////////////////////////////

void AtmosphericScattering::Init() {
	SetName("atmospheric scattering");

	buffer_ = std::make_unique<BufferResource<GPUBuffer>>(Sxavenger::GetDevicesObj(), 1);
	(*buffer_)[0].Reset();

	(*buffer_)[0].lightDir = { 0.0f, -1.0f, 0.0f };
	(*buffer_)[0].eSun     = 2000.0f;

	sSystemConsole->GetMissRecorder()->SetAddress(0, buffer_->GetGPUVirtualAddress());
}

void AtmosphericScattering::Term() {
}

void AtmosphericScattering::SetAttributeImGui() {

	auto& v = (*buffer_)[0];

	ImGui::ColorEdit3("wave length", &v.waveLength.r);

	if (ImGui::DragFloat3("light direction", &v.lightDir.x, 0.01f)) {
		v.lightDir = Normalize(v.lightDir);
	}

	ImGui::DragFloat("inner radius", &v.innerRadius, 0.01f, 0.0f, 128.0f);
	ImGui::DragFloat("outer radius", &v.outerRadius, 0.01f, v.innerRadius, 256.0f);

	ImGui::DragFloat("kr", &v.kr, 0.0001f, 0.0f, 0.0f, "%.4f");
	ImGui::DragFloat("km", &v.km, 0.0001f, 0.0f, 0.0f, "%.4f");
	ImGui::DragFloat("eSun", &v.eSun, 0.01f);
	ImGui::DragFloat("g", &v.g, 0.01f);
	ImGui::DragFloat("scaleDepth", &v.scaleDepth, 0.01f);
	ImGui::SliderFloat("exposure", &v.exposure, 0.0f, 1.0f);

	if (ImGui::Button("Reset")) {
		v.Reset();
	}


}
