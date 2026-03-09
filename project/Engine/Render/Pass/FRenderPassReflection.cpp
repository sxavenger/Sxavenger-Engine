#include "FRenderPassReflection.h"
SXAVENGER_ENGINE_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* render
#include "../FRenderCore.h"

////////////////////////////////////////////////////////////////////////////////////////////
// FRenderPassReflection class methods
////////////////////////////////////////////////////////////////////////////////////////////

void FRenderPassReflection::Render(const DirectXQueueContext* context, const Config& config) {

	if (config.CheckStatus(Config::Status::Geometry_Warning)) {
		return;
	}

	if (!config.option.Test(FBaseRenderPass::Config::Option::Reflection)) {
		return;
	}

	context->BeginEvent(L"RenderPass - Reflection");
	System::BeginRecordGpu(std::format("[{}] RenderPass - Reflection", magic_enum::enum_name(config.tag)));

	{
		BeginPassReflection(context, config.buffer);

		PassReflection(context, config);

		EndPassReflection(context, config.buffer);
	}
	
	ProcessReflectionCalculate(context, config);

	TransitionReflectionPass(context, config);

	System::EndRecordGpu();
	context->EndEvent();
}

void FRenderPassReflection::BeginPassReflection(const DirectXQueueContext* context, FRenderTargetBuffer* buffer) {
	buffer->GetGBuffer(FReflectionGBuffer::Layout::ReflectionRadiance)->TransitionBeginUnordered(context);
	buffer->GetGBuffer(FReflectionGBuffer::Layout::ReflectionPosition)->TransitionBeginUnordered(context);
}

void FRenderPassReflection::EndPassReflection(const DirectXQueueContext* context, FRenderTargetBuffer* buffer) {
	buffer->GetGBuffer(FReflectionGBuffer::Layout::ReflectionRadiance)->TransitionEndUnordered(context);
	buffer->GetGBuffer(FReflectionGBuffer::Layout::ReflectionPosition)->TransitionEndUnordered(context);
}

void FRenderPassReflection::PassReflection(const DirectXQueueContext* context, const Config& config) {

	auto commandList = context->GetCommandList();

	auto core = FRenderCore::GetInstance()->GetReflection();
	core->GetContext()->SetStateObject(context->GetDxCommand());

	//* output
	commandList->SetComputeRootDescriptorTable(0, config.buffer->GetGBuffer(FReflectionGBuffer::Layout::ReflectionRadiance)->GetGPUHandleUAV()); //!< gReflectionRadiance
	commandList->SetComputeRootDescriptorTable(1, config.buffer->GetGBuffer(FReflectionGBuffer::Layout::ReflectionPosition)->GetGPUHandleUAV()); //!< gReflectionPosition

	//* scene
	commandList->SetComputeRootShaderResourceView(2, config.scene->GetTopLevelAS().GetGPUVirtualAddress()); //!< gScene

	//* G-Buffer
	commandList->SetComputeRootDescriptorTable(3, config.buffer->GetDepth()->GetRasterizerGPUHandleSRV());                           //!< gDepth
	commandList->SetComputeRootDescriptorTable(4, config.buffer->GetGBuffer(FDeferredGBuffer::Layout::Normal)->GetGPUHandleSRV());   //!< gNormal
	commandList->SetComputeRootDescriptorTable(5, config.buffer->GetGBuffer(FDeferredGBuffer::Layout::Position)->GetGPUHandleSRV()); //!< gPosition

	//* camera
	commandList->SetComputeRootConstantBufferView(6, config.camera->GetGPUVirtualAddress()); //!< gCamera

	//* light
	// Directional Light
	FScene::LightAddress directionalLightAddress = config.scene->GetDirectionalLightAddress();
	commandList->SetComputeRoot32BitConstants(7, 1, &directionalLightAddress.count, 0);
	commandList->SetComputeRootShaderResourceView(8, directionalLightAddress.transforms);
	commandList->SetComputeRootShaderResourceView(9, directionalLightAddress.parameters);

	// Point Light
	FScene::LightAddress pointLightAddress = config.scene->GetPointLightAddress();
	commandList->SetComputeRoot32BitConstants(10, 1, &pointLightAddress.count, 0);
	commandList->SetComputeRootShaderResourceView(11, pointLightAddress.transforms);
	commandList->SetComputeRootShaderResourceView(12, pointLightAddress.parameters);

	// Spot Light
	FScene::LightAddress spotLightAddress = config.scene->GetSpotLightAddress();
	commandList->SetComputeRoot32BitConstants(13, 1, &spotLightAddress.count, 0);
	commandList->SetComputeRootShaderResourceView(14, spotLightAddress.transforms);
	commandList->SetComputeRootShaderResourceView(15, spotLightAddress.parameters);

	core->GetContext()->DispatchRays(context->GetDxCommand(), config.buffer->GetSize());
}

