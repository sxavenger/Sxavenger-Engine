#include "PostProcessVolumetricFog.h"
SXAVENGER_ENGINE_USING
DXOBJECT_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/System.h>
#include <Engine/Render/FRenderTargetBuffer.h>
#include <Engine/Render/Scene/FScene.h>
#include <Engine/Render/FRenderCore.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Parameter structure methods
////////////////////////////////////////////////////////////////////////////////////////////

void PostProcessVolumetricFog::Parameter::Init() {
	startDistance = 0.0f;
	viewDistance  = 100.0f;
}

void PostProcessVolumetricFog::Parameter::SetImGuiCommand() {
}

////////////////////////////////////////////////////////////////////////////////////////////
// PostProcessVolumetricFog class methods
////////////////////////////////////////////////////////////////////////////////////////////

void PostProcessVolumetricFog::Init() {
	parameter_ = std::make_unique<ConstantBuffer<Parameter>>();
	parameter_->Create(System::GetDxDevice());
	parameter_->At().Init();

	name_ = "Volumetric Fog";
}

void PostProcessVolumetricFog::Process(const DirectXQueueContext* context, const ProcessInfo& info) {

	auto process = info.buffer->GetProcessTextures();
	process->NextProcess();
	process->GetCurrentTexture()->TransitionBeginUnordered(context);

	auto core = FRenderCore::GetInstance()->GetProcess();

	core->SetPipeline(FRenderCoreProcess::ProcessType::VolumetricFogLuminance, context);

	BindBufferDesc desc = {};
	// common
	desc.Set32bitConstants("Dimension", 2, &info.buffer->GetSize());
	desc.Set32bitConstants("Information", 1, &info.weight);

	//* textures
	desc.SetHandle("gInput", process->GetPrevTexture()->GetGPUHandleSRV());
	desc.SetHandle("gOutput", process->GetCurrentTexture()->GetGPUHandleUAV());

	//* parameter
	desc.SetAddress("gParameter", parameter_->GetGPUVirtualAddress());

	//* G-buffer
	desc.SetHandle("gPosition", info.buffer->GetGBuffer(FDeferredGBuffer::Layout::Position)->GetGPUHandleSRV());

	//* camera
	desc.SetAddress("gCamera", info.camera->GetGPUVirtualAddress());

	//* scene
	desc.SetAddress("gScene", info.scene->GetTopLevelAS().GetGPUVirtualAddress());

	//* light
	FScene::LightAddress directionalLightAddress = info.scene->GetDirectionalLightAddress();
	desc.Set32bitConstants("DirectionalLightCount", 1, &directionalLightAddress.count);
	desc.SetAddress("gDirectionalLightTransforms", directionalLightAddress.transforms);
	desc.SetAddress("gDirectionalLightParameters", directionalLightAddress.parameters);

	core->BindComputeBuffer(FRenderCoreProcess::ProcessType::VolumetricFogLuminance, context, desc);
	core->Dispatch(context, info.buffer->GetSize());

	process->GetCurrentTexture()->TransitionEndUnordered(context);

}

void PostProcessVolumetricFog::ShowInspectorImGui() {
	parameter_->At().SetImGuiCommand();
}
