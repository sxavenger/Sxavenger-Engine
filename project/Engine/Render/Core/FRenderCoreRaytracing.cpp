#include "FRenderCoreRaytracing.h"
_DXROBJECT_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/SxavengerSystem.h>

////////////////////////////////////////////////////////////////////////////////////////////
// FRenderCoreRaytracing class methods
////////////////////////////////////////////////////////////////////////////////////////////

void FRenderCoreRaytracing::Init() {
	CreateRaygeneration();
	CreateMiss();
	CreateHitgroup();
}

const DxrObject::ExportGroup* FRenderCoreRaytracing::GetExportGroup(RaygenerationExportType raygeneration) const {
	return &raygenerationExportGroups_[GetIndex(raygeneration)].second;
}

const DxrObject::ExportGroup* FRenderCoreRaytracing::GetExportGroup(MissExportType miss) const {
	return &missExportGroups_[GetIndex(miss)].second;
}

const DxrObject::ExportGroup* FRenderCoreRaytracing::GetExportGroup(HitgroupExportType hitgroup) const {
	return &hitgroupExportGroups_[GetIndex(hitgroup)].second;
}

void FRenderCoreRaytracing::CreateRaygeneration() {

	{ //!< Default

		auto& [blob, expt] = raygenerationExportGroups_[GetIndex(RaygenerationExportType::Default)];
		blob.Create(kDirectory_ / "Default.raygeneration.hlsl");

		expt.ExportRaygeneration(L"mainRaygeneration");
		expt.SetBlob(&blob);

	}
}

void FRenderCoreRaytracing::CreateMiss() {

	{ //!< Default
		auto& [blob, expt] = missExportGroups_[GetIndex(MissExportType::Default)];
		blob.Create(kDirectory_ / "Default.miss.hlsl");

		expt.ExportMiss(L"mainMiss");
		expt.SetBlob(&blob);
	}

}

void FRenderCoreRaytracing::CreateHitgroup() {

	{ //!< Mesh
		auto& [blob, expt] = hitgroupExportGroups_[GetIndex(HitgroupExportType::Mesh)];
		blob.Create(kDirectory_ / "hitgroup" / "Mesh.hitgroup.hlsl");

		//* hitgroup
		ExportGroup::Hitgroup hitgroup = {};
		hitgroup.type       = D3D12_HIT_GROUP_TYPE_TRIANGLES;
		hitgroup.closesthit = L"mainClosesthit";
		hitgroup.anyhit     = L"mainAnyhit";

		expt.ExportHitgroup(L"Geometry", hitgroup);
		expt.SetBlob(&blob);

		//* root signature
		LocalRootSignatureDesc desc = {};
		desc.SetSamplerLinear(DxObject::SamplerMode::MODE_WRAP, DxObject::ShaderVisibility::VISIBILITY_ALL, 0);
		desc.SetVirtualSRV(0, 10); //!< gVertices
		desc.SetVirtualSRV(1, 11); //!< gIndices
		desc.SetVirtualCBV(2, 0);  //!< gMaterial

		expt.CreateRootSignature(SxavengerSystem::GetDxDevice(), desc);

	}

}
