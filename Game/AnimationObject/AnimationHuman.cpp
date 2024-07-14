#include "AnimationHuman.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
// engine
#include <Performance.h>
#include <MyEngine.h>

// primitive
#include <PrimitiveDrawer.h>

////////////////////////////////////////////////////////////////////////////////////////////
// AnimationHuman class methods
////////////////////////////////////////////////////////////////////////////////////////////

void AnimationHuman::Init() {

	CreateGraphicsPipeline();

	/*
	 simpleSkin: "./Resources/model/simpleSkin", "simpleSkin.gltf"
	 human:      "./resources/model/human", "walk.gltf"
	*/

	model_ = std::make_unique<Model>("./Resources/model/simpleSkin", "simpleSkin.gltf");
	animation_ = ModelMethods::LoadAnimationFile("./resources/model/human", "walk.gltf");
	skeleton_ = ModelMethods::CreateSkeleton(model_->GetRootNode());

	SetThisAttribute("human");

}

void AnimationHuman::Term() {
}

void AnimationHuman::Update() {

	animationTime_ += Performance::GetDeltaTime(SecondsUnit::s);
	animationTime_ = std::fmod(animationTime_, animation_.duration);

	skeleton_.Update(animation_, animationTime_);

}

void AnimationHuman::Draw() {

	auto drawer = PrimitiveDrawer::GetInstance();

	const float radius = 0.2f;

	for (auto& joint : skeleton_.joints) {
		Vector3f pos = Matrix::Transform({0.0f, 0.0f, 0.0f}, joint.localMatrix);

		drawer->DrawSphere(pos, radius, 8, {0.0f, 1.0f, 0.0f, 1.0f});
	}

	drawer->DrawAll3D();

}

void AnimationHuman::SetAttributeImGui() {

	transform_.SetImGuiCommand();

}
