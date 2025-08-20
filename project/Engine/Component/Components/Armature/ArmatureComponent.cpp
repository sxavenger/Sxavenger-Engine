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

void ArmatureComponent::SetSkeleton(const Uuid& skeleton) {
	referenceSkeleton_ = skeleton;
	skeleton_          = referenceSkeleton_.WaitRequire()->GetSkeleton();
}

void ArmatureComponent::UpdateAnimation(const Animation& animation, TimePointd<TimeUnit::second> time, bool isLoop) {
	if (referenceSkeleton_.Empty()) {
		Logger::WarningRuntime("warning | [ArmatureComponent] UpdateAnimation", "reference skeleton is empty.");
		return;
	}

	skeleton_.Update(animation, time, isLoop);
}

void ArmatureComponent::TransitionAnimation(
	const Animation& animationA, TimePointd<TimeUnit::second> timeA, bool isLoopA,
	const Animation& animationB, TimePointd<TimeUnit::second> timeB, bool isLoopB,
	float t) {

	if (referenceSkeleton_.Empty()) {
		Logger::WarningRuntime("warning | [ArmatureComponent] TransitionAnimation", "reference skeleton is empty.");
		return;
	}

	skeleton_.TransitionAnimation(
		animationA, timeA, isLoopA,
		animationB, timeB, isLoopB,
		t
	);
}

void ArmatureComponent::ShowComponentInspector() {
	if (referenceSkeleton_.Empty()) {
		ImGui::TextDisabled("reference skeleton is empty.");
		return;
	}

	Matrix4x4 mat = Matrix4x4::Identity();

	if (auto parent = BaseComponent::GetBehaviour()->GetParent()) {
		if (auto component = parent->GetComponent<TransformComponent>()) {
			mat = component->GetMatrix();
		}
	}

	PushBornLine(mat, skeleton_.joints);
}

void ArmatureComponent::PushBornLine(const Matrix4x4& mat, const std::vector<Joint>& joints) {

	for (const auto& joint : joints) {
		if (!joint.parent.has_value()) {
			continue; //!< skip root joint.
		}

		Vector3f origin = Matrix4x4::Transform(Matrix4x4::GetTranslation(joint.skeletonSpaceMatrix), mat);
		Vector3f parent = Matrix4x4::Transform(Matrix4x4::GetTranslation(joints[joint.parent.value()].skeletonSpaceMatrix), mat);

		// born line
		SxavengerContent::PushLineOverlay(origin, parent, Color4f::Convert(0xFAFA00FF), 0.4f);

		Vector3f x = Matrix4x4::Transform(kUnitX3<float> * 4.0f, joint.skeletonSpaceMatrix * mat);
		Vector3f y = Matrix4x4::Transform(kUnitY3<float> * 4.0f, joint.skeletonSpaceMatrix * mat);
		Vector3f z = Matrix4x4::Transform(kUnitZ3<float> * 4.0f, joint.skeletonSpaceMatrix * mat);

		// local axis
		SxavengerContent::PushLineOverlay(origin, x, Color4f::Convert(0xFA0000FF), 0.4f);
		SxavengerContent::PushLineOverlay(origin, y, Color4f::Convert(0x00FA00FF), 0.4f);
		SxavengerContent::PushLineOverlay(origin, z, Color4f::Convert(0x0000FAFF), 0.4f);

		// point
		SxavengerContent::PushPointOverlay(origin, Color4f::Convert(0xFAFAFAFF), 4.0f);
	}
}
