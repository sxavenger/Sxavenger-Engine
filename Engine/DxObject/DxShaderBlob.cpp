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

void DxObject::GraphicsBlob::Merge(GraphicsBlob* other) {

	auto& otherBlobs = other->GetGraphicsBlobs();

	for (uint32_t i = 0; i < GraphicShaderType::kCountOfGraphicShaderType; ++i) {
		if (otherBlobs[i] == nullptr) {
			continue; //!< ここのblobは存在してない
		}

		if (graphicsBlobs_[i] == nullptr) {
			// otherからのblobの登録
			graphicsBlobs_[i] = otherBlobs[i];
			continue;
		}

		assert(false); //!< blob同士の衝突
	}

	// todo: mesh pipelineのbool値での安全性の向上
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