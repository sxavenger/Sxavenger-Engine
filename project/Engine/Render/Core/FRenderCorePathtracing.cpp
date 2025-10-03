#include "FRenderCorePathtracing.h"
_DXROBJECT_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/SxavengerSystem.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Reservoir structure methods
////////////////////////////////////////////////////////////////////////////////////////////

void FRenderCorePathtracing::Config::Update() {
	isResetMoment = false;
}

////////////////////////////////////////////////////////////////////////////////////////////
// FRenderCorePathtracing class methods
////////////////////////////////////////////////////////////////////////////////////////////

void FRenderCorePathtracing::Init() {
	CreateRaygeneration();
	CreateMiss();
	CreateHitgroup();

	CreateDenoiser();
	CreateContext();
}

const DxrObject::ExportGroup* FRenderCorePathtracing::GetExportGroup(RaygenerationExportType raygeneration) const {
	return &raygenerationExportGroups_[GetIndex(raygeneration)].second;
}

const DxrObject::ExportGroup* FRenderCorePathtracing::GetExportGroup(MissExportType miss) const {
	return &missExportGroups_[GetIndex(miss)].second;
}

const DxrObject::ExportGroup* FRenderCorePathtracing::GetExportGroup(HitgroupExportType hitgroup) const {
	return &hitgroupExportGroups_[GetIndex(hitgroup)].second;
}

void FRenderCorePathtracing::SetDenoiserPipeline(DenoiserType type, const DirectXQueueContext* context) {
	denoisers_[GetIndex(type)]->SetPipeline(context->GetDxCommand());
}

void FRenderCorePathtracing::BindDenoiserBuffer(DenoiserType type, const DirectXQueueContext* context, const DxObject::BindBufferDesc& desc) {
	denoisers_[GetIndex(type)]->BindComputeBuffer(context->GetDxCommand(), desc);
}

void FRenderCorePathtracing::DispatchDenoiser(const DirectXQueueContext* context, const Vector2ui& size) {
	context->GetCommandList()->Dispatch(DxObject::RoundUp(size.x, 16), DxObject::RoundUp(size.y, 16), 1);
}

void FRenderCorePathtracing::UpdateShaderTable(const DxrObject::TopLevelAS* topLevelAS) {
	context_->UpdateShaderTable(SxavengerSystem::GetDxDevice(), topLevelAS);
}

void FRenderCorePathtracing::CreateRaygeneration() {

	{ //!< Default

		auto& [blob, expt] = raygenerationExportGroups_[GetIndex(RaygenerationExportType::Default)];
		blob.Create(kDirectory_ / "Default.raygeneration.hlsl");

		expt.ExportRaygeneration(L"mainRaygeneration");
		expt.SetBlob(&blob);

	}
}

void FRenderCorePathtracing::CreateMiss() {

	{ //!< Default
		auto& [blob, expt] = missExportGroups_[GetIndex(MissExportType::Default)];
		blob.Create(kDirectory_ / "Default.miss.hlsl");

		expt.ExportMiss(L"mainMiss");
		expt.SetBlob(&blob);
	}

}

void FRenderCorePathtracing::CreateHitgroup() {

	{ //!< Mesh
		auto& [blob, expt] = hitgroupExportGroups_[GetIndex(HitgroupExportType::Mesh)];
		blob.Create(kDirectory_ / "hitgroup" / "Mesh.hitgroup.hlsl");

		//* hitgroup
		ExportGroup::Hitgroup hitgroup = {};
		hitgroup.type       = D3D12_HIT_GROUP_TYPE_TRIANGLES;
		hitgroup.closesthit = L"mainClosesthit";
		hitgroup.anyhit     = L"mainAnyhit";

		expt.ExportHitgroup(L"Mesh", hitgroup);
		expt.SetBlob(&blob);

		//* root signature
		LocalRootSignatureDesc desc = {};
		desc.SetSamplerLinear(DxObject::SamplerMode::MODE_WRAP, DxObject::ShaderVisibility::VISIBILITY_ALL, 0);
		desc.SetVirtualSRV(0, 10); //!< gVertices
		desc.SetVirtualSRV(1, 11); //!< gIndices
		desc.SetVirtualSRV(2, 0);  //!< gMaterial

		expt.CreateRootSignature(SxavengerSystem::GetDxDevice(), desc);

	}


	{ //!< Emissive

		auto& [blob, expt] = hitgroupExportGroups_[GetIndex(HitgroupExportType::Emissive)];
		blob.Create(kDirectory_ / "hitgroup" / "Emissive.hitgroup.hlsl");

		//* hitgroup
		ExportGroup::Hitgroup hitgroup = {};
		hitgroup.type       = D3D12_HIT_GROUP_TYPE_TRIANGLES;
		hitgroup.closesthit = L"mainEmissiveClosesthit";
		hitgroup.anyhit     = L"mainEmissiveAnyhit";

		expt.ExportHitgroup(L"Emissive", hitgroup);
		expt.SetBlob(&blob);

		//* root signature
		LocalRootSignatureDesc desc = {};
		desc.SetSamplerLinear(DxObject::SamplerMode::MODE_WRAP, DxObject::ShaderVisibility::VISIBILITY_ALL, 0);
		desc.SetVirtualSRV(0, 10); //!< gVertices
		desc.SetVirtualSRV(1, 11); //!< gIndices
		desc.SetVirtualSRV(2, 0);  //!< gMaterial

		expt.CreateRootSignature(SxavengerSystem::GetDxDevice(), desc);
	}
}

