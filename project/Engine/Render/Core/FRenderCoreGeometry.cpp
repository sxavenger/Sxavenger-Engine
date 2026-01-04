#include "FRenderCoreGeometry.h"
SXAVENGER_ENGINE_USING
DXOBJECT_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* render
#include "../GBuffer/FDeferredGBuffer.h"
#include "../GBuffer/FMainGBuffer.h"

//* engine
#include <Engine/System/Configuration/Configuration.h>

//* external
#include <magic_enum.hpp>

////////////////////////////////////////////////////////////////////////////////////////////
// FRenderCoreGeometry class methods
////////////////////////////////////////////////////////////////////////////////////////////

void FRenderCoreGeometry::Init() {
	CreateDesc();
	CreatePipeline();
}

void FRenderCoreGeometry::SetPipeline(Type type, const DirectXQueueContext* context, const Vector2ui& size) {
	pipelines_[static_cast<uint32_t>(type)]->SetPipeline(context->GetDxCommand(), size);
}

void FRenderCoreGeometry::BindGraphicsBuffer(Type type, const DirectXQueueContext* context, const DxObject::BindBufferDesc& desc) {
	pipelines_[static_cast<uint32_t>(type)]->BindGraphicsBuffer(context->GetDxCommand(), desc);
}

void FRenderCoreGeometry::CreateDesc() {

	//* deferred
	deferredDesc_ = {};
	deferredDesc_.CreateDefaultDesc();

	deferredDesc_.rtvFormats.clear();

	//!< Deferred Buffer
	deferredDesc_.SetRTVFormat(FDeferredGBuffer::GetFormat(FDeferredGBuffer::Layout::Albedo));
	deferredDesc_.SetRTVFormat(FDeferredGBuffer::GetFormat(FDeferredGBuffer::Layout::Normal));
	deferredDesc_.SetRTVFormat(FDeferredGBuffer::GetFormat(FDeferredGBuffer::Layout::MaterialARM));
	deferredDesc_.SetRTVFormat(FDeferredGBuffer::GetFormat(FDeferredGBuffer::Layout::Position));

	D3D12_RENDER_TARGET_BLEND_DESC blend = {};
	blend.BlendEnable           = true;
	blend.LogicOpEnable         = false;
	blend.SrcBlend              = D3D12_BLEND_ONE;
	blend.DestBlend             = D3D12_BLEND_ZERO;
	blend.BlendOp               = D3D12_BLEND_OP_ADD;
	blend.SrcBlendAlpha         = D3D12_BLEND_ONE;
	blend.DestBlendAlpha        = D3D12_BLEND_ZERO;
	blend.BlendOpAlpha          = D3D12_BLEND_OP_ADD;
	blend.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	deferredDesc_.SetBlendDesc(0, blend);
	deferredDesc_.SetIndependentBlendEnable(false);

	//* forward
	forwardDesc_ = {};
	forwardDesc_.CreateDefaultDesc();
	forwardDesc_.SetBlendMode(0, BlendMode::kBlendModeNormal_AlphaMax);
	forwardDesc_.SetRTVFormat(0, FMainGBuffer::kColorFormat);
}

void FRenderCoreGeometry::CreatePipeline() {

	{
		auto pipeline = std::make_unique<CustomReflectionGraphicsPipeline>();
		//* blob
		pipeline->CreateContent(kDirectory / "Geometry" / "Mesh" / "Default.vs.hlsl", GraphicsShaderType::vs);
		pipeline->CreateContent(kDirectory / "Geometry" / "Mesh" / "Forward.ps.hlsl", GraphicsShaderType::ps);
		pipeline->RegisterBlob();

		//* root signature
		pipeline->ReflectionRootSignature(System::GetDxDevice());

		//* pipeline
		pipeline->CreatePipeline(System::GetDxDevice(), forwardDesc_);

		pipelines_[static_cast<uint32_t>(Type::Forward_MeshVS)] = std::move(pipeline);
	}

	{
		auto pipeline = std::make_unique<CustomReflectionGraphicsPipeline>();
		//* blob
		pipeline->CreateContent(kDirectory / "Geometry" / "Mesh" / "Default.vs.hlsl",  GraphicsShaderType::vs);
		pipeline->CreateContent(kDirectory / "Geometry" / "Mesh" / "Deferred.ps.hlsl", GraphicsShaderType::ps);
		pipeline->RegisterBlob();

		//* root signature
		pipeline->ReflectionRootSignature(System::GetDxDevice());

		//* pipeline
		pipeline->CreatePipeline(System::GetDxDevice(), deferredDesc_);

		pipelines_[static_cast<uint32_t>(Type::Deferred_MeshVS)] = std::move(pipeline);
	}

	{
		auto pipeline = std::make_unique<CustomReflectionGraphicsPipeline>();
		//* blob
		pipeline->CreateContent(kDirectory / "Geometry" / "Mesh" / "Default.as.hlsl",  GraphicsShaderType::as);
		pipeline->CreateContent(kDirectory / "Geometry" / "Mesh" / "Default.ms.hlsl",  GraphicsShaderType::ms);
		pipeline->CreateContent(kDirectory / "Geometry" / "Mesh" / "Deferred.ps.hlsl", GraphicsShaderType::ps);
		pipeline->RegisterBlob();

		//* root signature
		pipeline->ReflectionRootSignature(System::GetDxDevice());

		//* pipeline
		pipeline->CreatePipeline(System::GetDxDevice(), deferredDesc_);

		pipelines_[static_cast<uint32_t>(Type::Deferred_MeshMS)] = std::move(pipeline);
	}

	{
		auto pipeline = std::make_unique<CustomReflectionGraphicsPipeline>();
		pipeline->CreateContent(kDirectory / "Particle" / "GPUParticle" / "GPUParticle.vs.hlsl", GraphicsShaderType::vs);
		pipeline->CreateContent(kDirectory / "Particle" / "GPUParticle" / "GPUParticle.ps.hlsl", GraphicsShaderType::ps);
		pipeline->RegisterBlob();

		//* root signature
		pipeline->ReflectionRootSignature(System::GetDxDevice());

		//* pipeline
		GraphicsPipelineDesc desc = forwardDesc_;
		desc.ClearElement();
		desc.SetElement("POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT);
		desc.SetElement("TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT);
		desc.SetElement("NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT);

		desc.SetRasterizer(D3D12_CULL_MODE_NONE, D3D12_FILL_MODE_SOLID);
		desc.SetDepthStencil(true, D3D12_DEPTH_WRITE_MASK_ZERO);
		desc.SetBlendMode(0, BlendMode::kBlendModeAdd);
		
		desc.SetRTVFormat(FMainGBuffer::kColorFormat);

		pipeline->CreatePipeline(System::GetDxDevice(), desc);

		pipelines_[static_cast<uint32_t>(Type::Forward_GPUParticleVS)] = std::move(pipeline);

	}
}
