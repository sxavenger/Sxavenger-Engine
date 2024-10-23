#include "DxrShaderBlob.h"
_DXROBJECT_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* externals
#include <DxrShaderBlobManager.h>

//=========================================================================================
// static variables
//=========================================================================================

ShaderBlobManager* ShaderBlob::manager_ = nullptr;

////////////////////////////////////////////////////////////////////////////////////////////
// ShaderBlob class methods
////////////////////////////////////////////////////////////////////////////////////////////

void ShaderBlob::Create(const std::wstring& filename) {
	blob_ = manager_->GetBlob(filename);
}

void ShaderBlob::SetEntryPointExport(const ExportGroup* exportGroup) {
	Assert(exportGroup != nullptr);
	exports_.emplace_back(exportGroup);
}
