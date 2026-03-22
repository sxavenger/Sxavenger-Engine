#include "FRenderCoreReflection.h"
SXAVENGER_ENGINE_USING
DXOBJECT_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/Utility/Convert.h>
#include <Engine/System/System.h>
#include <Engine/Assets/Asset/AssetMaterial.h>

////////////////////////////////////////////////////////////////////////////////////////////
// FRenderCoreReflection class methods
////////////////////////////////////////////////////////////////////////////////////////////

void FRenderCoreReflection::Init() {
	CreateRaygeneration();
	CreateMiss();
	CreateHitgroup();
	CreateContext();

	CreateComputePipeline(Process::Calculate, kDirectory / "ReflectionCalculate.cs.hlsl");
}

const DxrObject::ExportGroup* FRenderCoreReflection::GetExportGroup(RaygenerationExportType raygeneration) const {
	return &raygenerationExportGroups_[static_cast<uint32_t>(raygeneration)].second;
}

const DxrObject::ExportGroup* FRenderCoreReflection::GetExportGroup(MissExportType miss) const {
	return &missExportGroups_[static_cast<uint32_t>(miss)].second;
}

const DxrObject::ExportGroup* FRenderCoreReflection::GetExportGroup(HitgroupExportType hitgroup) const {
	return &hitgroupExportGroups_[static_cast<uint32_t>(hitgroup)].second;
}

void FRenderCoreReflection::UpdateShaderTable(const DxrObject::TopLevelAS* topLevelAS) {
	context_->UpdateShaderTable(System::GetDxDevice(), topLevelAS);
}

void FRenderCoreReflection::SetPipeline(Process process, const DirectXQueueContext* context) const {
	pipelines_[static_cast<uint32_t>(process)]->SetPipeline(context->GetDxCommand());
}

void FRenderCoreReflection::BindComputeBuffer(Process process, const DirectXQueueContext* context, const DxObject::BindBufferDesc& desc) const {
	pipelines_[static_cast<uint32_t>(process)]->BindComputeBuffer(context->GetDxCommand(), desc);
}

void FRenderCoreReflection::Dispatch(const DirectXQueueContext* context, const Vector2ui& size) const {
	Vector3ui dispatch = DxObject::RoundUp(Vector3ui{ size.x, size.y, 1 }, kThreadGroupSize);
	context->GetCommandList()->Dispatch(dispatch.x, dispatch.y, dispatch.z);
}

void FRenderCoreReflection::CreateRaygeneration() {

	{ //!< Default
		auto& [blob, expt] = raygenerationExportGroups_[static_cast<uint32_t>(RaygenerationExportType::Default)];
		blob.Create(kDirectory / "Default.raygeneration.hlsl");

		expt.ExportRaygeneration(L"mainRaygeneration");
		expt.SetBlob(&blob);
	}

}

void FRenderCoreReflection::CreateMiss() {

	{ //!< Default
		auto& [blob, expt] = missExportGroups_[static_cast<uint32_t>(MissExportType::Default)];
		blob.Create(kDirectory / "Default.miss.hlsl");

		expt.ExportMiss(L"mainMiss");
		expt.SetBlob(&blob);
	}

}

