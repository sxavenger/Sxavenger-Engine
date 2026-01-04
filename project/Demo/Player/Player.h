#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/Runtime/Performance/DeltaTimePoint.h>
#include <Engine/Assets/Content/ContentModel.h>
#include <Engine/Assets/Content/ContentObserver.h>
#include <Engine/Assets/Asset/AssetAnimation.h>
#include <Engine/Assets/Asset/AssetParameter.h>
#include <Engine/Module/GameObject/GameObject.h>
#include <Engine/Module/Parameter/SerializeParameter.h>

//* externals
#include <magic_enum.hpp>

//* c++
#include <array>

////////////////////////////////////////////////////////////////////////////////////////////
// Player class
////////////////////////////////////////////////////////////////////////////////////////////
class Player
	: public SxxEngine::GameObject {
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

	void Load();

	void Awake();

	void Update();

	void Inspectable() override;

	//* setter *//

	void SetIdle();
	void SetWalk(const Vector3f& velocity);
	void SetDash(const Vector3f& velocity);

	void SetFacingQuaternion(const Quaternion& quaternion);

private:

	////////////////////////////////////////////////////////////////////////////////////////////
	// AnimationState structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct AnimationState {
		AnimationType                                type = AnimationType::Idle;
		SxxEngine::DeltaTimePointd<TimeUnit::second> time = {};
	};

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* assets *//

	SxxEngine::ContentObserver<SxxEngine::ContentModel> model_;
	std::array<SxxEngine::AssetParameter<SxxEngine::AssetAnimation>, magic_enum::enum_count<AnimationType>()> animations_;

	//* parameter *//

	SxxEngine::SerializeParameter<float> walkspeed_{ "walk speed", 0.1f };
	SxxEngine::SerializeParameter<float> dashSpeed_{ "dash speed", 0.15f };

	//* animation state *//

	AnimationState                currentAnimationState_;
	std::optional<AnimationState> prevAnimationState_;

	SxxEngine::DeltaTimePointd<TimeUnit::second> animationTransitionTime_;

	//* variables *//

	Vector3f velocity_ = kOrigin3<float>;

	//=========================================================================================
	// private methods
	//=========================================================================================

	void SetAnimationState(AnimationType type);

	void UpdateAnimation();

	void Move();

};
