#include "FRenderCoreLuxGlobalIllumination.h"
SXAVENGER_ENGINE_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/Components/Component/MeshRenderer/MeshRendererCommon.h>

//* lib
#include <Lib/Adapter/String/EncodedString.h>

////////////////////////////////////////////////////////////////////////////////////////////
// FRenderCoreLuxGlobalIllumination class methods
////////////////////////////////////////////////////////////////////////////////////////////

void FRenderCoreLuxGlobalIllumination::Init() {
	CreateRaygeneration();
	CreateMiss();
	CreateHitgroup();
	CreateContext();
	CreatePipeline();
}

const DxrObject::ExportGroup* FRenderCoreLuxGlobalIllumination::GetExportGroup(RaygenerationExportType raygeneration) const {
	return &raygenerationExportGroups_[static_cast<uint32_t>(raygeneration)].second;
}

const DxrObject::ExportGroup* FRenderCoreLuxGlobalIllumination::GetExportGroup(MissExportType miss) const {
	return &missExportGroups_[static_cast<uint32_t>(miss)].second;
}

const DxrObject::ExportGroup* FRenderCoreLuxGlobalIllumination::GetExportGroup(HitgroupExportType hitgroup) const {
	return &hitgroupExportGroups_[static_cast<uint32_t>(hitgroup)].second;
}

void FRenderCoreLuxGlobalIllumination::UpdateShaderTable(const DxrObject::TopLevelAS* topLevelAS) {
	context_->UpdateShaderTable(System::GetDxDevice(), topLevelAS);
}

void FRenderCoreLuxGlobalIllumination::SetPipeline(Process process, const DirectXQueueContext* context) const {
	pipelines_[static_cast<uint8_t>(process)].SetPipeline(context->GetDxCommand());
}

void FRenderCoreLuxGlobalIllumination::BindComputeBuffer(Process process, const DirectXQueueContext* context, const DxObject::BindBufferDesc& desc) const {
	pipelines_[static_cast<uint8_t>(process)].BindComputeBuffer(context->GetDxCommand(), desc);
}

void FRenderCoreLuxGlobalIllumination::Dispatch(const DirectXQueueContext* context, const Vector2ui& resolution) const {
	static const Vector3ui kNumThreads = { 16, 16, 1 };
	context->GetCommandList()->Dispatch(DxObject::RoundUp(resolution.x, kNumThreads.x), DxObject::RoundUp(resolution.y, kNumThreads.y), DxObject::RoundUp(1, kNumThreads.z));
}

void FRenderCoreLuxGlobalIllumination::CreateRaygeneration() {

	{ //!< Default
		auto& [blob, group] = raygenerationExportGroups_[static_cast<uint32_t>(RaygenerationExportType::Default)];
		blob.Create(kDirectory / "RT" / "Default.raygeneration.hlsl");

		group.ExportRaygeneration(L"mainRaygeneration");
		group.SetBlob(&blob);
	}
}

void FRenderCoreLuxGlobalIllumination::CreateMiss() {

	{ //!< Default
		auto& [blob, group] = missExportGroups_[static_cast<uint32_t>(MissExportType::Default)];
		blob.Create(kDirectory / "RT" / "Default.miss.hlsl");

		group.ExportMiss(L"mainMiss");
		group.SetBlob(&blob);
	}
}

void FRenderCoreLuxGlobalIllumination::CreateHitgroup() {

	{ //!< Mesh
		auto& [blob, group] = hitgroupExportGroups_[static_cast<uint32_t>(HitgroupExportType::Mesh)];
		blob.Create(kDirectory / "RT" / "hitgroup" / "Mesh.hitgroup.hlsl");

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
		blob.Create(kDirectory / "RT" / "hitgroup" / "Emissive.hitgroup.hlsl");
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

void FRenderCoreLuxGlobalIllumination::CreateContext() {

	//!< StateObjectの生成
	context_ = std::make_unique<DxrObject::StateObjectContext>();

	{ //!< Global Root Signatureの生成

		DxrObject::GlobalRootSignatureDesc desc = {};

		//* output buffers
		desc.SetHandleUAV(0, 0, 1); //!< gRadiance

		//* scene
		desc.SetVirtualSRV(1, 0, 1); //!< gScene

		//* camera
		desc.SetVirtualCBV(2, 0, 1); //!< gCamera

		//* setting
		desc.Set32bitConstants(3, DxObject::ShaderVisibility::VISIBILITY_ALL, 4, 1, 1); //!< gSetting

		//* resolution
		desc.Set32bitConstants(4, DxObject::ShaderVisibility::VISIBILITY_ALL, 2, 2, 1); //!< Resolution

		//* GBuffer
		desc.SetHandleSRV(5, 0, 3); //!< gDepth
		desc.SetHandleSRV(6, 1, 3); //!< gAlbedo
		desc.SetHandleSRV(7, 2, 3); //!< gNormal
		desc.SetHandleSRV(8, 3, 3); //!< gMaterialARM

		//* light
		// Directional Light
		desc.Set32bitConstants(9, DxObject::ShaderVisibility::VISIBILITY_ALL, 1, 0, 2); //!< gDirectionalLightCount
		desc.SetVirtualSRV(10, 0, 2); //!< gDirectionalLightTransforms
		desc.SetVirtualSRV(11, 1, 2); //!< gDirectionalLights

		// Point Light
		desc.Set32bitConstants(12, DxObject::ShaderVisibility::VISIBILITY_ALL, 1, 1, 2); //!< gPointLightCount
		desc.SetVirtualSRV(13, 2, 2); //!< gPointLightTransforms
		desc.SetVirtualSRV(14, 3, 2); //!< gPointLights

		// Spot Light
		desc.Set32bitConstants(15, DxObject::ShaderVisibility::VISIBILITY_ALL, 1, 2, 2); //!< gSpotLightCount
		desc.SetVirtualSRV(16, 4, 2); //!< gSpotLightTransforms
		desc.SetVirtualSRV(17, 5, 2); //!< gSpotLights

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

void FRenderCoreLuxGlobalIllumination::CreatePipeline() {
	CreateComputePipeline(Process::IrradianceCalculate, kDirectory / "Process" / "IrradianceCalculate.cs.hlsl");
	CreateComputePipeline(Process::Solve, kDirectory / "Process" / "Solve.cs.hlsl");
}

void FRenderCoreLuxGlobalIllumination::CreateComputePipeline(Process process, const std::filesystem::path& filepath) {
	pipelines_[static_cast<uint8_t>(process)].CreateBlob(filepath);
	pipelines_[static_cast<uint8_t>(process)].ReflectionPipeline(System::GetDxDevice());
}
