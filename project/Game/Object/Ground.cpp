#include "Ground.h"

////////////////////////////////////////////////////////////////////////////////////////////
// Ground class methods
////////////////////////////////////////////////////////////////////////////////////////////

void Ground::Init() {
	AModelActor::Init();
	AModelActor::SetName("ground");
	AModelActor::SetModel(SxavengerAsset::TryImport<AssetModel>("asset/models/bricks/bricks.gltf"));

	float scale = 12.0f;

	TransformComponent::GetTransform().scale     = { scale, scale, scale };
	TransformComponent::GetTransform().translate = { 0.0f, -scale, 0.0f };
	TransformComponent::UpdateMatrix();

	TextureComponent::GetTransform2d().scale     = { scale * 2.0f, scale * 2.0f };
	TextureComponent::GetTransform2d().translate = { 0.0f, 0.0f };
	TextureComponent::Transfer();
}
