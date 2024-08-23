#include "DxShaderNode.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* DxObject
#include <DxShaderBlobManager.h>

////////////////////////////////////////////////////////////////////////////////////////////
// GraphicsShaderNode class methods
////////////////////////////////////////////////////////////////////////////////////////////

void DxObject::GraphicsShaderNode::Create(const std::wstring& filename, GraphicShaderType type) {

	// typeの確認
	assert(type < GraphicShaderType::kCountOfGraphicShaderType);

	// nodeの取得
	nodes_[type] = manager_->GetBlob(filename, static_cast<ShaderType>(type));

	if (type == GraphicShaderType::GRAPHICS_MESH || type == GraphicShaderType::GRAPHICS_AMPLIFICATION) {
		isUseMeshPipeline_ = true;
	}

}
