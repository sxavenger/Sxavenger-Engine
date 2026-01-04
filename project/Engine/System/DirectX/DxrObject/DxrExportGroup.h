#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* DXROBJECT
#include "DxrObjectCommon.h"
#include "DxrRootSignature.h"
#include "DxrRaytracingBlob.h"

//* c++
#include <optional>
#include <string>

////////////////////////////////////////////////////////////////////////////////////////////
// DXROBJECT namespace
////////////////////////////////////////////////////////////////////////////////////////////
DXROBJECT_NAMESPACE_BEGIN

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
	// Hitgroup structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct Hitgroup {
		D3D12_HIT_GROUP_TYPE type = D3D12_HIT_GROUP_TYPE_TRIANGLES;
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

	void ExportHitgroup(const std::wstring& name, const Hitgroup& hitgroup);

	void SetBlob(const RaytracingBlob* blob) { blob_ = blob; }

	//* getter *//

	ExportType GetType() const;

	const std::wstring& GetName() const { return name_; }

	const Hitgroup& GetHitgroup() const { return hitgroup_; }

	const RaytracingBlob* GetBlob() const { return blob_; }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	std::wstring name_ = L"";
	Hitgroup hitgroup_ = {};

	std::optional<ExportType> type_ = std::nullopt;

	const RaytracingBlob* blob_ = nullptr;

};

DXROBJECT_NAMESPACE_END
