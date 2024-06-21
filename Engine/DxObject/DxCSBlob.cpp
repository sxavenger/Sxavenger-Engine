#include "DxCSBlob.h"

//=========================================================================================
// static variables
//=========================================================================================

DxObject::ShaderManager* DxObject::CSBlob::manager_ = nullptr;

////////////////////////////////////////////////////////////////////////////////////////////
// CSBlob class
////////////////////////////////////////////////////////////////////////////////////////////

void DxObject::CSBlob::Init(const std::wstring& fileName) {
	assert(manager_ != nullptr); //!< シェーダーテーブルの未設定

	// get shader blob
	csBlob_ = manager_->GetBlob(fileName, COMPUTE);
	assert(csBlob_ != nullptr);
}

void DxObject::CSBlob::Term() {
	csBlob_ = nullptr;
}
