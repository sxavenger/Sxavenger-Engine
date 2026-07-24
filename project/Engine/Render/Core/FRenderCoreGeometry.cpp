#include "FRenderCoreGeometry.h"
SXAVENGER_ENGINE_USING
DXOBJECT_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* render
#include "../Buffer/FGBuffer.h"
#include "../Buffer/FTransparentBuffer.h"
#include "../Buffer/FDepthStencilBuffer.h"

//* engine
#include <Engine/System/System.h>

//* external
#include <magic_enum.hpp>

////////////////////////////////////////////////////////////////////////////////////////////
// FRenderCoreGeometry class methods
////////////////////////////////////////////////////////////////////////////////////////////

void FRenderCoreGeometry::Init() {
	CreateDesc();
	CreatePipeline();
}

void FRenderCoreGeometry::SetPipeline(Pipeline pipeline, const DirectXQueueContext* context, const Vector2ui& resolution) const {
	pipelines_[static_cast<uint32_t>(pipeline)].SetPipeline(context->GetDxCommand(), resolution);
}

void FRenderCoreGeometry::BindGraphicsBuffer(Pipeline pipeline, const DirectXQueueContext* context, const DxObject::BindBufferDesc& desc) const {
	pipelines_[static_cast<uint32_t>(pipeline)].BindGraphicsBuffer(context->GetDxCommand(), desc);
}

