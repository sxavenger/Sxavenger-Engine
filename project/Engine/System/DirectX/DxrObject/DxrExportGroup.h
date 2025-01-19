#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* DXROBJECT
#include "DxrObjectCommon.h"
#include "DxrRootSignature.h"

//* c++
#include <optional>
#include <string>

////////////////////////////////////////////////////////////////////////////////////////////
// DXROBJECT namespace
////////////////////////////////////////////////////////////////////////////////////////////
_DXROBJECT_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// ExportType enum class
////////////////////////////////////////////////////////////////////////////////////////////
enum class ExportType : uint8_t {
	Raygeneration,
	Miss,
	Hitgroup,
	//Callable
};
constexpr uint8_t kExportTypeCount = static_cast<uint8_t>(ExportType::Hitgroup) + 1;

////////////////////////////////////////////////////////////////////////////////////////////
// ExportGroup class
////////////////////////////////////////////////////////////////////////////////////////////
class ExportGroup
	: public LocalRootSignature {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// HitgroupEntry structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct HitgroupEntry {
		std::wstring closesthit   = L"";
		std::wstring anyhit       = L"";
		std::wstring intersection = L"";
	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	ExportGroup()  = default;
	~ExportGroup() = default;

	//* export options *//

	void ExportRaygeneration(const std::wstring& entry);

	void ExportMiss(const std::wstring& entry);

	void ExportHitgroup(const std::wstring& name, const HitgroupEntry& hitgroup);

	//* getter *//

	ExportType GetType() const;

	const std::wstring& GetName() const { return name_; }

	const HitgroupEntry& GetHitgroup() const { return hitgroup_; }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	std::wstring name_      = L"";
	HitgroupEntry hitgroup_ = {};

	std::optional<ExportType> type_ = std::nullopt;

};

_DXROBJECT_NAMESPACE_END
