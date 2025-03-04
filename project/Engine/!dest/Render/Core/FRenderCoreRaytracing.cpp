#include "FRenderCoreRaytracing.h"
_DXROBJECT_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/SxavengerSystem.h>

//=========================================================================================
// static variables
//=========================================================================================

const std::filesystem::path FRenderCoreRaytracing::kDirectory_ = "packages/shaders/raytracing";

////////////////////////////////////////////////////////////////////////////////////////////
// FSceneRenderer class
////////////////////////////////////////////////////////////////////////////////////////////

void FRenderCoreRaytracing::Init() {
	InitRaygenerationExportGroup();
	InitMissExportGroup();
	InitHitgroupExportGroup();
}

void FRenderCoreRaytracing::InitRaygenerationExportGroup() {

	{
		auto& [blob, expt] = raygenerationExportGroups_[static_cast<uint32_t>(RaygenerationExportType::Default)];
		blob.Create(kDirectory_ / "reflection/reflectionRaygeneration.hlsl");

		expt.ExportRaygeneration(L"mainRaygen");
		expt.SetBlob(&blob);

		LocalRootSignatureDesc desc = {};
		desc.SetHandleSRV(0, 0); //!< gDepth
		desc.SetHandleSRV(1, 1); //!< gNormal
		desc.SetHandleSRV(2, 2); //!< gPosition
		desc.SetVirtualCBV(3, 0); //!< gCamera

		expt.CreateRootSignature(SxavengerSystem::GetDxDevice(), desc);
	}
}

void FRenderCoreRaytracing::InitMissExportGroup() {

	{
		auto& [blob, expt] = missExportGroups_[static_cast<uint32_t>(MissExportType::Default)];
		blob.Create(kDirectory_ / "reflection/reflectionMiss.hlsl");

		expt.ExportMiss(L"mainMiss");
		expt.SetBlob(&blob);
	}
}

void FRenderCoreRaytracing::InitHitgroupExportGroup() {

	{
		auto& [blob, expt] = hitgroupExportGroups_[static_cast<uint32_t>(HitgroupExportType::Geometry)];
		blob.Create(kDirectory_ / "reflection/reflectionGeometry.hlsl");

		ExportGroup::Hitgroup entry = {};
		entry.closesthit = L"mainGeometryClosesthit";

		expt.ExportHitgroup(L"Geometry", entry);
		expt.SetBlob(&blob);

		LocalRootSignatureDesc desc = {};
		desc.SetVirtualSRV(0, 0); //!< gVertices
		desc.SetVirtualSRV(1, 1); //!< gIndices
		desc.SetHandleSRV(2, 2); //!< gAlbedo
		desc.SetSamplerLinear(DxObject::SamplerMode::MODE_WRAP, DxObject::ShaderVisibility::VISIBILITY_ALL, 0);

		expt.CreateRootSignature(SxavengerSystem::GetDxDevice(), desc);
	}

}
