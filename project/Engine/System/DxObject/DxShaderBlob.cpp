#include "DxShaderBlob.h"
_DXOBJECT_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
// DxObject
#include "DxShaderBlobManager.h"


//=========================================================================================
// BaseShaderBlob class static variables
//=========================================================================================

ShaderBlobManager* BaseShaderBlob::manager_ = nullptr;

////////////////////////////////////////////////////////////////////////////////////////////
// GraphicBlob class methods
////////////////////////////////////////////////////////////////////////////////////////////

void GraphicsBlob::Create(const std::wstring& filename, GraphicShaderType type) {
	// typeの確認
	Assert(type < GraphicShaderType::kCountOfGraphicShaderType);

	graphicsBlobs_[type] = manager_->GetBlob(filename, static_cast<ShaderType>(type));

	if (type == GraphicShaderType::GRAPHICS_MESH || type == GraphicShaderType::GRAPHICS_AMPLIFICATION) {
		isUseMeshPipeline_ = true;
	}
}

void GraphicsBlob::Term() {
	isUseMeshPipeline_ = false;
}

void GraphicsBlob::Merge(GraphicsBlob* other) {

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

		Assert(false); //!< blob同士の衝突
	}

	// todo: mesh pipelineのbool値での安全性の向上
}

////////////////////////////////////////////////////////////////////////////////////////////
// CSBlob class methods
////////////////////////////////////////////////////////////////////////////////////////////

void CSBlob::Create(const std::wstring& filename) {
	Assert(blob_ == nullptr); //!< blobがある状態でもう一度取得しようとしてる

	blob_ = manager_->GetBlob(filename, COMPUTE);
}

void CSBlob::Term() {
}