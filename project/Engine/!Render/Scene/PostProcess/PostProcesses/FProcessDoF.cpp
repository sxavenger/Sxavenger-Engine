#include "FProcessDoF.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* render
#include <Engine/!Render/FRenderCore.h>

//* engine
#include <Engine/System/SxavengerSystem.h>

////////////////////////////////////////////////////////////////////////////////////////////
// FProcessDoF class methods
////////////////////////////////////////////////////////////////////////////////////////////

void FProcessDoF::Init() {
	parameter_ = std::make_unique<DxObject::DimensionBuffer<Parameter>>();
	parameter_->Create(SxavengerSystem::GetDxDevice(), 1);
	parameter_->At(0).Init();
}

void FProcessDoF::Process(const FPostProcess::ProcessContext& context) {
	context.textures->NextIndex();

	FRenderCore::GetInstance()->GetProcess()->SetPipeline(FRenderCoreProcess::ProcessType::DoF, context.context);

	DxObject::BindBufferDesc parameter = context.parameter;

	parameter.SetHandle("gInput",  context.textures->GetPrevTexture(1)->GetGPUHandleUAV());
	parameter.SetHandle("gOutput", context.textures->GetPrevTexture(0)->GetGPUHandleUAV());
	parameter.SetAddress("gParameter", parameter_->GetGPUVirtualAddress());

	FRenderCore::GetInstance()->GetProcess()->BindComputeBuffer(FRenderCoreProcess::ProcessType::DoF, context.context, parameter);
	FRenderCore::GetInstance()->GetProcess()->Dispatch(context.context, context.size);
}

void FProcessDoF::SetImGuiCommand() {
	ImGui::DragFloat("focus", &parameter_->At(0).focus, 0.01f, 0.0f, 128.0f);
	ImGui::DragFloat("f",     &parameter_->At(0).f,     0.01f, 0.0f, 128.0f);

	static const uint32_t min = 0;
	static const uint32_t max = 16;
	ImGui::DragScalarN("strength", ImGuiDataType_::ImGuiDataType_U32, static_cast<void*>(&parameter_->At(0).strength.x), 2, 1, static_cast<const void*>(&min), static_cast<const void*>(&max), "%u");
	ImGui::CheckboxFlags("Debug View", &parameter_->At(0).isDebugView, 1);
}
