#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/Runtime/Input/Input.h>
#include <Engine/System/Runtime/Performance/DeltaTimePoint.h>
#include <Engine/Asset/AssetStorage.h>
#include <Engine/Asset/Observer/AssetObserver.h>
#include <Engine/Module/GameObject/GameObject.h>
//* component
#include <Engine/Component/Components/Transform/TransformComponent.h>

//* c++
#include <array>

////////////////////////////////////////////////////////////////////////////////////////////
// Player class
////////////////////////////////////////////////////////////////////////////////////////////
class Player
	: public GameObject {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// AnimationType enum class
	////////////////////////////////////////////////////////////////////////////////////////////
	enum class AnimationType : uint8_t {
		Idle,
		Walk
	};
	static inline constexpr uint8_t kNumAnimationType = static_cast<uint8_t>(AnimationType::Walk) + 1;

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	void Load() override;

	void Awake() override;

	void Start() override;

	void Update() override;

	//* inspectable *//

	void Inspectable() override;

	//* setter *//

	void SetCameraTarget(TransformComponent* target, const TimePointf<TimeUnit::second>& time);

private:

	////////////////////////////////////////////////////////////////////////////////////////////
	// AnimationState structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct AnimationState {
		AnimationType                     type;
		DeltaTimePointf<TimeUnit::second> time;
	};

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* external *//

	const KeyboardInput* keyboard_ = nullptr;
	const GamepadInput* gamepad_   = nullptr;

	//* component *//

	TransformComponent* transform_;

	//* asset *//

	AssetObserver<AssetModel> model_;
	std::array<AssetObserver<AssetAnimator>, kNumAnimationType> animators_;

	//* parameter *//

	float speed_ = 0.1f;

	Vector3f velocity_ = {};

	//* animation
	AnimationState                animationState_;
	std::optional<AnimationState> preAnimationState_;

	DeltaTimePointf<TimeUnit::second> animationTransitionTime_;

	//* camera *//
	// todo: cameraの処理を分ける

	std::unique_ptr<MonoBehaviour> camera_;

	TransformComponent* target_ = nullptr;
	DeltaTimePointf<TimeUnit::second> timer_;
	TimePointf<TimeUnit::second> time_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	Vector2f GetInputDirection();

	void Move();

	void UpdateArmature();

	void UpdateCamera();

	void SetAnimationState(AnimationType type);

	static Quaternion CalculateDirectionQuaterion(const Vector3f& direction);

};
