#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* component
#include "../BaseComponent.h"

//* engine
#include <Engine/Content/Animation/Skeleton.h>
#include <Engine/Preview/Asset/AssetSkeleton.h>
#include <Engine/Preview/Asset/AssetParameter.h>

////////////////////////////////////////////////////////////////////////////////////////////
// ArmatureComponent class
////////////////////////////////////////////////////////////////////////////////////////////
class ArmatureComponent final
	: public BaseComponent {
	//* unityの場合, armature一つ一つがEntityであるがhackとして実装
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	ArmatureComponent(MonoBehaviour* behaviour) : BaseComponent(behaviour) {}
	~ArmatureComponent() override = default;

	//* armature option *//

	void SetSkeleton(const Uuid& skeleton);

	void UpdateAnimation(const Animation& animation, TimePointd<TimeUnit::second> time, bool isLoop = true);

	void TransitionAnimation(
		const Animation& animationA, TimePointd<TimeUnit::second> timeA, bool isLoopA,
		const Animation& animationB, TimePointd<TimeUnit::second> timeB, bool isLoopB,
		float t
	);

	const Skeleton& GetSkeleton() const { return skeleton_; }

	//* inspector *//

	void ShowComponentInspector() override;

	//* json option *//

	json PerseToJson() const override;

	void InputJson(const json& data) override;


	//=========================================================================================
	// public variables
	//=========================================================================================

	static const std::string kArmatureName;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	AssetParameter<AssetSkeleton> referenceSkeleton_;
	Skeleton skeleton_;


	//!< hack: animationをここでするのはどうか

	//=========================================================================================
	// private methods
	//=========================================================================================

	void PushBornLine(const Matrix4x4& mat, const std::vector<Joint>& joints);

};
