#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* player 
#include "BasePlayerState.h"
#include "PlayerStateRoot.h"
#include "PlayerStateRolling.h"
#include "PlayerStatePunch.h"
#include "PlayerStateHook.h"
#include "PlayerStateElbow.h"
#include "PlayerStateKick.h"

//* engine

//#include <Engine/Module/Behavior/AnimationBehavior.h>
//#include <Engine/Module/Camera/Camera3d.h>
#include <Engine/Module/Skeleton/SkeletonMesh.h>

//#include <Engine/Module/VisualLayer/VisualDoF.h>

//* engine
#include <Engine/System/Runtime/Input/Input.h>
#include <Engine/Asset/SxavengerAsset.h>
#include <Engine/Module/Collider/Collider.h>
#include <Engine/!Render/Scene/Actor/Geometry/GeometryActors/AModelAnimationActor.h>
#include <Engine/Editor/EditorComponent/AttributeComponent.h>

//* game
#include <Game/Object/GameCamera.h>

//* c++
#include <memory>
#include <optional>

////////////////////////////////////////////////////////////////////////////////////////////
// Player class
////////////////////////////////////////////////////////////////////////////////////////////
class Player
	: public AttributeComponent, public AModelAnimationActor {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// Animation enum class
	////////////////////////////////////////////////////////////////////////////////////////////
	enum AnimationState : uint8_t {
		Idle,
		Walking,
		Running,
		Rolling,
		Punching,
		Hooking,
		Elbow,
		Kick,
	};
	static const uint8_t kAnimationCount = AnimationState::Kick + 1;

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	Player()  = default;
	~Player() = default;

	void Init();
	void Init(const QuaternionTransform& transform);

	void Term();

	void Update();

	void SetAnimationState(AnimationState state);

	void AttributeImGui() override;

	void SetShake(TimePointf<TimeUnit::second> time, const Vector2f& strength);

	void SetCamera(GameCamera* camera) { camera_ = camera; }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* external *//

	const KeyboardInput* keyboard_ = nullptr;
	const GamepadInput*  gamepad_  = nullptr;

	GameCamera* camera_ = nullptr;

	//* state *//

	std::unique_ptr<BasePlayerState>                state_        = nullptr;
	std::optional<std::unique_ptr<BasePlayerState>> requestState_ = std::nullopt;

	//* geometry *//

	std::unique_ptr<AModelAnimationActor> geometry_;

	//* animation *//

	std::array<AssetObserver<AssetAnimator>, kAnimationCount> animators_;

	AnimationState animationState_ = AnimationState::Idle;
	TimePointf<TimeUnit::second> time_;

	AnimationState prevAnimationState_ = AnimationState::Idle;
	TimePointf<TimeUnit::second> prevTime_;

	float animationT_ = 1.0f;

	//* camera control *//

	Vector3f pivot_ = kOrigin3<float>;
	Vector2f pivotRotation_ = {}; //!< x: lon, y: lat

	float distance_ = 6.0f;

	Vector3f offset_ = { 0.0f, 1.0f, 0.0f };

	Quaternion target_ = Quaternion::Identity();

	TimePointf<TimeUnit::second> shakeTime_  = {};
	TimePointf<TimeUnit::second> shakeTimer_ = {};

	Vector2f strength_ = {};


	//* collider *//

	std::unique_ptr<Collider> hitCollider_;

	//* visual

	//std::unique_ptr<VisualDoF> dof_;

	//std::shared_ptr<Texture> texture_ = nullptr;

	//=========================================================================================
	// private methods
	//=========================================================================================

	void UpdateState();

	void UpdateAnimation();

	void UpdateCamera();
	
public:
	////////////////////////////////////////////////////////////////////////////////////////////
	// friend class
	////////////////////////////////////////////////////////////////////////////////////////////
	friend PlayerStateRoot;
	friend PlayerStateRolling;
	friend PlayerStatePunch;
	friend PlayerStateHook;
	friend PlayerStateElbow;
	friend PlayerStateKick;
};
