#include "ADirectionalLightActor.h"
_DXOBJECT_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* render
#include <Engine/Render/FRenderCore.h>

////////////////////////////////////////////////////////////////////////////////////////////
// ADirectionalLightActor class methods
////////////////////////////////////////////////////////////////////////////////////////////

void ADirectionalLightActor::Init() {

	TransformComponent::CreateBuffer();

	cb_ = std::make_unique<DxObject::DimensionBuffer<Parameter>>();
	cb_->Create(SxavengerSystem::GetDxDevice(), 1);
	cb_->At(0).Init();

	shadow_ = std::make_unique<DxObject::DimensionBuffer<RayQueryShadow>>();
	shadow_->Create(SxavengerSystem::GetDxDevice(), 1);
	shadow_->At(0).Init();

	AActor::name_ = "directional light actor";
}

void ADirectionalLightActor::Render(const RendererContext& context) {
	FRenderCore::GetInstance()->GetLight()->SetPipeline(
		FRenderCoreLight::LightType::Directional, context.context, context.size
	);

	FRenderCore::GetInstance()->GetLight()->BindIABuffer(context.context);

	BindBufferDesc parameter = context.parameter;
	parameter.SetAddress("gTransform",        TransformComponent::GetGPUVirtualAddress());
	parameter.SetAddress("gDirectionalLight", cb_->GetGPUVirtualAddress());
	parameter.SetAddress("gShadow",           shadow_->GetGPUVirtualAddress());

	FRenderCore::GetInstance()->GetLight()->BindGraphicsBuffer(
		FRenderCoreLight::LightType::Directional, context.context, parameter
	);

	FRenderCore::GetInstance()->GetLight()->DrawCall(context.context);
}

void ADirectionalLightActor::InspectorImGui() {
	ImGui::ColorEdit3("color",      &GetParameter().color.r);
	ImGui::DragFloat("intensity",   &GetParameter().intensity, 0.01f, 0.0f, 12.0f);

	ImGui::DragFloat("shadow strength", &GetShadow().strength, 0.01f, 0.0f, 1.0f);
}
