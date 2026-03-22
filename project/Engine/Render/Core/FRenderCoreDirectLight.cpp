#include "FRenderCoreDirectLight.h"
SXAVENGER_ENGINE_USING
DXOBJECT_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* render
#include "../Buffer/FLightAccumulationBuffer.h"

//* engine
#include <Engine/System/System.h>

////////////////////////////////////////////////////////////////////////////////////////////
// FRenderCoreDirectLight class methods
////////////////////////////////////////////////////////////////////////////////////////////

void FRenderCoreDirectLight::Init() {
	CreateDesc();
	CreatePipeline();
}

void FRenderCoreDirectLight::SetPipeline(Type type, const DirectXQueueContext* context, const Vector2ui& resolution) const {
	pipelines_[static_cast<uint8_t>(type)].SetPipeline(context->GetDxCommand(), resolution);
}

void FRenderCoreDirectLight::BindGraphicsBuffer(Type type, const DirectXQueueContext* context, const DxObject::BindBufferDesc& desc) const {
	pipelines_[static_cast<uint8_t>(type)].BindGraphicsBuffer(context->GetDxCommand(), desc);
}

void FRenderCoreDirectLight::DrawCall(const DirectXQueueContext* context, UINT instanceCount) const {
	context->GetCommandList()->DrawInstanced(3, instanceCount, 0, 0);
}

void FRenderCoreDirectLight::CreateDesc() {

	{ //!< Front

		auto& desc = descs_[static_cast<uint8_t>(Desc::Front)] = {};

		//!< blendの設定
		D3D12_RENDER_TARGET_BLEND_DESC blend = {};
		blend.BlendEnable           = true;
		blend.LogicOpEnable         = false;
		blend.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
		blend.SrcBlend              = D3D12_BLEND_ONE;
		blend.DestBlend             = D3D12_BLEND_ONE;
		blend.BlendOp               = D3D12_BLEND_OP_ADD;
		blend.SrcBlendAlpha         = D3D12_BLEND_ONE;
		blend.DestBlendAlpha        = D3D12_BLEND_ONE;
		blend.BlendOpAlpha          = D3D12_BLEND_OP_MAX;

		//!< descの設定
		desc.SetPrimitive(PrimitiveType::TrianglList);

		desc.SetRasterizer(D3D12_CULL_MODE_BACK, D3D12_FILL_MODE_SOLID);
		desc.SetDepthStencil(true, D3D12_DEPTH_WRITE_MASK_ZERO, D3D12_COMPARISON_FUNC_GREATER); //!< Depthが1.0以外に書き込み

		desc.SetBlendDesc(0, blend);
		desc.SetIndependentBlendEnable(false);

		desc.SetRTVFormat(0, FLightAccumulationBuffer::GetFormat(FLightAccumulationBuffer::Layout::Direct));
		desc.SetDSVFormat(kDefaultDepthFormat);
	}

	{ //!< Back

		auto& desc = descs_[static_cast<uint8_t>(Desc::Back)] = {};

		//!< blendの設定
		D3D12_RENDER_TARGET_BLEND_DESC blend = {};
		blend.BlendEnable           = true;
		blend.LogicOpEnable         = false;
		blend.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
		blend.SrcBlend              = D3D12_BLEND_ONE;
		blend.DestBlend             = D3D12_BLEND_ONE;
		blend.BlendOp               = D3D12_BLEND_OP_ADD;
		blend.SrcBlendAlpha         = D3D12_BLEND_ONE;
		blend.DestBlendAlpha        = D3D12_BLEND_ONE;
		blend.BlendOpAlpha          = D3D12_BLEND_OP_MAX;

		//!< descの設定
		desc.SetPrimitive(PrimitiveType::TrianglList);

		desc.SetRasterizer(D3D12_CULL_MODE_BACK, D3D12_FILL_MODE_SOLID);
		desc.SetDepthStencil(true, D3D12_DEPTH_WRITE_MASK_ZERO, D3D12_COMPARISON_FUNC_EQUAL); //!< Depthが1.0に書き込み

		desc.SetBlendDesc(0, blend);
		desc.SetIndependentBlendEnable(false);

		desc.SetRTVFormat(0, FLightAccumulationBuffer::GetFormat(FLightAccumulationBuffer::Layout::Direct));
		desc.SetDSVFormat(kDefaultDepthFormat);
	}

	{ //!< Aerial

		auto& desc = descs_[static_cast<uint8_t>(Desc::Aerial)] = {};

		//!< blendの設定
		D3D12_RENDER_TARGET_BLEND_DESC blend = {};
		blend.BlendEnable           = true;
		blend.LogicOpEnable         = false;
		blend.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
		blend.SrcBlend              = D3D12_BLEND_ONE;
		blend.DestBlend             = D3D12_BLEND_INV_SRC_ALPHA;
		blend.BlendOp               = D3D12_BLEND_OP_ADD;
		blend.SrcBlendAlpha         = D3D12_BLEND_ONE;
		blend.DestBlendAlpha        = D3D12_BLEND_ONE;
		blend.BlendOpAlpha          = D3D12_BLEND_OP_MAX;

		//!< descの設定
		desc.SetPrimitive(PrimitiveType::TrianglList);

		desc.SetRasterizer(D3D12_CULL_MODE_BACK, D3D12_FILL_MODE_SOLID);
		desc.SetDepthStencil(true, D3D12_DEPTH_WRITE_MASK_ZERO, D3D12_COMPARISON_FUNC_GREATER); //!< Depthが1.0以外に書き込み

		desc.SetBlendDesc(0, blend);
		desc.SetIndependentBlendEnable(false);

		desc.SetRTVFormat(0, FLightAccumulationBuffer::GetFormat(FLightAccumulationBuffer::Layout::Direct));
		desc.SetDSVFormat(kDefaultDepthFormat);
	}

}

