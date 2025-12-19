#include "FRenderCoreRestir.h"
SXAVENGER_ENGINE_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/System.h>

//* lib
#include <Lib/Adapter/Random/Random.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Seed structure methods
////////////////////////////////////////////////////////////////////////////////////////////

void FRenderCoreRestir::Seed::Set() {
	std::generate(seed.begin(), seed.end(), []() {
		return Random::UniformDistribution<uint32_t>(std::numeric_limits<uint32_t>::min(), std::numeric_limits<uint32_t>::max());
	});
}

////////////////////////////////////////////////////////////////////////////////////////////
// FRenderCoreRestir class methods
////////////////////////////////////////////////////////////////////////////////////////////

void FRenderCoreRestir::Init() {
	CreateRaygeneration();
	CreateMiss();
	CreateHitgroup();
	CreateContext();

	CreatePipeline();
}

const DxrObject::ExportGroup* FRenderCoreRestir::GetExportGroup(RaygenerationExportType raygeneration) const {
	return &raygenerationExportGroups_[static_cast<uint32_t>(raygeneration)].second;
}

const DxrObject::ExportGroup* FRenderCoreRestir::GetExportGroup(MissExportType miss) const {
	return &missExportGroups_[static_cast<uint32_t>(miss)].second;
}

const DxrObject::ExportGroup* FRenderCoreRestir::GetExportGroup(HitgroupExportType hitgroup) const {
	return &hitgroupExportGroups_[static_cast<uint32_t>(hitgroup)].second;
}

void FRenderCoreRestir::UpdateShaderTable(const DxrObject::TopLevelAS* topLevelAS) {
	context_->UpdateShaderTable(System::GetDxDevice(), topLevelAS);
}

void FRenderCoreRestir::SetPipeline(Process process, const DirectXQueueContext* context) const {
	pipelines_[static_cast<uint32_t>(process)]->SetPipeline(context->GetDxCommand());
}

void FRenderCoreRestir::BindComputeBuffer(Process process, const DirectXQueueContext* context, const DxObject::BindBufferDesc& desc) const {
	pipelines_[static_cast<uint32_t>(process)]->BindComputeBuffer(context->GetDxCommand(), desc);
}

void FRenderCoreRestir::Dispatch(const DirectXQueueContext* context, const Vector2ui& size) const {
	Vector3ui dispatch = DxObject::RoundUp(Vector3ui{ size.x, size.y, 1 }, kThreadGroupSize);
	context->GetCommandList()->Dispatch(dispatch.x, dispatch.y, dispatch.z);
}

void FRenderCoreRestir::CreateRaygeneration() {

	{ //!< Default
		auto& [blob, expt] = raygenerationExportGroups_[static_cast<uint32_t>(RaygenerationExportType::Default)];
		blob.Create(kDirectory / "Default.raygeneration.hlsl");

		expt.ExportRaygeneration(L"mainRaygeneration");
		expt.SetBlob(&blob);
	}

}

void FRenderCoreRestir::CreateMiss() {


	{ //!< Default
		auto& [blob, expt] = missExportGroups_[static_cast<uint32_t>(MissExportType::Default)];
		blob.Create(kDirectory / "Default.miss.hlsl");

		expt.ExportMiss(L"mainMiss");
		expt.SetBlob(&blob);
	}
}

void FRenderCoreRestir::CreateHitgroup() {

	{  //!< Mesh
		auto& [blob, expt] = hitgroupExportGroups_[static_cast<uint32_t>(HitgroupExportType::Mesh)];
		blob.Create(kDirectory / "hitgroup" / "Mesh.hitgroup.hlsl");

		//* hitgroup
		DxrObject::ExportGroup::Hitgroup hitgroup = {};
		hitgroup.type       = D3D12_HIT_GROUP_TYPE_TRIANGLES;
		hitgroup.closesthit = L"mainClosesthit";
		hitgroup.anyhit     = L"mainAnyhit";

		expt.ExportHitgroup(L"Mesh", hitgroup);
		expt.SetBlob(&blob);

		//* root signature
		DxrObject::LocalRootSignatureDesc desc = {};
		desc.SetSamplerLinear(DxObject::SamplerMode::MODE_WRAP, DxObject::ShaderVisibility::VISIBILITY_ALL, 0);
		desc.SetVirtualSRV(0, 10); //!< gVertices
		desc.SetVirtualSRV(1, 11); //!< gIndices
		desc.SetVirtualSRV(2, 0);  //!< gMaterial

		expt.CreateRootSignature(System::GetDxDevice(), desc);
	}

	{ //!< Emissive
		auto& [blob, expt] = hitgroupExportGroups_[static_cast<uint32_t>(HitgroupExportType::Emissive)];
		blob.Create(kDirectory / "hitgroup" / "Emissive.hitgroup.hlsl");
		//* hitgroup
		DxrObject::ExportGroup::Hitgroup hitgroup = {};
		hitgroup.type       = D3D12_HIT_GROUP_TYPE_TRIANGLES;
		hitgroup.closesthit = L"mainEmissiveClosesthit";
		hitgroup.anyhit     = L"mainEmissiveAnyhit";

		expt.ExportHitgroup(L"Emissive", hitgroup);
		expt.SetBlob(&blob);

		//* root signature
		DxrObject::LocalRootSignatureDesc desc = {};
		desc.SetSamplerLinear(DxObject::SamplerMode::MODE_WRAP, DxObject::ShaderVisibility::VISIBILITY_ALL, 0);
		desc.SetVirtualSRV(0, 10); //!< gVertices
		desc.SetVirtualSRV(1, 11); //!< gIndices
		desc.SetVirtualSRV(2, 0);  //!< gMaterial

		expt.CreateRootSignature(System::GetDxDevice(), desc);

	}

}

