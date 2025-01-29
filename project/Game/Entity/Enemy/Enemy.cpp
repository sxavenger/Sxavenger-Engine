#include "Enemy.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* lib
#include <Lib/MyMath.h>

//* game
#include "../Player/PlayerAttackCollider.h"

////////////////////////////////////////////////////////////////////////////////////////////
// Enemy class methods
////////////////////////////////////////////////////////////////////////////////////////////

void Enemy::Init() {

	AModelAnimationActor::SetName("enemy");
	AModelAnimationActor::SetTransparency(AGeometryActor::Transparency::Opaque);

	AModelAnimationActor::SetModel(SxavengerAsset::TryImport<AssetModel>("asset/model/sample/idle.gltf"));
	AModelAnimationActor::Init();

	//* visual *//

	texture_ = SxavengerAsset::TryImport<AssetTexture>("asset/textures/pattern_camo.jpg");

	//* animation *//

	animators_[AnimationState::FightingIdle]   = SxavengerAsset::TryImport<AssetAnimator>("asset/model/sample/fighting_idle.gltf");
	animators_[AnimationState::Approach]       = SxavengerAsset::TryImport<AssetAnimator>("asset/model/sample/approach.gltf");
	animators_[AnimationState::Straight]       = SxavengerAsset::TryImport<AssetAnimator>("asset/model/sample/straight.gltf");
	animators_[AnimationState::ReactionLight]  = SxavengerAsset::TryImport<AssetAnimator>("asset/model/sample/reaction_light.gltf");
	animators_[AnimationState::ReactionHeavy]  = SxavengerAsset::TryImport<AssetAnimator>("asset/model/sample/reaction_heavy.gltf");
	animators_[AnimationState::Knock]          = SxavengerAsset::TryImport<AssetAnimator>("asset/model/sample/knock.gltf");

	//* state *//

	state_ = std::make_unique<EnemyStateRoot>(this);
	state_->Init(); 

	//* collider *//

	hitCollider_ = std::make_unique<Collider>();
	hitCollider_->SetToCollection();

	hitCollider_->SetColliderBoundingCapsule({
		{0.0f, 1.0f, 0.0f}, //!< direction
		0.3f, 1.0f //!< radius, height
	});

	hitCollider_->SetParent(this);
	hitCollider_->GetTransform().translate = { 0.0f, 1.0f, 0.0f };
	hitCollider_->SetTypeId(ColliderType::kEnemy);
	hitCollider_->SetTargetId(ColliderType::kPlayerAttack);
	hitCollider_->SetOnCollisionEnter([this](_MAYBE_UNUSED Collider* const target) {
			OnCollisionEnter(target);
		}
	);

	isDead_ = false;
}

void Enemy::Init(const QuaternionTransform& transform) {
	Init();

	AModelAnimationActor::GetTransform() = transform;
	AModelAnimationActor::UpdateMatrix();
}

void Enemy::Update() {
	UpdateState();

	AModelAnimationActor::GetTransform().translate.x = std::clamp(AnimationBehavior::GetTransform().translate.x, -12.0f, 12.0f);
	AModelAnimationActor::GetTransform().translate.z = std::clamp(AnimationBehavior::GetTransform().translate.z, -12.0f, 12.0f);

	AModelAnimationActor::UpdateMatrix();
	hitCollider_->UpdateMatrix();

	//* update skeleton *//

	UpdateAnimation();
}

void Enemy::SetAnimationState(AnimationState state) {
	if (state == animationState_) {
		return;
	}

	animationT_         = 0.0f;
	prevAnimationState_ = animationState_;
	prevTime_           = time_;
	animationState_     = state;
	time_               = {};
}

void Enemy::OnCollisionEnter(_MAYBE_UNUSED Collider* const other) {
	if (other->CheckTypeId(ColliderType::kPlayerAttack)) {
		PlayerAttackCollider* attackCollider = dynamic_cast<PlayerAttackCollider*>(other);

		hp_ -= attackCollider->damage_;

		if (hp_ < 0.0f) { //!< dead!
			requestState_ = std::make_unique<EnemyStateKnock>(this);

		} else if (attackCollider->strength_ == AttackStrength::Light) {
			requestState_ = std::make_unique<EnemyStateReactionLight>(this);

		} else {
			requestState_ = std::make_unique<EnemyStateReactionHeavy>(this);
		}

		Vector3f position      = { GetPosition().x, 1.0f, GetPosition().z };
		Vector3f otherPosition = { other->GetPosition().x, 1.0f, other->GetPosition().z };

		AnimationBehavior::GetTransform().rotate
			= ToQuaternion(CalculateEuler(Normalize(otherPosition - position)));
	}
}

void Enemy::DrawSystematic(_MAYBE_UNUSED const SxavGraphicsFrame* frame) {
	if (skeletonMesh_ == nullptr || !model_.has_value()) {
		return;
	}

	model_.value().CheckAndReload();
	std::shared_ptr<Model> model = model_.value().Lock();

	if (!model->IsCompleted()) {
		return;
	}

	sConsole->SetGraphicsPipeline(kDefaultVS_AlbedoPS_Deferred, SxavengerSystem::GetMainThreadContext(), frame->GetSize());

	DxObject::BindBufferDesc bind = {};
	bind.SetAddress("gCamera", frame->GetCamera()->GetGPUVirtualAddress());
	bind.SetAddress("gTransform", TransformComponent::GetGPUVirtualAddress());
	bind.SetAddress("gUVTransform", MaterialComponent::GetTransformGPUVirtualAddress());
	bind.SetAddress("gColor", MaterialComponent::GetColorGPUVirtualAddress());
	bind.SetHandle("gAlbedo", texture_->GetGPUHandleSRV());

	for (uint32_t i = 0; i < model->GetMeshSize(); ++i) {
		skeletonMesh_->SetIABuffer(i);

		sConsole->BindGraphicsBuffer(kDefaultVS_AlbedoPS_Deferred, SxavengerSystem::GetMainThreadContext(), bind);

		skeletonMesh_->DrawCall(i);
	}
}

void Enemy::AttributeImGui() {

	if (ImGui::Button("root")) {
		requestState_ = std::make_unique<EnemyStateRoot>(this);
		target_ = nullptr;
	}
}

void Enemy::UpdateAnimation() {
	if (animationT_ < 1.0f) {
		animationT_ += SxavengerSystem::GetDeltaTime().time / 0.2f;

		AModelAnimationActor::GetSkeleton()->UpdateTransitionAnimation(
			animators_[prevAnimationState_].WaitGet()->GetAnimation(0), prevTime_, true,
			animators_[animationState_].WaitGet()->GetAnimation(0), time_, true,
			animationT_
		);

	} else {
		AModelAnimationActor::GetSkeleton()->UpdateAnimation(animators_[animationState_].WaitGet()->GetAnimation(0), time_);
	}

	AModelAnimationActor::GetSkeleton()->UpdateBottomLevelAS(SxavengerSystem::GetMainThreadContext());
}

void Enemy::UpdateState() {
	//!< apply request state
	if (requestState_.has_value()) {
		state_->Term(); //!< 前のstateを終了

		state_ = std::move(requestState_.value()); //!< request stateを適用
		state_->Init();

		requestState_ = std::nullopt;
	}

	//!< update state
	state_->Update();
}
