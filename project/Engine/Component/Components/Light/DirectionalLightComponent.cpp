#include "DirectionalLightComponent.h"
_DXOBJECT_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* component
#include "../../MonoBehaviour.h"

//* engine
#include <Engine/System/SxavengerSystem.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Parameter class methods
////////////////////////////////////////////////////////////////////////////////////////////

void DirectionalLightComponent::Parameter::Init() {
	color     = kWhite3;
	intensity = 1.0f;
}

////////////////////////////////////////////////////////////////////////////////////////////
// DirectionalLightComponent class methods
////////////////////////////////////////////////////////////////////////////////////////////

void DirectionalLightComponent::InspectorImGui() {
	auto& parameter = parameter_->At(0);
	ImGui::ColorEdit3("color",      &parameter.color.r);
	ImGui::SliderFloat("intensity", &parameter.intensity, 0.0f, 1.0f);
}

void DirectionalLightComponent::Init() {
	CreateParameterBuffer();
	LightSource::CreateShadowBuffer();
}

void DirectionalLightComponent::CreateParameterBuffer() {
	parameter_ = std::make_unique<DimensionBuffer<Parameter>>();
	parameter_->Create(SxavengerSystem::GetDxDevice(), 1);
	parameter_->At(0).Init();
}

const D3D12_GPU_VIRTUAL_ADDRESS& DirectionalLightComponent::GetParameterBufferAddress() const {
	Assert(parameter_ != nullptr, "directional light buffer is not create.");
	return parameter_->GetGPUVirtualAddress();
}

const TransformComponent* DirectionalLightComponent::GetTransform() const {
	return BaseComponent::GetBehaviour()->RequireComponent<TransformComponent>();
}
