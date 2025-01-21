#include "FSceneRenderer.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* render
#include "../FRenderCore.h"

//* engine
#include <Engine/System/SxavengerSystem.h>

////////////////////////////////////////////////////////////////////////////////////////////
// FSceneRenderer class methods
////////////////////////////////////////////////////////////////////////////////////////////

void FSceneRenderer::Render(const DirectXThreadContext* context) {
	if (!CheckRender()) { //!< scene or view is not set
		return;
	}

	//* ベースパス
	RenderOpaqueGeometries(context);

	//* 照明と影の処理
	ProcessLighting(context);

	/*SetupRaytracing(context);
	ProcessRaytracingReflection(context);
	context->TransitionAllocator();*/

	//* 合成処理
	// todo:
	//HACKProcessSSAO(context);

	//* 透明パス
	RenderTransparentGeometries(context);

	//* ポストプロセス
	// todo:
}

bool FSceneRenderer::CheckRender() const {
	if (textures_ == nullptr || scene_ == nullptr || camera_ == nullptr) {
		return false;
	}

	return true;
}

const D3D12_GPU_DESCRIPTOR_HANDLE& FSceneRenderer::GetDebugTexture() const {
	return textures_->GetGBuffer(FSceneTextures::GBufferLayout::Result)->GetGPUHandleSRV();
}

void FSceneRenderer::HACKProcessSSAO(const DirectXThreadContext* context) {
	if (p_ == nullptr) {
		p_ = std::make_unique<DxObject::ReflectionComputePipelineState>();
		p_->CreateBlob("packages/shaders/render/process/ssao.cs.hlsl");
		p_->ReflectionPipeline(SxavengerSystem::GetDxDevice());
	}

	p_->SetPipeline(context->GetDxCommand());

	DxObject::BindBufferDesc desc = {};
	desc.SetHandle("gDepth", textures_->GetDepth()->GetRasterizerGPUHandleSRV());
	desc.SetHandle("gNormal", textures_->GetGBuffer(FSceneTextures::GBufferLayout::Normal)->GetGPUHandleSRV());
	desc.SetHandle("gPosition", textures_->GetGBuffer(FSceneTextures::GBufferLayout::Position)->GetGPUHandleSRV());
	desc.SetHandle("gOutput", textures_->GetGBuffer(FSceneTextures::GBufferLayout::Result)->GetGPUHandleUAV());
	desc.SetAddress("gParameter", textures_->GetParameter());

	p_->BindComputeBuffer(context->GetDxCommand(), desc);
	p_->Dispatch(context->GetDxCommand(), { DxObject::RoundUp(textures_->GetSize().x, 16), DxObject::RoundUp(textures_->GetSize().y, 16), 1 });
}

void FSceneRenderer::RenderOpaqueGeometries(const DirectXThreadContext* context) {

	const auto& geometries = scene_->GetGeometries();

	textures_->BeginBasePass(context);

	AGeometryActor::RendererContext rendererContext = {};
	rendererContext.context = context;
	rendererContext.size    = textures_->GetSize();

	rendererContext.parameter.SetAddress("gCamera", camera_->GetGPUVirtualAddress());

	// 不透明なジオメトリを描画
	for (auto geometry : geometries) {
		if (geometry->CheckVisibility(AGeometryActor::Transparency::Opaque)) {
			geometry->RenderOpaque(rendererContext);
		}
	}

	textures_->EndBasePass(context);
}

void FSceneRenderer::ProcessLighting(const DirectXThreadContext* context) {

	const auto& lights = scene_->GetLights();

	textures_->BeginLightingPass(context);

	ALightActor::RendererContext rendererContext = {};
	rendererContext.context        = context;
	rendererContext.size           = textures_->GetSize();

	rendererContext.parameter.SetAddress("gCamera",  camera_->GetGPUVirtualAddress());
	rendererContext.parameter.SetHandle("gDepth",    textures_->GetDepth()->GetRasterizerGPUHandleSRV());
	rendererContext.parameter.SetHandle("gAlbedo",   textures_->GetGBuffer(FSceneTextures::GBufferLayout::Albedo_AO)->GetGPUHandleUAV());
	rendererContext.parameter.SetHandle("gNormal",   textures_->GetGBuffer(FSceneTextures::GBufferLayout::Normal)->GetGPUHandleUAV());
	rendererContext.parameter.SetHandle("gPosition", textures_->GetGBuffer(FSceneTextures::GBufferLayout::Position)->GetGPUHandleUAV());

	// light empty描画
	RenderEmptyLight(rendererContext);

	// 照明の処理
	for (auto light : lights) {
		if (light->IsActive()) {
			light->Render(rendererContext);
		}
	}

	textures_->EndLightingPass(context);
}

void FSceneRenderer::RenderTransparentGeometries(const DirectXThreadContext* context) {
	context;

	const auto& geometries = scene_->GetGeometries();

	AGeometryActor::RendererContext rendererContext = {};
	rendererContext.context = context;
	rendererContext.size    = textures_->GetSize();

	rendererContext.parameter.SetAddress("gCamera", camera_->GetGPUVirtualAddress());

	// 半透明なジオメトリを描画
	for (auto geometry : geometries) {
		if (geometry->CheckVisibility(AGeometryActor::Transparency::Transparent)) {
			geometry->RenderTransparent(rendererContext);
		}
	};
}

