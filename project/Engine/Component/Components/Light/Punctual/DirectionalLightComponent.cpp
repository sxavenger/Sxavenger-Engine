#include "DirectionalLightComponent.h"
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

DirectionalLightComponent::DirectionalLightComponent(MonoBehaviour* behaviour)
	: BaseComponent(behaviour) {

	parameter_ = std::make_unique<DimensionBuffer<Parameter>>();
	parameter_->Create(SxavengerSystem::GetDxDevice(), 1);
	parameter_->At(0).Init();
}

void DirectionalLightComponent::ShowComponentInspector() {
	auto& parameter = parameter_->At(0);
	ImGui::ColorEdit3("color", &parameter.color.r);

	std::string format = "%.3flux";
	SxImGui::DragFloat("intensity", &parameter.intensity, 0.1f, 0.0f, std::nullopt, format.c_str());

	parameter.shadow.ShowInspector();

	//* push line
	Vector3f dir = RequireTransform()->GetTransform().GetForward();
	SxavengerContent::PushLine(RequireTransform()->GetPosition(), RequireTransform()->GetPosition() + dir, Color4f{ parameter.color, 1.0f }, 0.6f);
}

const D3D12_GPU_VIRTUAL_ADDRESS& DirectionalLightComponent::GetGPUVirtualAddress() const {
	Exception::Assert(parameter_ != nullptr, "directional light buffer is not create.");
	return parameter_->GetGPUVirtualAddress();
}

const DirectionalLightComponent::Parameter& DirectionalLightComponent::GetParameter() const {
	Exception::Assert(parameter_ != nullptr, "directional light buffer is not create.");
	return parameter_->At(0);
}

DirectionalLightComponent::Parameter& DirectionalLightComponent::GetParameter() {
	Exception::Assert(parameter_ != nullptr, "directional light buffer is not create.");
	return parameter_->At(0);
}

const TransformComponent* DirectionalLightComponent::RequireTransform() const {
	return BaseComponent::GetBehaviour()->RequireComponent<TransformComponent>();
}

json DirectionalLightComponent::PerseToJson() const {
	json data = json::object();

	data["color"]           = JsonSerializeFormatter<Color3f>::Serialize(parameter_->At(0).color);
	data["intensity"]       = JsonSerializeFormatter<float>::Serialize(parameter_->At(0).intensity);
	data["shadow_strength"] = JsonSerializeFormatter<float>::Serialize(parameter_->At(0).shadow.strength);
	data["shadow_flag"]     = JsonSerializeFormatter<uint32_t>::Serialize(parameter_->At(0).shadow.flag.Get());

	return data;
}

void DirectionalLightComponent::InputJson(const json& data) {
	parameter_->At(0).color           = JsonSerializeFormatter<Color3f>::Deserialize(data["color"]);
	parameter_->At(0).intensity       = JsonSerializeFormatter<float>::Deserialize(data["intensity"]);
	parameter_->At(0).shadow.strength = JsonSerializeFormatter<float>::Deserialize(data["shadow_strength"]);
	parameter_->At(0).shadow.flag     = JsonSerializeFormatter<uint32_t>::Deserialize(data["shadow_flag"]);
}
