#include "FRenderCoreGeometry.h"
SXAVENGER_ENGINE_USING
DXOBJECT_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* render
#include "../GBuffer/FDeferredGBuffer.h"
#include "../GBuffer/FTransparentGBuffer.h"
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

	{ //* deferred
		deferredDesc_ = {};
		deferredDesc_.CreateDefaultDesc();

		//!< Deferred Buffer
		deferredDesc_.ClearRTVFormat();
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
	}

	{ //* forward

		forwardDesc_ = {};
		forwardDesc_.CreateDefaultDesc();

		//!< Transparent Buffer
		forwardDesc_.ClearRTVFormat();
		forwardDesc_.SetRTVFormat(FTransparentGBuffer::GetFormat(FTransparentGBuffer::Layout::Accumulate));
		forwardDesc_.SetRTVFormat(FTransparentGBuffer::GetFormat(FTransparentGBuffer::Layout::Revealage));

		D3D12_RENDER_TARGET_BLEND_DESC accumulation = {};
		accumulation.BlendEnable           = true;
		accumulation.LogicOpEnable         = false;
		accumulation.SrcBlend              = D3D12_BLEND_ONE;
		accumulation.DestBlend             = D3D12_BLEND_ONE;
		accumulation.BlendOp               = D3D12_BLEND_OP_ADD;
		accumulation.SrcBlendAlpha         = D3D12_BLEND_ONE;
		accumulation.DestBlendAlpha        = D3D12_BLEND_ONE;
		accumulation.BlendOpAlpha          = D3D12_BLEND_OP_ADD;
		accumulation.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

		forwardDesc_.SetBlendDesc(0, accumulation);

		D3D12_RENDER_TARGET_BLEND_DESC revealage = {};
		revealage.BlendEnable           = true;
		revealage.LogicOpEnable         = false;
		revealage.SrcBlend              = D3D12_BLEND_ZERO;
		revealage.DestBlend             = D3D12_BLEND_INV_SRC_COLOR;
		revealage.BlendOp               = D3D12_BLEND_OP_ADD;
		revealage.SrcBlendAlpha         = D3D12_BLEND_ZERO;
		revealage.DestBlendAlpha        = D3D12_BLEND_ZERO;
		revealage.BlendOpAlpha          = D3D12_BLEND_OP_ADD;
		revealage.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_RED;

		forwardDesc_.SetBlendDesc(1, revealage);
		forwardDesc_.SetIndependentBlendEnable(true);

		forwardDesc_.SetDepthStencil(true, D3D12_DEPTH_WRITE_MASK_ZERO);

	}
	
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
		pipeline->CreateContent(kDirectory / "Geometry" / "Mesh" / "Default.as.hlsl", GraphicsShaderType::as);
		pipeline->CreateContent(kDirectory / "Geometry" / "Mesh" / "Default.ms.hlsl", GraphicsShaderType::ms);
		pipeline->CreateContent(kDirectory / "Geometry" / "Mesh" / "Forward.ps.hlsl", GraphicsShaderType::ps);
		pipeline->RegisterBlob();

		//* root signature
		pipeline->ReflectionRootSignature(System::GetDxDevice());

		//* pipeline
		pipeline->CreatePipeline(System::GetDxDevice(), forwardDesc_);

		pipelines_[static_cast<uint32_t>(Type::Forward_MeshMS)] = std::move(pipeline);
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
}
