#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/DirectX/DxrObject/DxrExportGroup.h>
#include <Engine/System/DirectX/DxrObject/DxrRaytracingBlob.h>

//* c++
#include <array>

////////////////////////////////////////////////////////////////////////////////////////////
// FSceneRenderer class
////////////////////////////////////////////////////////////////////////////////////////////
class FRenderCoreRaytracing {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// RaygenerationExportType enum class
	////////////////////////////////////////////////////////////////////////////////////////////
	enum class RaygenerationExportType : uint32_t {
		Default,
	};
	static const uint32_t kRaygenerationExportTypeCount = static_cast<uint32_t>(RaygenerationExportType::Default) + 1;

	////////////////////////////////////////////////////////////////////////////////////////////
	// MissExportType enum class
	////////////////////////////////////////////////////////////////////////////////////////////
	enum class MissExportType : uint32_t {
		Default,
	};
	static const uint32_t kMissExportTypeCount = static_cast<uint32_t>(MissExportType::Default) + 1;

	////////////////////////////////////////////////////////////////////////////////////////////
	// HitgroupExportType enum class
	////////////////////////////////////////////////////////////////////////////////////////////
	enum class HitgroupExportType : uint32_t {
		Geometry,
	};
	static const uint32_t kHitgroupExportTypeCount = static_cast<uint32_t>(HitgroupExportType::Geometry) + 1;

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	FRenderCoreRaytracing()  = default;
	~FRenderCoreRaytracing() = default;

	void Init();

	//* getter *//

	const DxrObject::ExportGroup& GetRaygenerationExport(RaygenerationExportType type) const { return raygenerationExportGroups_[static_cast<uint32_t>(type)].second; }

	const DxrObject::ExportGroup& GetMissExport(MissExportType type) const { return missExportGroups_[static_cast<uint32_t>(type)].second; }

	const DxrObject::ExportGroup& GetHitgroupExport(HitgroupExportType type) const { return hitgroupExportGroups_[static_cast<uint32_t>(type)].second; }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	std::array<std::pair<DxrObject::RaytracingBlob, DxrObject::ExportGroup>, kRaygenerationExportTypeCount> raygenerationExportGroups_;
	std::array<std::pair<DxrObject::RaytracingBlob, DxrObject::ExportGroup>, kMissExportTypeCount>          missExportGroups_;
	std::array<std::pair<DxrObject::RaytracingBlob, DxrObject::ExportGroup>, kHitgroupExportTypeCount>      hitgroupExportGroups_;

	static const std::filesystem::path kDirectory_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	void InitRaygenerationExportGroup();
	void InitMissExportGroup();
	void InitHitgroupExportGroup();

};