void FRenderCorePathtracing::CreateDenoiser() {

	{
		//* EdgeStoping
		auto& denoiser = denoisers_[GetIndex(DenoiserType::EdgeStopping)];
		denoiser = std::make_unique<CustomReflectionComputePipeline>();
		denoiser->CreateContent(kPackagesShaderDirectory / "render" / "denoiser" / "EdgeStopping.cs.hlsl");
		denoiser->RegisterBlob();
		denoiser->ReflectionPipeline(SxavengerSystem::GetDxDevice());
	}

}

void FRenderCorePathtracing::CreateContext() {

	// path tracing stateobjectの初期化
	context_ = std::make_unique<DxrObject::StateObjectContext>();

	{ //!< global root signatureの生成
		DxrObject::GlobalRootSignatureDesc desc = {};
		//* lighting textures
		desc.SetHandleUAV(0, 0, 1); //!< gIndirect
		desc.SetHandleUAV(1, 1, 1); //!< gIndirectSampleCount

		//* scene
		desc.SetVirtualSRV(2, 0, 1); //!< gScene

		//* deferred textures
		desc.SetVirtualCBV(3, 0, 1); //!< gDeferredBufferIndex

		//* camera
		desc.SetVirtualCBV(4, 1, 1); //!< gCamera

		//* config
		desc.Set32bitConstants(5, DxObject::ShaderVisibility::VISIBILITY_ALL, 3, 2, 1); //!< Config

		//* light
		// Directional Light
		desc.SetVirtualCBV(6, 0, 2); //!< gDirectionalLightCount
		desc.SetVirtualSRV(7, 0, 2); //!< gDirectionalLightTransforms
		desc.SetVirtualSRV(8, 1, 2); //!< gDirectionalLights

		// Point Light
		desc.SetVirtualCBV(9, 1, 2);  //!< gPointLightCount
		desc.SetVirtualSRV(10, 2, 2); //!< gPointLightTransforms
		desc.SetVirtualSRV(11, 3, 2); //!< gPointLights

		// Spot Light
		desc.SetVirtualCBV(12, 2, 2); //!< gSpotLightCount
		desc.SetVirtualSRV(13, 4, 2); //!< gSpotLightTransforms
		desc.SetVirtualSRV(14, 5, 2); //!< gSpotLights

		// Sky Light
		desc.SetVirtualCBV(15, 3, 2);                                                                 //!< gSkyLight
		desc.SetSamplerLinear(DxObject::MODE_WRAP, DxObject::ShaderVisibility::VISIBILITY_ALL, 0, 2); //!< gSampler

		context_->CreateRootSignature(SxavengerSystem::GetDxDevice(), desc);
	}

	{ //!< state objectの生成
		DxrObject::StateObjectDesc desc = {};
		desc.AddExport(GetExportGroup(FRenderCorePathtracing::RaygenerationExportType::Default));
		desc.AddExport(GetExportGroup(FRenderCorePathtracing::MissExportType::Default));
		desc.AddExport(GetExportGroup(FRenderCorePathtracing::HitgroupExportType::Mesh));
		desc.AddExport(GetExportGroup(FRenderCorePathtracing::HitgroupExportType::Emissive));

		// 仮paraemter
		desc.SetAttributeStride(sizeof(float) * 2);
		desc.SetPayloadStride(sizeof(float) * 5);
		desc.SetMaxRecursionDepth(3);

		context_->CreateStateObject(SxavengerSystem::GetDxDevice(), std::move(desc));
	}

}
