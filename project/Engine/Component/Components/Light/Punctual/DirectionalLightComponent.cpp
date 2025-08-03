#include "DirectionalLightComponent.h"
_DXOBJECT_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* component
#include "../../../Entity/MonoBehaviour.h"

//* engine
#include <Engine/System/SxavengerSystem.h>
#include <Engine/Content/SxavengerContent.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Parameter class methods
////////////////////////////////////////////////////////////////////////////////////////////

void DirectionalLightComponent::Parameter::Init() {
	color     = kWhite3<>;
	intensity = 1.0f;
}

////////////////////////////////////////////////////////////////////////////////////////////
// DirectionalLightComponent class methods
////////////////////////////////////////////////////////////////////////////////////////////

void DirectionalLightComponent::ShowComponentInspector() {
	auto& parameter = parameter_->At(0);
	ImGui::ColorEdit3("color", &parameter.color.r);

	std::string format = "%.3flux";
	ImGui::DragFloat("intensity", &parameter.intensity, 0.1f, 0.0f, std::numeric_limits<float>::max(), format.c_str());

	LightCommon::ShowCommonInspector();

	//* push line
	Vector3f dir = RequireTransform()->GetTransform().GetForward();
	SxavengerContent::PushLine(RequireTransform()->GetPosition(), RequireTransform()->GetPosition() + dir, kColor, 0.6f);
}

void DirectionalLightComponent::Init() {
	CreateParameterBuffer();
	LightCommon::CreateShadowBuffer();
}

void DirectionalLightComponent::CreateParameterBuffer() {
	parameter_ = std::make_unique<DimensionBuffer<Parameter>>();
	parameter_->Create(SxavengerSystem::GetDxDevice(), 1); 
	parameter_->At(0).Init();
}

const D3D12_GPU_VIRTUAL_ADDRESS& DirectionalLightComponent::GetParameterBufferAddress() const {
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

	if (parameter_ == nullptr) {
		return data;
	}

	data["color"]           = GeometryJsonSerializer::ToJson(parameter_->At(0).color);
	data["intensity"]       = parameter_->At(0).intensity;
	data["shadow_strength"] = LightCommon::GetShadowParameter().strength;
	data["shadow_flag"]     = LightCommon::GetShadowParameter().flag.Get();

	return data;
}

void DirectionalLightComponent::InputJson(const json& data) {
	parameter_->At(0).color                    = GeometryJsonSerializer::JsonToColor3f(data["color"]);
	parameter_->At(0).intensity                = data["intensity"].get<float>();
	LightCommon::GetShadowParameter().strength = data["shadow_strength"].get<float>();
	LightCommon::GetShadowParameter().flag     = data["shadow_flag"].get<uint32_t>();
}
