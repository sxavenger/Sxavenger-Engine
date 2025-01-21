#include "AFloorActor.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/Asset/SxavengerAsset.h>

////////////////////////////////////////////////////////////////////////////////////////////
// AFloorActor class methods
////////////////////////////////////////////////////////////////////////////////////////////

void AFloorActor::Init() {
	TransformComponent::CreateBuffer();
	TextureComponent::CreateBuffer();

	ia_.Create(4, 2);
	UpdateInputAssembler();

	SetTexture(SxavengerAsset::TryImport<AssetTexture>("packages/textures/tile_black.png"));
}

void AFloorActor::Term() {
}

void AFloorActor::SetSize(const Vector2f& size) {
	size_ = size;
	UpdateInputAssembler();
	UpdateTextureComponent();
}

void AFloorActor::InspectorImGui() {
	ImGui::DragFloat2("size", &size_.x, 0.01f, 0.0f, 100.0f);
	UpdateInputAssembler();
	UpdateTextureComponent();
}

void AFloorActor::UpdateInputAssembler() {

	auto vertex = ia_.GetVertex();

	vertex->At(0).position = { -size_.x * 0.5f, 0.0f, -size_.y * 0.5f };
	vertex->At(0).texcoord = { 0.0f, 0.0f };
	vertex->At(0).normal   = { 0.0f, 1.0f, 0.0f };

	vertex->At(1).position = { size_.x * 0.5f, 0.0f, -size_.y * 0.5f };
	vertex->At(1).texcoord = { 1.0f, 0.0f };
	vertex->At(1).normal   = { 0.0f, 1.0f, 0.0f };

	vertex->At(2).position = { size_.x * 0.5f, 0.0f, size_.y * 0.5f };
	vertex->At(2).texcoord = { 1.0f, 1.0f };
	vertex->At(2).normal   = { 0.0f, 1.0f, 0.0f };

	vertex->At(3).position = { -size_.x * 0.5f, 0.0f, size_.y * 0.5f };
	vertex->At(3).texcoord = { 0.0f, 1.0f };
	vertex->At(3).normal   = { 0.0f, 1.0f, 0.0f };

	auto index = ia_.GetIndex();
	index->At(0) = { 0, 1, 2 };
	index->At(1) = { 0, 2, 3 };

}

void AFloorActor::UpdateTextureComponent() {
	TextureComponent::GetTransform2d().scale = { size_.x, size_.y };
	TextureComponent::Transfer();
}
