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

	points_ = {
		kOrigin3,
		kUnit3,
		{0.0f, 2.0f, 1.0f},
		{3.0f, 2.0f, 0.0f}
	};

	SetToConsole("rail camera");

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

	Vector3f position = CatmullRomPosition(points_, t_);

	// TODO: レールの先に向かせたい
	float nextT = t_ + 0.1f * Performance::GetDeltaTime(s).time; //!< 次のframeでのt
	Vector3f direction = Normalize(CatmullRomPosition(points_, nextT) - position);

	camera_->SetTransform(kUnit3, CalculateEuler(direction), position);
}

void RailCamera::Draw() {
	DrawCatmullrom(100);
}

void RailCamera::SetAttributeImGui() {

	ImGui::Text("member");
	ImGui::Separator();

	ImGui::SliderFloat("t", &t_, 0.0f, 1.0f);

	ImGui::Text("parameter");
	ImGui::Separator();

	for (uint32_t i = 0; i < points_.size(); ++i) {
		std::string label = "position ##" + std::format("{:p}", reinterpret_cast<void*>(&points_[i]));
		ImGui::DragFloat3(label.c_str(), &points_[i].x, 0.01f);
	}
}

void RailCamera::DrawCatmullrom(uint32_t kSubdivision) {
	for (uint32_t i = 0; i < kSubdivision; ++i) {

		float t = static_cast<float>(i) / kSubdivision;
		float nextT = static_cast<float>(i + 1) / kSubdivision;

		SxavengerGame::DrawLine(
			CatmullRomPosition(points_, t),
			CatmullRomPosition(points_, nextT),
			ToColor4f(0xFA0000FF)
		);
	}
}