void FRenderCoreGeometry::CreateDesc() {

	{ //!< deferred
		auto& desc = descs_[static_cast<uint8_t>(Desc::Deferred)] = {};

		desc.SetElement("POSITION",  0, DXGI_FORMAT_R32G32B32A32_FLOAT);
		desc.SetElement("TEXCOORD",  0, DXGI_FORMAT_R32G32_FLOAT);
		desc.SetElement("NORMAL",    0, DXGI_FORMAT_R32G32B32_FLOAT);
		desc.SetElement("TANGENT",   0, DXGI_FORMAT_R32G32B32_FLOAT);
		desc.SetElement("BITANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT);

		desc.SetRasterizer(D3D12_CULL_MODE_BACK, D3D12_FILL_MODE_SOLID);
		desc.SetDepthStencil(true);

		desc.SetPrimitive(PrimitiveType::TriangleList);

		D3D12_RENDER_TARGET_BLEND_DESC blend = {};
		blend.BlendEnable           = false;
		blend.LogicOpEnable         = false;
		blend.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

		desc.SetBlendDesc(0, blend);
		desc.SetIndependentBlendEnable(false);

		desc.SetRTVFormat(FGBuffer::GetFormat(FGBuffer::Layout::Albedo));
		desc.SetRTVFormat(FGBuffer::GetFormat(FGBuffer::Layout::Normal));
		desc.SetRTVFormat(FGBuffer::GetFormat(FGBuffer::Layout::MaterialARM));
		desc.SetRTVFormat(FGBuffer::GetFormat(FGBuffer::Layout::Address));
		desc.SetDSVFormat(FDepthStencilBuffer::GetFormat(FDepthStencilBuffer::Layout::Scene));

	}

	{ //* forward prepass

		//!< Forward Opaque Depth Pre-pass.
		auto& desc = descs_[static_cast<uint8_t>(Desc::ForwardPrepass)] = {};
		
		desc.SetElement("POSITION",  0, DXGI_FORMAT_R32G32B32A32_FLOAT);
		desc.SetElement("TEXCOORD",  0, DXGI_FORMAT_R32G32_FLOAT);
		desc.SetElement("NORMAL",    0, DXGI_FORMAT_R32G32B32_FLOAT);
		desc.SetElement("TANGENT",   0, DXGI_FORMAT_R32G32B32_FLOAT);
		desc.SetElement("BITANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT);

		desc.SetRasterizer(D3D12_CULL_MODE_BACK, D3D12_FILL_MODE_SOLID);
		desc.SetDepthStencil(true);

		desc.SetPrimitive(PrimitiveType::TriangleList);

		desc.SetDSVFormat(FDepthStencilBuffer::GetFormat(FDepthStencilBuffer::Layout::Scene));
	}

	{ //* forward transparent

		auto& desc = descs_[static_cast<uint8_t>(Desc::ForwardTransparent)] = {};

		desc.SetElement("POSITION",  0, DXGI_FORMAT_R32G32B32A32_FLOAT);
		desc.SetElement("TEXCOORD",  0, DXGI_FORMAT_R32G32_FLOAT);
		desc.SetElement("NORMAL",    0, DXGI_FORMAT_R32G32B32_FLOAT);
		desc.SetElement("TANGENT",   0, DXGI_FORMAT_R32G32B32_FLOAT);
		desc.SetElement("BITANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT);

		desc.SetRasterizer(D3D12_CULL_MODE_BACK, D3D12_FILL_MODE_SOLID);
		desc.SetDepthStencil(true, D3D12_DEPTH_WRITE_MASK_ZERO, D3D12_COMPARISON_FUNC_LESS_EQUAL);

		desc.SetPrimitive(PrimitiveType::TriangleList);

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

		desc.SetBlendDesc(0, accumulation);

		D3D12_RENDER_TARGET_BLEND_DESC revealage = {};
		revealage.BlendEnable           = true;
		revealage.LogicOpEnable         = false;
		revealage.SrcBlend              = D3D12_BLEND_ZERO;
		revealage.DestBlend             = D3D12_BLEND_INV_SRC_COLOR;
		revealage.BlendOp               = D3D12_BLEND_OP_ADD;
		revealage.SrcBlendAlpha         = D3D12_BLEND_ZERO;
		revealage.DestBlendAlpha        = D3D12_BLEND_INV_SRC_ALPHA;
		revealage.BlendOpAlpha          = D3D12_BLEND_OP_ADD;
		revealage.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

		desc.SetBlendDesc(1, revealage);
		desc.SetIndependentBlendEnable(true);
		
		desc.SetRTVFormat(FTransparentBuffer::GetFormat(FTransparentBuffer::Layout::Accumulate));
		desc.SetRTVFormat(FTransparentBuffer::GetFormat(FTransparentBuffer::Layout::Revealage));

		desc.SetDSVFormat(FDepthStencilBuffer::GetFormat(FDepthStencilBuffer::Layout::Scene));
	}
	
}

void FRenderCoreGeometry::CreatePipeline() {

	{ //!< forward prepass [vs-ps]
		auto& pipeline = pipelines_[static_cast<uint32_t>(Pipeline::ForwardPrepass_MeshVS)];

		//* blob
		pipeline.CreateContent(kDirectory / "Mesh" / "Default.vs.hlsl",        GraphicsShaderType::Vertex);
		pipeline.CreateContent(kDirectory / "Mesh" / "ForwardPrepass.ps.hlsl", GraphicsShaderType::Pixel);
		pipeline.RegisterBlob();

		//* root signature
		pipeline.ReflectionRootSignature(System::GetDxDevice());

		//* pipeline
		pipeline.CreatePipeline(System::GetDxDevice(), descs_[static_cast<uint32_t>(Desc::ForwardPrepass)]);
	}

	{ //!< forward prepass [ms-ps]
		auto& pipeline = pipelines_[static_cast<uint32_t>(Pipeline::ForwardPrepass_MeshMS)];

		//* blob
		pipeline.CreateContent(kDirectory / "Mesh" / "Default.as.hlsl",        GraphicsShaderType::Amplification);
		pipeline.CreateContent(kDirectory / "Mesh" / "Default.ms.hlsl",        GraphicsShaderType::Mesh);
		pipeline.CreateContent(kDirectory / "Mesh" / "ForwardPrepass.ps.hlsl", GraphicsShaderType::Pixel);
		pipeline.RegisterBlob();

		//* root signature
		pipeline.ReflectionRootSignature(System::GetDxDevice());

		//* pipeline
		pipeline.CreatePipeline(System::GetDxDevice(), descs_[static_cast<uint32_t>(Desc::ForwardPrepass)]);
	}

	{ //!< forward transparent [vs-ps]
		auto& pipeline = pipelines_[static_cast<uint32_t>(Pipeline::ForwardTransparent_MeshVS)];

		//* blob
		pipeline.CreateContent(kDirectory / "Mesh" / "Default.vs.hlsl",            GraphicsShaderType::Vertex);
		pipeline.CreateContent(kDirectory / "Mesh" / "ForwardTransparent.ps.hlsl", GraphicsShaderType::Pixel);
		pipeline.RegisterBlob();

		//* root signature
		pipeline.ReflectionRootSignature(System::GetDxDevice());

		//* pipeline
		pipeline.CreatePipeline(System::GetDxDevice(), descs_[static_cast<uint32_t>(Desc::ForwardTransparent)]);
	}

	{ //!< forward transparent [ms-ps]
		auto& pipeline = pipelines_[static_cast<uint32_t>(Pipeline::ForwardTransparent_MeshMS)];

		//* blob
		pipeline.CreateContent(kDirectory / "Mesh" / "Default.as.hlsl",            GraphicsShaderType::Amplification);
		pipeline.CreateContent(kDirectory / "Mesh" / "Default.ms.hlsl",            GraphicsShaderType::Mesh);
		pipeline.CreateContent(kDirectory / "Mesh" / "ForwardTransparent.ps.hlsl", GraphicsShaderType::Pixel);
		pipeline.RegisterBlob();

		//* root signature
		pipeline.ReflectionRootSignature(System::GetDxDevice());

		//* pipeline
		pipeline.CreatePipeline(System::GetDxDevice(), descs_[static_cast<uint32_t>(Desc::ForwardTransparent)]);
	}

	{ //!< deferred [vs-ps]
		auto& pipeline = pipelines_[static_cast<uint32_t>(Pipeline::Deferred_MeshVS)];

		//* blob
		pipeline.CreateContent(kDirectory / "Mesh" / "Default.vs.hlsl",  GraphicsShaderType::Vertex);
		pipeline.CreateContent(kDirectory / "Mesh" / "Deferred.ps.hlsl", GraphicsShaderType::Pixel);
		pipeline.RegisterBlob();

		//* root signature
		pipeline.ReflectionRootSignature(System::GetDxDevice());

		//* pipeline
		pipeline.CreatePipeline(System::GetDxDevice(), descs_[static_cast<uint32_t>(Desc::Deferred)]);
	}

	{ //!< deferred [ms-ps]
		auto& pipeline = pipelines_[static_cast<uint32_t>(Pipeline::Deferred_MeshMS)];

		//* blob
		pipeline.CreateContent(kDirectory / "Mesh" / "Default.as.hlsl",  GraphicsShaderType::Amplification);
		pipeline.CreateContent(kDirectory / "Mesh" / "Default.ms.hlsl",  GraphicsShaderType::Mesh);
		pipeline.CreateContent(kDirectory / "Mesh" / "Deferred.ps.hlsl", GraphicsShaderType::Pixel);
		pipeline.RegisterBlob();

		//* root signature
		pipeline.ReflectionRootSignature(System::GetDxDevice());

		//* pipeline
		pipeline.CreatePipeline(System::GetDxDevice(), descs_[static_cast<uint32_t>(Desc::Deferred)]);
	}
}
