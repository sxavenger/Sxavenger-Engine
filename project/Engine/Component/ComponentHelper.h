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

	static void UpdateTransform();

	static void ApplyAnimation(
		MonoBehaviour* behaviour,
		const Animation& animation, TimePointf<TimeUnit::second> time, bool isLoop = true
	);

	static void UpdateSkinning();

	//* create helper *//

	static std::unique_ptr<MonoBehaviour> CreateStaticModelBehaviour(const std::filesystem::path& filepath);

	static std::unique_ptr<MonoBehaviour> CreateCameraMonoBehaviour();

	static std::unique_ptr<MonoBehaviour> CreateDirectionalLightMonoBehaviour();

private:
};
