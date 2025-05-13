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
	Vector3f dir = GetTransform()->GetTransform().GetForward();
	SxavengerContent::PushLine(GetTransform()->GetPosition(), GetTransform()->GetPosition() + dir, kColor);
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
	Assert(parameter_ != nullptr, "directional light buffer is not create.");
	return parameter_->GetGPUVirtualAddress();
}

const TransformComponent* DirectionalLightComponent::GetTransform() const {
	return BaseComponent::GetBehaviour()->RequireComponent<TransformComponent>();
}
