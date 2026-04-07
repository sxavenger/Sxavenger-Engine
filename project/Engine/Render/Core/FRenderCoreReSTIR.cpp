#include "FRenderCoreReSTIR.h"
SXAVENGER_ENGINE_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/Components/Component/MeshRenderer/MeshRendererCommon.h>

//* lib
#include <Lib/Adapter/String/EncodedString.h>

////////////////////////////////////////////////////////////////////////////////////////////
// FRenderCoreReSTIR class methods
////////////////////////////////////////////////////////////////////////////////////////////

void FRenderCoreReSTIR::Init() {
	CreateRaygeneration();
	CreateMiss();
	CreateHitgroup();
	CreateContext();
	CreatePipeline();
}

const DxrObject::ExportGroup* FRenderCoreReSTIR::GetExportGroup(RaygenerationExportType raygeneration) const {
	return &raygenerationExportGroups_[static_cast<uint32_t>(raygeneration)].second;
}

const DxrObject::ExportGroup* FRenderCoreReSTIR::GetExportGroup(MissExportType miss) const {
	return &missExportGroups_[static_cast<uint32_t>(miss)].second;
}

const DxrObject::ExportGroup* FRenderCoreReSTIR::GetExportGroup(HitgroupExportType hitgroup) const {
	return &hitgroupExportGroups_[static_cast<uint32_t>(hitgroup)].second;
}

void FRenderCoreReSTIR::UpdateShaderTable(const DxrObject::TopLevelAS* topLevelAS) {
	context_->UpdateShaderTable(System::GetDxDevice(), topLevelAS);
}

void FRenderCoreReSTIR::SetPipeline(Process process, const DirectXQueueContext* context) const {
	pipelines_[static_cast<uint8_t>(process)].SetPipeline(context->GetDxCommand());
}

void FRenderCoreReSTIR::BindComputeBuffer(Process process, const DirectXQueueContext* context, const DxObject::BindBufferDesc& desc) const {
	pipelines_[static_cast<uint8_t>(process)].BindComputeBuffer(context->GetDxCommand(), desc);
}

void FRenderCoreReSTIR::Dispatch(const DirectXQueueContext* context, const Vector2ui& resolution) const {
	static const Vector3ui kNumThreads = { 16, 16, 1 };
	context->GetCommandList()->Dispatch(DxObject::RoundUp(resolution.x, kNumThreads.x), DxObject::RoundUp(resolution.y, kNumThreads.y), DxObject::RoundUp(1, kNumThreads.z));
}

void FRenderCoreReSTIR::CreateRaygeneration() {

	{ //!< Default
		auto& [blob, group] = raygenerationExportGroups_[static_cast<uint32_t>(RaygenerationExportType::Default)];
		blob.Create(kDirectory / "PT" / "Default.raygeneration.hlsl");

		group.ExportRaygeneration(L"mainRaygeneration");
		group.SetBlob(&blob);
	}
}

void FRenderCoreReSTIR::CreateMiss() {

	{ //!< Default
		auto& [blob, group] = missExportGroups_[static_cast<uint32_t>(MissExportType::Default)];
		blob.Create(kDirectory / "PT" / "Default.miss.hlsl");

		group.ExportMiss(L"mainMiss");
		group.SetBlob(&blob);
	}
}

void FRenderCoreReSTIR::CreateHitgroup() {

	{ //!< Mesh
		auto& [blob, group] = hitgroupExportGroups_[static_cast<uint32_t>(HitgroupExportType::Mesh)];
		blob.Create(kDirectory / "PT" / "hitgroup" / "Mesh.hitgroup.hlsl");

		//* hitgroup
		DxrObject::ExportGroup::Hitgroup hitgroup = {};
		hitgroup.type       = D3D12_HIT_GROUP_TYPE_TRIANGLES;
		hitgroup.closesthit = L"mainClosesthit";
		hitgroup.anyhit     = L"mainAnyhit";

		group.ExportHitgroup(EncodedString::Convert(magic_enum::enum_name(MeshRendererCommon::Mode::Opaque)), hitgroup);
		group.SetBlob(&blob);

		//* root signature
		DxrObject::LocalRootSignatureDesc desc = {};
		desc.SetSamplerLinear(DxObject::SamplerMode::Wrap, DxObject::ShaderVisibility::VISIBILITY_ALL, 0);
		desc.SetVirtualSRV(0, 10); //!< gVertices
		desc.SetVirtualSRV(1, 11); //!< gIndices
		desc.SetVirtualSRV(2, 0);  //!< gMaterial

		group.CreateRootSignature(System::GetDxDevice(), desc);
	}

	{ //!< Emissive
		auto& [blob, group] = hitgroupExportGroups_[static_cast<uint32_t>(HitgroupExportType::Emissive)];
		blob.Create(kDirectory / "PT" / "hitgroup" / "Emissive.hitgroup.hlsl");
		//* hitgroup
		DxrObject::ExportGroup::Hitgroup hitgroup = {};
		hitgroup.type       = D3D12_HIT_GROUP_TYPE_TRIANGLES;
		hitgroup.closesthit = L"mainEmissiveClosesthit";
		hitgroup.anyhit     = L"mainEmissiveAnyhit";

		group.ExportHitgroup(EncodedString::Convert(magic_enum::enum_name(MeshRendererCommon::Mode::Emissive)), hitgroup);
		group.SetBlob(&blob);

		//* root signature
		DxrObject::LocalRootSignatureDesc desc = {};
		desc.SetSamplerLinear(DxObject::SamplerMode::Wrap, DxObject::ShaderVisibility::VISIBILITY_ALL, 0);
		desc.SetVirtualSRV(0, 10); //!< gVertices
		desc.SetVirtualSRV(1, 11); //!< gIndices
		desc.SetVirtualSRV(2, 0);  //!< gMaterial

		group.CreateRootSignature(System::GetDxDevice(), desc);

	}
}

