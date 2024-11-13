#include "RaytracingPipeline.h"
_DXOBJECT_USING
_DXROBJECT_USING

////////////////////////////////////////////////////////////////////////////////////////////
// RaytracingPipeline class methods
////////////////////////////////////////////////////////////////////////////////////////////

void RaytracingPipeline::Init() {
	CreateRaygeneration();
	CreateMiss();
	CreateHitgroup();
}

void RaytracingPipeline::Term() {
}

void RaytracingPipeline::CreateRaygeneration() {

	{
		auto& pipeline = pipelines_[kRaygeneration_Default];
		pipeline.Create(1);

		// exports
		pipeline.exports[0]->CreateRaygeneration(L"mainRaygeneration");

		// blob
		pipeline.blob->Create(L"raygeneration.hlsl");
		pipeline.SetExport();
	}
}

void RaytracingPipeline::CreateMiss() {

	{
		auto& pipeline = pipelines_[kMiss_Default];
		pipeline.Create(1);

		// export
		pipeline.exports[0]->CreateMiss(L"mainMiss");

		LocalRootSignatureDesc desc = {};
		desc.SetCBV(0, 0); //!< Atmospher

		pipeline.exports[0]->CreateRootSignature(desc);

		// blob
		pipeline.blob->Create(L"miss.hlsl");
		pipeline.SetExport();
	}
}

void RaytracingPipeline::CreateHitgroup() {

	{
		auto& pipeline = pipelines_[kHitgroup_Test];
		pipeline.Create(1);

		// export
		pipeline.exports[0]->CreateHitgroup(L"test", L"mainClosesthit");

		LocalRootSignatureDesc desc = {};
		desc.SetVirtualSRV(0, 0); //!< Vertices
		desc.SetVirtualSRV(1, 1); //!< Indices
		desc.SetCBV(2, 0);        //!< UVTransform
		desc.SetSRV(3, 2);        //!< Albedo
		desc.SetSampler(MODE_WRAP, 0);

		pipeline.exports[0]->CreateRootSignature(desc);

		// blob
		pipeline.blob->Create(L"closesthit.hlsl");
		pipeline.SetExport();

	}

	{
		auto& pipeline = pipelines_[kHitgroup_Behavior];
		pipeline.Create(1);

		// export
		pipeline.exports[0]->CreateHitgroup(L"behavior", L"ClosesthitBehavior");

		LocalRootSignatureDesc desc = {};
		desc.SetVirtualSRV(0, 0); //!< Vertices
		desc.SetVirtualSRV(1, 1); //!< Indices
		desc.SetCBV(2, 0);        //!< UVTransform
		desc.SetSRV(3, 2);        //!< Albedo
		desc.SetSampler(MODE_WRAP, 0);
		desc.SetCBV(4, 1);        //!< PBRMaterial

		pipeline.exports[0]->CreateRootSignature(desc);

		// blob
		pipeline.blob->Create(L"BehaviorClosesthit.hlsl");
		pipeline.SetExport();

	}
}
