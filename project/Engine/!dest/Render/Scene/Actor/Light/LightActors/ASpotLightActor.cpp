#include "ASpotLightActor.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* render
#include <Engine/Render/FRenderCore.h>

////////////////////////////////////////////////////////////////////////////////////////////
// ASpotLightActor class methods
////////////////////////////////////////////////////////////////////////////////////////////

void ASpotLightActor::Init() {
	TransformComponent::CreateBuffer();

	cb_ = std::make_unique<DxObject::DimensionBuffer<Parameter>>();
	cb_->Create(SxavengerSystem::GetDxDevice(), 1);
	cb_->At(0).Init();

	shadow_ = std::make_unique<DxObject::DimensionBuffer<RayQueryShadow>>();
	shadow_->Create(SxavengerSystem::GetDxDevice(), 1);
	shadow_->At(0).Init();

	AActor::name_ = "spot light actor";
}

void ASpotLightActor::Render(const RendererContext& context) {
	FRenderCore::GetInstance()->GetLight()->SetPipeline(
		FRenderCoreLight::LightType::Spot, context.context, context.size
	);

	FRenderCore::GetInstance()->GetLight()->BindIABuffer(context.context);

	DxObject::BindBufferDesc parameter = context.parameter;
	parameter.SetAddress("gTransform",  TransformComponent::GetGPUVirtualAddress());
	parameter.SetAddress("gSpotLight", cb_->GetGPUVirtualAddress());
	parameter.SetAddress("gShadow",    shadow_->GetGPUVirtualAddress());

	FRenderCore::GetInstance()->GetLight()->BindGraphicsBuffer(
		FRenderCoreLight::LightType::Spot, context.context, parameter
	);

	FRenderCore::GetInstance()->GetLight()->DrawCall(context.context);
}

void ASpotLightActor::InspectorImGui() {
	ImGui::ColorEdit3("color",      &GetParameter().color.r);
	ImGui::DragFloat("intensity",   &GetParameter().intensity, 0.01f, 0.0f, 12.0f);
	ImGui::DragFloat("distance",    &GetParameter().distance, 1.0f, 0.0f, 128.0f);
	ImGui::SliderFloat("falloff",   &GetParameter().falloff,  -1.0f, 1.0f);
	ImGui::SliderFloat("angle",     &GetParameter().angle,    -1.0f, 1.0f);

	ImGui::DragFloat("shadow strength", &GetShadow().strength, 0.01f, 0.0f, 1.0f);
}
