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

////////////////////////////////////////////////////////////////////////////////////////////
// Player class methods
////////////////////////////////////////////////////////////////////////////////////////////

void Player::Load() {
	// modelの読み込み
	model_ = SxavengerAsset::TryImport<AssetModel>("assets/models/human/idle.gltf");

	// animatorの読み込み
	animators_[static_cast<uint8_t>(AnimationType::Idle)] = SxavengerAsset::TryImport<AssetAnimator>("assets/models/human/idle.gltf");
	animators_[static_cast<uint8_t>(AnimationType::Walk)] = SxavengerAsset::TryImport<AssetAnimator>("assets/models/human/walking.gltf");
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

	camera_ = std::make_unique<MonoBehaviour>();
	camera_->SetParent(this);
	camera_->SetName("camera");

	auto camera = camera_->AddComponent<CameraComponent>();
	camera->SetTag(CameraComponent::Tag::GameCamera);

	{
		auto process = camera_->AddComponent<PostProcessLayerComponent>();
		process->AddPostProcess<PostProcessBloom>();
		process->AddPostProcess<PostProcessDoF>();
	}

	{
		auto texture = SxavengerAsset::TryImport<AssetTexture>("assets/textures/LUT/lut_greenish.png");
		auto lut = camera_->AddComponent<CompositeProcessLayerComponent>()->AddPostProcess<CompositeProcessLUT>();
		lut->CreateTexture(SxavengerSystem::GetMainThreadContext(), texture, { 16, 16 });
	}

	auto transform = camera_->AddComponent<TransformComponent>();
	transform->translate.z = -3.0f;
	transform->translate.y = 1.5f;

	auto collider = MonoBehaviour::AddComponent<ColliderComponent>();
	collider->SetTag("player");

	CollisionBoundings::Capsule capsule = {};
	capsule.direction = { 0.0f, 1.0f, 0.0f };
	capsule.radius    = 0.5f;
	capsule.length    = 2.5f;

	collider->SetColliderBoundingCapsule(capsule);
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

	SxImGui::DragFloat("speed", &speed_, 0.01f, 0.0f);
}

void Player::SetCameraTarget(TransformComponent* target, const TimePointf<TimeUnit::second>& time) {
	target_ = target;
	time_ = time;
	timer_.Reset();
}

Vector2f Player::GetInputDirection() {

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

	// todo: gamepadのdirectionを追加

	return direction.Normalize();
}

void Player::Move() {

	Vector2f direction = GetInputDirection();

	if (All(direction == kOrigin2<float>)) {
		SetAnimationState(AnimationType::Idle);
		return;
	}

	SetAnimationState(AnimationType::Walk);

	transform_->translate += Vector3f{ direction.x, 0.0f, direction.y } * speed_;

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

	const Quaternion& quat = camera_->GetComponent<TransformComponent>()->rotate;

	if (timer_ >= time_) {
		camera_->GetComponent<TransformComponent>()->rotate = Quaternion::Slerp(quat, Quaternion::Identity(), 0.4f);
		return;
	}

	if (target_ == nullptr) {
		WarningRuntime("[Player]: nullptr warning", "target is nullptr");
		return;
	}

	timer_.AddDeltaTime();

	Vector3f direction = target_->GetPosition() - camera_->GetComponent<TransformComponent>()->GetPosition();
	direction = direction.Normalize();

	camera_->GetComponent<TransformComponent>()->rotate = Quaternion::Slerp(quat, CalculateDirectionQuaterion(direction), 0.08f);
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
