#include "FRenderPassTonemap.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* render
#include "../FRenderCore.h"

////////////////////////////////////////////////////////////////////////////////////////////
// FRenderPassTonemap class methods
////////////////////////////////////////////////////////////////////////////////////////////

void FRenderPassTonemap::Render(const DirectXQueueContext* context, const Config& config) {

	if (!config.option.Test(FBaseRenderPass::Config::Option::Tonemap)) {
		return;
	}

	context->BeginEvent(L"RenderPass - Tonemap");

	FRenderCore::GetInstance()->GetProcess()->SetPipeline(
		FRenderCoreProcess::CompositeType::Tonemap, context
	);

	config.buffer->GetGBuffer(FMainGBuffer::Layout::Scene)->TransitionBeginUnordered(context);

	DxObject::BindBufferDesc parameter = {};
	// common
	parameter.Set32bitConstants("Dimension", 2, &config.buffer->GetSize());
	parameter.SetHandle("gTexture", config.buffer->GetGBuffer(FMainGBuffer::Layout::Scene)->GetGPUHandleUAV());

	parameter.Set32bitConstants("ColorSpaceBuffer", 1, &config.colorSpace);

	FRenderCore::GetInstance()->GetProcess()->BindComputeBuffer(
		FRenderCoreProcess::CompositeType::Tonemap, context, parameter
	);

	FRenderCore::GetInstance()->GetProcess()->Dispatch(context, config.buffer->GetSize());

	config.buffer->GetGBuffer(FMainGBuffer::Layout::Scene)->TransitionEndUnordered(context);

	// TODO: slope, toe, shoulder... をparameterで調整可能に

	context->EndEvent();

}
