#include "Player.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/Game/SxavengerGame.h>
#include <Engine/Console/SystemConsole.h>

//* lib
#include <Lib/MyMath.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Player class methods
////////////////////////////////////////////////////////////////////////////////////////////

void Player::Init() {
	SetName("player");

	model_ = SxavengerGame::LoadModel("resources/model/CG2", "axis.obj");
	model_->ApplyRaytracing();
	CreateRaytracingRecorder();

	transform_.transform.scale = { 0.1f, 0.1f, 0.1f };
	transform_.UpdateMatrix();

	camera_ = sSystemConsole->GetGameCamera();
	camera_->SetProjection({ 16.0f, 9.0f }, 12.9f, 0.01f, 1000.0f);

	rail_ = std::make_unique<Rail>();
	rail_->Init();
	SetChild(rail_.get());

	score_ = std::make_unique<Score>();
	score_->Init();
	SetChild(score_.get());

	bullet_ = std::make_unique<PlayerBullet>();
	bullet_->Init();
	bullet_->SetScore(score_.get());
	SetChild(bullet_.get());

	ModelBehavior::renderingFlag_ = kBehaviorRender_Raytracing | kBehaviorRender_Systematic;
}

void Player::Term() {
}

void Player::Update() {
	Move();
	transform_.UpdateMatrix();

	Shot();
}

void Player::SetAttributeImGui() {

	ImGui::CheckboxFlags("isVisible", &(ModelBehavior::renderingFlag_), kBehaviorRender_Raytracing | kBehaviorRender_Systematic);

	ImGui::DragFloat("loop time", &loopTime_.time, 0.01f, 0.0f, 128.0f);
	ImGui::SliderFloat("loop timer", &loopTimer_.time, 0.0f, loopTime_.time);

	ImGui::DragFloat3("camera offset", &cameraOffset_.x, 0.01f);
}

void Player::Move() {
	Assert(rail_ != nullptr, "rail is nullptr.");

	// timeからrailの上の座標を取得
	loopTimer_.AddDeltaTime();
	loopTimer_.time = std::fmod(loopTimer_.time, loopTime_.time); //!< ループする場合
	//loopTimer_.time = std::min(loopTimer_.time, loopTime_.time);    //!< しない場合

	float t = loopTimer_.time / loopTime_.time;
	t = std::fmod(t, 1.0f);

	transform_.transform.translate = rail_->LoopCatmullRomPosition(t);

	// railから向きを計算
	float nextT = t + 0.01f;
	nextT = std::fmod(nextT, 1.0f);

	Vector3f nextPosition = rail_->LoopCatmullRomPosition(nextT);

	direction_ = Normalize(nextPosition - transform_.transform.translate);
	Vector3f rotate = CalculateEuler(direction_);

	transform_.transform.rotate = ToQuaternion(rotate);

	// 1人称視点なのでcameraも同じように更新

	camera_->SetTransform(kUnit3, rotate, transform_.transform.translate + Matrix::Transform(cameraOffset_, Matrix::MakeRotate(rotate)));
}

void Player::Shot() {

	Matrix4x4 invViewProj = sSystemConsole->GetGameCamera()->GetViewProjMatrix().Inverse();

	Vector2f mouse = Sxavenger::GetInput()->GetMousePos();
	mouse = {
		mouse.x / kWindowSize.x * 2.0f - 1.0f,
		mouse.y / kWindowSize.y * 2.0f - 1.0f,
	};

	Vector3f direction = Matrix::Transform({ mouse.x, -mouse.y, 1.0f }, invViewProj);

	bullet_->Update();
	bullet_->Shoot(Sxavenger::IsPressKey(DIK_SPACE), transform_.GetWorldPosition(), direction);




}