void FRenderCoreDirectLight::CreatePipeline() {

	//!< Unlit
	CreatePipeline(Type::Unlit, Desc::Front, "LightRender2d.vs.hlsl", "Unlit.ps.hlsl");

	//!< Punctual light
	CreatePipeline(Type::Directional, Desc::Front, "LightRender2d.vs.hlsl", "DirectionalLight.ps.hlsl");
	CreatePipeline(Type::Point, Desc::Front, "LightRender2d.vs.hlsl", "PointLight.ps.hlsl");
	CreatePipeline(Type::Spot, Desc::Front, "LightRender2d.vs.hlsl", "SpotLight.ps.hlsl");

	//!< Rect light
	CreatePipeline(Type::Rect, Desc::Front, "LightRender2d.vs.hlsl", "RectLight.ps.hlsl");

	//!< Sky light
	CreatePipeline(Type::SkyLight, Desc::Front, "LightRender2d.vs.hlsl", "SkyLight.ps.hlsl");
	CreatePipeline(Type::SkyLightEnvironment, Desc::Back, "LightRender2d.vs.hlsl", "SkyLightEnvironment.ps.hlsl");
	CreatePipeline(Type::SkyAtmosphereEnvironment, Desc::Back, "LightRender2d.vs.hlsl", "SkyAtmosphereEnvironment.ps.hlsl");
	CreatePipeline(Type::SkyAtmosphereAerial, Desc::Aerial, "LightRender2d.vs.hlsl", "SkyAtmosphereAerial.ps.hlsl");

}

void FRenderCoreDirectLight::CreatePipeline(
	Type type, Desc desc,
	std::filesystem::path vertex, std::filesystem::path pixel) {

	auto& pipeline = pipelines_[static_cast<uint8_t>(type)];

	//!< blobの設定
	pipeline.CreateContent(kDirectory / vertex, DxObject::GraphicsShaderType::Vertex);
	pipeline.CreateContent(kDirectory / pixel, DxObject::GraphicsShaderType::Pixel);
	pipeline.RegisterBlob();

	SamplerBindDesc sampler = {};
	sampler.SetSamplerLinear("gBRDFSampler",   SamplerMode::Clamp);
	sampler.SetSamplerLinear("gAerialSampler", SamplerMode::Clamp);

	//!< pipelineの作成
	pipeline.ReflectionRootSignature(System::GetDxDevice(), sampler);
	pipeline.CreatePipeline(System::GetDxDevice(), descs_[static_cast<uint8_t>(desc)]);

}
