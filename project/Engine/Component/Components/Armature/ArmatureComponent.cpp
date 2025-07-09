#include "ArmatureComponent.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* component
#include "../../Entity/MonoBehaviour.h"
#include "../Transform/TransformComponent.h"

//* engine
#include <Engine/System/Utility/Logger.h>
#include <Engine/System/UI/SxImGui.h>
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
	if (!skeleton_.has_value()) {
		ImGui::TextDisabled("skeleton is not set.");
		return;
	}

	const auto& skeleton = GetSkeleton();

	Matrix4x4 mat = Matrix4x4::Identity();

	if (auto parent = BaseComponent::GetBehaviour()->GetParent()) {
		if (auto component = parent->GetComponent<TransformComponent>()) {
			mat = component->GetMatrix();
		}
	}

	PushBornLine(mat, skeleton.joints, skeleton.joints[skeleton.root]);
}

void ArmatureComponent::PushBornLine(const Matrix4x4& mat, const std::vector<Joint>& joints, const Joint& joint) {

	Vector3f positionA = Matrix4x4::Transform(Matrix4x4::GetTranslation(joint.skeletonSpaceMatrix), mat);

	for (const auto childIndex : joint.children) {

		const Joint& child = joints[childIndex];
		Vector3f positionB = Matrix4x4::Transform(Matrix4x4::GetTranslation(child.skeletonSpaceMatrix), mat);

		SxavengerContent::PushLineOverlay(positionA, positionB, { 1.0f, 1.0f, 0.0f, 1.0f });

		PushBornLine(mat, joints, child);
	}
}
