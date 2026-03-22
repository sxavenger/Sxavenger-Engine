#include "FRenderCoreCanvas.h"
SXAVENGER_ENGINE_USING
DXOBJECT_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* render
#include "../Buffer/FMainBuffer.h"

//* engine
#include <Engine/System/System.h>

////////////////////////////////////////////////////////////////////////////////////////////
// FRenderCoreCanvas class methods
////////////////////////////////////////////////////////////////////////////////////////////

void FRenderCoreCanvas::Init() {
	CreateDesc();
	CreatePipeline();
}

void FRenderCoreCanvas::SetPipeline(Pipeline pipeline, const DirectXQueueContext* context, const Vector2ui& resolution) const {
	pipelines_[static_cast<uint8_t>(pipeline)].SetPipeline(context->GetDxCommand(), resolution);
}

void FRenderCoreCanvas::BindGraphicsBuffer(Pipeline pipeline, const DirectXQueueContext* context, const DxObject::BindBufferDesc& desc) const {
	pipelines_[static_cast<uint8_t>(pipeline)].BindGraphicsBuffer(context->GetDxCommand(), desc);
}

void FRenderCoreCanvas::CreateDesc() {

	desc_.SetElement("POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT);
	desc_.SetElement("TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT);
	desc_.SetElement("COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT);

	desc_.SetRasterizer(D3D12_CULL_MODE_NONE, D3D12_FILL_MODE_SOLID);
	desc_.SetDepthStencil(true, D3D12_DEPTH_WRITE_MASK_ALL, D3D12_COMPARISON_FUNC_LESS_EQUAL);

	desc_.SetBlendMode(0, BlendMode::Normal_AlphaMax);
	desc_.SetIndependentBlendEnable(false);

	desc_.SetPrimitive(PrimitiveType::TrianglList);

	desc_.SetRTVFormat(0, FMainBuffer::GetFormat(FMainBuffer::Layout::Canvas));
	desc_.SetDSVFormat(kDefaultDepthFormat);

}

void FRenderCoreCanvas::CreatePipeline() {
	CreatePipeline(Pipeline::Sprite, "Sprite.vs.hlsl", "Sprite.ps.hlsl");
	CreatePipeline(Pipeline::Text, "Text.vs.hlsl", "Text.ps.hlsl");
}

void FRenderCoreCanvas::CreatePipeline(Pipeline type, std::filesystem::path vertex, std::filesystem::path pixel) {
	auto& pipeline = pipelines_[static_cast<uint8_t>(type)];
	pipeline.CreateContent(kDirectory / vertex, GraphicsShaderType::Vertex);
	pipeline.CreateContent(kDirectory / pixel, GraphicsShaderType::Pixel);
	pipeline.RegisterBlob();

	pipeline.ReflectionRootSignature(System::GetDxDevice());
	pipeline.CreatePipeline(System::GetDxDevice(), desc_);
}
