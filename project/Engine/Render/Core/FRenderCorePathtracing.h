#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/DirectX/DxrObject/DxrExportGroup.h>
#include <Engine/System/DirectX/DxrObject/DxrRaytracingBlob.h>
#include <Engine/System/Config/SxavengerConfig.h>

//* c++
#include <array>
#include <type_traits>

////////////////////////////////////////////////////////////////////////////////////////////
// FRenderCorePathtracing class
////////////////////////////////////////////////////////////////////////////////////////////
class FRenderCorePathtracing {
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
		Emissive,
	};
	static inline constexpr uint32_t kHitgroupExportTypeCount = static_cast<uint32_t>(HitgroupExportType::Emissive) + 1;

	////////////////////////////////////////////////////////////////////////////////////////////
	// Reservoir structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct Reservoir {
	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		void Init();

		void IncrimentFrame();

		void ResetFrame();

		//=========================================================================================
		// public variables
		//=========================================================================================

		uint32_t sampleCount;      //!< 合計sample数
		uint32_t frameSampleCount; //!< frameごとのsample数
		uint32_t currentFrame;     //!< 現在のframe

	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	FRenderCorePathtracing()  = default;
	~FRenderCorePathtracing() = default;

	void Init();

	const DxrObject::ExportGroup* GetExportGroup(RaygenerationExportType raygeneration) const;
	const DxrObject::ExportGroup* GetExportGroup(MissExportType miss) const;
	const DxrObject::ExportGroup* GetExportGroup(HitgroupExportType hitgroup) const;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	std::array<std::pair<DxrObject::RaytracingBlob, DxrObject::ExportGroup>, kRaygenerationExportTypeCount> raygenerationExportGroups_;
	std::array<std::pair<DxrObject::RaytracingBlob, DxrObject::ExportGroup>, kMissExportTypeCount>          missExportGroups_;
	std::array<std::pair<DxrObject::RaytracingBlob, DxrObject::ExportGroup>, kHitgroupExportTypeCount>      hitgroupExportGroups_;

	static inline const std::filesystem::path kDirectory_ = kPackagesShaderDirectory / "render/pathtracing";

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
