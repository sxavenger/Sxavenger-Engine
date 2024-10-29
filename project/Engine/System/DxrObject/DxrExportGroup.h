#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* DXROBJECT
#include <Engine/System/DxrObject/DxrObjectCommon.h>
#include <Engine/System/DxrObject/DxrRootSignature.h>

//* c++
#include <string>
#include <optional>

////////////////////////////////////////////////////////////////////////////////////////////
// DXROBJECT namespace
////////////////////////////////////////////////////////////////////////////////////////////
_DXROBJECT_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// ExportGroupType enum class
////////////////////////////////////////////////////////////////////////////////////////////
enum class ExportGroupType : uint32_t {
	kExportGroup_Raygeneration,
	kExportGroup_Miss,
	kExportGroup_Hitgroup,
	kCountOfExportGroupType
};

////////////////////////////////////////////////////////////////////////////////////////////
// ExportGroup class
////////////////////////////////////////////////////////////////////////////////////////////
class ExportGroup {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// HitgroupEntryPoints structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct HitgroupEntryPoints {
		std::wstring closesthit;
		std::wstring anyhit;
		std::wstring intersection; //!< using for "AABB" Geometry
	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	ExportGroup()  = default;
	~ExportGroup() = default;

	//* Create Exports *//

	void CreateRaygeneration(const std::wstring& entryPoint);

	void CreateMiss(const std::wstring& entryPoint);

	void CreateHitgroup(
		const std::wstring& subobjectName,
		const std::wstring& closestHit, const std::wstring& anyhit = L"", const std::wstring& intersection = L""
	);

	//* Create RootSignature *//

	void CreateRootSignature(const LocalRootSignatureDesc& desc);

	//* getter *//

	const ExportGroupType GetExportType() const { return type_; }

	const std::wstring& GetName() const { return name_; }

	const HitgroupEntryPoints& GetExportHitgroupEntryPoint() const;

	LocalRootSignature* GetLocalRootSignature() const { return rootSignature_.get(); }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* parameter *//

	std::wstring    name_ = L"";
	ExportGroupType type_ = ExportGroupType::kCountOfExportGroupType;

	std::optional<HitgroupEntryPoints> entryPoints_ = std::nullopt;

	//* member *//

	std::unique_ptr<LocalRootSignature> rootSignature_;

};

_DXROBJECT_NAMESPACE_END