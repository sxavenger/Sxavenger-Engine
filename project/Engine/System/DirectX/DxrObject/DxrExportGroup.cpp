#include "DxrExportGroup.h"
SXAVENGER_ENGINE_USING
DXROBJECT_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/Utility/StreamLogger.h>

////////////////////////////////////////////////////////////////////////////////////////////
// ExportGroup class methods
////////////////////////////////////////////////////////////////////////////////////////////

void ExportGroup::ExportRaygeneration(const std::wstring& entry) {
	type_ = ExportType::Raygeneration;

	StreamLogger::AssertA(!entry.empty(), "entry point is empty.");
	name_ = entry;
}

void ExportGroup::ExportMiss(const std::wstring& entry) {
	type_ = ExportType::Miss;

	StreamLogger::AssertA(!entry.empty(), "entry point is empty.");
	name_ = entry;
}

void ExportGroup::ExportHitgroup(const std::wstring& name, const Hitgroup& hitgroup) {
	type_ = ExportType::Hitgroup;

	StreamLogger::AssertA(!name.empty(), "name is empty.");

	//!< shader entry point check
	StreamLogger::AssertA(!hitgroup.closesthit.empty(), "closesthit is empty.");

	if (hitgroup.type == D3D12_HIT_GROUP_TYPE_PROCEDURAL_PRIMITIVE) {
		StreamLogger::AssertA(!hitgroup.intersection.empty(), "intersection is empty.");
	}
	
	name_     = name;
	hitgroup_ = hitgroup;
}

ExportType ExportGroup::GetType() const {
	StreamLogger::AssertA(type_.has_value(), "type is not set.");
	return type_.value();
}
