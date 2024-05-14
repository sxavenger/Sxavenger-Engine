#include "DxShaderBlob.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include <Logger.h>

// DxObject
#include <DxShaderTable.h>

//=========================================================================================
// static variables
//=========================================================================================

DxObject::ShaderTable* DxObject::ShaderBlob::shaderTable_ = nullptr;

////////////////////////////////////////////////////////////////////////////////////////////
// ShaderBlob class methods
////////////////////////////////////////////////////////////////////////////////////////////

void DxObject::ShaderBlob::Init(const std::wstring& vsFileName, const std::wstring& psFileName) {

	// VS
	shaderBlob_MS_ = shaderTable_->GetShaderBlob(vsFileName, ShaderType::Mesh);

	assert(shaderBlob_MS_ != nullptr);
	Log("[DxObject::ShaderBlob]: shaderBlob_MS_ << Complete Create");

	// PS
	shaderBlob_PS_ = shaderTable_->GetShaderBlob(psFileName, ShaderType::Pixel);

	assert(shaderBlob_PS_ != nullptr);
	Log("[DxObject.ShaderBlob]: shaderBlob_PS_ << Complete Create");

}

void DxObject::ShaderBlob::Term() {
	shaderBlob_MS_ = nullptr;
	shaderBlob_PS_ = nullptr;
}