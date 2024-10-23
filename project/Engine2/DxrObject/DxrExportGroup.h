#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* DxrObjectCommon
#include <DxrObjectCommon.h>

//* c++
#include <string>
#include <variant>

//* DxrObject
#include <DxrRootSignature.h>

////////////////////////////////////////////////////////////////////////////////////////////
// DxrObject
////////////////////////////////////////////////////////////////////////////////////////////
_DXROBJECT_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// ExportGroupType enum
////////////////////////////////////////////////////////////////////////////////////////////
enum ExportGroupType {
	kExport_Raygeneration,
	kExport_Miss,
	kExport_Hitgroup,

	kCountOfExportGroupType
};

////////////////////////////////////////////////////////////////////////////////////////////
// ExportGroup class
////////////////////////////////////////////////////////////////////////////////////////////
class ExportGroup {
public: //!< enviornment

	////////////////////////////////////////////////////////////////////////////////////////////
	// ExportEntryPointHitgroup structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct ExportEntryPointHitgroup {
		std::wstring closesthit;
		std::wstring anyhit;
		std::wstring intersection; //!< use for AABB geometrys
	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	ExportGroup() = default;

	//* base
	//! @param[in] name: subobjectの名前. (raygeneration, missはnameがEntryPointとなる)
	void Create(const std::wstring& name, ExportGroupType type);

	//* rootSignature
	void CreateRootSignature(const LocalRootSignatureDesc& desc);

	//* hitgroup export option *//

	//* entryPoint methods
	void SetEntryPointHitgroup(const std::wstring& closesthit, const std::wstring& anyhit = L"", const std::wstring& intersection = L"");

	const ExportEntryPointHitgroup& GetExportHitgroupEntryPoints() const;

	//* Getter *//

	const std::wstring& GetName() const { return name_; }

	const ExportGroupType GetExportGroupType() const { return type_; }

	const LocalRootSignature* GetLocalRootSignature() const { return rootSignature_.get(); }

private:


	//=========================================================================================
	// private variables
	//=========================================================================================

	//* export parameter *//

	std::wstring    name_ = L"";
	ExportGroupType type_ = kCountOfExportGroupType;

	//* export data *//

	std::variant<ExportEntryPointHitgroup> entryPoint_;
	std::unique_ptr<LocalRootSignature>    rootSignature_;

};

_DXROBJECT_NAMESPACE_END