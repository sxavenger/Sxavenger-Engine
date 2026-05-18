#include "FRenderCoreSkyLight.h"
SXAVENGER_ENGINE_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/System.h>

////////////////////////////////////////////////////////////////////////////////////////////
// FRenderCoreSkyLight class methods
////////////////////////////////////////////////////////////////////////////////////////////

void FRenderCoreSkyLight::Init() {
	CreatePipeline();
}

void FRenderCoreSkyLight::SetPipeline(Pipeline pipeline, const DirectXQueueContext* context) const {
	pipelines_[static_cast<size_t>(pipeline)].SetPipeline(context->GetDxCommand());
}

void FRenderCoreSkyLight::BindComputeBuffer(Pipeline pipeline, const DirectXQueueContext* context, const DxObject::BindBufferDesc& desc) const {
	pipelines_[static_cast<size_t>(pipeline)].BindComputeBuffer(context->GetDxCommand(), desc);
}

void FRenderCoreSkyLight::Dispatch(const DirectXQueueContext* context, const Vector2ui& resolution) const {
	context->GetCommandList()->Dispatch(
		DxObject::RoundUp(resolution.x, kThreadGroupSize.x),
		DxObject::RoundUp(resolution.y, kThreadGroupSize.y),
		kThreadGroupSize.z
	);
}

void FRenderCoreSkyLight::CreatePipeline() {
	CreateComputePipeline(Pipeline::InitialReservoir, L"Visibility/InitialReservoir.cs.hlsl");
	CreateComputePipeline(Pipeline::SpatialReservoir, L"Visibility/SpatialReservoir.cs.hlsl");
	CreateComputePipeline(Pipeline::History,          L"Visibility/History.cs.hlsl");
	CreateComputePipeline(Pipeline::Solve,            L"Visibility/Solve.cs.hlsl");
}

void FRenderCoreSkyLight::CreateComputePipeline(Pipeline pipeline, std::filesystem::path compute) {

	auto& p = pipelines_[static_cast<size_t>(pipeline)];
	p.CreateBlob(kDirectory / compute);
	p.ReflectionPipeline(System::GetDxDevice());

}
