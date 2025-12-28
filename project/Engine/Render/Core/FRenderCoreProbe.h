#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/Foundation.h>
#include <Engine/System/Configuration/Configuration.h>
#include <Engine/System/DirectX/DxrObject/DxrExportGroup.h>
#include <Engine/System/DirectX/DxrObject/DxrRaytracingBlob.h>
#include <Engine/System/DirectX/DxrObject/DxrStateObjectContext.h>
#include <Engine/Module/Pipeline/CustomComputePipeline.h>

//* lib
#include <Lib/Geometry/Vector2.h>
#include <Lib/Geometry/Vector3.h>

//* externals
#include <magic_enum.hpp>

//* c++
#include <array>
#include <type_traits>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// FRenderCoreProbe class
////////////////////////////////////////////////////////////////////////////////////////////
class FRenderCoreProbe {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// RaygenerationExportType enum class
	////////////////////////////////////////////////////////////////////////////////////////////
	enum class RaygenerationExportType : uint32_t {
		Default,
	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// MissExportType enum class
	////////////////////////////////////////////////////////////////////////////////////////////
	enum class MissExportType : uint32_t {
		Default,
	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// HitgroupExportType enum class
	////////////////////////////////////////////////////////////////////////////////////////////
	enum class HitgroupExportType : uint32_t {
		Mesh,
		Emissive,
	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// Config structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct Config {

		//=========================================================================================
		// public variables
		//=========================================================================================

		Vector3ui probeCount = Vector3ui{ 12, 12, 12 };
		uint32_t sampleCount = 16;
		Vector2ui resolution = Vector2ui{ 8, 8 };
		float probeOffset    = 4.0f;
		float hysteresis     = 0.85f;  // [0.85, 0.98]

	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// Process enum class
	////////////////////////////////////////////////////////////////////////////////////////////
	enum class Process : uint32_t {
		Update,
		Evaluate,
	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	void Init();

	//* export option *//

	const DxrObject::ExportGroup* GetExportGroup(RaygenerationExportType raygeneration) const;
	const DxrObject::ExportGroup* GetExportGroup(MissExportType miss) const;
	const DxrObject::ExportGroup* GetExportGroup(HitgroupExportType hitgroup) const;

	//* context option *//

	void UpdateShaderTable(const DxrObject::TopLevelAS* topLevelAS);

	DxrObject::StateObjectContext* GetContext() const { return context_.get(); }

	//* pipeline option *//

	void SetPipeline(Process process, const DirectXQueueContext* context) const;

	void BindComputeBuffer(Process process, const DirectXQueueContext* context, const DxObject::BindBufferDesc& desc) const;

	void Dispatch(const DirectXQueueContext* context, const Vector2ui& size) const;
	void Dispatch(const DirectXQueueContext* context, const Vector3ui& size) const;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* export groups *//

	std::array<std::pair<DxrObject::RaytracingBlob, DxrObject::ExportGroup>, magic_enum::enum_count<RaygenerationExportType>()> raygenerationExportGroups_;
	std::array<std::pair<DxrObject::RaytracingBlob, DxrObject::ExportGroup>, magic_enum::enum_count<MissExportType>()>          missExportGroups_;
	std::array<std::pair<DxrObject::RaytracingBlob, DxrObject::ExportGroup>, magic_enum::enum_count<HitgroupExportType>()>      hitgroupExportGroups_;

	static inline const std::filesystem::path kDirectory = kPackagesDirectory / "shaders" / "render" / "Probe";

	//* context *//

	std::unique_ptr<DxrObject::StateObjectContext> context_;

	//* pipelines *//

	static inline const Vector3ui kThreadGroupSize = Vector3ui(16, 16, 1);

	std::array<std::unique_ptr<CustomReflectionComputePipeline>, magic_enum::enum_count<Process>()> pipelines_;

	//=========================================================================================
	// private variables
	//=========================================================================================

	void CreateRaygeneration();
	void CreateMiss();
	void CreateHitgroup();
	void CreateContext();

	void CreatePipeline();
	void CreateComputePipeline(Process process, const std::filesystem::path& filepath);
	void CreateComputePipeline(Process process, const std::filesystem::path& filepath, const DxObject::SamplerBindDesc& desc);

};

SXAVENGER_ENGINE_NAMESPACE_END
