#include "DxrRaytracingShaderBlob.h"
_DXROBJECT_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* forward
#include <Engine/System/DxrObject/DxrRaytracingShaderBlobManager.h>

//=========================================================================================
// static variables
//=========================================================================================

RaytracingShaderBlobManager* RaytracingShaderBlob::manager_ = nullptr;

////////////////////////////////////////////////////////////////////////////////////////////
// RaytracingShaderBlob class methods
////////////////////////////////////////////////////////////////////////////////////////////

void RaytracingShaderBlob::Create(const std::wstring& filename) {
	blob_ = manager_->GetBlob(filename);
}

void RaytracingShaderBlob::Term() {
}

void RaytracingShaderBlob::SetExportGroup(const ExportGroup* exportGroup) {
	Assert(exportGroup != nullptr, "ExportGroup is nullptr.");
	exports_.emplace(exportGroup);
}

void RaytracingShaderBlob::SetRaytracingShaderBlobManager(RaytracingShaderBlobManager* manager) {
	manager_ = manager;
}

////////////////////////////////////////////////////////////////////////////////////////////
// RaytracingShaderBlobGroup class methods
////////////////////////////////////////////////////////////////////////////////////////////

void RaytracingShaderBlobGroup::Create() {
}

void RaytracingShaderBlobGroup::Term() {
}

void RaytracingShaderBlobGroup::SetRaytracingShaderBlob(const RaytracingShaderBlob* blob) {
	blobs_.emplace(blob);
}


