#include "APointLightActor.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* render
#include <Engine/!Render/FRenderCore.h>


////////////////////////////////////////////////////////////////////////////////////////////
// ALightActor class methods
////////////////////////////////////////////////////////////////////////////////////////////

void APointLightActor::Init() {

	TransformComponent::CreateBuffer();

	cb_ = std::make_unique<DxObject::DimensionBuffer<Parameter>>();
	cb_->Create(SxavengerSystem::GetDxDevice(), 1);
	cb_->At(0).Init();

	AActor::name_ = "point light actor";
}

void APointLightActor::Render(const RendererContext& context) {

	FRenderCore::GetInstance()->GetLight()->SetPipeline(
		FRenderCoreLight::LightType::Point, context.context, context.size
	);

	FRenderCore::GetInstance()->GetLight()->BindIABuffer(context.context);

	DxObject::BindBufferDesc parameter = context.parameter;
	parameter.SetAddress("gTransform",  TransformComponent::GetGPUVirtualAddress());
	parameter.SetAddress("gPointLight", cb_->GetGPUVirtualAddress());

	FRenderCore::GetInstance()->GetLight()->BindGraphicsBuffer(
		FRenderCoreLight::LightType::Point, context.context, parameter
	);

	FRenderCore::GetInstance()->GetLight()->DrawCall(context.context);
}

void APointLightActor::InspectorImGui() {
	ImGui::ColorEdit3("color",      &GetParameter().color_intensity.r);
	ImGui::DragFloat("intensity",   &GetParameter().color_intensity.a, 1.0f, 0.0f, 12.0f);
	ImGui::DragFloat("distance",    &GetParameter().distance, 1.0f, 0.0f, 128.0f);
}
