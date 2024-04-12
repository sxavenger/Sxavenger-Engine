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
	shaderBlob_VS_ = shaderTable_->GetShaderBlob(vsFileName, ShaderType::Vertex);

	assert(shaderBlob_VS_ != nullptr);
	Log("[DxObject.ShaderBlob]: shaderBlob_VS_ << Complete Create \n");

	// PS
	shaderBlob_PS_ = shaderTable_->GetShaderBlob(psFileName, ShaderType::Pixel);

	assert(shaderBlob_PS_ != nullptr);
	Log("[DxObject.ShaderBlob]: shaderBlob_PS_ << Complete Create \n");

}

void DxObject::ShaderBlob::Init(
	const std::wstring& vsFileName, const std::wstring& gsFileName, const std::wstring& psFileName) {

	// VS
	shaderBlob_VS_ = shaderTable_->GetShaderBlob(vsFileName, ShaderType::Vertex);

	// GS
	shaderBlob_GS_ = shaderTable_->GetShaderBlob(gsFileName, ShaderType::Geometry);

	// PS
	shaderBlob_PS_ = shaderTable_->GetShaderBlob(psFileName, ShaderType::Pixel);

}

void DxObject::ShaderBlob::Term() {
	shaderBlob_VS_ = nullptr;
	shaderBlob_GS_ = nullptr;
	shaderBlob_PS_ = nullptr;
}