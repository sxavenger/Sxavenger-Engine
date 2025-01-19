#include "DxrExportGroup.h"
_DXROBJECT_USING

////////////////////////////////////////////////////////////////////////////////////////////
// ExportGroup class methods
////////////////////////////////////////////////////////////////////////////////////////////

void ExportGroup::ExportRaygeneration(const std::wstring& entry) {
	type_ = ExportType::Raygeneration;

	Assert(!entry.empty(), "entry point is empty.");
	name_ = entry;
}

void ExportGroup::ExportMiss(const std::wstring& entry) {
	type_ = ExportType::Miss;

	Assert(!entry.empty(), "entry point is empty.");
	name_ = entry;
}

void ExportGroup::ExportHitgroup(const std::wstring& name, const Hitgroup& hitgroup) {
	type_ = ExportType::Hitgroup;

	Assert(!name.empty(), "name is empty.");

	//!< shader entry point check
	Assert(!hitgroup.closesthit.empty(), "closesthit is empty.");

	if (hitgroup.type == D3D12_HIT_GROUP_TYPE_PROCEDURAL_PRIMITIVE) {
		Assert(!hitgroup.intersection.empty(), "intersection is empty.");
	}
	
	name_     = name;
	hitgroup_ = hitgroup;
}

ExportType ExportGroup::GetType() const {
	Assert(type_.has_value(), "type is not set.");
	return type_.value();
}
