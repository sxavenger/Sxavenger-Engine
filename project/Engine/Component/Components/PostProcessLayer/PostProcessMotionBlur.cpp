#include "PostProcessMotionBlur.h"
_DXOBJECT_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* render
#include <Engine/Render/FRenderTargetBuffer.h>
#include <Engine/Render/FRenderCore.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Parameter structure methods
////////////////////////////////////////////////////////////////////////////////////////////

void PostProcessMotionBlur::Parameter::Init() {
	intensity = 1.0f;
}

void PostProcessMotionBlur::Parameter::SetImGuiCommand() {
	ImGui::DragFloat("intensity", &intensity, 0.01f, 0.0f, 10.0f);
}

////////////////////////////////////////////////////////////////////////////////////////////
// PostProcessMotionBlur class methods
////////////////////////////////////////////////////////////////////////////////////////////

void PostProcessMotionBlur::Init() {
	parameter_ = std::make_unique<ConstantBuffer<Parameter>>();
	parameter_->Create(SxavengerSystem::GetDxDevice());
	parameter_->At().Init();

	name_ = "Motion Blur";
}

void PostProcessMotionBlur::Process(const DirectXQueueContext* context, const ProcessInfo& info) {

	auto process = info.buffer->GetProcessTextures();
	process->NextProcess();
	process->GetCurrentTexture()->TransitionBeginUnordered(context);

	auto core = FRenderCore::GetInstance()->GetProcess();

	core->SetPipeline(FRenderCoreProcess::ProcessType::MotionBlur, context);

	BindBufferDesc desc = {};
	// common
	desc.Set32bitConstants("Dimension", 2,  &info.buffer->GetSize());
	desc.Set32bitConstants("Infomation", 1, &info.weight);

	//* textures
	desc.SetHandle("gInput",  process->GetPrevTexture()->GetGPUHandleSRV());
	desc.SetHandle("gOutput", process->GetCurrentTexture()->GetGPUHandleUAV());

	// parameter
	desc.SetAddress("gParameter", parameter_->GetGPUVirtualAddress());

	desc.SetHandle("gVelocity", info.buffer->GetGBuffer(FDeferredGBuffer::Layout::Velocity)->GetGPUHandleSRV());
	desc.SetAddress("gCamera",  info.camera->GetGPUVirtualAddress());
	
	core->BindComputeBuffer(FRenderCoreProcess::ProcessType::MotionBlur, context, desc);
	core->Dispatch(context, info.buffer->GetSize());

	process->GetCurrentTexture()->TransitionEndUnordered(context);

}

void PostProcessMotionBlur::ShowInspectorImGui() {
	parameter_->At().SetImGuiCommand();
}
