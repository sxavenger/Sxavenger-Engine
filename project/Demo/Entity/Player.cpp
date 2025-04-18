#include "Player.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/SxavengerSystem.h>
#include <Engine/Asset/SxavengerAsset.h>
//* component
#include <Engine/Component/Components/Transform/TransformComponent.h>
#include <Engine/Component/Components/Armature/ArmatureComponent.h>
#include <Engine/Component/ComponentHelper.h>

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
	MonoBehaviour::AddComponent<TransformComponent>();
	model_.WaitGet()->CreateSkinnedMeshBehaviour(this);
}

void Player::Start() {
	// animation関係の設定
	animationType_ = AnimationType::Idle;
	animationTime_ = { 0.0f };
	UpdateArmature(); //!< animationを一度だけ更新しておく
}

void Player::Update() {
	animationTime_.AddDeltaTime();
}

void Player::UpdateArmature() {
	ComponentHelper::ApplyAnimation(
		this,
		animators_[static_cast<uint8_t>(animationType_)].WaitGet()->GetAnimation(0), animationTime_
	);
}
