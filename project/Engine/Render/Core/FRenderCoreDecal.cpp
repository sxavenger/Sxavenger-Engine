#include "FRenderCoreDecal.h"
SXAVENGER_ENGINE_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* render
#include "../Buffer/FDepthStencilBuffer.h"

//* engine
#include <Engine/System/System.h>

////////////////////////////////////////////////////////////////////////////////////////////
// FRenderCoreDecal class methods
////////////////////////////////////////////////////////////////////////////////////////////

void FRenderCoreDecal::Init() {
	CreateDesc();
	CreatePipeline();
}

void FRenderCoreDecal::SetPipeline(Pipeline pipeline, const DirectXQueueContext* context, const Vector2ui& resolution) const {
	pipelines_[static_cast<uint32_t>(pipeline)].SetPipeline(context->GetDxCommand(), resolution);
}

void FRenderCoreDecal::BindGraphicsBuffer(Pipeline pipeline, const DirectXQueueContext* context, const DxObject::BindBufferDesc& desc) const {
	pipelines_[static_cast<uint32_t>(pipeline)].BindGraphicsBuffer(context->GetDxCommand(), desc);
}

void FRenderCoreDecal::DrawCall(const DirectXQueueContext* context) {
	context->GetCommandList()->DrawInstanced(36, 1, 0, 0);
}

void FRenderCoreDecal::CreateDesc() {

	{ //!< decal
		auto& desc = descs_[static_cast<uint8_t>(Desc::Decal)] = {};

		desc.SetRasterizer(D3D12_CULL_MODE_NONE, D3D12_FILL_MODE_SOLID);
		desc.SetDepthStencil(true, D3D12_DEPTH_WRITE_MASK_ZERO, D3D12_COMPARISON_FUNC_ALWAYS);

		desc.SetPrimitive(DxObject::PrimitiveType::TriangleList);

		desc.SetDSVFormat(FDepthStencilBuffer::GetFormat(FDepthStencilBuffer::Layout::Scene));
	}

}

void FRenderCoreDecal::CreatePipeline() {

	{ //!< decal
		auto& pipeline = pipelines_[static_cast<uint32_t>(Pipeline::Decal)];

		//* blob
		pipeline.CreateContent(kDirectory / "Decal.vs.hlsl", DxObject::GraphicsShaderType::Vertex);
		pipeline.CreateContent(kDirectory / "Decal.ps.hlsl", DxObject::GraphicsShaderType::Pixel);
		pipeline.RegisterBlob();

		//* root signature
		pipeline.ReflectionRootSignature(System::GetDxDevice());

		//* pipeline
		pipeline.CreatePipeline(System::GetDxDevice(), descs_[static_cast<uint32_t>(Desc::Decal)]);
	}

}
