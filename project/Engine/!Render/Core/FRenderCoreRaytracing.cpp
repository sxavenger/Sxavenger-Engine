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
}

void FRenderCoreRaytracing::InitRaygenerationExportGroup() {

	{
		auto& [expt, blob] = raygenerationExportGroups_[static_cast<uint32_t>(RaygenerationExportType::Default)];
		expt.ExportRaygeneration(L"mainRaygen");

		LocalRootSignatureDesc desc = {};
		desc.SetHandleSRV(0, 0); //!< gDepth
		desc.SetHandleSRV(1, 1); //!< gNormal
		desc.SetHandleSRV(2, 2); //!< gPosition
		desc.SetVirtualCBV(3, 0); //!< gCamera
		desc.SetHandleUAV(4, 0); //!< gReflection

		expt.CreateRootSignature(SxavengerSystem::GetDxDevice(), desc);

		blob.Create(kDirectory_ / "reflection/reflectionRaygeneration.hlsl");
		blob.SetExport(&expt);
	}
}

void FRenderCoreRaytracing::InitMissExportGroup() {

	{
		auto& [expt, blob] = missExportGroups_[static_cast<uint32_t>(MissExportType::Default)];
		expt.ExportMiss(L"mainMiss");

		blob.Create(kDirectory_ / "reflection/reflectionMiss.hlsl");
		blob.SetExport(&expt);
	}
}

void FRenderCoreRaytracing::InitHitgroupExportGroup() {

	{
		auto& [expt, blob] = hitgroupExportGroups_[static_cast<uint32_t>(HitgroupExportType::Geometry)];

		ExportGroup::HitgroupEntry entry = {};
		entry.closesthit = L"mainGeometryClosesthit";

		expt.ExportHitgroup(L"Geometry", entry);

		LocalRootSignatureDesc desc = {};
		desc.SetHandleSRV(0, 0); //!< gVertices
		desc.SetHandleSRV(1, 1); //!< gIndices
		desc.SetHandleSRV(2, 2); //!< gAlbedo
		desc.SetSamplerLinear(DxObject::SamplerMode::MODE_WRAP, DxObject::ShaderVisibility::VISIBILITY_ALL, 0);

		expt.CreateRootSignature(SxavengerSystem::GetDxDevice(), desc);

		blob.Create(kDirectory_ / "reflection/reflectionGeometry.hlsl");
		blob.SetExport(&expt);
	}

}
