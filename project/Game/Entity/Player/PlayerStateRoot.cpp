#include "PlayerStateRoot.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* player
#include "Player.h"

//* engine
#include <Engine/System/Runtime/Performance/Performance.h>

//* lib
#include <Lib/Geometry/VectorComparison.h>
#include <Lib/MyMath.h>

////////////////////////////////////////////////////////////////////////////////////////////
// PlayerStateRoot class methods
////////////////////////////////////////////////////////////////////////////////////////////

void PlayerStateRoot::Init() {
}

void PlayerStateRoot::Term() {
	player_->GetTransform().rotate = player_->target_; //!< キャラクターの向きを設定
}

void PlayerStateRoot::Update() {
	if (player_->gamepad_->IsConnect()) { //!< gamepadが接続されている
		MoveGamepad(); 
		RotateCameraGamepad();
		ActionGamepad();

	} else {
		MoveKeyboard();
	}

	player_->GetTransform().rotate = Slerp(player_->GetTransform().rotate, player_->target_, 0.2f);

	player_->time_ += SxavengerSystem::GetDeltaTime();
}

void PlayerStateRoot::RotateCameraGamepad() {
	if (!player_->gamepad_->IsConnect()) {
		return; //!< gamepadが接続されていない
	}

	// stickの入力を取得
	Vector2f direction = {};

	static const float deadzone = 0.1f;
	//!< value: 0.0f ~ 1.0f の間のdeadzoneを設定

	const Vector2f stick = player_->gamepad_->GetStickNormalized(GamepadStickId::STICK_RIGHT);

	const float active = std::abs(stick.x) + std::abs(stick.y);

	if (active >= deadzone) { //!< deadzoneと実際の入力値を比較
		direction = {
			-stick.x,
			stick.y
		};
	}

	player_->pivotRotation_ += direction * rotationDelta_;
}

void PlayerStateRoot::MoveGamepad() {
	if (!player_->gamepad_->IsConnect()) {
		return; //!< gamepadが接続されていない
	}

	static const float deadzone = 0.1f;
	//!< value: 0.0f ~ 1.0f の間のdeadzoneを設定
	// todo: parameter化

	const Vector2f stick = player_->gamepad_->GetStickNormalized(GamepadStickId::STICK_LEFT);

	const float active = std::abs(stick.x) + std::abs(stick.y);

	if (active < deadzone) { //!< deadzoneと実際の入力値を比較
		player_->SetAnimationState(Player::AnimationState::Idle); //!< 入力がない場合はidle状態に遷移
		return;
	}

	// 移動
	Vector3f direction = { stick.x, 0.0f, stick.y };

	static const Vector3f base = { 0.0f, 0.0f, -1.0f };
	direction = RotateVector(direction, MakeAxisAngle({0.0f, 1.0f, 0.0f}, player_->pivotRotation_.x));

	bool isRun = player_->gamepad_->IsPress(GamepadButtonId::BUTTON_A);

	if (isRun) {
		player_->GetTransform().translate += direction * runSpeed_ * SxavengerSystem::GetDeltaTime().time;
		player_->SetAnimationState(Player::AnimationState::Running);

	} else {
		player_->GetTransform().translate += direction * speed_ * SxavengerSystem::GetDeltaTime().time;
		player_->SetAnimationState(Player::AnimationState::Walking);
	}

	player_->target_ = ToQuaternion(CalculateEuler(direction));
}

void PlayerStateRoot::ActionGamepad() {
	if (!player_->gamepad_->IsConnect()) {
		return; //!< gamepadが接続されていない
	}

	if (player_->gamepad_->IsTrigger(GamepadButtonId::BUTTON_X)) {
		player_->requestState_ = std::make_unique<PlayerStateRolling>(player_);
	}

	if (player_->gamepad_->IsTrigger(GamepadButtonId::BUTTON_B)) {
		player_->requestState_ = std::make_unique<PlayerStatePunch>(player_);
	}
}

void PlayerStateRoot::MoveKeyboard() {
	// keyの入力を取得
	Vector2f direction = {};

	if (player_->keyboard_->IsPress(KeyId::KEY_W)) {
		direction.y += 1.0f;
	}

	if (player_->keyboard_->IsPress(KeyId::KEY_S)) {
		direction.y -= 1.0f;
	}

	if (player_->keyboard_->IsPress(KeyId::KEY_A)) {
		direction.x -= 1.0f;
	}

	if (player_->keyboard_->IsPress(KeyId::KEY_D)) {
		direction.x += 1.0f;
	}

	// 移動
	Vector2f move = direction * speed_;

	player_->GetTransform().translate += Vector3f(move.x, 0.0f, move.y) * SxavengerSystem::GetDeltaTime().time;

	// animation stateの処理
	if (Any(direction != kOrigin2<float>)) {
		player_->animationState_ = Player::AnimationState::Walking;
		player_->target_ = ToQuaternion(CalculateEuler({direction.x, 0.0f, direction.y}));

	} else {
		player_->animationState_ = Player::AnimationState::Idle;
	}
}