void FRenderCoreReflection::CreateHitgroup() {

	DxrObject::LocalRootSignatureDesc desc = {};
	desc.SetSamplerLinear(DxObject::SamplerMode::MODE_WRAP, DxObject::ShaderVisibility::VISIBILITY_ALL, 0);
	desc.SetVirtualSRV(0, 10); //!< gVertices
	desc.SetVirtualSRV(1, 11); //!< gIndices
	desc.SetVirtualSRV(2, 0);  //!< gMaterial

	{ //!< Mesh
		auto& [blob, expt] = hitgroupExportGroups_[static_cast<uint32_t>(HitgroupExportType::Mesh)];
		blob.Create(kDirectory / "hitgroup" / "Mesh.hitgroup.hlsl");

		//* hitgroup
		DxrObject::ExportGroup::Hitgroup hitgroup = {};
		hitgroup.type       = D3D12_HIT_GROUP_TYPE_TRIANGLES;
		hitgroup.closesthit = L"mainMeshClosesthit";
		hitgroup.anyhit     = L"mainMeshAnyhit";

		expt.ExportHitgroup(ToWString(magic_enum::enum_name(AssetMaterial::Mode::Opaque)), hitgroup);
		expt.SetBlob(&blob);

		//* root signature
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

		expt.ExportHitgroup(ToWString(magic_enum::enum_name(AssetMaterial::Mode::Emissive)), hitgroup);
		expt.SetBlob(&blob);

		//* root signature
		expt.CreateRootSignature(System::GetDxDevice(), desc);

	}
}

void FRenderCoreReflection::CreateContext() {

	//!< Path tracing StateObjectの初期化
	context_ = std::make_unique<DxrObject::StateObjectContext>();

	{ //!< Global Root Signatureの生成

		DxrObject::GlobalRootSignatureDesc desc = {};

		//* output
		desc.SetHandleUAV(0, 0, 1); //!< gReflectionRadiance
		desc.SetHandleUAV(1, 1, 1); //!< gReflectionPosition

		//* scene
		desc.SetVirtualSRV(2, 0, 1); //!< gScene

		//* G-Buffer
		desc.SetHandleSRV(3, 1, 1); //!< gDepth
		desc.SetHandleSRV(4, 2, 1); //!< gNormal
		desc.SetHandleSRV(5, 3, 1); //!< gPosition

		//* camera
		desc.SetVirtualCBV(6, 0, 1); //!< gCamera

		//* light
		// Directional Light
		desc.Set32bitConstants(7, DxObject::ShaderVisibility::VISIBILITY_ALL, 1, 0, 2); //!< gDirectionalLightCount
		desc.SetVirtualSRV(8, 0, 2); //!< gDirectionalLightTransforms
		desc.SetVirtualSRV(9, 1, 2); //!< gDirectionalLights

		// Point Light
		desc.Set32bitConstants(10, DxObject::ShaderVisibility::VISIBILITY_ALL, 1, 1, 2); //!< gPointLightCount
		desc.SetVirtualSRV(11, 2, 2); //!< gPointLightTransforms
		desc.SetVirtualSRV(12, 3, 2); //!< gPointLights

		// Spot Light
		desc.Set32bitConstants(13, DxObject::ShaderVisibility::VISIBILITY_ALL, 1, 2, 2); //!< gSpotLightCount
		desc.SetVirtualSRV(14, 4, 2); //!< gSpotLightTransforms
		desc.SetVirtualSRV(15, 5, 2); //!< gSpotLights

		context_->CreateRootSignature(System::GetDxDevice(), desc);
	}

	{ //!< State Object の生成
		DxrObject::StateObjectDesc desc = {};
		desc.AddExport(GetExportGroup(RaygenerationExportType::Default));
		desc.AddExport(GetExportGroup(MissExportType::Default));
		desc.AddExport(GetExportGroup(HitgroupExportType::Mesh));
		desc.AddExport(GetExportGroup(HitgroupExportType::Emissive));

		// 仮parameter
		desc.SetAttributeStride(sizeof(float) * 2);
		desc.SetPayloadStride(sizeof(float) * 8);
		desc.SetMaxRecursionDepth(2);

		context_->CreateStateObject(System::GetDxDevice(), std::move(desc));
	}

}

void FRenderCoreReflection::CreateComputePipeline(Process process, const std::filesystem::path& filepath) {
	pipelines_[static_cast<uint8_t>(process)] = std::make_unique<CustomReflectionComputePipeline>();
	pipelines_[static_cast<uint8_t>(process)]->CreateBlob(filepath);
	pipelines_[static_cast<uint8_t>(process)]->ReflectionPipeline(System::GetDxDevice());
}
