#include "RailCamera.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/Performance.h>
#include <Engine/Game/SxavengerGame.h>
#include <Engine/Beta/BetaConsole.h>

//* lib
#include <Lib/MyMath.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Rail class methods
////////////////////////////////////////////////////////////////////////////////////////////

void RailCamera::Init() {
	camera_ = sBetaConsole->GetGameCamera();

	rail_ = std::make_unique<Rail>();
	rail_->Init();

	SetToConsole("rail camera");
	SetChild(rail_.get());

	ModelBehavior::model_ = SxavengerGame::LoadModel("resources/model/CG2", "axis.obj");

	transform_.transform.translate = { 0.0f, 0.0f, 4.0f };
	transform_.UpdateMatrix();
}

void RailCamera::Term() {
}

void RailCamera::Update() {

	if (t_ < 1.0f) {
		t_ += 0.1f * Performance::GetDeltaTime(s).time;
		t_ = std::min(t_, 1.0f);
	}

	Vector3f position = CatmullRomPosition(rail_->GetPoints(), t_);

	float nextT = t_ + 0.1f * Performance::GetDeltaTime(s).time; //!< 次のframeでのt
	Vector3f direction = Normalize(CatmullRomPosition(rail_->GetPoints(), nextT) - position);

	camera_->SetTransform(kUnit3, CalculateEuler(direction), position);

	// TODO: レールの上向きに合わせてカメラを回転
}

void RailCamera::Draw() {
}

void RailCamera::SetAttributeImGui() {

	ImGui::SliderFloat("t", &t_, 0.0f, 1.0f);

}
