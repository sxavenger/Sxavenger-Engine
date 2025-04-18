#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/Runtime/Input/Input.h>
#include <Engine/System/Runtime/Performance/DeltaTimePoint.h>
#include <Engine/Asset/AssetCollection.h>
#include <Engine/Asset/Observer/AssetObserver.h>
#include <Engine/Module/GameObject/GameObject.h>

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

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* external *//

	const KeyboardInput* keyboard_ = nullptr;
	const GamepadInput* gamepad_   = nullptr;

	//* asset *//

	AssetObserver<AssetModel> model_;
	std::array<AssetObserver<AssetAnimator>, kNumAnimationType> animators_;

	//* parameter *//

	//* animation
	AnimationType animationType_ = AnimationType::Idle;
	DeltaTimePointf<TimeUnit::second> animationTime_ = { 0.0f };

	//=========================================================================================
	// private methods
	//=========================================================================================

	void UpdateArmature();

};
