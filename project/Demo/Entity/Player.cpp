#include "Player.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/Component/ComponentHelper.h>
#include <Engine/Component/Components/Collider/ColliderComponent.h>
#include <Engine/Component/Components/MeshRenderer/SkinnedMeshRendererComponent.h>
#include <Engine/Preview/Asset/UAssetAnimation.h>
#include <Engine/Preview/Asset/UAssetParameter.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Player class methods
////////////////////////////////////////////////////////////////////////////////////////////

void Player::Load() {
	// modelの読み込み
	model_ = sUContentStorage->Import<UContentModel>("assets/models/human/idle.gltf");

	// animatorの読み込み
	animators_[static_cast<uint8_t>(AnimationType::Idle)] = sUContentStorage->Import<UContentAnimation>("assets/models/human/idle.gltf");
	animators_[static_cast<uint8_t>(AnimationType::Walk)] = sUContentStorage->Import<UContentAnimation>("assets/models/human/walking.gltf");
	animators_[static_cast<uint8_t>(AnimationType::Dash)] = sUContentStorage->Import<UContentAnimation>("assets/models/human/dash.gltf");
}

void Player::Awake() {
	// inputの登録
	keyboard_ = SxavengerSystem::GetInput()->GetKeyboardInput();
	gamepad_  = SxavengerSystem::GetInput()->GetGamepadInput(0);

	// behaviourの設定
	MonoBehaviour::SetName("player");

	// component登録
	transform_ = MonoBehaviour::AddComponent<TransformComponent>();
	ComponentHelper::CreateSkinnedMeshBehaviour(this, model_.WaitGet());

	auto collider = MonoBehaviour::AddComponent<ColliderComponent>();
	collider->SetTag("Player");
	collider->SetColliderBoundingAABB(
		CollisionBoundings::AABB{
			.min = { -0.5f, 0.0f, -0.5f },
			.max = { 0.5f, 2.0f, 0.5f }
		}
	);


}

void Player::Start() {
	transform_->translate = { 0.0f, 0.0f, 0.0f };
}

void Player::Update() {
	Move();
	UpdateArmature();
	UpdateMeshEnable();
}

void Player::Inspectable() {
	SerializeGuiFormatter<float>::DragScalar(walkspeed_, 0.01f, 0.0f, std::nullopt);
	SerializeGuiFormatter<float>::DragScalar(dashSpeed_, 0.01f, 0.0f, std::nullopt);
}

Vector2f Player::GetInputDirection() const {

	Vector2f direction = {};

	//* keyboard inputs *//

	if (keyboard_->IsPress(KeyId::KEY_W)) {
		direction.y += 1.0f;
	}

	if (keyboard_->IsPress(KeyId::KEY_S)) {
		direction.y -= 1.0f;
	}

	if (keyboard_->IsPress(KeyId::KEY_A)) {
		direction.x -= 1.0f;
	}

	if (keyboard_->IsPress(KeyId::KEY_D)) {
		direction.x += 1.0f;
	}

	//* gamepad inputs *//

	if (gamepad_->IsConnect()) {

		Vector2f stick  = gamepad_->GetStickNormalized(GamepadStickId::STICK_LEFT);
		float length    = stick.Length();
		float threshold = 0.2f;

		if (length >= threshold) {
			direction += stick;
		}
	}

	return direction.Normalize();
	// todo: stickの強さで速度変化
}

bool Player::IsInputDush() const {

	//* keyboard inputs *//

	if (keyboard_->IsPress(KeyId::KEY_LSHIFT)) {
		return true;
	}

	//* gamepad inputs *//

	if (gamepad_->IsConnect() && gamepad_->IsPress(GamepadButtonId::BUTTON_A)) {
		return true;
	}

	return false;
}

void Player::Move() {

	Vector2f input = GetInputDirection();

	if (All(input == kOrigin2<float>)) { //!< 入力がない場合

		Vector3f inverse = -velocity_;

		if (inverse.Length() > 0.01f) {
			velocity_ += inverse * 0.1f; // 徐々に減速

		} else {
			velocity_ = Vector3f{ 0.0f, 0.0f, 0.0f }; // 完全に停止
		}

	} else {

		Vector3f direction = { input.x, 0.0f, input.y };

		if (camera_ != nullptr) {
			direction = Quaternion::RotateVector(direction, camera_->GetRotation());
		}
	
		direction = Vector3f{ direction.x, 0.0f, direction.z }.Normalize();

		float speed = IsInputDush() ? dashSpeed_ : walkspeed_;

		velocity_ += direction * speed;

		if (velocity_.Length() >= speed) {
			velocity_ = velocity_.Normalize() * speed; // ダッシュ速度を制限
		}

		transform_->rotate = Quaternion::Slerp(transform_->rotate, CalculateDirectionQuaterion(direction), 0.2f);
	}

	transform_->translate += velocity_ * SxavengerSystem::GetDeltaTimef().time;
	transform_->UpdateMatrix();

	// animationの更新
	float length = velocity_.Length();

	if (length <= 0.01f) {
		SetAnimationState(AnimationType::Idle);

	} else if (length <= walkspeed_ + walkspeed_ * 0.1f) {
		SetAnimationState(AnimationType::Walk);

	} else {
		SetAnimationState(AnimationType::Dash);
	}
}

void Player::UpdateArmature() {

	animationState_.time.AddDeltaTime();
	UAssetParameter<UAssetAnimation> animationA = animators_[static_cast<uint8_t>(animationState_.type)].WaitGet()->GetAnimation(0);

	if (preAnimationState_.has_value()) {

		animationTransitionTime_.AddDeltaTime();

		float t = Saturate(static_cast<float>(animationTransitionTime_.time / 0.5));

		UAssetParameter<UAssetAnimation> animationB = animators_[static_cast<uint8_t>((*preAnimationState_).type)].WaitGet()->GetAnimation(0);

		ComponentHelper::ApplyAnimationTransition(
			this,
			animationB.WaitRequire()->GetAnimation(), (*preAnimationState_).time, true,
			animationA.WaitRequire()->GetAnimation(), animationState_.time, true,
			t
		);

		if (t >= 1.0f) {
			preAnimationState_ = std::nullopt;
		}

	} else {

		ComponentHelper::ApplyAnimation(
			this,
			animationA.WaitRequire()->GetAnimation(), animationState_.time, true
		);

	}
}

void Player::UpdateMeshEnable() {

	bool isEnable = true;

	if (camera_ != nullptr) {
		//!< 1人称視点の場合はmeshを非表示にする
		isEnable = (camera_->GetPerspective() != PerspectiveCamera::Perspective::FirstPerson);
	}

	ComponentHelper::ForEachBehaviour(this, [isEnable](MonoBehaviour* behaviour) {
		if (auto skinnedMesh = behaviour->GetComponent<SkinnedMeshRendererComponent>()) {
			skinnedMesh->SetEnable(isEnable);
		}
	});

}

void Player::SetAnimationState(AnimationType type) {
	if (type != animationState_.type) {
		preAnimationState_ = animationState_;
		animationTransitionTime_.Reset();
		animationState_.type = type;
		animationState_.time = { 0.0f };
	}
}

Quaternion Player::CalculateDirectionQuaterion(const Vector3f& direction) {
	Vector3f result = {};

	// ロール, ピッチ, ロー回転
	result.y = std::atan2(direction.x, direction.z);

	float length = Vector3f{ direction.x, 0.0f, direction.z }.Length();
	result.x = std::atan2(-direction.y, length);

	return Quaternion::ToQuaternion(result);
}

