#include "FPostProcessDemo.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* render
#include <Engine/!Render/FRenderCore.h>

////////////////////////////////////////////////////////////////////////////////////////////
// FPostProcessDemo class methods
////////////////////////////////////////////////////////////////////////////////////////////

void FPostProcessDemo::Init() {
	pipeline_ = std::make_unique<CustomReflectionComputePipeline>();
	pipeline_->CreateAsset("packages/shaders/render/postprocess/testSSAO.hlsl");
	pipeline_->RegisterBlob();
	pipeline_->ReflectionPipeline(SxavengerSystem::GetDxDevice());

	parameter_ = std::make_unique<DxObject::DimensionBuffer<Parameter>>();
	parameter_->Create(SxavengerSystem::GetDxDevice(), 1);

	buffer_ = std::make_unique<UnorderedTexture>();
	buffer_->Create({ 1280, 720 }, DXGI_FORMAT_R32G32B32A32_FLOAT);

	pipeline1_ = std::make_unique<CustomReflectionComputePipeline>();
	pipeline1_->CreateAsset("packages/shaders/render/postprocess/testSSAOBlur.cs.hlsl");
	pipeline1_->RegisterBlob();
	pipeline1_->ReflectionPipeline(SxavengerSystem::GetDxDevice());
}

void FPostProcessDemo::Process(const FPostProcess::ProcessContext& context) {
	if (!isActive_) {
		return;
	}

	{
		pipeline_->SetPipeline(context.context->GetDxCommand());

		DxObject::BindBufferDesc parameter = context.parameter;

		/*parameter.SetHandle("gInput", context.textures->GetPrevTexture(1)->GetGPUHandleUAV());*/
		parameter.SetHandle("gOutput", buffer_->GetGPUHandleUAV());
		parameter.SetAddress("gParameter", parameter_->GetGPUVirtualAddress());

		pipeline_->BindComputeBuffer(context.context->GetDxCommand(), parameter);
		FRenderCore::GetInstance()->GetProcess()->Dispatch(context.context, context.size);
	}

	context.textures->NextIndex();

	{
		pipeline1_->SetPipeline(context.context->GetDxCommand());

		DxObject::BindBufferDesc parameter = context.parameter;

		parameter.SetHandle("gInput", context.textures->GetPrevTexture(1)->GetGPUHandleUAV());
		parameter.SetHandle("gIntermediate", buffer_->GetGPUHandleSRV());
		parameter.SetHandle("gOutput", context.textures->GetPrevTexture(0)->GetGPUHandleUAV());
		parameter.SetAddress("gParameter", parameter_->GetGPUVirtualAddress());

		pipeline1_->BindComputeBuffer(context.context->GetDxCommand(), parameter);
		FRenderCore::GetInstance()->GetProcess()->Dispatch(context.context, context.size);
	}

	
}

void FPostProcessDemo::AttributeImGui() {
	ImGui::Checkbox("active", &isActive_);

	ImGui::DragFloat("bias",       &parameter_->At(0).angleBias, 0.01f, 0.0f, 90.0f * kDegToRad);
	ImGui::DragFloat("max radius", &parameter_->At(0).maxRadius, 0.01f, 0.0f, 12.0f);
	ImGui::DragFloat("radius", &parameter_->At(0).radius, 0.01f, 0.0f, 128.0f);
	ImGui::DragFloat("strength", &parameter_->At(0).strength, 0.01f, 0.0f, 12.0f);
	ImGui::DragFloat("filtter", &parameter_->At(0).filtter, 0.01f, 0.0f, 128.0f);
	ImGui::DragFloat("scale", &parameter_->At(0).scale, 0.01f, 0.0f, 128.0f);

}
