#include "FRenderCoreGeometry.h"
_DXOBJECT_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* render
#include "../FRenderTargetTextures.h"

//* engine
#include <Engine/System/Config/SxavengerConfig.h>

//=========================================================================================
// static variables
//=========================================================================================

const std::filesystem::path FRenderCoreGeometry::kDirectory_ = kPackagesShaderDirectory / "render/geometry/mesh";

////////////////////////////////////////////////////////////////////////////////////////////
// FRenderCoreGeometry class methods
////////////////////////////////////////////////////////////////////////////////////////////

void FRenderCoreGeometry::Init() {
	CreateDesc();
	CreateDeferred();
	CreateForward();
}

void FRenderCoreGeometry::SetPipeline(
	RenderType type, VertexStage vs, PixelStage ps,
	const DirectXThreadContext* context, const Vector2ui& size) {
	graphicsPipelines_[type][vs][ps]->SetPipeline(context->GetDxCommand(), size);
}

void FRenderCoreGeometry::BindGraphicsBuffer(
	RenderType type, VertexStage vs, PixelStage ps,
	const DirectXThreadContext* context, const DxObject::BindBufferDesc& desc) {
	graphicsPipelines_[type][vs][ps]->BindGraphicsBuffer(context->GetDxCommand(), desc);
}

void FRenderCoreGeometry::CreateDesc() {

	//* deffered 
	defferedDesc_ = {};
	defferedDesc_.CreateDefaultDesc();

	defferedDesc_.rtvFormats.clear();
	//!< Geometry
	defferedDesc_.SetRTVFormat(FRenderTargetTextures::GetFormat(FRenderTargetTextures::GBufferLayout::Normal));
	defferedDesc_.SetRTVFormat(FRenderTargetTextures::GetFormat(FRenderTargetTextures::GBufferLayout::MaterialARM));
	defferedDesc_.SetRTVFormat(FRenderTargetTextures::GetFormat(FRenderTargetTextures::GBufferLayout::Albedo));
	defferedDesc_.SetRTVFormat(FRenderTargetTextures::GetFormat(FRenderTargetTextures::GBufferLayout::Position));

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

	defferedDesc_.SetBlendDesc(0, blend);
	defferedDesc_.SetIndependentBlendEnable(false);

	//* forward
	forwardDesc_ = {};
	forwardDesc_.CreateDefaultDesc();
	forwardDesc_.SetRTVFormat(0, DXGI_FORMAT_R32G32B32A32_FLOAT);
}

void FRenderCoreGeometry::CreateDeferred() {

	auto& deferred = graphicsPipelines_[RenderType::Deffered];

	{
		auto& pipeline = deferred[VertexStage::DefaultVS][PixelStage::Albedo];
		pipeline = std::make_unique<CustomReflectionGraphicsPipeline>();
		pipeline->CreateAsset(kDirectory_ / "Default.vs.hlsl",        GraphicsShaderType::vs);
		pipeline->CreateAsset(kDirectory_ / "AlbedoDeferred.ps.hlsl", GraphicsShaderType::ps);
		pipeline->RegisterBlob();

		pipeline->ReflectionRootSignature(SxavengerSystem::GetDxDevice());
		pipeline->CreatePipeline(SxavengerSystem::GetDxDevice(), defferedDesc_);
	}
}

void FRenderCoreGeometry::CreateForward() {
	
	auto& forward = graphicsPipelines_[RenderType::Forward];

	{
		auto& pipeline = forward[VertexStage::DefaultVS][PixelStage::Albedo];
		pipeline = std::make_unique<CustomReflectionGraphicsPipeline>();
		pipeline->CreateAsset(kDirectory_ / "Default.vs.hlsl",        GraphicsShaderType::vs);
		pipeline->CreateAsset(kDirectory_ / "AlbedoForward.ps.hlsl",  GraphicsShaderType::ps);
		pipeline->RegisterBlob();

		pipeline->ReflectionRootSignature(SxavengerSystem::GetDxDevice());
		pipeline->CreatePipeline(SxavengerSystem::GetDxDevice(), forwardDesc_);
	}
}
