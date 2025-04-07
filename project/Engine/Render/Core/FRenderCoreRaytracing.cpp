#include "FRenderCoreRaytracing.h"
_DXROBJECT_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/Config/SxavengerConfig.h>
#include <Engine/System/SxavengerSystem.h>

////////////////////////////////////////////////////////////////////////////////////////////
// FRenderCoreRaytracing class methods
////////////////////////////////////////////////////////////////////////////////////////////

void FRenderCoreRaytracing::Init() {
	CreateRaygeneration();
	CreateMiss();
	CreateHitgroup();
}

void FRenderCoreRaytracing::CreateRaygeneration() {

	{ //!< Default

		auto& [blob, expt] = raygenerationExportGroups_[GetIndex(RaygenerationExportType::Default)];
		blob.Create(kPackagesShaderDirectory / "render/raytracing" / "Default.raygeneration.hlsl");

		expt.ExportRaygeneration(L"mainRaygen");
		expt.SetBlob(&blob);

	}
}

void FRenderCoreRaytracing::CreateMiss() {

	{ //!< Default
		auto& [blob, expt] = missExportGroups_[GetIndex(MissExportType::Default)];
		blob.Create(kPackagesShaderDirectory / "render/raytracing" / "Default.miss.hlsl");

		expt.ExportMiss(L"mainMiss");
		expt.SetBlob(&blob);
	}

}

void FRenderCoreRaytracing::CreateHitgroup() {

	{ //!< Geometry
		auto& [blob, expt] = hitgroupExportGroups_[GetIndex(HitgroupExportType::Geometry)];
		blob.Create(kPackagesShaderDirectory / "render/raytracing/hitgroup" / "Geometry.hitgroup.hlsl");

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
