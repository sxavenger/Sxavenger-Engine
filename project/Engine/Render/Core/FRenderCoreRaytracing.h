#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/DirectX/DxrObject/DxrExportGroup.h>
#include <Engine/System/DirectX/DxrObject/DxrRaytracingBlob.h>

//* c++
#include <array>
#include <type_traits>

////////////////////////////////////////////////////////////////////////////////////////////
// FRenderCoreRaytracing class
////////////////////////////////////////////////////////////////////////////////////////////
class FRenderCoreRaytracing {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// RaygenerationExportType enum class
	////////////////////////////////////////////////////////////////////////////////////////////
	enum class RaygenerationExportType : uint32_t {
		Default,
	};
	static inline constexpr uint32_t kRaygenerationExportTypeCount = static_cast<uint32_t>(RaygenerationExportType::Default) + 1;

	////////////////////////////////////////////////////////////////////////////////////////////
	// MissExportType enum class
	////////////////////////////////////////////////////////////////////////////////////////////
	enum class MissExportType : uint32_t {
		Default,
	};
	static inline constexpr uint32_t kMissExportTypeCount = static_cast<uint32_t>(MissExportType::Default) + 1;

	////////////////////////////////////////////////////////////////////////////////////////////
	// HitgroupExportType enum class
	////////////////////////////////////////////////////////////////////////////////////////////
	enum class HitgroupExportType : uint32_t {
		Mesh,
	};
	static inline constexpr uint32_t kHitgroupExportTypeCount = static_cast<uint32_t>(HitgroupExportType::Mesh) + 1;

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	FRenderCoreRaytracing()  = default;
	~FRenderCoreRaytracing() = default;

	void Init();

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	std::array<std::pair<DxrObject::RaytracingBlob, DxrObject::ExportGroup>, kRaygenerationExportTypeCount> raygenerationExportGroups_;
	std::array<std::pair<DxrObject::RaytracingBlob, DxrObject::ExportGroup>, kMissExportTypeCount>          missExportGroups_;
	std::array<std::pair<DxrObject::RaytracingBlob, DxrObject::ExportGroup>, kHitgroupExportTypeCount>      hitgroupExportGroups_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	void CreateRaygeneration();

	void CreateMiss();

	void CreateHitgroup();

	template <typename _Ty>
	static constexpr std::underlying_type_t<_Ty> GetIndex(const _Ty& _enum) {
		return static_cast<std::underlying_type_t<_Ty>>(_enum);
	}

};
