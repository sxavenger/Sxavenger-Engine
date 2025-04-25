#include "PointLightComponent.h"
_DXOBJECT_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* component
#include "../../Entity/MonoBehaviour.h"

//* engine
#include <Engine/System/SxavengerSystem.h>
#include <Engine/Content/SxavengerContent.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Parameter structure methods
////////////////////////////////////////////////////////////////////////////////////////////

void PointLightComponent::Parameter::Init() {
	color     = kWhite3<>;
	unit      = Units::Candela;
	intensity = 1.0f;
	radius    = 1.0f;
}

////////////////////////////////////////////////////////////////////////////////////////////
// PointLightComponent class methods
////////////////////////////////////////////////////////////////////////////////////////////

void PointLightComponent::ShowComponentInspector() {
	auto& parameter = parameter_->At(0);
	ImGui::ColorEdit3("color", &parameter.color.x);

	static const char* items[] = { "Lumen", "Candela" };

	if (ImGui::BeginCombo("unit", items[static_cast<uint32_t>(parameter.unit)])) {
		for (uint32_t i = 0; i < 2; ++i) {
			if (ImGui::Selectable(items[i], parameter.unit == static_cast<Units>(i))) {
				Units preUnit = parameter.unit;
				parameter.unit = static_cast<Units>(i);

				// intensityの変換
				if (preUnit == Units::Lumen && parameter.unit == Units::Candela) {
					parameter.intensity /= 4.0f * kPi; //!< lumen to candela

				} else if (preUnit == Units::Candela && parameter.unit == Units::Lumen) {
					parameter.intensity *= 4.0f * kPi; //!< candela to lumen
				}
			}
		}
		ImGui::EndCombo();
	}

	std::string format = parameter.unit == Units::Lumen ? "%.3flm" : "%.3fcd";
	ImGui::DragFloat("intensity", &parameter.intensity, 0.1f, 0.0f, std::numeric_limits<float>::max(), format.c_str());
	ImGui::DragFloat("radius",  &parameter.radius, 0.1f, 0.0f, std::numeric_limits<float>::max());

	LightCommon::ShowCommonInspector();

	//* push line
	SxavengerContent::GetDebugPrimitive()->PushSphere(GetTransform()->GetPosition(), parameter.radius, kColor);
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
