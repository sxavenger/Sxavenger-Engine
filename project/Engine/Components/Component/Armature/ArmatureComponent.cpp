#include "ArmatureComponent.h"
SXAVENGER_ENGINE_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* component
#include "../../Entity/EntityBehaviour.h"
#include "../Transform/TransformComponent.h"

//* engine
#include <Engine/System/Utility/RuntimeLogger.h>
#include <Engine/System/UI/SxImGui.h>
#include <Engine/Graphics/Graphics.h>
#include <Engine/Assets/Asset/AssetStorage.h>
#include <Engine/Assets/Content/ContentStorage.h>

////////////////////////////////////////////////////////////////////////////////////////////
// ArmatureComponent class methods
////////////////////////////////////////////////////////////////////////////////////////////

void ArmatureComponent::SetSkeleton(const Uuid& skeleton) {
	referenceSkeleton_ = skeleton;
	skeleton_          = referenceSkeleton_.WaitRequire()->GetSkeleton();
}

void ArmatureComponent::UpdateAnimation(const Animation& animation, TimePointd<TimeUnit::second> time, bool isLoop) {
	if (referenceSkeleton_.Empty()) {
		RuntimeLogger::LogWarning("[ArmatureComponent]", "reference skeleton is empty.");
		return;
	}

	skeleton_.Update(animation, time, isLoop);
}

void ArmatureComponent::TransitionAnimation(
	const Animation& prevAnimation, TimePointd<TimeUnit::second> prevTime, bool prevIsLoop,
	const Animation& currAnimation, TimePointd<TimeUnit::second> currTime, bool currIsLoop,
	float t) {

	if (referenceSkeleton_.Empty()) {
		RuntimeLogger::LogWarning("[ArmatureComponent]", "reference skeleton is empty.");
		return;
	}

	skeleton_.TransitionAnimation(
		prevAnimation, prevTime, prevIsLoop,
		currAnimation, currTime, currIsLoop,
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

json ArmatureComponent::PerseToJson() const {
	json data = json::object();
	data["skeleton"] = referenceSkeleton_.Serialize();
	return data;
}

void ArmatureComponent::InputJson(const json& data) {

	Uuid skeleton = Uuid::Deserialize(data["skeleton"].get<std::string>());

	// skeletonのuuidが存在しない場合は, tableから読み込み

	if (!sAssetStorage->Contains<AssetSkeleton>(skeleton)) {
		const auto& filepath = sAssetStorage->GetFilepath(skeleton);
		sContentStorage->Import<ContentModel>(filepath);
	}

	SetSkeleton(skeleton);
}

void ArmatureComponent::PushBornLine(const Matrix4x4& mat, const std::vector<Joint>& joints) {

	for (const auto& joint : joints) {
		if (!joint.parent.has_value()) {
			continue; //!< skip root joint.
		}

		Vector3f origin = Matrix4x4::Transform(Matrix4x4::GetTranslation(joint.skeletonSpaceMatrix), mat);
		Vector3f parent = Matrix4x4::Transform(Matrix4x4::GetTranslation(joints[joint.parent.value()].skeletonSpaceMatrix), mat);

		// born line
		Graphics::PushLineOverlay(origin, parent, Color4f::Convert(0xFAFA00FF), 0.4f);

		Vector3f x = Matrix4x4::Transform(kUnitX3<float> * 4.0f, joint.skeletonSpaceMatrix * mat);
		Vector3f y = Matrix4x4::Transform(kUnitY3<float> * 4.0f, joint.skeletonSpaceMatrix * mat);
		Vector3f z = Matrix4x4::Transform(kUnitZ3<float> * 4.0f, joint.skeletonSpaceMatrix * mat);

		// local axis
		Graphics::PushLineOverlay(origin, x, Color4f::Convert(0xFA0000FF), 0.4f);
		Graphics::PushLineOverlay(origin, y, Color4f::Convert(0x00FA00FF), 0.4f);
		Graphics::PushLineOverlay(origin, z, Color4f::Convert(0x0000FAFF), 0.4f);

		// point
		Graphics::PushPointOverlay(origin, Color4f::Convert(0xFAFAFAFF), 4.0f);
	}
}
