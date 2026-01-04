#include "DirectionalLightComponent.h"
SXAVENGER_ENGINE_USING
DXOBJECT_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* component
#include "../../../Entity/EntityBehaviour.h"

//* engine
#include <Engine/System/Utility/StreamLogger.h>
#include <Engine/System/UI/SxImGui.h>
#include <Engine/System/System.h>
#include <Engine/Graphics/Graphics.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Parameter class methods
////////////////////////////////////////////////////////////////////////////////////////////

void DirectionalLightComponent::Parameter::Init() {
	shadow.Init();
	color     = kWhite3<>;
	intensity = 10.0f;
}

////////////////////////////////////////////////////////////////////////////////////////////
// DirectionalLightComponent class methods
////////////////////////////////////////////////////////////////////////////////////////////

DirectionalLightComponent::DirectionalLightComponent(EntityBehaviour* behaviour)
	: BaseComponent(behaviour) {

	parameter_ = std::make_unique<ConstantBuffer<Parameter>>();
	parameter_->Create(System::GetDxDevice());
	parameter_->At().Init();
}

void DirectionalLightComponent::ShowComponentInspector() {
	auto& parameter = parameter_->At();
	ImGui::ColorEdit3("color", &parameter.color.r);

	std::string format = "%.3flux";
	SxImGui::DragFloat("intensity", &parameter.intensity, 0.1f, 0.0f, std::nullopt, format.c_str());

	parameter.shadow.ShowInspector();

	//* push line
	Vector3f dir = RequireTransform()->GetTransform().GetForward();
	Graphics::PushLine(RequireTransform()->GetPosition(), RequireTransform()->GetPosition() + dir, Color4f{ parameter.color, 1.0f }, 0.6f);
}

const D3D12_GPU_VIRTUAL_ADDRESS& DirectionalLightComponent::GetGPUVirtualAddress() const {
	StreamLogger::AssertA(parameter_ != nullptr, "directional light buffer is not create.");
	return parameter_->GetGPUVirtualAddress();
}

const DirectionalLightComponent::Parameter& DirectionalLightComponent::GetParameter() const {
	StreamLogger::AssertA(parameter_ != nullptr, "directional light buffer is not create.");
	return parameter_->At();
}

DirectionalLightComponent::Parameter& DirectionalLightComponent::GetParameter() {
	StreamLogger::AssertA(parameter_ != nullptr, "directional light buffer is not create.");
	return parameter_->At();
}

const TransformComponent* DirectionalLightComponent::RequireTransform() const {
	return BaseComponent::GetBehaviour()->RequireComponent<TransformComponent>();
}

json DirectionalLightComponent::PerseToJson() const {
	json data = json::object();

	data["color"]           = JsonSerializeFormatter<Color3f>::Serialize(parameter_->At().color);
	data["intensity"]       = JsonSerializeFormatter<float>::Serialize(parameter_->At().intensity);
	data["shadow_strength"] = JsonSerializeFormatter<float>::Serialize(parameter_->At().shadow.strength);
	data["shadow_flag"]     = JsonSerializeFormatter<uint32_t>::Serialize(parameter_->At().shadow.flag.Get());

	return data;
}

void DirectionalLightComponent::InputJson(const json& data) {
	parameter_->At().color           = JsonSerializeFormatter<Color3f>::Deserialize(data["color"]);
	parameter_->At().intensity       = JsonSerializeFormatter<float>::Deserialize(data["intensity"]);
	parameter_->At().shadow.strength = JsonSerializeFormatter<float>::Deserialize(data["shadow_strength"]);
	parameter_->At().shadow.flag     = JsonSerializeFormatter<uint32_t>::Deserialize(data["shadow_flag"]);
}
