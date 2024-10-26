#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/DxrObject/DxrExportGroup.h>
#include <Engine/System/DxrObject/DxrRaytracingShaderBlob.h>

//* c++
#include <memory>
#include <array>

////////////////////////////////////////////////////////////////////////////////////////////
// RaytracingPipelineType enum
////////////////////////////////////////////////////////////////////////////////////////////
enum RaytracingPipelineType {

	//* raygeneration *//
	kRaygeneration_Default,

	//* miss *//
	kMiss_Default,

	//* hitgroup *//
	kHitgroup_Test,

	kCountOfRaytracingPipeline
};

////////////////////////////////////////////////////////////////////////////////////////////
// RaytracingPipeline class
////////////////////////////////////////////////////////////////////////////////////////////
class RaytracingPipeline {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	RaytracingPipeline()  = default;
	~RaytracingPipeline() { Term(); }

	void Init();

	void Term();

	const DxrObject::ExportGroup* GetExport(RaytracingPipelineType type, uint32_t index) const { return pipelines_[type].exports[index].get(); }

	const DxrObject::RaytracingShaderBlob* GetBlob(RaytracingPipelineType type) const { return pipelines_[type].blob.get(); }

private:

	////////////////////////////////////////////////////////////////////////////////////////////
	// PipelineElement structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct PipelineElement {
		std::vector<std::unique_ptr<DxrObject::ExportGroup>> exports;
		std::unique_ptr<DxrObject::RaytracingShaderBlob>     blob;

		void Create(uint32_t exportSize) {
			exports.resize(exportSize);
			for (auto& expt : exports) {
				expt = std::make_unique<DxrObject::ExportGroup>();
			}

			blob = std::make_unique<DxrObject::RaytracingShaderBlob>();
		}

		void SetExport() {
			for (const auto& expt : exports) {
				blob->SetExportGroup(expt.get());
			}
		}
	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// using
	////////////////////////////////////////////////////////////////////////////////////////////

	template <class T>
	using RaytracingPipelineArray = std::array<T, kCountOfRaytracingPipeline>;

	//=========================================================================================
	// private varaibles
	//=========================================================================================

	RaytracingPipelineArray<PipelineElement> pipelines_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	void CreateRaygeneration();
	void CreateMiss();
	void CreateHitgroup();

};