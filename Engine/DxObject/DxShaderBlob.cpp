#include "DxShaderBlob.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include <Logger.h>

// DxObject
#include <DxCompilers.h>

//=========================================================================================
// static variables
//=========================================================================================
const std::wstring DxObject::ShaderBlob::directory_
	= L"./Resources/hlsl/";

DxObject::Compilers* DxObject::ShaderBlob::compilers_ = nullptr;

////////////////////////////////////////////////////////////////////////////////////////////
// ShaderBlob class methods
////////////////////////////////////////////////////////////////////////////////////////////

void DxObject::ShaderBlob::Init(const std::wstring& vsFileName, const std::wstring& psFileName) {

	// VS
	shaderBlob_VS_ = DxObjectMethod::CompileShader(
		directory_ + vsFileName, L"vs_6_0",
		compilers_->GetDxcUtils(), compilers_->GetDxcCompilder(), compilers_->GetIncluderHandler()
	);

	assert(shaderBlob_VS_ != nullptr);
	Log("[DxObject.ShaderBlob]: shaderBlob_VS_ << Complete Create \n");

	// PS
	shaderBlob_PS_ = DxObjectMethod::CompileShader(
		directory_ + psFileName, L"ps_6_0",
		compilers_->GetDxcUtils(), compilers_->GetDxcCompilder(), compilers_->GetIncluderHandler()
	);

	assert(shaderBlob_PS_ != nullptr);
	Log("[DxObject.ShaderBlob]: shaderBlob_PS_ << Complete Create \n");

}

void DxObject::ShaderBlob::Init(
	const std::wstring& vsFileName, const std::wstring& gsFileName, const std::wstring& psFileName) {

	// VS
	shaderBlob_VS_ = DxObjectMethod::CompileShader(
		directory_ + vsFileName, L"vs_6_0",
		compilers_->GetDxcUtils(), compilers_->GetDxcCompilder(), compilers_->GetIncluderHandler()
	);

	assert(shaderBlob_VS_ != nullptr);
	Log("[DxObject.ShaderBlob]: shaderBlob_VS_ << Complete Create \n");

	// GS
	shaderBlob_GS_ = DxObjectMethod::CompileShader(
		directory_ + gsFileName, L"gs_6_0",
		compilers_->GetDxcUtils(), compilers_->GetDxcCompilder(), compilers_->GetIncluderHandler()
	);

	assert(shaderBlob_GS_ != nullptr);
	Log("[DxObject.ShaderBlob]: shaderBlob_GS_ << Complete Create \n");

	// PS
	shaderBlob_PS_ = DxObjectMethod::CompileShader(
		directory_ + psFileName, L"ps_6_0",
		compilers_->GetDxcUtils(), compilers_->GetDxcCompilder(), compilers_->GetIncluderHandler()
	);

	assert(shaderBlob_PS_ != nullptr);
	Log("[DxObject.ShaderBlob]: shaderBlob_PS_ << Complete Create \n");

}

void DxObject::ShaderBlob::Term() {
}