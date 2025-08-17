#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/Runtime/Input/Input.h>
#include <Engine/System/Runtime/Performance/DeltaTimePoint.h>
#include <Engine/Asset/AssetStorage.h>
#include <Engine/Asset/Observer/AssetObserver.h>
#include <Engine/Component/Components/Transform/TransformComponent.h>
#include <Engine/Module/GameObject/GameObject.h>

//* demo
#include <Demo/Object/PivotCamera.h>

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
		Walk,
		Dash,
	};
	static inline constexpr uint8_t kNumAnimationType = static_cast<uint8_t>(AnimationType::Dash) + 1;

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

private:

	////////////////////////////////////////////////////////////////////////////////////////////
	// AnimationState structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct AnimationState {
		AnimationType                     type;
		DeltaTimePointd<TimeUnit::second> time;
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

	float walkspeed_ = 0.1f;
	float dashSpeed_ = 0.15f;

	Vector3f velocity_ = {};

	//* animation
	AnimationState                animationState_;
	std::optional<AnimationState> preAnimationState_;

	DeltaTimePointd<TimeUnit::second> animationTransitionTime_;

	//* camera *//

	std::unique_ptr<PivotCamera> camera_;
	//!< ここにcameraは存在してはいけない.

	//=========================================================================================
	// private methods
	//=========================================================================================

	Vector2f GetInputDirection() const;

	bool IsInputDush() const;

	void Move();

	void UpdateArmature();

	void UpdateCamera();

	void SetAnimationState(AnimationType type);

	static Quaternion CalculateDirectionQuaterion(const Vector3f& direction);

};
