#include "DxrExportGroup.h"

_DXROBJECT_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// ExportGroup class methods
////////////////////////////////////////////////////////////////////////////////////////////


void ExportGroup::CreateRaygeneration(const std::wstring& entryPoint) {
	Assert(!entryPoint.empty(), "ExportGroup: entry point is empty.");

	name_ = entryPoint;
	type_ = ExportGroupType::kExportGroup_Raygeneration;
}

void ExportGroup::CreateMiss(const std::wstring& entryPoint) {
	Assert(!entryPoint.empty(), "ExportGroup: entry point is empty.");

	name_ = entryPoint;
	type_ = ExportGroupType::kExportGroup_Miss;
}

void ExportGroup::CreateHitgroup(
	const std::wstring& subobjectName,
	const std::wstring& closestHit, const std::wstring& anyhit, const std::wstring& intersection) {
	Assert(!subobjectName.empty(), "ExportGroup: subobject name is empty.");
	Assert(!closestHit.empty(), "ExportGroup: closesthit netry point is empty.");

	name_ = subobjectName;
	type_ = ExportGroupType::kExportGroup_Hitgroup;

	entryPoints_ = HitgroupEntryPoints{
		closestHit, anyhit, intersection
	};
}




_DXROBJECT_NAMESPACE_END