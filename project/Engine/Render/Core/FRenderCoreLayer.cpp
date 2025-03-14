#include "FRenderCoreLayer.h"
_DXOBJECT_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* render
#include "../FRenderTargetTextures.h"

//* engine
#include <Engine/System/Config/SxavengerConfig.h>

//=========================================================================================
// public methods
//=========================================================================================

const std::filesystem::path FRenderCoreLayer::kDirectory_ = kPackagesShaderDirectory / "render/canvas";

////////////////////////////////////////////////////////////////////////////////////////////
// FRenderCoreLight class methods
////////////////////////////////////////////////////////////////////////////////////////////

void FRenderCoreLayer::Init() {
	CreateDesc();
	CreatePipeline();
}

void FRenderCoreLayer::SetPipeline(PipelineType type, const DirectXThreadContext* context, const Vector2ui& size) const {
	pipelines_[static_cast<uint8_t>(type)]->SetPipeline(context->GetDxCommand(), size);
}

void FRenderCoreLayer::BindGraphicsBuffer(PipelineType type, const DirectXThreadContext* context, const DxObject::BindBufferDesc& desc) const {
	pipelines_[static_cast<uint8_t>(type)]->BindGraphicsBuffer(context->GetDxCommand(), desc);
}

void FRenderCoreLayer::CreateDesc() {

	desc_ = {};
	desc_.CreateDefaultDesc();

	desc_.ClearElement();
	desc_.SetElement("POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT);
	desc_.SetElement("TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT);
	desc_.SetElement("COLOR", 0,    DXGI_FORMAT_R32G32B32A32_FLOAT);

	desc_.SetRTVFormat(0, FRenderTargetTextures::GetFormat(FRenderTargetTextures::GBufferLayout::UI));

	desc_.SetBlendMode(0, BlendMode::kBlendModeNormalSrc);

	/*D3D12_RENDER_TARGET_BLEND_DESC desc = {};
	desc.BlendEnable           = true;
	desc.SrcBlend              = D3D12_BLEND_SRC_ALPHA;
	desc.BlendOp               = D3D12_BLEND_OP_ADD;
	desc.DestBlend             = D3D12_BLEND_INV_SRC_ALPHA;
	desc.SrcBlendAlpha         = D3D12_BLEND_ONE;
	desc.BlendOpAlpha          = D3D12_BLEND_OP_MAX;
	desc.DestBlendAlpha        = D3D12_BLEND_ONE;
	desc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	desc_.SetBlendDesc(0, desc);*/

	desc_.SetRasterizer(D3D12_CULL_MODE_NONE, D3D12_FILL_MODE_SOLID);

}

void FRenderCoreLayer::CreatePipeline() {

	{
		auto& pipeline = pipelines_[static_cast<uint8_t>(PipelineType::Sprite)];
		pipeline = std::make_unique<CustomReflectionGraphicsPipeline>();
		pipeline->CreateAsset(kDirectory_ / "sprite.vs.hlsl", GraphicsShaderType::vs);
		pipeline->CreateAsset(kDirectory_ / "sprite.ps.hlsl", GraphicsShaderType::ps);
		pipeline->RegisterBlob();

		pipeline->ReflectionRootSignature(SxavengerSystem::GetDxDevice());
		pipeline->CreatePipeline(SxavengerSystem::GetDxDevice(), desc_);
	}
}
