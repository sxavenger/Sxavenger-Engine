#include "SpotLightComponent.h"
SXAVENGER_ENGINE_USING
DXOBJECT_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* component
#include "../../../Entity/MonoBehaviour.h"

//* engine
#include <Engine/System/Utility/StreamLogger.h>
#include <Engine/System/UI/SxImGui.h>
#include <Engine/System/System.h>
#include <Engine/Graphics/Graphics.h>

//* externals
#include <magic_enum.hpp>

////////////////////////////////////////////////////////////////////////////////////////////
// Parameter structure methods
////////////////////////////////////////////////////////////////////////////////////////////

void SpotLightComponent::Parameter::Init() {
	shadow.Init();
	color     = kWhite3<>;
	unit      = LightCommon::Units::Candela;
	intensity = 8.0f;
	radius    = 10.0f;
	coneAngle = { 0.0f, kDegToRad * 44.0f };
}

////////////////////////////////////////////////////////////////////////////////////////////
// SpotLightComponent class methods
////////////////////////////////////////////////////////////////////////////////////////////

SpotLightComponent::SpotLightComponent(MonoBehaviour* behaviour)
	: BaseComponent(behaviour) {

	parameter_ = std::make_unique<ConstantBuffer<Parameter>>();
	parameter_->Create(System::GetDxDevice());
	parameter_->At().Init();
}

void SpotLightComponent::ShowComponentInspector() {
	auto& parameter = parameter_->At();
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
	SxImGui::DragFloat("inner cone angle", &parameter.coneAngle.x, 0.01f, 0.0f, parameter.coneAngle.y);
	SxImGui::DragFloat("outer cone angle", &parameter.coneAngle.y, 0.01f, parameter.coneAngle.x, kDegToRad * 80.0f);

	parameter.shadow.ShowInspector();

	//* push line

	Graphics::PushCone(RequireTransform()->GetPosition(), RequireTransform()->GetDirection(), parameter.radius, parameter.coneAngle.y, Color4f{ parameter.color, 1.0f });
}

const D3D12_GPU_VIRTUAL_ADDRESS& SpotLightComponent::GetGPUVirtualAddress() const {
	StreamLogger::AssertA(parameter_ != nullptr, "spot light buffer is not create.");
	return parameter_->GetGPUVirtualAddress();
}

void SpotLightComponent::SetUnit(LightCommon::Units unit) {
	auto& parameter = parameter_->At();

	if (parameter.unit == unit) {
		return;
	}

	// intensityの変換
	if (parameter.unit == LightCommon::Units::Lumen && unit == LightCommon::Units::Candela) {
		//parameter.intensity /= 4.0f * kPi; //!< lumen to candela

	} else if (parameter.unit == LightCommon::Units::Candela && unit == LightCommon::Units::Lumen) {
		//parameter.intensity *= 4.0f * kPi; //!< candela to lumen
	}

	parameter.unit = unit;
}

SpotLightComponent::Parameter& SpotLightComponent::GetParameter() {
	StreamLogger::AssertA(parameter_ != nullptr, "spot light buffer is not create.");
	return parameter_->At();
}

const SpotLightComponent::Parameter& SpotLightComponent::GetParameter() const {
	StreamLogger::AssertA(parameter_ != nullptr, "spot light buffer is not create.");
	return parameter_->At();
}

const TransformComponent* SpotLightComponent::RequireTransform() const {
	return BaseComponent::GetBehaviour()->RequireComponent<TransformComponent>();
}

json SpotLightComponent::PerseToJson() const {
	json data = json::object();

	data["color"]           = JsonSerializeFormatter<Color3f>::Serialize(parameter_->At().color);
	data["unit"]            = magic_enum::enum_name(parameter_->At().unit);
	data["intensity"]       = JsonSerializeFormatter<float>::Serialize(parameter_->At().intensity);
	data["radius"]          = JsonSerializeFormatter<float>::Serialize(parameter_->At().radius);
	data["coneAngle"]       = JsonSerializeFormatter<Vector2f>::Serialize(parameter_->At().coneAngle);
	data["shadow_strength"] = JsonSerializeFormatter<float>::Serialize(parameter_->At().shadow.strength);
	data["shadow_flag"]     = JsonSerializeFormatter<uint32_t>::Serialize(parameter_->At().shadow.flag.Get());

	return data;
}

void SpotLightComponent::InputJson(const json& data) {
	parameter_->At().color           = JsonSerializeFormatter<Color3f>::Deserialize(data["color"]);
	parameter_->At().unit            = magic_enum::enum_cast<LightCommon::Units>(JsonSerializeFormatter<std::string>::Deserialize(data["unit"])).value();
	parameter_->At().intensity       = JsonSerializeFormatter<float>::Deserialize(data["intensity"]);
	parameter_->At().radius          = JsonSerializeFormatter<float>::Deserialize(data["radius"]);
	parameter_->At().coneAngle       = JsonSerializeFormatter<Vector2f>::Deserialize(data["coneAngle"]);
	parameter_->At().shadow.strength = JsonSerializeFormatter<float>::Deserialize(data["shadow_strength"]);
	parameter_->At().shadow.flag     = JsonSerializeFormatter<uint32_t>::Deserialize(data["shadow_flag"]);
}
