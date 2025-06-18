#include "ArmatureComponent.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* component
#include "../../Entity/MonoBehaviour.h"

//* engine
#include <Engine/System/Utility/Logger.h>
#include <Engine/Content/SxavengerContent.h>

//=========================================================================================
// static cosnt variables
//=========================================================================================

const std::string ArmatureComponent::kArmatureName = "Armature";

////////////////////////////////////////////////////////////////////////////////////////////
// ArmatureComponent class methods
////////////////////////////////////////////////////////////////////////////////////////////

const Skeleton& ArmatureComponent::GetSkeleton() const {
	Exception::Assert(skeleton_.has_value(), "skeleton is not set.");
	return skeleton_.value();
}

void ArmatureComponent::UpdateAnimation(const Animation& animation, TimePointf<TimeUnit::second> time, bool isLoop) {
	if (!skeleton_.has_value()) {
		Logger::WarningRuntime("warning | [ArmatureComponent] UpdateAnimation", "skeleton is not set.");
		return;
	}

	skeleton_.value().Update(animation, time, isLoop);
}

void ArmatureComponent::TransitionAnimation(
	const Animation& animationA, TimePointf<TimeUnit::second> timeA, bool isLoopA,
	const Animation& animationB, TimePointf<TimeUnit::second> timeB, bool isLoopB,
	float t) {

	if (!skeleton_.has_value()) {
		Logger::WarningRuntime("warning | [ArmatureComponent] TransitionAnimation", "skeleton is not set.");
		return;
	}

	skeleton_.value().TransitionAnimation(
		animationA, timeA, isLoopA,
		animationB, timeB, isLoopB,
		t
	);
}

void ArmatureComponent::ShowComponentInspector() {
}
