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

	transform_.transform.scale = { 0.1f, 0.1f, 0.1f };
	transform_.UpdateMatrix();

	camera_ = sSystemConsole->GetGameCamera();

	rail_ = std::make_unique<Rail>();
	rail_->Init();
	SetChild(rail_.get());

	/*bullet_ = std::make_unique<PlayerBullet>();
	bullet_->Init();
	SetChild(bullet_.get());*/
}

void Player::Term() {
}

void Player::Update() {
	Move();
	transform_.UpdateMatrix();

	//bullet_->Update();

}

void Player::SetAttributeImGui() {
	ImGui::DragFloat("loop time", &loopTime_.time, 0.01f, 0.0f, 128.0f);
	ImGui::SliderFloat("loop timer", &loopTimer_.time, 0.0f, loopTime_.time);
}

void Player::Move() {
	Assert(rail_ != nullptr, "rail is nullptr.");

	// timeからrailの上の座標を取得
	++loopTimer_; //!< deltaTimeの加算
	//loopTimer_.time = std::fmod(loopTimer_.time, loopTime_.time); //!< ループする場合
	loopTimer_.time = std::min(loopTimer_.time, loopTime_.time);    //!< しない場合

	float t = loopTimer_.time / loopTime_.time;
	t = std::fmod(t, 1.0f);

	transform_.transform.translate = rail_->LoopCatmullRomPosition(t);

	// railから向きを計算
	float nextT = t + 0.01f;
	nextT = std::fmod(nextT, 1.0f);

	Vector3f nextPosition = rail_->LoopCatmullRomPosition(nextT);

	Vector3f direction = Normalize(nextPosition - transform_.transform.translate);
	Vector3f rotate = CalculateEuler(direction);

	transform_.transform.rotate = ToQuaternion(rotate);

	// 1人称視点なのでcameraも同じように更新

	// 同じ視点だとモデルにかぶるので調整
	const Vector3f offset = { 0.0f, 0.2f, -1.0f };

	camera_->SetTransform(kUnit3, rotate, transform_.transform.translate + Matrix::Transform(offset, Matrix::MakeRotate(rotate)));
}