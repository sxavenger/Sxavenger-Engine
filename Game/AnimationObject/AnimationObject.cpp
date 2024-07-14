#include "AnimationObject.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
// engine
#include <MyEngine.h>

//-----------------------------------------------------------------------------------------
// using
//-----------------------------------------------------------------------------------------
using namespace DxObject;

////////////////////////////////////////////////////////////////////////////////////////////
// AnimationObject Base class methods
////////////////////////////////////////////////////////////////////////////////////////////

void AnimationObject::SetAttributeImGui() {
	ImGui::Text("animation object base class attributes.");
}

void AnimationObject::CreateGraphicsPipeline() {

	// Graphics
	blob_ = std::make_unique<GraphicsBlob>();
	blob_->Create(L"object3d/object3d.VS.hlsl", GRAPHICS_VERTEX);
	blob_->Create(L"object3d/object3d.PS.hlsl", GRAPHICS_PIXEL);

	pipeline_ = std::make_unique<GraphicsPipeline>();

	GraphicsRootSignatureDesc rootDesc;
	rootDesc.Resize(3, 1);
	rootDesc.SetCBV(0, VISIBILITY_VERTEX, 0);
	rootDesc.SetCBV(1, VISIBILITY_VERTEX, 1);
	rootDesc.SetSRV(2, VISIBILITY_PIXEL, 0);
	rootDesc.SetSampler(0, MODE_WRAP, VISIBILITY_PIXEL, 0);

	pipeline_->CreateRootSignature(MyEngine::GetDevicesObj(), rootDesc);

	GraphicsPipelineDesc pipelineDesc;
	pipelineDesc.CreateDefaultDesc();

	pipeline_->CreatePipeline(MyEngine::GetDevicesObj(), blob_.get(), pipelineDesc);

}

void AnimationObject::SetPipeline() {
	pipeline_->SetPipeline(MyEngine::GetCommandList());
}
