#include "PointLightComponent.h"
_DXOBJECT_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* component
#include "../../../Entity/MonoBehaviour.h"

//* engine
#include <Engine/System/SxavengerSystem.h>
#include <Engine/Content/SxavengerContent.h>

//* externals
#include <magic_enum.hpp>

////////////////////////////////////////////////////////////////////////////////////////////
// Parameter structure methods
////////////////////////////////////////////////////////////////////////////////////////////

void PointLightComponent::Parameter::Init() {
	color     = kWhite3<>;
	unit      = Units::Candela;
	intensity = 1.0f;
	radius    = 8.0f;
}

////////////////////////////////////////////////////////////////////////////////////////////
// PointLightComponent class methods
////////////////////////////////////////////////////////////////////////////////////////////

void PointLightComponent::ShowComponentInspector() {
	auto& parameter = parameter_->At(0);
	ImGui::ColorEdit3("color", &parameter.color.x);

	if (ImGui::BeginCombo("unit", magic_enum::enum_name(parameter.unit).data())) {

		for (const auto& [value, name] : magic_enum::enum_entries<Units>()) {
			if (ImGui::Selectable(name.data(), parameter.unit == value)) {
				Units preUnit = parameter.unit;
				parameter.unit = value;

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
	SxavengerContent::GetDebugPrimitive()->PushSphere(RequireTransform()->GetPosition(), parameter.radius, kColor);
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
	Exception::Assert(parameter_ != nullptr, "point light buffer is not create.");
	return parameter_->GetGPUVirtualAddress();
}

const PointLightComponent::Parameter& PointLightComponent::GetParameter() const {
	Exception::Assert(parameter_ != nullptr, "point light buffer is not create.");
	return parameter_->At(0);
}

const TransformComponent* PointLightComponent::RequireTransform() const {
	return BaseComponent::GetBehaviour()->RequireComponent<TransformComponent>();
}

json PointLightComponent::PerseToJson() const {
	json data = json::object();

	data["color"]           = JsonSerializeFormatter<Color3f>::Serialize(parameter_->At(0).color);
	data["unit"]            = magic_enum::enum_name(parameter_->At(0).unit);
	data["intensity"]       = JsonSerializeFormatter<float>::Serialize(parameter_->At(0).intensity);
	data["radius"]          = JsonSerializeFormatter<float>::Serialize(parameter_->At(0).radius);
	data["shadow_strength"] = JsonSerializeFormatter<float>::Serialize(LightCommon::GetShadowParameter().strength);
	data["shadow_flag"]     = JsonSerializeFormatter<uint32_t>::Serialize(LightCommon::GetShadowParameter().flag.Get());

	return data;
}

void PointLightComponent::InputJson(const json& data) {
	parameter_->At(0).color                    = JsonSerializeFormatter<Color3f>::Deserialize(data["color"]);
	parameter_->At(0).unit                     = magic_enum::enum_cast<Units>(JsonSerializeFormatter<std::string>::Deserialize(data["unit"])).value();
	parameter_->At(0).intensity                = JsonSerializeFormatter<float>::Deserialize(data["intensity"]);
	parameter_->At(0).radius                   = JsonSerializeFormatter<float>::Deserialize(data["radius"]);
	LightCommon::GetShadowParameter().strength = JsonSerializeFormatter<float>::Deserialize(data["shadow_strength"]);
	LightCommon::GetShadowParameter().flag     = JsonSerializeFormatter<uint32_t>::Deserialize(data["shadow_flag"]);
}
