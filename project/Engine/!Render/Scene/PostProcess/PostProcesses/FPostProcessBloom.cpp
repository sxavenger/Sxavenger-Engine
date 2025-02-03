#include "FPostProcessBloom.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* render
#include <Engine/!Render/FRenderCore.h>

//* engine
#include <Engine/System/SxavengerSystem.h>

////////////////////////////////////////////////////////////////////////////////////////////
// FPostProcessBloom class methods
////////////////////////////////////////////////////////////////////////////////////////////

void FPostProcessBloom::Init() {
	parameter_ = std::make_unique<DxObject::DimensionBuffer<Parameter>>();
	parameter_->Create(SxavengerSystem::GetDxDevice(), 1);
	parameter_->At(0).Init();
}

void FPostProcessBloom::Process(const ProcessContext& context) {
	Assert(context.textures->GetSize() >= 3, "bloom process is need texture over 3.");

	{ //!< bloom process
		context.textures->NextIndex();

		FRenderCore::GetInstance()->GetProcess()->SetPipeline(FRenderCoreProcess::ProcessType::Bloom, context.context);

		DxObject::BindBufferDesc parameter = context.parameter;

		parameter.SetHandle("gInput",  context.textures->GetPrevTexture(1)->GetGPUHandleUAV());
		parameter.SetHandle("gOutput", context.textures->GetPrevTexture(0)->GetGPUHandleUAV());
		parameter.SetAddress("gParameter", parameter_->GetGPUVirtualAddress());

		FRenderCore::GetInstance()->GetProcess()->BindComputeBuffer(FRenderCoreProcess::ProcessType::Bloom, context.context, parameter);
		FRenderCore::GetInstance()->GetProcess()->Dispatch(context.context, context.size);
	}

	{ //!< Overlay
		context.textures->NextIndex();

		FRenderCore::GetInstance()->GetProcess()->SetPipeline(FRenderCoreProcess::ProcessType::Overlay, context.context);

		DxObject::BindBufferDesc parameter = context.parameter;
		parameter.SetHandle("gInputDst", context.textures->GetPrevTexture(2)->GetGPUHandleUAV());
		parameter.SetHandle("gInputSrc", context.textures->GetPrevTexture(1)->GetGPUHandleUAV());
		parameter.SetHandle("gOutput",   context.textures->GetPrevTexture(0)->GetGPUHandleUAV());
		parameter.SetAddress("gParameter", parameter_->GetGPUVirtualAddress());

		FRenderCore::GetInstance()->GetProcess()->BindComputeBuffer(FRenderCoreProcess::ProcessType::Overlay, context.context, parameter);
		FRenderCore::GetInstance()->GetProcess()->Dispatch(context.context, context.size);
	}
}

void FPostProcessBloom::SetImGuiCommand() {

	auto& parameter = parameter_->At(0);

	static const uint32_t min = 0;
	static const uint32_t max = 32;

	ImGui::Checkbox("isActive", &isActive_);
	ImGui::DragScalarN("size", ImGuiDataType_U32, &parameter.size.x, 2, 1.0f, &min, &max);
	ImGui::DragFloat("threshold", &parameter_->At(0).threshold, 0.01f, 0.0f, 16.0f);

}
