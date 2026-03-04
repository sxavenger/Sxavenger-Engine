#include "RectLightComponent.h"
SXAVENGER_ENGINE_USING
DXOBJECT_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* component
#include "../../../Entity/EntityBehaviour.h"

//* engine
#include <Engine/System/Utility/StreamLogger.h>
#include <Engine/System/UI/SxGui.h>
#include <Engine/System/System.h>
#include <Engine/Graphics/Graphics.h>

//* externals
#include <magic_enum.hpp>

////////////////////////////////////////////////////////////////////////////////////////////
// Parameter structure methods
////////////////////////////////////////////////////////////////////////////////////////////

void RectLightComponent::Parameter::Init() {
	shadow.Init();
	color     = kWhite3<>;
	unit      = LightCommon::Units::Candela;
	intensity = 8.0f;
	radius    = 10.0f;
	source    = { 2.0f, 2.0f };
}

////////////////////////////////////////////////////////////////////////////////////////////
// PointLightComponent class methods
////////////////////////////////////////////////////////////////////////////////////////////

RectLightComponent::RectLightComponent(EntityBehaviour* behaviour)
	: BaseComponent(behaviour) {

	parameter_ = std::make_unique<ConstantBuffer<Parameter>>();
	parameter_->Create(System::GetDxDevice());
	parameter_->At().Init();
}

void RectLightComponent::ShowComponentInspector() {
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
	SxGui::DragScalar<float>("intensity", &parameter.intensity, 0.1f, 0.0f, std::nullopt, format.c_str());
	SxGui::DragScalar<float>("radius",    &parameter.radius, 0.1f, 0.0f);
	SxGui::DragVector2("source",          &parameter.source.x, 0.1f, 0.0f);

	parameter.shadow.ShowInspector();

	//* push line (rect)
	{
		auto transform = RequireTransform();

		Vector2f half = parameter.source * 0.5f;

		Vector3f rect[4] = {};
		rect[0] = Matrix4x4::Transform(Vector3f{ -half.x,  half.y, 0.0f }, transform->GetMatrix());
		rect[1] = Matrix4x4::Transform(Vector3f{  half.x,  half.y, 0.0f }, transform->GetMatrix());
		rect[2] = Matrix4x4::Transform(Vector3f{  half.x, -half.y, 0.0f }, transform->GetMatrix());
		rect[3] = Matrix4x4::Transform(Vector3f{ -half.x, -half.y, 0.0f }, transform->GetMatrix());

		for (size_t i = 0; i < 4; ++i) {
			Graphics::PushLine(rect[i], rect[(i + 1) % 4], Color4f{ parameter.color, 1.0f }, 0.6f);
		}

	}

	//* push line (sphere)
	Graphics::PushSphere(RequireTransform()->GetPosition(), parameter.radius, Color4f{ parameter.color, 1.0f });
}

const D3D12_GPU_VIRTUAL_ADDRESS RectLightComponent::GetGPUVirtualAddress() const {
	StreamLogger::AssertA(parameter_ != nullptr, "rect light buffer is not create.");
	return parameter_->GetGPUVirtualAddress();
}

void RectLightComponent::SetUnit(LightCommon::Units unit) {
	auto& parameter = parameter_->At();

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

const RectLightComponent::Parameter& RectLightComponent::GetParameter() const {
	StreamLogger::AssertA(parameter_ != nullptr, "rect light buffer is not create.");
	return parameter_->At();
}

const TransformComponent* RectLightComponent::RequireTransform() const {
	return BaseComponent::GetBehaviour()->RequireComponent<TransformComponent>();
}

json RectLightComponent::ParseToJson() const {
	json data = json::object();

	data["color"]           = JsonSerializeFormatter<Color3f>::Serialize(parameter_->At().color);
	data["unit"]            = magic_enum::enum_name(parameter_->At().unit);
	data["intensity"]       = JsonSerializeFormatter<float>::Serialize(parameter_->At().intensity);
	data["radius"]          = JsonSerializeFormatter<float>::Serialize(parameter_->At().radius);
	data["source"]          = JsonSerializeFormatter<Vector2f>::Serialize(parameter_->At().source);
	data["shadow_strength"] = JsonSerializeFormatter<float>::Serialize(parameter_->At().shadow.strength);
	data["shadow_flag"]     = JsonSerializeFormatter<uint32_t>::Serialize(parameter_->At().shadow.flag.Get());

	return data;
}

void RectLightComponent::InputJson(const json& data) {
	parameter_->At().color           = JsonSerializeFormatter<Color3f>::Deserialize(data["color"]);
	parameter_->At().unit            = magic_enum::enum_cast<LightCommon::Units>(JsonSerializeFormatter<std::string>::Deserialize(data["unit"])).value();
	parameter_->At().intensity       = JsonSerializeFormatter<float>::Deserialize(data["intensity"]);
	parameter_->At().radius          = JsonSerializeFormatter<float>::Deserialize(data["radius"]);
	parameter_->At().source          = JsonSerializeFormatter<Vector2f>::Deserialize(data["source"]);
	parameter_->At().shadow.strength = JsonSerializeFormatter<float>::Deserialize(data["shadow_strength"]);
	parameter_->At().shadow.flag     = JsonSerializeFormatter<uint32_t>::Deserialize(data["shadow_flag"]);
}
