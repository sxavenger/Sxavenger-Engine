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
	transform_.transform.scale = { 0.2f, 0.2f, 0.2f };
	transform_.UpdateMatrix();

	upVector_ = { 0.0f, 1.0f, 0.0f };
	//sBetaConsole->BreakPoint();
}

void RailCamera::Term() {
}

void RailCamera::Update() {

	
	if (t_ < 1.0f) {
		t_ += 0.02f * Performance::GetDeltaTime(s).time;
		t_ = std::fmod(t_, 1.0f);
	}

	Vector3f position = rail_->LoopCatmullRomPosition(t_);

	float nextT = std::fmod(t_ + 0.01f, 1.0f); //!< 次のframeでのt
	Vector3f nextPosition = rail_->LoopCatmullRomPosition(nextT);

	Vector3f direction = Normalize(nextPosition - position);

	Vector3f rotate = CalculateEuler(direction);
	//rotate.z = std::atan2(direction.y, direction.x);

	//* camera rotate up *//

	Vector3f right = Normalize(Cross(direction, upVector_));
	Vector3f newUp = Normalize(Cross(right, direction));

	upVector_ = newUp;

	camera_->SetTransform(kUnit3, rotate, position);

	// TODO: レールの上向きに合わせてカメラを回転

	transform_.transform.translate = direction * 2.0f + camera_->GetWorldPosition();
	transform_.UpdateMatrix();
}

void RailCamera::Draw() {
	SxavengerGame::DrawLine(
		camera_->GetWorldPosition(), camera_->GetWorldPosition() + upVector_,
		ToColor4f(0x00FA00FF)
	);
}

void RailCamera::SetAttributeImGui() {

	ImGui::SliderFloat("t", &t_, 0.0f, 1.0f);
	ImGui::Text("up vector x: %.2f, y: %.2f, z: %.2f", upVector_.x, upVector_.y, upVector_.z);

}
