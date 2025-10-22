#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* entity
#include "Entity/MonoBehaviour.h"
#include "Entity/MonoBehaviourContainer.h"

//* component
#include "Components/Camera/CameraComponent.h"

//* engine
#include <Engine/System/Runtime/Performance/TimePoint.h>
#include <Engine/Content/Animation/Animation.h>
#include <Engine/Preview/Content/UContentModel.h>
#include <Engine/Preview/Asset/UAssetMaterial.h>

////////////////////////////////////////////////////////////////////////////////////////////
// ComponentHelper class
////////////////////////////////////////////////////////////////////////////////////////////
//! @brief Component/Entity関連のHelperクラス.
class ComponentHelper {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* game loop helper *//

	static void BeginFrame();

	//* container helper *//

	static void UpdateTransform();

	static void UpdateSkinning();

	static void UpdateParticle();

	//* create helper *//

	//* common
	static std::unique_ptr<MonoBehaviour> CreateMonoBehaviour();

	static void CreateTransformMonoBehaviour(MonoBehaviour* root);
	static std::unique_ptr<MonoBehaviour> CreateTransformMonoBehaviour();

	//* camera
	static std::unique_ptr<MonoBehaviour> CreateCameraMonoBehaviour();

	//* light
	static void CreateDirectionalLightMonoBehaviour(MonoBehaviour* root);
	static std::unique_ptr<MonoBehaviour> CreateDirectionalLightMonoBehaviour();

	static void CreatePointLightMonoBehaviour(MonoBehaviour* root);
	static std::unique_ptr<MonoBehaviour> CreatePointLightMonoBehaviour();

	static void CreateSpotLightMonoBehaviour(MonoBehaviour* root);
	static std::unique_ptr<MonoBehaviour> CreateSpotLightMonoBehaviour();

	//* mesh
	static void CreateStaticMeshBehaviour(MonoBehaviour* root, const std::shared_ptr<UContentModel>& model);
	static std::unique_ptr<MonoBehaviour> CreateStaticMeshBehaviour(const std::shared_ptr<UContentModel>& model);

	static void CreateSkinnedMeshBehaviour(MonoBehaviour* root, const std::shared_ptr<UContentModel>& model);
	static std::unique_ptr<MonoBehaviour> CreateSkinnedMeshBehaviour(const std::shared_ptr<UContentModel>& model);

	//* option helper *//

	static void ApplyAnimation(
		MonoBehaviour* behaviour,
		const Animation& animation, TimePointd<TimeUnit::second> time, bool isLoop = true
	);

	static void ApplyAnimationTransition(
		MonoBehaviour* behaviour,
		const Animation& animationA, TimePointd<TimeUnit::second> timeA, bool isLoopA,
		const Animation& animationB, TimePointd<TimeUnit::second> timeB, bool isLoopB,
		float transitionT
	);

	//* storage helper *//

	static void RegisterComponents();

	//* behaviour helper *//

	static void ForEachBehaviour(MonoBehaviour* behaviour, const std::function<void(MonoBehaviour*)>& function);

	//* render helper *//

	static void AttachMeshEnable(MonoBehaviour* root, bool isEnable);

	static void DetachBehaviourMaterial(MonoBehaviour* root);

	static void ModifyBehaviourMaterial(MonoBehaviour* root, const std::function<void(UAssetMaterial*)>& function);

	//* component helper *//

	static CameraComponent* GetCameraComponent(CameraComponent::Tag tag);

private:
};
