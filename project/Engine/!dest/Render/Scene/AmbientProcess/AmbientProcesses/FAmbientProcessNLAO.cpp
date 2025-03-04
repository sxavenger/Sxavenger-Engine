#include "FAmbientProcessNLAO.h"
_DXOBJECT_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* render
#include <Engine/Render/FRenderCore.h>

////////////////////////////////////////////////////////////////////////////////////////////
// FAmbientProcessNLAO class methods
////////////////////////////////////////////////////////////////////////////////////////////

void FAmbientProcessNLAO::Init() {
	parameter_ = std::make_unique<DxObject::DimensionBuffer<Parameter>>();
	parameter_->Create(SxavengerSystem::GetDxDevice(), 1);
	parameter_->At(0).Init();
}

void FAmbientProcessNLAO::Process(const ProcessContext& context) {
	{ //!< NLAO
		D3D12_RESOURCE_BARRIER barriers[] = {
			context.textures->GetGBuffer(FSceneTextures::GBufferLayout::Material_AO)->TransitionBeginUnordered(),
		};

		context.context->GetCommandList()->ResourceBarrier(_countof(barriers), barriers);

		FRenderCore::GetInstance()->GetProcess()->SetPipeline(FRenderCoreProcess::ProcessType::NLAO, context.context);

		BindBufferDesc parameter = context.parameter;
		parameter.SetHandle("gMaterial_AO", context.textures->GetGBuffer(FSceneTextures::GBufferLayout::Material_AO)->GetGPUHandleUAV());
		parameter.SetHandle("gPosition",    context.textures->GetGBuffer(FSceneTextures::GBufferLayout::Position)->GetGPUHandleSRV());
		parameter.SetHandle("gNormal",      context.textures->GetGBuffer(FSceneTextures::GBufferLayout::Normal)->GetGPUHandleSRV());
		parameter.SetAddress("gParameter",  parameter_->GetGPUVirtualAddress());

		FRenderCore::GetInstance()->GetProcess()->BindComputeBuffer(FRenderCoreProcess::ProcessType::NLAO, context.context, parameter);
		FRenderCore::GetInstance()->GetProcess()->Dispatch(context.context, context.size);
	}

	{ //!< NLAO Blur
		D3D12_RESOURCE_BARRIER barriers[] = {
			context.textures->GetGBuffer(FSceneTextures::GBufferLayout::Material_AO)->TransitionEndUnordered(),
			context.textures->GetGBuffer(FSceneTextures::GBufferLayout::Main)->TransitionBeginUnordered(),
		};

		context.context->GetCommandList()->ResourceBarrier(_countof(barriers), barriers);

		FRenderCore::GetInstance()->GetProcess()->SetPipeline(FRenderCoreProcess::ProcessType::NLAO_Blur, context.context);

		BindBufferDesc parameter = context.parameter;
		parameter.SetHandle("gMain",        context.textures->GetGBuffer(FSceneTextures::GBufferLayout::Main)->GetGPUHandleUAV());
		parameter.SetHandle("gMaterial_AO", context.textures->GetGBuffer(FSceneTextures::GBufferLayout::Material_AO)->GetGPUHandleSRV());
		parameter.SetAddress("gParameter",  parameter_->GetGPUVirtualAddress());

		FRenderCore::GetInstance()->GetProcess()->BindComputeBuffer(FRenderCoreProcess::ProcessType::NLAO_Blur, context.context, parameter);
		FRenderCore::GetInstance()->GetProcess()->Dispatch(context.context, context.size);
	}

	{ //!< reset
		D3D12_RESOURCE_BARRIER barriers[] = {
			context.textures->GetGBuffer(FSceneTextures::GBufferLayout::Main)->TransitionEndUnordered(),
		};

		context.context->GetCommandList()->ResourceBarrier(_countof(barriers), barriers);
	}
}

void FAmbientProcessNLAO::SetImGuiCommand() {
	auto& parameter = parameter_->At(0);

	static const uint32_t min = 0;
	static const uint32_t max = 32;

	ImGui::DragScalarN("samples", ImGuiDataType_U32, &parameter.samples.x, 2, 1.0f, &min, &max);
	ImGui::DragScalarN("size",    ImGuiDataType_U32, &parameter.size.x, 2, 1.0f, &min, &max);
	ImGui::DragFloat("radius",    &parameter.radius, 0.01f, 0.0f, 128.0f);
	ImGui::DragFloat("strength",  &parameter.strength, 0.01f, 0.0f, 1.0f);
}
