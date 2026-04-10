#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* render
#include "FBaseRenderCore.h"

//* engine
#include <Engine/Foundation.h>
#include <Engine/System/Configuration/Configuration.h>
#include <Engine/System/DirectX/DxrObject/DxrExportGroup.h>
#include <Engine/System/DirectX/DxrObject/DxrRaytracingBlob.h>
#include <Engine/System/DirectX/DxrObject/DxrStateObjectContext.h>
#include <Engine/Module/Pipeline/CustomComputePipeline.h>

//* lib
#include <Lib/Adapter/Random/Random.h>

//* externals
#include <magic_enum.hpp>

//* c++
#include <array>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// FRenderCoreLuxGlobalIllumination class
////////////////////////////////////////////////////////////////////////////////////////////
class FRenderCoreLuxGlobalIllumination final
	: public FBaseRenderCore {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// RaygenerationExportType enum class
	////////////////////////////////////////////////////////////////////////////////////////////
	enum class RaygenerationExportType : uint32_t {
		Default,
	};
	static const size_t kRaygenerationExportCount = magic_enum::enum_count<RaygenerationExportType>();

	////////////////////////////////////////////////////////////////////////////////////////////
	// MissExportType enum class
	////////////////////////////////////////////////////////////////////////////////////////////
	enum class MissExportType : uint32_t {
		Default,
	};
	static const size_t kMissExportCount = magic_enum::enum_count<MissExportType>();

	////////////////////////////////////////////////////////////////////////////////////////////
	// HitgroupExportType enum class
	////////////////////////////////////////////////////////////////////////////////////////////
	enum class HitgroupExportType : uint32_t {
		Mesh,
		Emissive,
	};
	static const size_t kHitgroupExportCount = magic_enum::enum_count<HitgroupExportType>();

	////////////////////////////////////////////////////////////////////////////////////////////
	// Setting structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct Setting {
	public:

		//=========================================================================================
		// public variables
		//=========================================================================================

		Vector2ui downscale = { 16, 16 };   //!< 描画解像度のダウンスケール率.
		Vector2ui atlas     = { 8, 8 }; //!< 1区間のサイズ

	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// Process enum class
	////////////////////////////////////////////////////////////////////////////////////////////
	enum class Process : uint8_t {
		IrradianceCalculate,
		Solve,
	};
	static const size_t kProcessCount = magic_enum::enum_count<Process>();

public:

	//=========================================================================================
	// public variables
	//=========================================================================================

	void Init() override;

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

	void Dispatch(const DirectXQueueContext* context, const Vector2ui& resolution) const;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* directory *//

	static inline const std::filesystem::path kDirectory = kPackagesDirectory / "shaders" / "render" / "Lux";

	//* DirectX Raytracing *//

	std::array<std::pair<DxrObject::RaytracingBlob, DxrObject::ExportGroup>, kRaygenerationExportCount> raygenerationExportGroups_;
	std::array<std::pair<DxrObject::RaytracingBlob, DxrObject::ExportGroup>, kMissExportCount>          missExportGroups_;
	std::array<std::pair<DxrObject::RaytracingBlob, DxrObject::ExportGroup>, kHitgroupExportCount>      hitgroupExportGroups_;

	std::unique_ptr<DxrObject::StateObjectContext> context_;

	//* pipeline *//

	std::array<CustomReflectionComputePipeline, kProcessCount> pipelines_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	//* create helper methods *//

	void CreateRaygeneration();

	void CreateMiss();

	void CreateHitgroup();

	void CreateContext();

	void CreatePipeline();

	void CreateComputePipeline(Process process, const std::filesystem::path& filepath);

};

SXAVENGER_ENGINE_NAMESPACE_END
