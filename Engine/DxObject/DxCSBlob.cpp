#include "DxCSBlob.h"

//=========================================================================================
// static variables
//=========================================================================================
DxObject::ShaderTable* DxObject::CSBlob::shaderTable_ = nullptr;

////////////////////////////////////////////////////////////////////////////////////////////
// CSBlob class
////////////////////////////////////////////////////////////////////////////////////////////

void DxObject::CSBlob::Init(const std::wstring& fileName) {
	assert(shaderTable_ != nullptr); //!< シェーダーテーブルの未設定

	// get shader blob
	csBlob_ = shaderTable_->GetShaderBlob(fileName, COMPUTE);
	assert(csBlob_ != nullptr);
}

void DxObject::CSBlob::Term() {
	csBlob_ = nullptr;
}
