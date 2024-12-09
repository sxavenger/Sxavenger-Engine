#include "VisualDoF.h"
_DXOBJECT_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/SxavengerSystem.h>
#include <Engine/Console/Console.h>

////////////////////////////////////////////////////////////////////////////////////////////
// VisualDoF class methods
////////////////////////////////////////////////////////////////////////////////////////////

void VisualDoF::Init() {
	parameter_ = std::make_unique<DimensionBuffer<Parameter>>();
	parameter_->Create(SxavengerSystem::GetDxDevice(), 1);

	// 仮
	(*parameter_)[0].f           = 0.01f;
	(*parameter_)[0].focusLength = 0.01f;
}

void VisualDoF::Term() {
}

void VisualDoF::Process(_MAYBE_UNUSED const DirectXThreadContext* context, _MAYBE_UNUSED SxavGraphicsFrame* frame) {
	frame->GetVisual()->NextResultBufferIndex();

	sConsole->SetComputePipeline(kVisual_DoF, context);

	BindBufferDesc bind = {};
	bind.SetAddress("gConfig", frame->GetGetConfigVirtualAddress());
	bind.SetHandle("gInput",   frame->GetVisual()->GetPrevBuffer()->GetGPUHandleSRV());
	bind.SetHandle("gDepth",   frame->GetDepthBufferController()->GetRasterizerGPUHandleSRV());
	bind.SetAddress("gCamera", frame->GetCamera()->GetGPUVirtualAddress());
	bind.SetAddress("gParam",  parameter_->GetGPUVirtualAddress());
	bind.SetHandle("gOutput",  frame->GetVisual()->GetResultBuffer()->GetGPUHandleUAV());

	sConsole->BindComputeBuffer(kVisual_DoF, context, bind);
	sConsole->DispatchCompute(context, frame->GetSize());
}

void VisualDoF::SetLayerImGui() {
	ImGui::CheckboxFlags("debug DoF", &(*parameter_)[0].isDebugDoF, 1);
	ImGui::DragFloat("focus length", &(*parameter_)[0].focusLength, 0.01f);
	ImGui::DragFloat("f", &(*parameter_)[0].f, 0.01f);
}

void VisualDoF::SetForcus(const Camera3d* camera, const Vector3f& position) {

	Vector3f clip = Matrix::Transform(position, camera->GetViewProj());
	Vector3f viewDepth = Matrix::Transform({ 0.0f, 0.0f, clip.z }, camera->GetProjInverse());

	(*parameter_)[0].focusLength = viewDepth.z;

}
