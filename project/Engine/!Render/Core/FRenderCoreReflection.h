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

//* externals
#include <magic_enum.hpp>

//* c++
#include <array>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// FRenderCoreReflection class
////////////////////////////////////////////////////////////////////////////////////////////
class FRenderCoreReflection {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// RaygenerationExportType enum class
	////////////////////////////////////////////////////////////////////////////////////////////
	enum class RaygenerationExportType : uint8_t {
		Default,
	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// MissExportType enum class
	////////////////////////////////////////////////////////////////////////////////////////////
	enum class MissExportType : uint8_t {
		Default,
	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// HitgroupExportType enum class
	////////////////////////////////////////////////////////////////////////////////////////////
	enum class HitgroupExportType : uint8_t {
		Mesh,
		Emissive,
	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// Process enum class
	////////////////////////////////////////////////////////////////////////////////////////////
	enum class Process : uint8_t {
		Calculate,
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

	static inline const std::filesystem::path kDirectory = kPackagesDirectory / "shaders" / "render" / "Reflection";

	//* export groups *//

	std::array<std::pair<DxrObject::RaytracingBlob, DxrObject::ExportGroup>, magic_enum::enum_count<RaygenerationExportType>()> raygenerationExportGroups_;
	std::array<std::pair<DxrObject::RaytracingBlob, DxrObject::ExportGroup>, magic_enum::enum_count<MissExportType>()>          missExportGroups_;
	std::array<std::pair<DxrObject::RaytracingBlob, DxrObject::ExportGroup>, magic_enum::enum_count<HitgroupExportType>()>      hitgroupExportGroups_;

	//* context *//

	std::unique_ptr<DxrObject::StateObjectContext> context_;

	//* pipelines *//

	static inline const Vector3ui kThreadGroupSize = Vector3ui(16, 16, 1);

	std::array<std::unique_ptr<CustomReflectionComputePipeline>, magic_enum::enum_count<Process>()> pipelines_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	void CreateRaygeneration();

	void CreateMiss();

	void CreateHitgroup();

	void CreateContext();

	//* helper method *//

	void CreateComputePipeline(Process process, const std::filesystem::path& filepath);

};

SXAVENGER_ENGINE_NAMESPACE_END
