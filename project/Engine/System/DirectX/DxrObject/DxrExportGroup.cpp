#include "DxrExportGroup.h"
_DXROBJECT_USING

////////////////////////////////////////////////////////////////////////////////////////////
// ExportGroup class methods
////////////////////////////////////////////////////////////////////////////////////////////

void ExportGroup::ExportRaygeneration(const std::wstring& entry) {
	Assert(!entry.empty(), "entry point is empty.");
	name_ = entry;
	type_ = ExportType::Raygeneration;
}

void ExportGroup::ExportMiss(const std::wstring& entry) {
	Assert(!entry.empty(), "entry point is empty.");
	name_ = entry;
	type_ = ExportType::Miss;
}

void ExportGroup::ExportHitgroup(const std::wstring& name, const HitgroupEntry& hitgroup) {
	Assert(!name.empty(),                "name is empty.");
	Assert(!hitgroup.closesthit.empty(), "closesthit is empty.");
	name_     = name;
	hitgroup_ = hitgroup;
	type_     = ExportType::Hitgroup;
}

ExportType ExportGroup::GetType() const {
	Assert(type_.has_value(), "type is not set.");
	return type_.value();
}
