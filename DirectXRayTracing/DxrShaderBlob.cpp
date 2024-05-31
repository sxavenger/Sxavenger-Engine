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

void DxrObject::ShaderBlob::Create(const std::wstring& filePath) {

	assert(manager_ != nullptr); //!< shaderTableが設定されてない

	blob_ = manager_->GetShaderBlob(filePath);
}

void DxrObject::ShaderBlob::SetShader(const std::wstring& mainFunctionName, ShaderType type, const std::wstring hitgroup) {
	
	if (type == ShaderType::CLOSESTHIT_SHADER) {
		assert(!hitgroup.empty()); //!< hitgroupを使用する関数なのにnameが空
	}

	datas_.push_back({ mainFunctionName, type, hitgroup });
}

void DxrObject::ShaderBlob::Term() {
	datas_.clear();
}