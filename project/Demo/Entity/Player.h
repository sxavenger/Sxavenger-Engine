#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/Runtime/Input/Input.h>
#include <Engine/System/Runtime/Performance/DeltaTimePoint.h>
#include <Engine/Component/Components/Transform/TransformComponent.h>
#include <Engine/Module/GameObject/GameObject.h>
#include <Engine/Adapter/Parameter/SerializeParameter.h>
#include <Engine/Adapter/Parameter/SerializeGui.h>
#include <Engine/Preview/Content/UContentModel.h>
#include <Engine/Preview/Content/UContentAnimation.h>
#include <Engine/Preview/Content/UContentObserver.h>

//* demo
#include <Demo/Object/PerspectiveCamera.h>

//* external
#include <externals/magic_enum/magic_enum.hpp>

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

public:

	//=========================================================================================
	// public methods
	//=========================================================================================
	
	void Load() override;

	void Awake() override;

	void Start() override;

	void Update() override;

	void Inspectable() override;

	//* setter *//

	void SetCamera(const PerspectiveCamera* camera) { camera_ = camera; }
	
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

	//* input *//

	const KeyboardInput* keyboard_ = nullptr;
	const GamepadInput* gamepad_   = nullptr;

	//* component *//

	TransformComponent* transform_ = nullptr;

	//* external *//

	const PerspectiveCamera* camera_ = nullptr;

	//* asset *//

	UContentObserver<UContentModel> model_;
	std::array<UContentObserver<UContentAnimation>, magic_enum::enum_count<AnimationType>()> animators_;

	//* parameter *//

	SerializeParameter<float> walkspeed_{ "walk speed", 0.1f };
	SerializeParameter<float> dashSpeed_{ "dash speed", 0.15f };

	Vector3f velocity_ = {};

	//* animation
	AnimationState                animationState_;
	std::optional<AnimationState> preAnimationState_;

	DeltaTimePointd<TimeUnit::second> animationTransitionTime_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	Vector2f GetInputDirection() const;

	bool IsInputDush() const;

	void Move();

	void UpdateArmature();

	void UpdateMeshEnable();

	void SetAnimationState(AnimationType type);

	static Quaternion CalculateDirectionQuaterion(const Vector3f& direction);

};
