#include "ObjectDemo.h"

#include <Sxavenger.h>
#include <SxavengerGraphics.h>

void ObjectDemo::Init() {

	Sxavenger::LoadTexture("resources/uvChecker.png");
	Sxavenger::LoadTexture("resources/tile_black.png");

	transform_[0].Init(); //!< WorldTransform
	transform_[1].Init(); //!< WorldTransform
	material_.Init();  //!< ObjectColor

	{ // model

		model_ = std::make_unique<Model>("resources/model", "multiMaterial.obj");
		
	}

	Attribute::SetThisAttribute("demo");
}

void ObjectDemo::Draw() {

	SxavengerGraphics::DrawDefaultLightingTexture(
		model_->GetMesh(0),
		transform_[0], material_,
		model_->GetTextureHandle(TEXTURE_DIFFUSE)
	);

}

void ObjectDemo::SetAttributeImGui() {
	
	if (ImGui::TreeNode("transform[0]")) {
		transform_[0].SetImGuiCommand();
		ImGui::TreePop();
	}

	if (ImGui::TreeNode("transform[1]")) {
		transform_[1].transform.SetImGuiCommand();
		ImGui::TreePop();
	}
	
	if (ImGui::TreeNode("material")) {
		material_.SetImGuiCommand();
		ImGui::TreePop();
	}
	
	transform_[0].UpdateMatrix();
	transform_[1].UpdateMatrix();
	material_.Transfer();

	Collider::SetColliderImGuiCommand();
}
