#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* entity
#include "BehaviourAddress.h"
#include "EntityBehaviour.h"

//* engine
#include <Engine/Foundation.h>
#include <Engine/Graphics/Animation/Animation.h>
#include <Engine/Assets/Content/ContentModel.h>

//* c++
#include <string>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// BehaviourHelper class
////////////////////////////////////////////////////////////////////////////////////////////
class BehaviourHelper {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	static BehaviourAddress Create();
	static BehaviourAddress Create(const std::string& name);

	static void Destroy(BehaviourAddress& address);

	//* create helper *//

	static void CreateTransformBehaviour(const BehaviourAddress& address);
	static BehaviourAddress CreateTransformBehaviour();

	static void CreateCameraBehaviour(const BehaviourAddress& address);
	static BehaviourAddress CreateCameraBehaviour();

	static void CreateDirectionalLightBehaviour(const BehaviourAddress& address);
	static BehaviourAddress CreateDirectionalLightBehaviour();

	static void CreatePointLightBehaviour(const BehaviourAddress& address);
	static BehaviourAddress CreatePointLightBehaviour();

	static void CreateSpotLightBehaviour(const BehaviourAddress& address);
	static BehaviourAddress CreateSpotLightBehaviour();

	static void CreateStaticMeshBehaviour(const BehaviourAddress& address, const std::shared_ptr<ContentModel>& model);
	static BehaviourAddress CreateStaticMeshBehaviour(const std::shared_ptr<ContentModel>& model);

	static void CreateSkinnedMeshBehaviour(const BehaviourAddress& address, const std::shared_ptr<ContentModel>& model);
	static BehaviourAddress CreateSkinnedMeshBehaviour(const std::shared_ptr<ContentModel>& model);

	//* animation helper *//

	static void ApplyAnimation(
		const BehaviourAddress& address,
		const Animation& animation, TimePointd<TimeUnit::second> time, bool isLoop = true
	);

	static void ApplyAnimationTransition(
		const BehaviourAddress& address,
		const Animation& prevAnimation, TimePointd<TimeUnit::second> prevTime, bool prevIsLoop,
		const Animation& currAnimation, TimePointd<TimeUnit::second> currTime, bool currIsLoop,
		float transitionT
	);

private:

	//=========================================================================================
	// private methods
	//=========================================================================================

	static void CreateStaticMeshBehaviourNode(const BehaviourAddress& parent, const BornNode& node, const std::shared_ptr<ContentModel>& model);

};

SXAVENGER_ENGINE_NAMESPACE_END
