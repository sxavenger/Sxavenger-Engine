#include "PointLightComponent.h"
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

//* externals
#include <magic_enum.hpp>

////////////////////////////////////////////////////////////////////////////////////////////
// Parameter structure methods
////////////////////////////////////////////////////////////////////////////////////////////

void PointLightComponent::Parameter::Init() {
	shadow.Init();
	color     = kWhite3<>;
	unit      = LightCommon::Units::Candela;
	intensity = 8.0f;
	radius    = 10.0f;
}

////////////////////////////////////////////////////////////////////////////////////////////
// PointLightComponent class methods
////////////////////////////////////////////////////////////////////////////////////////////

void PointLightComponent::ShowComponentInspector() {
	auto& parameter = parameter_->At(0);
	ImGui::ColorEdit3("color", &parameter.color.x);

	if (ImGui::BeginCombo("unit", magic_enum::enum_name(parameter.unit).data())) {

		for (const auto& [value, name] : magic_enum::enum_entries<LightCommon::Units>()) {
			if (ImGui::Selectable(name.data(), parameter.unit == value)) {
				SetUnit(value);
			}
		}

		ImGui::EndCombo();
	}

	std::string format = (parameter.unit == LightCommon::Units::Lumen ? "%.3flm" : "%.3fcd");
	SxImGui::DragFloat("intensity",        &parameter.intensity, 0.1f, 0.0f, std::nullopt, format.c_str());
	SxImGui::DragFloat("radius",           &parameter.radius, 0.1f, 0.0f);

	parameter.shadow.ShowInspector();

	//* push line
	SxavengerContent::PushSphere(RequireTransform()->GetPosition(), parameter.radius, Color4f{ parameter.color, 1.0f });
}

void PointLightComponent::Init() {
	parameter_ = std::make_unique<DimensionBuffer<Parameter>>();
	parameter_->Create(SxavengerSystem::GetDxDevice(), 1);
	parameter_->At(0).Init();
}

const D3D12_GPU_VIRTUAL_ADDRESS& PointLightComponent::GetGPUVirtualAddress() const {
	Exception::Assert(parameter_ != nullptr, "point light buffer is not create.");
	return parameter_->GetGPUVirtualAddress();
}

void PointLightComponent::SetUnit(LightCommon::Units unit) {
	auto& parameter = parameter_->At(0);

	if (parameter.unit == unit) {
		return;
	}

	// intensityの変換
	if (parameter.unit == LightCommon::Units::Lumen && unit == LightCommon::Units::Candela) {
		parameter.intensity /= 4.0f * kPi; //!< lumen to candela

	} else if (parameter.unit == LightCommon::Units::Candela && unit == LightCommon::Units::Lumen) {
		parameter.intensity *= 4.0f * kPi; //!< candela to lumen
	}

	parameter.unit = unit;
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
	data["shadow_strength"] = JsonSerializeFormatter<float>::Serialize(parameter_->At(0).shadow.strength);
	data["shadow_flag"]     = JsonSerializeFormatter<uint32_t>::Serialize(parameter_->At(0).shadow.flag.Get());

	return data;
}

void PointLightComponent::InputJson(const json& data) {
	parameter_->At(0).color           = JsonSerializeFormatter<Color3f>::Deserialize(data["color"]);
	parameter_->At(0).unit            = magic_enum::enum_cast<LightCommon::Units>(JsonSerializeFormatter<std::string>::Deserialize(data["unit"])).value();
	parameter_->At(0).intensity       = JsonSerializeFormatter<float>::Deserialize(data["intensity"]);
	parameter_->At(0).radius          = JsonSerializeFormatter<float>::Deserialize(data["radius"]);
	parameter_->At(0).shadow.strength = JsonSerializeFormatter<float>::Deserialize(data["shadow_strength"]);
	parameter_->At(0).shadow.flag     = JsonSerializeFormatter<uint32_t>::Deserialize(data["shadow_flag"]);
}
