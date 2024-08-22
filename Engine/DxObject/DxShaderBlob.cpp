#include "DxShaderBlob.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
// DxObject
#include <DxShaderBlobManager.h>


//=========================================================================================
// BaseShaderBlob class static variables
//=========================================================================================

DxObject::ShaderBlobManager* DxObject::BaseShaderBlob::manager_ = nullptr;

////////////////////////////////////////////////////////////////////////////////////////////
// GraphicBlob class methods
////////////////////////////////////////////////////////////////////////////////////////////

void DxObject::GraphicsBlob::Create(const std::wstring& filename, GraphicShaderType type) {
	// typeの確認
	assert(type < GraphicShaderType::kCountOfGraphicShaderType);

	graphicsBlobs_[type] = manager_->GetBlob(filename, static_cast<ShaderType>(type));

	if (type == GraphicShaderType::GRAPHICS_MESH || type == GraphicShaderType::GRAPHICS_AMPLIFICATION) {
		isUseMeshPipeline_ = true;
	}
}

void DxObject::GraphicsBlob::Term() {
	isUseMeshPipeline_ = false;
}

////////////////////////////////////////////////////////////////////////////////////////////
// CSBlob class methods
////////////////////////////////////////////////////////////////////////////////////////////

void DxObject::CSBlob::Create(const std::wstring& filename) {
	assert(blob_ == nullptr); //!< blobがある状態でもう一度取得しようとしてる

	blob_ = manager_->GetBlob(filename, COMPUTE);
}

void DxObject::CSBlob::Term() {
}