#include "PointLightComponent.h"
_DXOBJECT_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* component
#include "../../Entity/MonoBehaviour.h"

//* engine
#include <Engine/System/SxavengerSystem.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Parameter structure methods
////////////////////////////////////////////////////////////////////////////////////////////

void PointLightComponent::Parameter::Init() {
	color     = kWhite3<>;
	unit      = Units::Lumen;
	intensity = 1.0f;
	distancae = 12.0f;
}

////////////////////////////////////////////////////////////////////////////////////////////
// PointLightComponent class methods
////////////////////////////////////////////////////////////////////////////////////////////

void PointLightComponent::ShowComponentInspector() {
	auto& parameter = parameter_->At(0);
	ImGui::ColorEdit3("color", &parameter.color.x);

	Units preUnit = parameter.unit;

	static const char* items[] = { "Lumen", "Candela" };

	if (ImGui::BeginCombo("unit", items[static_cast<uint32_t>(parameter.unit)])) {
		for (uint32_t i = 0; i < 2; ++i) {
			if (ImGui::Selectable(items[i], parameter.unit == static_cast<Units>(i))) {
				parameter.unit = static_cast<Units>(i);

				// intensityの変換
				if (preUnit == Units::Lumen && parameter.unit == Units::Candela) {
					parameter.intensity /= 4.0f * kPi;

				} else if (preUnit == Units::Candela && parameter.unit == Units::Lumen) {
					parameter.intensity *= 4.0f * kPi;
				}
			}
		}
		ImGui::EndCombo();
	}

	ImGui::DragFloat("intensity", &parameter.intensity, 0.1f, 0.0f, 128.0f);
}

void PointLightComponent::Init() {
	CreateParameterBuffer();
	LightCommon::CreateShadowBuffer();
}

void PointLightComponent::CreateParameterBuffer() {
	parameter_ = std::make_unique<DimensionBuffer<Parameter>>();
	parameter_->Create(SxavengerSystem::GetDxDevice(), 1);
	parameter_->At(0).Init();
}

const D3D12_GPU_VIRTUAL_ADDRESS& PointLightComponent::GetParameterBufferAddress() const {
	Assert(parameter_ != nullptr, "point light buffer is not create.");
	return parameter_->GetGPUVirtualAddress();
}

const TransformComponent* PointLightComponent::GetTransform() const {
	return BaseComponent::GetBehaviour()->RequireComponent<TransformComponent>();
}
