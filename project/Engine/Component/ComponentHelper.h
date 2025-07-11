#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* entity
#include "Entity/MonoBehaviour.h"
#include "Entity/MonoBehaviourContainer.h"

//* engine
#include <Engine/System/Runtime/Performance/TimePoint.h>
#include <Engine/Content/Animation/Animation.h>

////////////////////////////////////////////////////////////////////////////////////////////
// ComponentHelper class
////////////////////////////////////////////////////////////////////////////////////////////
class ComponentHelper {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* container helper *//

	static void UpdateTransform();

	static void UpdateSkinning();

	static void UpdateParticle();

	//* create helper *//

	static std::unique_ptr<MonoBehaviour> CreateMonoBehaviour();

	static std::unique_ptr<MonoBehaviour> CreateTransformBehaviour();

	static std::unique_ptr<MonoBehaviour> CreateStaticModelBehaviour(const std::filesystem::path& filepath);

	static std::unique_ptr<MonoBehaviour> CreateStaticNodeModelBehaviour(const std::filesystem::path& filepath);

	static std::unique_ptr<MonoBehaviour> CreateCameraMonoBehaviour();

	static std::unique_ptr<MonoBehaviour> CreateDirectionalLightMonoBehaviour();

	static std::unique_ptr<MonoBehaviour> CreatePointLightMonoBehaviour();

	//* option helper *//

	static void ApplyAnimation(
		MonoBehaviour* behaviour,
		const Animation& animation, TimePointf<TimeUnit::second> time, bool isLoop = true
	);

	static void ApplyAnimationTransition(
		MonoBehaviour* behaviour,
		const Animation& animationA, TimePointf<TimeUnit::second> timeA, bool isLoopA,
		const Animation& animationB, TimePointf<TimeUnit::second> timeB, bool isLoopB,
		float transitionT
	);

private:
};
