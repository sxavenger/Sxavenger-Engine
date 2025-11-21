#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/DirectX/DxrObject/DxrExportGroup.h>
#include <Engine/System/DirectX/DxrObject/DxrRaytracingBlob.h>
#include <Engine/System/DirectX/DxrObject/DxrStateObjectContext.h>
#include <Engine/System/Config/SxavengerConfig.h>
#include <Engine/Module/Pipeline/CustomComputePipeline.h>

//* externals
#include <magic_enum.hpp>

//* c++
#include <array>
#include <type_traits>

////////////////////////////////////////////////////////////////////////////////////////////
// FRenderCoreRestir class
////////////////////////////////////////////////////////////////////////////////////////////
class FRenderCoreRestir {
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
	public:

		//=========================================================================================
		// public variables
		//=========================================================================================

		uint32_t maxSampleCount  = 1024; //!< 合計sample数
		uint32_t samplesPerFrame = 1;    //!< frameごとのsample数
	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// Seed structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct Seed {
	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		Seed() { Set(); }

		void Set();

		//=========================================================================================
		// public variables
		//=========================================================================================

		std::array<uint32_t, 3> seed; //!< シード値

	};


	////////////////////////////////////////////////////////////////////////////////////////////
	// Reservoir structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct Reservoir {
	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		std::array<float, 16> data;
		float weight;
		float w;
		uint32_t m;

	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// Process enum class
	////////////////////////////////////////////////////////////////////////////////////////////
	enum class Process : uint8_t {
		Temporal,
		Spatial,
		Texture
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

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* export groups *//

	std::array<std::pair<DxrObject::RaytracingBlob, DxrObject::ExportGroup>, magic_enum::enum_count<RaygenerationExportType>()> raygenerationExportGroups_;
	std::array<std::pair<DxrObject::RaytracingBlob, DxrObject::ExportGroup>, magic_enum::enum_count<MissExportType>()>          missExportGroups_;
	std::array<std::pair<DxrObject::RaytracingBlob, DxrObject::ExportGroup>, magic_enum::enum_count<HitgroupExportType>()>      hitgroupExportGroups_;

	static inline const std::filesystem::path kDirectory_ = kPackagesShaderDirectory / "render/ReSTIR";

	//* context *//

	std::unique_ptr<DxrObject::StateObjectContext> context_;

	//* pipelines *//

	static inline const Vector3ui kThreadGroupSize_ = Vector3ui(16, 16, 1);

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

};
