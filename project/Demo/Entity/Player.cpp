#include "Player.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/SxavengerSystem.h>
#include <Engine/System/UI/SxImGui.h>
#include <Engine/Asset/SxavengerAsset.h>
//* component
#include <Engine/Component/Components/Armature/ArmatureComponent.h>
#include <Engine/Component/Components/Camera/CameraComponent.h>
#include <Engine/Component/Components/Collider/ColliderComponent.h>
#include <Engine/Component/Components/PostProcessLayer/PostProcessLayerComponent.h>
#include <Engine/Component/Components/CompositeProcessLayer/CompositeProcessLayerComponent.h>
#include <Engine/Component/ComponentHelper.h>

//* lib
#include <Lib/Geometry/VectorComparision.h>
#include <Lib/Easing.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Player class methods
////////////////////////////////////////////////////////////////////////////////////////////

void Player::Load() {
	// modelの読み込み
	model_ = SxavengerAsset::TryImport<AssetModel>("assets/models/human/idle.gltf");

	// animatorの読み込み
	animators_[static_cast<uint8_t>(AnimationType::Idle)] = SxavengerAsset::TryImport<AssetAnimator>("assets/models/human/idle.gltf");
	animators_[static_cast<uint8_t>(AnimationType::Walk)] = SxavengerAsset::TryImport<AssetAnimator>("assets/models/human/walking.gltf");
	animators_[static_cast<uint8_t>(AnimationType::Dash)] = SxavengerAsset::TryImport<AssetAnimator>("assets/models/human/dash.gltf");
}

void Player::Awake() {
	// inputの登録
	keyboard_ = SxavengerSystem::GetInput()->GetKeyboardInput();
	gamepad_  = SxavengerSystem::GetInput()->GetGamepadInput(0);

	// behaviourの設定
	MonoBehaviour::SetName("player");

	// component登録
	transform_ = MonoBehaviour::AddComponent<TransformComponent>();
	model_.WaitGet()->CreateSkinnedMeshBehaviour(this);

	camera_ = std::make_unique<PivotCameraActor>();
	camera_->Init();
	camera_->SetName("camera");

	auto camera = camera_->AddComponent<CameraComponent>();
	camera->SetTag(CameraComponent::Tag::None);

	{
		auto process = camera_->AddComponent<PostProcessLayerComponent>();
		process->AddPostProcess<PostProcessAutoExposure>();
		//process->AddPostProcess<PostProcessDoF>();
		process->AddPostProcess<PostProcessBloom>();
	}

	{
		//auto texture = SxavengerAsset::TryImport<AssetTexture>("assets/textures/LUT/lut_greenish.png");
		//auto lut = camera_->AddComponent<CompositeProcessLayerComponent>()->AddPostProcess<CompositeProcessLUT>();
		//lut->CreateTexture(SxavengerSystem::GetMainThreadContext(), texture, { 16, 16 });
	}

	auto transform = camera_->AddComponent<TransformComponent>();
	transform->translate.z = -3.0f;
	transform->translate.y = 1.5f;

}

void Player::Start() {
	// animation関係の設定
	SetAnimationState(AnimationType::Idle);
	UpdateArmature(); //!< animationを一度だけ更新しておく
}

void Player::Update() {
	Move();
	UpdateArmature();
	UpdateCamera();
}

void Player::Inspectable() {

	ImGui::Text("move");
	ImGui::Separator();

	SxImGui::DragFloat("walk speed", &walkspeed_, 0.01f, 0.0f);
	SxImGui::DragFloat("dash speed", &dashSpeed_, 0.01f, 0.0f);

	ImGui::Text("camera");
	SxImGui::DragVector3("offset", &offset_.x, 0.01f);
}

void Player::SetCameraTarget(TransformComponent* target, const TimePointf<TimeUnit::second>& time) {
	target_ = target;
	time_ = time;
	timer_.Reset();
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
		direction = Quaternion::RotateVector(direction, camera_->GetRotation());
		direction = Vector3f{ direction.x, 0.0f, direction.z }.Normalize();

		float speed = IsInputDush() ? dashSpeed_ : walkspeed_;

		velocity_ += direction * speed;

		if (velocity_.Length() >= speed) {
			velocity_ = velocity_.Normalize() * speed; // ダッシュ速度を制限
		}

		transform_->rotate = Quaternion::Slerp(transform_->rotate, CalculateDirectionQuaterion(direction), 0.2f);
	}

	transform_->translate += velocity_;
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
	const auto& animatorA = animators_[static_cast<uint8_t>(animationState_.type)].WaitGet()->GetAnimation(0);

	if (preAnimationState_.has_value()) {

		animationTransitionTime_.AddDeltaTime();

		float t = animationTransitionTime_.time / 0.5f;
		t = std::clamp(t, 0.0f, 1.0f);

		const auto& animatorB = animators_[static_cast<uint8_t>((*preAnimationState_).type)].WaitGet()->GetAnimation(0);

		ComponentHelper::ApplyAnimationTransition(
			this,
			animatorB, (*preAnimationState_).time, true,
			animatorA, animationState_.time, true,
			t
		);

		if (t >= 1.0f) {
			preAnimationState_ = std::nullopt;
		}

	} else {

		ComponentHelper::ApplyAnimation(
			this,
			animatorA, animationState_.time, true
		);

	}
}

void Player::UpdateCamera() {

	camera_->SetPoint(transform_->GetPosition() + offset_);

	const Quaternion& quat = camera_->GetComponent<TransformComponent>()->rotate;

	if (timer_ >= time_) {
		camera_->Update();
		return;
	}

	if (target_ == nullptr) {
		Logger::WarningRuntime("[Player]: nullptr warning", "target is nullptr");
		return;
	}

	timer_.AddDeltaTime();

	Vector3f direction = target_->GetPosition() - camera_->GetComponent<TransformComponent>()->GetPosition();
	direction = direction.Normalize();

	float t = std::lerp(0.0f, 0.4f, EaseOutCubic(timer_.time / time_.time));

	camera_->SetRotation(Quaternion::Slerp(quat, CalculateDirectionQuaterion(direction), t));
	camera_->Update();
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
