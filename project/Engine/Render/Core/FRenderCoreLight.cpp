#include "FRenderCoreLight.h"
SXAVENGER_ENGINE_USING
DXOBJECT_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* render
#include "../GBuffer/FLightingGBuffer.h"

//* engine
#include <Engine/System/System.h>

////////////////////////////////////////////////////////////////////////////////////////////
// FRenderCoreLight class methods
////////////////////////////////////////////////////////////////////////////////////////////

void FRenderCoreLight::Init() {
	CreatePipeline();
}

void FRenderCoreLight::DrawCall(const DirectXQueueContext* context, UINT instanceCount) const {
	context->GetCommandList()->DrawInstanced(3, instanceCount, 0, 0);
}

void FRenderCoreLight::SetPipeline(LightType type, const DirectXQueueContext* context, const Vector2ui& size) {
	//pipelines_[type]->SetPipeline(context->GetDxCommand(), size);
	pipelines_[type]->ReloadAndSetPipeline(context, size);
}

void FRenderCoreLight::BindGraphicsBuffer(LightType type, const DirectXQueueContext* context, const DxObject::BindBufferDesc& desc) {
	pipelines_[type]->BindGraphicsBuffer(context->GetDxCommand(), desc);
}

void FRenderCoreLight::CreatePipeline() {

	D3D12_RENDER_TARGET_BLEND_DESC blend = {};
	blend.BlendEnable           = true;
	blend.LogicOpEnable         = false;
	blend.SrcBlend              = D3D12_BLEND_ONE;
	blend.DestBlend             = D3D12_BLEND_ONE;
	blend.BlendOp               = D3D12_BLEND_OP_ADD;
	blend.SrcBlendAlpha         = D3D12_BLEND_ONE;
	blend.DestBlendAlpha        = D3D12_BLEND_ZERO;
	blend.BlendOpAlpha          = D3D12_BLEND_OP_ADD;
	blend.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	GraphicsPipelineDesc desc_front = {};
	desc_front.CreateDefaultDesc();
	desc_front.ClearElement();
	desc_front.SetDepthStencil(true, D3D12_DEPTH_WRITE_MASK_ZERO, D3D12_COMPARISON_FUNC_GREATER); //!< Depthが1.0以外に書き込み
	desc_front.SetRTVFormat(0, FLightingGBuffer::GetFormat(FLightingGBuffer::Layout::Direct));
	desc_front.SetIndependentBlendEnable(false);
	desc_front.SetBlendDesc(0, blend);

	GraphicsPipelineDesc desc_back = {};
	desc_back.CreateDefaultDesc();
	desc_back.ClearElement();
	desc_back.SetDepthStencil(true, D3D12_DEPTH_WRITE_MASK_ZERO, D3D12_COMPARISON_FUNC_EQUAL); //!< Depthが1.0に書き込み
	desc_back.SetRTVFormat(0, FLightingGBuffer::GetFormat(FLightingGBuffer::Layout::Direct));
	desc_back.SetIndependentBlendEnable(false);
	desc_back.SetBlendDesc(0, blend);

	pipelines_[LightType::Empty] = std::make_unique<CustomReflectionGraphicsPipeline>();
	pipelines_[LightType::Empty]->CreateContent(kDirectory / "LightRender2d.vs.hlsl", GraphicsShaderType::vs);
	pipelines_[LightType::Empty]->CreateContent(kDirectory / "Empty.ps.hlsl",         GraphicsShaderType::ps);
	pipelines_[LightType::Empty]->RegisterBlob();
	pipelines_[LightType::Empty]->ReflectionRootSignature(System::GetDxDevice());
	pipelines_[LightType::Empty]->CreatePipeline(System::GetDxDevice(), desc_front);

	pipelines_[LightType::Directional] = std::make_unique<CustomReflectionGraphicsPipeline>();
	pipelines_[LightType::Directional]->CreateContent(kDirectory / "LightRender2d.vs.hlsl",    GraphicsShaderType::vs);
	pipelines_[LightType::Directional]->CreateContent(kDirectory / "DirectionalLight.ps.hlsl", GraphicsShaderType::ps);
	pipelines_[LightType::Directional]->RegisterBlob();
	pipelines_[LightType::Directional]->ReflectionRootSignature(System::GetDxDevice());
	pipelines_[LightType::Directional]->CreatePipeline(System::GetDxDevice(), desc_front);

	pipelines_[LightType::Point] = std::make_unique<CustomReflectionGraphicsPipeline>();
	pipelines_[LightType::Point]->CreateContent(kDirectory / "LightRender2d.vs.hlsl", GraphicsShaderType::vs);
	pipelines_[LightType::Point]->CreateContent(kDirectory / "PointLight.ps.hlsl",    GraphicsShaderType::ps);
	pipelines_[LightType::Point]->RegisterBlob();
	pipelines_[LightType::Point]->ReflectionRootSignature(System::GetDxDevice());
	pipelines_[LightType::Point]->CreatePipeline(System::GetDxDevice(), desc_front);

	pipelines_[LightType::Spot] = std::make_unique<CustomReflectionGraphicsPipeline>();
	pipelines_[LightType::Spot]->CreateContent(kDirectory / "lightRender2d.vs.hlsl", GraphicsShaderType::vs);
	pipelines_[LightType::Spot]->CreateContent(kDirectory / "SpotLight.ps.hlsl",     GraphicsShaderType::ps);
	pipelines_[LightType::Spot]->RegisterBlob();
	pipelines_[LightType::Spot]->ReflectionRootSignature(System::GetDxDevice());
	pipelines_[LightType::Spot]->CreatePipeline(System::GetDxDevice(), desc_front);

	{
		SamplerBindDesc desc = {};
		desc.SetSamplerLinear("gBRDFSampler", SamplerMode::MODE_CLAMP);

		pipelines_[LightType::SkyLight] = std::make_unique<CustomReflectionGraphicsPipeline>();
		pipelines_[LightType::SkyLight]->CreateContent(kDirectory / "LightRender2d.vs.hlsl", GraphicsShaderType::vs);
		pipelines_[LightType::SkyLight]->CreateContent(kDirectory / "SkyLight.ps.hlsl",      GraphicsShaderType::ps);
		pipelines_[LightType::SkyLight]->RegisterBlob();
		pipelines_[LightType::SkyLight]->ReflectionRootSignature(System::GetDxDevice(), desc);
		pipelines_[LightType::SkyLight]->CreatePipeline(System::GetDxDevice(), desc_front);


		pipelines_[LightType::SkyLightEnvironment] = std::make_unique<CustomReflectionGraphicsPipeline>();
		pipelines_[LightType::SkyLightEnvironment]->CreateContent(kDirectory / "LightRender2d.vs.hlsl",       GraphicsShaderType::vs);
		pipelines_[LightType::SkyLightEnvironment]->CreateContent(kDirectory / "SkyLightEnvironment.ps.hlsl", GraphicsShaderType::ps);
		pipelines_[LightType::SkyLightEnvironment]->RegisterBlob();
		pipelines_[LightType::SkyLightEnvironment]->ReflectionRootSignature(System::GetDxDevice(), desc);
		pipelines_[LightType::SkyLightEnvironment]->CreatePipeline(System::GetDxDevice(), desc_back);

		pipelines_[LightType::SkyAtmosphereEnvironment] = std::make_unique<CustomReflectionGraphicsPipeline>();
		pipelines_[LightType::SkyAtmosphereEnvironment]->CreateContent(kDirectory / "LightRender2d.vs.hlsl",            GraphicsShaderType::vs);
		pipelines_[LightType::SkyAtmosphereEnvironment]->CreateContent(kDirectory / "SkyAtmosphereEnvironment.ps.hlsl", GraphicsShaderType::ps);
		pipelines_[LightType::SkyAtmosphereEnvironment]->RegisterBlob();
		pipelines_[LightType::SkyAtmosphereEnvironment]->ReflectionRootSignature(System::GetDxDevice(), desc);
		pipelines_[LightType::SkyAtmosphereEnvironment]->CreatePipeline(System::GetDxDevice(), desc_back);


	}
	
}
