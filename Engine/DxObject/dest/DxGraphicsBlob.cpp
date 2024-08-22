#include "DxGraphicsBlob.h"

//=========================================================================================
// public methods
//=========================================================================================

DxObject::ShaderManager* DxObject::GraphicsBlob::manager_ = nullptr;

////////////////////////////////////////////////////////////////////////////////////////////
// GraphicBlob class methods
////////////////////////////////////////////////////////////////////////////////////////////

void DxObject::GraphicsBlob::Create(const std::wstring& fileName, GraphicShaderType type) {
	// typeの確認
	assert(type < GraphicShaderType::kCountOfGraphicShaderType);
	
	graphicsBlobs_[type] = manager_->GetBlob(fileName, static_cast<ShaderType>(type));

	if (type == GraphicShaderType::GRAPHICS_MESH) {
		isUseMeshPipeline_ = true;
	}
}

void DxObject::GraphicsBlob::Term() {
	isUseMeshPipeline_ = false;
}
