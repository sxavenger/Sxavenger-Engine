#include "Cube.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include <MyEngine.h>
#include <TextureManager.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Cube class methods
////////////////////////////////////////////////////////////////////////////////////////////

void Cube::Init(SubobjectManager* subobjectManager) {

	// blas 
	blas_ = std::make_unique<DxrObject::BottomLevelAS>();
	subobjectManager->GetBlasModelData(blas_.get(), L"cube", TYPE_CUBE);

	// texture
	MyEngine::LoadTexture("resources/bricks/bricks_color.png");
	MyEngine::LoadTexture("resources/bricks/bricks_normaldx.png");
	MyEngine::LoadTexture("resources/bricks/bricks_ambientOcclusion.png");

	blas_->SetHandle(2, MyEngine::GetTexture("resources/bricks/bricks_color.png")->GetGPUHandleSRV());
	blas_->SetHandle(3, MyEngine::GetTexture("resources/bricks/bricks_normaldx.png")->GetGPUHandleSRV());
	blas_->SetHandle(4, MyEngine::GetTexture("resources/bricks/bricks_ambientOcclusion.png")->GetGPUHandleSRV());

	SetThisAttribute("mappingCube");

}

void Cube::Term() {
}

void Cube::SetAttributeImGui() {

	transform_.SetImGuiCommand();
	worldMatrix_ = transform_.GetMatrix();

}