void FRenderCoreRestir::CreateContext() {

	// path tracing stateobjectの初期化
	context_ = std::make_unique<DxrObject::StateObjectContext>();

	{ //!< global root signatureの生成

		DxrObject::GlobalRootSignatureDesc desc = {};
		//* result.
		desc.SetVirtualUAV(0, 0, 1); //!< ginitalizeReservoir
		desc.SetVirtualUAV(1, 1, 1); //!< gMoment

		//* scene
		desc.SetVirtualSRV(2, 0, 1); //!< gScene

		//* deferred textures
		desc.SetVirtualCBV(3, 0, 1); //!< gDeferredBufferIndex

		//* camera
		desc.SetVirtualCBV(4, 1, 1); //!< gCamera

		//* config
		desc.Set32bitConstants(5, DxObject::ShaderVisibility::VISIBILITY_ALL, 2, 2, 1); //!< Config
		desc.Set32bitConstants(6, DxObject::ShaderVisibility::VISIBILITY_ALL, 3, 3, 1); //!< Seed

		//* light
		// Directional Light
		desc.SetVirtualCBV(7, 0, 2); //!< gDirectionalLightCount
		desc.SetVirtualSRV(8, 0, 2); //!< gDirectionalLightTransforms
		desc.SetVirtualSRV(9, 1, 2); //!< gDirectionalLights

		// Point Light
		desc.SetVirtualCBV(10, 1, 2);  //!< gPointLightCount
		desc.SetVirtualSRV(11, 2, 2); //!< gPointLightTransforms
		desc.SetVirtualSRV(12, 3, 2); //!< gPointLights

		// Spot Light
		desc.SetVirtualCBV(13, 2, 2); //!< gSpotLightCount
		desc.SetVirtualSRV(14, 4, 2); //!< gSpotLightTransforms
		desc.SetVirtualSRV(15, 5, 2); //!< gSpotLights

		// Sky Light
		desc.SetVirtualCBV(16, 3, 2); //!< gSkyLight

		desc.SetSamplerLinear(DxObject::MODE_WRAP, DxObject::ShaderVisibility::VISIBILITY_ALL, 0, 2); //!< gSampler

		context_->CreateRootSignature(System::GetDxDevice(), desc);

	}

	{ //!< state objectの生成
		DxrObject::StateObjectDesc desc = {};
		desc.AddExport(GetExportGroup(RaygenerationExportType::Default));
		desc.AddExport(GetExportGroup(MissExportType::Default));
		desc.AddExport(GetExportGroup(HitgroupExportType::Mesh));
		desc.AddExport(GetExportGroup(HitgroupExportType::Emissive));

		// 仮paraemter
		desc.SetAttributeStride(4 * 2);
		desc.SetPayloadStride(4 * 11);
		desc.SetMaxRecursionDepth(3);

		context_->CreateStateObject(System::GetDxDevice(), std::move(desc));
	}

}

void FRenderCoreRestir::CreatePipeline() {
	CreateComputePipeline(Process::Reset,        kDirectory / "RestirReset.cs.hlsl");
	CreateComputePipeline(Process::Temporal,     kDirectory / "RestirTemporal.cs.hlsl");
	CreateComputePipeline(Process::Spatial,      kDirectory / "RestirSpatial.cs.hlsl");
	CreateComputePipeline(Process::Texture,      kDirectory / "RestirTexture.cs.hlsl");
	CreateComputePipeline(Process::EdgeStopping, kDirectory / "EdgeStopping.cs.hlsl");
}

void FRenderCoreRestir::CreateComputePipeline(Process process, const std::filesystem::path& filepath) {
	pipelines_[static_cast<uint8_t>(process)] = std::make_unique<CustomReflectionComputePipeline>();
	pipelines_[static_cast<uint8_t>(process)]->CreateBlob(filepath);
	pipelines_[static_cast<uint8_t>(process)]->ReflectionPipeline(System::GetDxDevice());

}