void FRenderCoreReSTIR::CreateContext() {

	//!< StateObjectの生成
	context_ = std::make_unique<DxrObject::StateObjectContext>();

	{ //!< Global Root Signatureの生成

		DxrObject::GlobalRootSignatureDesc desc = {};

		//* output buffers
		desc.SetVirtualUAV(0, 0, 1); //!< gInitializeReservoir
		desc.SetVirtualUAV(1, 1, 1); //!< gMoment

		//* scene
		desc.SetVirtualSRV(2, 0, 1); //!< gScene

		//* camera
		desc.SetVirtualCBV(3, 0, 1); //!< gCamera

		//* setting
		desc.Set32bitConstants(4, DxObject::ShaderVisibility::VISIBILITY_ALL, 2, 1, 1); //!< gSetting

		//* seed
		desc.Set32bitConstants(5, DxObject::ShaderVisibility::VISIBILITY_ALL, 1, 2, 1); //!< gSeed

		//* GBuffer
		desc.SetHandleSRV(6, 0, 3); //!< gDepth
		desc.SetHandleSRV(7, 1, 3); //!< gAlbedo
		desc.SetHandleSRV(8, 2, 3); //!< gNormal
		desc.SetHandleSRV(9, 3, 3); //!< gMaterialARM

		//* light
		// Directional Light
		desc.Set32bitConstants(10, DxObject::ShaderVisibility::VISIBILITY_ALL, 1, 0, 2); //!< gDirectionalLightCount
		desc.SetVirtualSRV(11, 0, 2); //!< gDirectionalLightTransforms
		desc.SetVirtualSRV(12, 1, 2); //!< gDirectionalLights

		// Point Light
		desc.Set32bitConstants(13, DxObject::ShaderVisibility::VISIBILITY_ALL, 1, 1, 2); //!< gPointLightCount
		desc.SetVirtualSRV(14, 2, 2); //!< gPointLightTransforms
		desc.SetVirtualSRV(15, 3, 2); //!< gPointLights

		// Spot Light
		desc.Set32bitConstants(16, DxObject::ShaderVisibility::VISIBILITY_ALL, 1, 2, 2); //!< gSpotLightCount
		desc.SetVirtualSRV(17, 4, 2); //!< gSpotLightTransforms
		desc.SetVirtualSRV(18, 5, 2); //!< gSpotLights

		context_->CreateRootSignature(System::GetDxDevice(), desc);
	}

	{ //!< state objectの生成
		DxrObject::StateObjectDesc desc = {};
		desc.AddExport(GetExportGroup(RaygenerationExportType::Default));
		desc.AddExport(GetExportGroup(MissExportType::Default));
		desc.AddExport(GetExportGroup(HitgroupExportType::Mesh));
		desc.AddExport(GetExportGroup(HitgroupExportType::Emissive));

		// 仮parameter
		desc.SetAttributeStride(4 * 2);
		desc.SetPayloadStride(4 * 11);
		desc.SetMaxRecursionDepth(3);

		context_->CreateStateObject(System::GetDxDevice(), std::move(desc));
	}

}

void FRenderCoreReSTIR::CreatePipeline() {
	CreateComputePipeline(Process::Reset, kDirectory / "Process" / "Reset.cs.hlsl");
	CreateComputePipeline(Process::Temporal, kDirectory / "Process" / "Temporal.cs.hlsl");
	CreateComputePipeline(Process::Solve, kDirectory / "Process" / "Solve.cs.hlsl");
}

void FRenderCoreReSTIR::CreateComputePipeline(Process process, const std::filesystem::path& filepath) {
	pipelines_[static_cast<uint8_t>(process)].CreateBlob(filepath);
	pipelines_[static_cast<uint8_t>(process)].ReflectionPipeline(System::GetDxDevice());
}
