#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* component
#include "../BaseComponent.h"

//* engine
#include <Engine/Content/Animation/Skeleton.h>

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

	void SetSkeleton(const Skeleton& skeleton) { skeleton_ = skeleton; }

	void UpdateAnimation(const Animation& animation, TimePointf<TimeUnit::second> time, bool isLoop = true);

	void TransitionAnimation(
		const Animation& animationA, TimePointf<TimeUnit::second> timeA, bool isLoopA,
		const Animation& animationB, TimePointf<TimeUnit::second> timeB, bool isLoopB,
		float t
	);

	//* getter *//

	const Skeleton& GetSkeleton() const;

	//* inspector *//

	void ShowComponentInspector() override;

	//=========================================================================================
	// public variables
	//=========================================================================================

	static const std::string kArmatureName;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	std::optional<Skeleton> skeleton_;

	//!< hack: animationをここでするのはどうか
	//!< 理想はanimator componentなどを作ってそこで管理する

	//=========================================================================================
	// private methods
	//=========================================================================================

	void PushBornLine(const Matrix4x4& mat, const std::vector<Joint>& joints, const Joint& joint);

};
