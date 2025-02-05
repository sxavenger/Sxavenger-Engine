#include "FPostProcessVignette.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* render
#include <Engine/!Render/FRenderCore.h>

//* engine
#include <Engine/System/SxavengerSystem.h>

////////////////////////////////////////////////////////////////////////////////////////////
// FPostProcessVignette class methods
////////////////////////////////////////////////////////////////////////////////////////////

void FPostProcessVignette::Init() {
	parameter_ = std::make_unique<DxObject::DimensionBuffer<Parameter>>();
	parameter_->Create(SxavengerSystem::GetDxDevice(), 1);
	parameter_->At(0).color = { 0.0f, 0.0f, 0.0f, 1.0f };
}

void FPostProcessVignette::Process(const ProcessContext& context) {
	context.textures->NextIndex();

	FRenderCore::GetInstance()->GetProcess()->SetPipeline(FRenderCoreProcess::ProcessType::Vignette, context.context);

	DxObject::BindBufferDesc parameter = context.parameter;
	parameter.SetHandle("gInput", context.textures->GetPrevTexture(1)->GetGPUHandleUAV());
	parameter.SetHandle("gOutput", context.textures->GetPrevTexture(0)->GetGPUHandleUAV());
	parameter.SetAddress("gParameter", parameter_->GetGPUVirtualAddress());

	FRenderCore::GetInstance()->GetProcess()->BindComputeBuffer(FRenderCoreProcess::ProcessType::Vignette, context.context, parameter);
	FRenderCore::GetInstance()->GetProcess()->Dispatch(context.context, context.size);
}

void FPostProcessVignette::SetImGuiCommand() {
	ImGui::ColorEdit3("color", &parameter_->At(0).color.r);
	ImGui::DragFloat("offset", &parameter_->At(0).offset, 0.01f);
	ImGui::DragFloat("power", &parameter_->At(0).power, 0.01f);
}
