#include "FRenderCoreProbe.h"
SXAVENGER_ENGINE_USING

////////////////////////////////////////////////////////////////////////////////////////////
// FRenderCoreProbe class
////////////////////////////////////////////////////////////////////////////////////////////

void FRenderCoreProbe::Init() {
	CreateRaygeneration();
	CreateMiss();
	CreateHitgroup();
	CreateContext();

	CreatePipeline();
}

const DxrObject::ExportGroup* FRenderCoreProbe::GetExportGroup(RaygenerationExportType raygeneration) const {
	return &raygenerationExportGroups_[static_cast<uint32_t>(raygeneration)].second;
}

const DxrObject::ExportGroup* FRenderCoreProbe::GetExportGroup(MissExportType miss) const {
	return &missExportGroups_[static_cast<uint32_t>(miss)].second;
}

const DxrObject::ExportGroup* FRenderCoreProbe::GetExportGroup(HitgroupExportType hitgroup) const {
	return &hitgroupExportGroups_[static_cast<uint32_t>(hitgroup)].second;
}

void FRenderCoreProbe::UpdateShaderTable(const DxrObject::TopLevelAS* topLevelAS) {
	context_->UpdateShaderTable(System::GetDxDevice(), topLevelAS);
}

void FRenderCoreProbe::SetPipeline(Process process, const DirectXQueueContext* context) const {
	pipelines_[static_cast<uint32_t>(process)]->SetPipeline(context->GetDxCommand());
}

void FRenderCoreProbe::BindComputeBuffer(Process process, const DirectXQueueContext* context, const DxObject::BindBufferDesc& desc) const {
	pipelines_[static_cast<uint32_t>(process)]->BindComputeBuffer(context->GetDxCommand(), desc);
}

void FRenderCoreProbe::Dispatch(const DirectXQueueContext* context, const Vector2ui& size) const {
	Vector3ui dispatch = DxObject::RoundUp(Vector3ui{ size.x, size.y, 1 }, kThreadGroupSize);
	context->GetCommandList()->Dispatch(dispatch.x, dispatch.y, dispatch.z);
}

void FRenderCoreProbe::Dispatch(const DirectXQueueContext* context, const Vector3ui& size) const {
	Vector3ui dispatch = DxObject::RoundUp(Vector3ui{ size.x, size.y, size.z }, kThreadGroupSize);
	context->GetCommandList()->Dispatch(dispatch.x, dispatch.y, dispatch.z);
}

void FRenderCoreProbe::SetGraphicsPipeline(const DirectXQueueContext* context) const {
	probeDebugPipeline_->SetPipeline(context->GetDxCommand());
}

void FRenderCoreProbe::BindGraphicsBuffer(const DirectXQueueContext* context, const DxObject::BindBufferDesc& desc) const {
	probeDebugPipeline_->BindGraphicsBuffer(context->GetDxCommand(), desc);
}

void FRenderCoreProbe::DrawCall(const DirectXQueueContext* context) const {

	Config conf = {};

	const uint32_t kLatitude  = 8;  //!< 緯度分割
	const uint32_t kLongitude = 16; //!< 経度分割

	context->GetCommandList()->DrawInstanced(kLatitude * kLongitude * 6, conf.probeCount.x * conf.probeCount.y * conf.probeCount.z, 0, 0);
}

void FRenderCoreProbe::CreateRaygeneration() {

	{ //!< Default
		auto& [blob, expt] = raygenerationExportGroups_[static_cast<uint32_t>(RaygenerationExportType::Default)];
		blob.Create(kDirectory / "probe.raygeneration.hlsl");

		expt.ExportRaygeneration(L"mainRaygeneration");
		expt.SetBlob(&blob);
	}


}

void FRenderCoreProbe::CreateMiss() {

	{ //!< Default
		 
		auto& [blob, expt] = missExportGroups_[static_cast<uint32_t>(MissExportType::Default)];
		blob.Create(kDirectory / "probe.miss.hlsl");
		expt.ExportMiss(L"mainMiss");
		expt.SetBlob(&blob);
	}

}

