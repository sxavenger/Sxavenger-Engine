#include "FRenderCore.h"
_DXOBJECT_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/Config/SxavengerDirectory.h>
#include <Engine/System/SxavengerSystem.h>

//=========================================================================================
// static variables
//=========================================================================================

const std::filesystem::path FRenderCore::kDirectory_ = kPackagesShaderDirectory / "render";

////////////////////////////////////////////////////////////////////////////////////////////
// FRenderCore class methods
////////////////////////////////////////////////////////////////////////////////////////////

void FRenderCore::Init() {
	CreateDefaultDesc();
	CreateDeferred();
	CreateForward();
}

void FRenderCore::SetPipeline(RenderType type, VertexStage vs, PixelStage ps, const DirectXThreadContext* context, const Vector2ui& size) {
	graphicsPipelines_[type][vs][ps]->ReloadAndSetPipeline(context, size);
}

void FRenderCore::BindGraphicsBuffer(RenderType type, VertexStage vs, PixelStage ps, const DirectXThreadContext* context, const DxObject::BindBufferDesc& desc) {
	graphicsPipelines_[type][vs][ps]->BindGraphicsBuffer(context->GetDxCommand(), desc);
}

void FRenderCore::CreateDefaultDesc() {

	//* deffered 
	defferedDefaultDesc_ = {};
	defferedDefaultDesc_.CreateDefaultDesc();

	defferedDefaultDesc_.rtvFormats.clear();
	//!< Geometry
	defferedDefaultDesc_.SetRTVFormat(DXGI_FORMAT_R8G8B8A8_UNORM);
	defferedDefaultDesc_.SetRTVFormat(DXGI_FORMAT_R8G8B8A8_UNORM);
	defferedDefaultDesc_.SetRTVFormat(DXGI_FORMAT_R32G32B32A32_FLOAT);
	defferedDefaultDesc_.SetRTVFormat(DXGI_FORMAT_R32G32B32A32_FLOAT);

	//* forward
	forwardDefaultDesc_ = {};
	forwardDefaultDesc_.CreateDefaultDesc();

}

void FRenderCore::CreateDeferred() {

	auto& deferred = graphicsPipelines_[RenderType::Deffered];

	{
		auto& pipeline = deferred[VertexStage::DefaultVS][PixelStage::Albedo];
		pipeline = std::make_unique<CustomReflectionGraphicsPipeline>();
		pipeline->CreateAsset(kDirectory_ / "geometry/default.vs.hlsl", GraphicsShaderType::vs);
		pipeline->CreateAsset(kDirectory_ / "geometry/albedoDeferred.ps.hlsl",  GraphicsShaderType::ps);
		pipeline->RegisterBlob();

		pipeline->ReflectionRootSignature(SxavengerSystem::GetDxDevice());
		pipeline->CreatePipeline(SxavengerSystem::GetDxDevice(), defferedDefaultDesc_);
	}
}

void FRenderCore::CreateForward() {

	auto& forward = graphicsPipelines_[RenderType::Forward];

	{
		auto& pipeline = forward[VertexStage::DefaultVS][PixelStage::Albedo];
		pipeline = std::make_unique<CustomReflectionGraphicsPipeline>();
		pipeline->CreateAsset(kDirectory_ / "geometry/default.vs.hlsl", GraphicsShaderType::vs);
		pipeline->CreateAsset(kDirectory_ / "geometry/albedo.ps.hlsl",  GraphicsShaderType::ps);
		pipeline->RegisterBlob();

		pipeline->ReflectionRootSignature(SxavengerSystem::GetDxDevice());
		pipeline->CreatePipeline(SxavengerSystem::GetDxDevice(), forwardDefaultDesc_);
	}
}
