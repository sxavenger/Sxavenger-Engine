#include "DxrExportGroup.h"
DXROBJECT_USING

////////////////////////////////////////////////////////////////////////////////////////////
// ExportGroup class methods
////////////////////////////////////////////////////////////////////////////////////////////

void ExportGroup::ExportRaygeneration(const std::wstring& entry) {
	type_ = ExportType::Raygeneration;

	Exception::Assert(!entry.empty(), "entry point is empty.");
	name_ = entry;
}

void ExportGroup::ExportMiss(const std::wstring& entry) {
	type_ = ExportType::Miss;

	Exception::Assert(!entry.empty(), "entry point is empty.");
	name_ = entry;
}

void ExportGroup::ExportHitgroup(const std::wstring& name, const Hitgroup& hitgroup) {
	type_ = ExportType::Hitgroup;

	Exception::Assert(!name.empty(), "name is empty.");

	//!< shader entry point check
	Exception::Assert(!hitgroup.closesthit.empty(), "closesthit is empty.");

	if (hitgroup.type == D3D12_HIT_GROUP_TYPE_PROCEDURAL_PRIMITIVE) {
		Exception::Assert(!hitgroup.intersection.empty(), "intersection is empty.");
	}
	
	name_     = name;
	hitgroup_ = hitgroup;
}

ExportType ExportGroup::GetType() const {
	Exception::Assert(type_.has_value(), "type is not set.");
	return type_.value();
}
