#include "FBaseRenderPass.h"
SXAVENGER_ENGINE_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/System.h>

//* lib
#include <Lib/Adapter/String/EncodedString.h>

////////////////////////////////////////////////////////////////////////////////////////////
// FBaseRenderPass class methods
////////////////////////////////////////////////////////////////////////////////////////////

void FBaseRenderPass::BeginRenderPass(const DirectXQueueContext* context, const std::string& pass, const FRenderConfig& config) {
	context->BeginEvent(EncodedString::Convert(pass));
	System::BeginRecordGpu(std::format("[{}] FRenderPass - {}", config.name, pass));
}

void FBaseRenderPass::EndRenderPass(const DirectXQueueContext* context) {
	System::EndRecordGpu();
	context->EndEvent();
}

void FBaseRenderPass::BeginEvent(const DirectXQueueContext* context, const std::string& event) {
	context->BeginEvent(EncodedString::Convert(event));
}

void FBaseRenderPass::EndEvent(const DirectXQueueContext* context) {
	context->EndEvent();
}