void FRenderPassReflection::ProcessReflectionCalculate(const DirectXQueueContext* context, const Config& config) {
	config.buffer->GetGBuffer(FReflectionGBuffer::Layout::Reflection)->TransitionBeginUnordered(context);

	auto core = FRenderCore::GetInstance()->GetReflection();
	core->SetPipeline(FRenderCoreReflection::Process::Calculate, context);

	DxObject::BindBufferDesc desc = {};
	desc.SetHandle("gReflection", config.buffer->GetGBuffer(FReflectionGBuffer::Layout::Reflection)->GetGPUHandleUAV());
	desc.Set32bitConstants("Dimension", 2, &config.buffer->GetSize());
	desc.SetHandle("gReflectionRadiance", config.buffer->GetGBuffer(FReflectionGBuffer::Layout::ReflectionRadiance)->GetGPUHandleSRV());
	desc.SetHandle("gReflectionPosition", config.buffer->GetGBuffer(FReflectionGBuffer::Layout::ReflectionPosition)->GetGPUHandleSRV());
	desc.SetAddress("gCamera", config.camera->GetGPUVirtualAddress());

	desc.SetHandle("gDepth",    config.buffer->GetDepth()->GetRasterizerGPUHandleSRV());
	desc.SetHandle("gAlbedo",   config.buffer->GetGBuffer(FDeferredGBuffer::Layout::Albedo)->GetGPUHandleSRV());
	desc.SetHandle("gNormal",   config.buffer->GetGBuffer(FDeferredGBuffer::Layout::Normal)->GetGPUHandleSRV());
	desc.SetHandle("gPosition", config.buffer->GetGBuffer(FDeferredGBuffer::Layout::Position)->GetGPUHandleSRV());
	desc.SetHandle("gMaterial", config.buffer->GetGBuffer(FDeferredGBuffer::Layout::MaterialARM)->GetGPUHandleSRV());

	core->BindComputeBuffer(FRenderCoreReflection::Process::Calculate, context, desc);
	core->Dispatch(context, config.buffer->GetSize());

	config.buffer->GetGBuffer(FReflectionGBuffer::Layout::Reflection)->TransitionEndUnordered(context);
}

void FRenderPassReflection::TransitionReflectionPass(const DirectXQueueContext* context, const Config& config) {

	config.buffer->TransitionBeginUnorderedMainScene(context);

	auto core = FRenderCore::GetInstance()->GetTransition();
	core->SetPipeline(FRenderCoreTransition::Transition::ReflectionTransition, context);

	DxObject::BindBufferDesc parameter = {};
	// common parameter
	parameter.Set32bitConstants("Dimension", 2, &config.buffer->GetSize());

	// input
	parameter.SetHandle("gReflection", config.buffer->GetGBuffer(FReflectionGBuffer::Layout::Reflection)->GetGPUHandleSRV());

	// output
	parameter.SetHandle("gOutput", config.buffer->GetGBuffer(FMainGBuffer::Layout::Scene)->GetGPUHandleUAV());

	core->BindComputeBuffer(FRenderCoreTransition::Transition::ReflectionTransition, context, parameter);
	core->Dispatch(context, config.buffer->GetSize());

	config.buffer->TransitionEndUnorderedMainScene(context);

}