void FSceneRenderer::RenderEmptyLight(const ALightActor::RendererContext& context) {

	FRenderCore::GetInstance()->GetLight()->SetPipeline(
		FRenderCoreLight::LightType::Empty, context.context, context.size
	);

	FRenderCore::GetInstance()->GetLight()->BindIABuffer(context.context);

	FRenderCore::GetInstance()->GetLight()->BindGraphicsBuffer(
		FRenderCoreLight::LightType::Empty, context.context, context.parameter
	);

	FRenderCore::GetInstance()->GetLight()->DrawCall(context.context);
}

//void FSceneRenderer::SetupRaytracing(const DirectXThreadContext* context) {
//	if (!config_.isUseRaytracing) {
//		return;
//	}
//
//	scene_->SetupTopLevelAS(context);
//
//	DxrObject::StateObjectDesc desc = {};
//
//	for (const auto& instance : scene_->GetTopLevelAS().GetInstances()) {
//		desc.SetExport(instance.expt);
//	}
//
//	desc.SetExport(&FRenderCore::GetInstance()->GetRaytracing()->GetRaygenerationExport(FRenderCoreRaytracing::RaygenerationExportType::Default));
//	desc.SetExport(&FRenderCore::GetInstance()->GetRaytracing()->GetMissExport(FRenderCoreRaytracing::MissExportType::Default));
//
//	desc.SetMaxRecursionDepth(1);
//	desc.SetAttributeStride(sizeof(Vector2f));
//	desc.SetPayloadStride(sizeof(Vector3f));
//
//	DxrObject::GlobalRootSignatureDesc rootDesc = {};
//	rootDesc.SetVirtualSRV(0, 10); //!< gScene
//	rootDesc.SetHandleUAV(1, 10);   //!< gReflection
//
//	stateObjectContext_.CreateRootSignature(SxavengerSystem::GetDxDevice(), rootDesc);
//
//	stateObjectContext_.CreateStateObject(SxavengerSystem::GetDxDevice(), desc);
//
//	DxrObject::WriteBindBufferDesc raygeneration = {};
//	raygeneration.SetHandle(0, textures_->GetDepth()->GetRasterizerGPUHandleSRV());                                  //!< gDepth
//	raygeneration.SetHandle(1, textures_->GetGBuffer(FSceneTextures::GBufferLayout::Normal)->GetGPUHandleSRV());     //!< gNormal
//	raygeneration.SetHandle(2, textures_->GetGBuffer(FSceneTextures::GBufferLayout::Position)->GetGPUHandleSRV());   //!< gPosition
//	raygeneration.SetAddress(3, camera_->GetGPUVirtualAddress());                                                    //!< gCamera
//
//	stateObjectContext_.UpdateShaderTable(
//		SxavengerSystem::GetDxDevice(),
//		&scene_->GetTopLevelAS(),
//		&raygeneration, nullptr
//	);
//}
//
//void FSceneRenderer::ProcessRaytracingReflection(const DirectXThreadContext* context) {
//	if (!config_.isUseRaytracing) {
//		return;
//	}
//
//	stateObjectContext_.SetStateObject(context->GetDxCommand());
//
//	context->GetDxCommand()->GetCommandList()->SetComputeRootShaderResourceView(0, scene_->GetTopLevelAS().GetGPUVirtualAddress());
//	context->GetDxCommand()->GetCommandList()->SetComputeRootDescriptorTable(1, textures_->GetGBuffer(FSceneTextures::GBufferLayout::Reflection)->GetGPUHandleUAV());
//
//	stateObjectContext_.DispatchRays(context->GetDxCommand(), GetSize());
//	
//	D3D12_RESOURCE_BARRIER barrier = textures_->GetGBuffer(FSceneTextures::GBufferLayout::Lighting)->TransitionBeginUnordered();
//	context->GetDxCommand()->GetCommandList()->ResourceBarrier(1, &barrier);
//
//	p_.SetPipeline(context->GetDxCommand());
//
//	DxObject::BindBufferDesc desc = {};
//	desc.SetHandle("gSrc",     textures_->GetGBuffer(FSceneTextures::GBufferLayout::Reflection)->GetGPUHandleSRV());
//	desc.SetHandle("gDst",     textures_->GetGBuffer(FSceneTextures::GBufferLayout::Lighting)->GetGPUHandleUAV());
//	desc.SetAddress("gConfig", textures_->GetParameter());
//
//	p_.BindComputeBuffer(context->GetDxCommand(), desc);
//
//	p_.Dispatch(
//		context->GetDxCommand(),
//		{ DxObject::RoundUp(textures_->GetSize().x, 16), DxObject::RoundUp(textures_->GetSize().y, 16), 1 }
//	);
//
//	barrier = textures_->GetGBuffer(FSceneTextures::GBufferLayout::Lighting)->TransitionEndUnordered();
//	context->GetDxCommand()->GetCommandList()->ResourceBarrier(1, &barrier);
//}
