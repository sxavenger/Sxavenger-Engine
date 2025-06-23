#include "RectLightComponent.h"
_DXOBJECT_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* component
#include "../../../Entity/MonoBehaviour.h"

//* engine
#include <Engine/System/UI/SxImGui.h>
#include <Engine/System/SxavengerSystem.h>
#include <Engine/Content/SxavengerContent.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Parameter structure methods
////////////////////////////////////////////////////////////////////////////////////////////

void RectLightComponent::Parameter::Init() {
	color     = kWhite3<>;
	unit      = Units::Candela;
	intensity = 1.0f;
	size      = Vector3f(1.0f, 1.0f, 1.0f);
	range     = 8.0f;
}

////////////////////////////////////////////////////////////////////////////////////////////
// RectLightComponent class methods
////////////////////////////////////////////////////////////////////////////////////////////

void RectLightComponent::ShowComponentInspector() {
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

	SxImGui::DragVector3("size", &parameter.size.x, 0.01f, 0.0f, std::numeric_limits<float>::max());
	ImGui::DragFloat("range",    &parameter.range, 0.1f, 0.0f, std::numeric_limits<float>::max());



	LightCommon::ShowCommonInspector();

	//* push line

	SxavengerContent::GetDebugPrimitive()->PushCube(
		GetTransform()->GetPosition(),
		parameter.size,
		Color4f::Convert(0xABCCC4A0)
	); //!< rect

	SxavengerContent::GetDebugPrimitive()->PushCube(
		GetTransform()->GetPosition(),
		{ parameter.size.x + parameter.range, parameter.size.y + parameter.range, parameter.size.z + parameter.range },
		kColor
	); //!< rect + range
}

void RectLightComponent::Init() {
	CreateParameterBuffer();
	LightCommon::CreateShadowBuffer();
}

void RectLightComponent::CreateParameterBuffer() {
	parameter_ = std::make_unique<DimensionBuffer<Parameter>>();
	parameter_->Create(SxavengerSystem::GetDxDevice(), 1);
	parameter_->At(0).Init();
}

const D3D12_GPU_VIRTUAL_ADDRESS& RectLightComponent::GetParameterBufferAddress() const {
	Exception::Assert(parameter_ != nullptr, "rect light buffer is not create.");
	return parameter_->GetGPUVirtualAddress();
}

const TransformComponent* RectLightComponent::GetTransform() const {
	return BaseComponent::GetBehaviour()->RequireComponent<TransformComponent>();
}
