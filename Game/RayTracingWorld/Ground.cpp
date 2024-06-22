#include "Ground.h"

//=========================================================================================
// static variables
//=========================================================================================

static const std::string tiles_[2] = {
	"black", "white"
};

////////////////////////////////////////////////////////////////////////////////////////////
// Ground class methods
////////////////////////////////////////////////////////////////////////////////////////////

void Ground::Init() {
	// IA
	model_ = std::make_unique<Model>("./Resources/model", "Ground.obj");

	blas_ = std::make_unique<DxrObject::BottomLevelAS>();
	blas_->Create(
		model_->GetMeshData(0).vertexResource.get(), model_->GetMeshData(0).indexResource.get(),
		L"ground"
	);

	// 仮で設定
	blas_->SetHandle(2, MyEngine::GetTextureHandleGPU("resources/tile_" + tiles_[tileType_] + ".png"));

	SetThisAttribute("ground");

}

void Ground::Term() {
}

void Ground::SetAttributeImGui() {
	
	if (ImGui::BeginCombo("tile", tiles_[tileType_].c_str())) {
		for (int i = 0; i < 2; ++i) {
			bool isSelect = (i == tileType_);

			if (ImGui::Selectable(tiles_[i].c_str(), isSelect)) {
				tileType_ = static_cast<TileType>(i);
			}
		}
		ImGui::EndCombo();
	}

	blas_->SetHandle(2, MyEngine::GetTextureHandleGPU("resources/tile_" + tiles_[tileType_] + ".png"));

}