void FRenderCoreProbe::CreateHitgroup() {

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

void FRenderCoreProbe::CreateContext() {

	// path tracing stateobjectの初期化
	context_ = std::make_unique<DxrObject::StateObjectContext>();

	{
		DxrObject::GlobalRootSignatureDesc desc = {};
		desc.SetHandleUAV(0, 0, 1); //!< gProbeSample
		desc.SetHandleUAV(1, 1, 1); //!< gProbeReservoir
		desc.SetHandleUAV(2, 2, 1); //!< gProbeMoment

		desc.SetVirtualSRV(3, 0, 1); //!< gScene

		desc.Set32bitConstants(4, DxObject::ShaderVisibility::VISIBILITY_ALL, 8, 0, 1); //!< gConfig
		desc.Set32bitConstants(5, DxObject::ShaderVisibility::VISIBILITY_ALL, 3, 1, 1); //!< gSeed

		desc.SetVirtualCBV(6, 2, 1); //!< gCamera

		//* light
		// Directional Light
		desc.SetVirtualCBV(7, 0, 2); //!< gDirectionalLightCount
		desc.SetVirtualSRV(8, 0, 2); //!< gDirectionalLightTransforms
		desc.SetVirtualSRV(9, 1, 2); //!< gDirectionalLights

		// Point Light
		desc.SetVirtualCBV(10, 1, 2); //!< gPointLightCount
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
		desc.SetPayloadStride(4 * 12);
		desc.SetMaxRecursionDepth(3);

		context_->CreateStateObject(System::GetDxDevice(), std::move(desc));
	}

}

void FRenderCoreProbe::CreatePipeline() {
	//* process
	{
		DxObject::SamplerBindDesc desc = {};
		desc.SetSamplerPoint("gSampler", DxObject::SamplerMode::MODE_CLAMP);

		CreateComputePipeline(Process::Update,   kDirectory / "ProbeUpdate.cs.hlsl", desc);
		CreateComputePipeline(Process::Evaluate, kDirectory / "ProbeEvaluate.cs.hlsl", desc);
	}

	//* graphics
	{
		probeDebugPipeline_ = std::make_unique<CustomReflectionGraphicsPipeline>();
		probeDebugPipeline_->CreateBlob(kDirectory / "DebugProbe.vs.hlsl", DxObject::GraphicsShaderType::vs);
		probeDebugPipeline_->CreateBlob(kDirectory / "DebugProbe.ps.hlsl", DxObject::GraphicsShaderType::ps);

		DxObject::SamplerBindDesc sampler = {};
		sampler.SetSamplerPoint("gSampler", DxObject::SamplerMode::MODE_CLAMP);

		probeDebugPipeline_->ReflectionRootSignature(System::GetDxDevice(), sampler);

		DxObject::GraphicsPipelineDesc desc = {};
		desc.SetRasterizer(D3D12_CULL_MODE_BACK, D3D12_FILL_MODE_SOLID);
		desc.SetDepthStencil(true);

		desc.SetBlendMode(0, BlendMode::Normal);
		desc.SetIndependentBlendEnable(false);

		desc.SetPrimitive(DxObject::PrimitiveType::TrianglList);

		desc.SetRTVFormat(DxObject::kDefaultOffscreenFormat);
		desc.SetDSVFormat(DxObject::kDefaultDepthFormat);

		probeDebugPipeline_->CreatePipeline(System::GetDxDevice(), desc);
	}
	
}

void FRenderCoreProbe::CreateComputePipeline(Process process, const std::filesystem::path& filepath) {
	pipelines_[static_cast<uint8_t>(process)] = std::make_unique<CustomReflectionComputePipeline>();
	pipelines_[static_cast<uint8_t>(process)]->CreateBlob(filepath);
	pipelines_[static_cast<uint8_t>(process)]->ReflectionPipeline(System::GetDxDevice());
}

void FRenderCoreProbe::CreateComputePipeline(Process process, const std::filesystem::path& filepath, const DxObject::SamplerBindDesc& desc) {
	pipelines_[static_cast<uint8_t>(process)] = std::make_unique<CustomReflectionComputePipeline>();
	pipelines_[static_cast<uint8_t>(process)]->CreateBlob(filepath);
	pipelines_[static_cast<uint8_t>(process)]->ReflectionPipeline(System::GetDxDevice(), desc);
}
