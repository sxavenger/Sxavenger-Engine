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

void FRenderCorePathtracing::Reservoir::Init() {
	sampleCount      = 512;
	frameSampleCount = 2;
	currentFrame     = 0;
}

void FRenderCorePathtracing::Reservoir::IncrimentFrame() {
	currentFrame++;
}

void FRenderCorePathtracing::Reservoir::ResetFrame() {
	currentFrame = 0;
}

uint32_t FRenderCorePathtracing::Reservoir::GetCurrentSampleCount() const {
	return std::min(currentFrame * frameSampleCount, sampleCount);
}

////////////////////////////////////////////////////////////////////////////////////////////
// FRenderCorePathtracing class methods
////////////////////////////////////////////////////////////////////////////////////////////

void FRenderCorePathtracing::Init() {
	CreateRaygeneration();
	CreateMiss();
	CreateHitgroup();

	CreateDenoiser();
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
		denoiser->CreateAsset(kPackagesShaderDirectory / "render" / "denoiser" / "EdgeStopping.cs.hlsl");
		denoiser->RegisterBlob();
		denoiser->ReflectionPipeline(SxavengerSystem::GetDxDevice());
	}

}
