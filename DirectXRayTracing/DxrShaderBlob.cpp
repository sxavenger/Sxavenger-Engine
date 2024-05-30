#include "DxrShaderBlob.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include <cassert>

#include <DxrShaderManager.h>

//=========================================================================================
// static variables
//=========================================================================================

DxrObject::ShaderManager* DxrObject::ShaderBlob::manager_ = nullptr;

////////////////////////////////////////////////////////////////////////////////////////////
// ShaderBlob class methods
////////////////////////////////////////////////////////////////////////////////////////////

void DxrObject::ShaderBlob::Init(
	const std::wstring& filePath,
	const std::wstring& raygeneration, const std::wstring& closesthit, const std::wstring& miss) {

	assert(manager_ != nullptr); //!< shaderTableが設定されてない

	blob_ = manager_->GetShaderBlob(filePath);

	mainFuncionName_[RAYGENERATION_SHAEAR] = raygeneration;
	mainFuncionName_[CLOSESTHIT_SHADER]    = closesthit;
	mainFuncionName_[MISS_SHADER]          = miss;
}

void DxrObject::ShaderBlob::Create(const std::wstring& filePath) {
	blob_ = manager_->GetShaderBlob(filePath);
}

void DxrObject::ShaderBlob::SetShader(const std::wstring& mainFunctionName, ShaderType type) {
}

void DxrObject::ShaderBlob::Term() {
}