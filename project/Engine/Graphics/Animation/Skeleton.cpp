#include "Skeleton.h"
SXAVENGER_ENGINE_USING

////////////////////////////////////////////////////////////////////////////////////////////
// Skeleton structure methods
////////////////////////////////////////////////////////////////////////////////////////////

void Skeleton::Create(const BornNode& node) {

	root = CreateJoint(node, std::nullopt);

	// 名前でmapping
	for (const auto& joint : joints) {
		jointMap.emplace(joint.name, joint.index);
	}

	UpdateMatrix();
}

void Skeleton::Update(const Animation& animation, TimePointd<TimeUnit::second> time, bool isLoop) {
	if (isLoop) {
		time = Mod(time, animation.duration);
	}

	ApplyAnimation(animation, time);
	UpdateMatrix();
}

void Skeleton::TransitionAnimation(
	const Animation& prevAnimation, TimePointd<TimeUnit::second> prevTime, bool prevIsLoop,
	const Animation& currAnimation, TimePointd<TimeUnit::second> currTime, bool currIsLoop,
	float t) {

	if (prevIsLoop) {
		prevTime = Mod(prevTime, prevAnimation.duration);
	}

	if (currIsLoop) {
		currTime = Mod(currTime, currAnimation.duration);
	}

	ApplyTransitionAnimation(
		prevAnimation, prevTime,
		currAnimation, currTime,
		t
	);

	UpdateMatrix();
}

const Joint& Skeleton::GetJoint(const std::string& name) const {
	return joints[jointMap.at(name)];
}

uint32_t Skeleton::CreateJoint(const BornNode& node, const std::optional<uint32_t>& parent) {

	Joint joint = {};

	joint.name  = node.name;
	joint.index = static_cast<uint32_t>(joints.size());

	joint.parent = parent;

	joint.transform = node.transform;

	joint.localMatrix         = node.localMatrix;
	joint.skeletonSpaceMatrix = Matrix4x4::Identity();

	joints.emplace_back(joint); //!< jointを追加

	for (const auto& child : node.children) {
		CreateJoint(child, joint.index);
	}

	return joint.index;
}

std::optional<QuaternionTransform> Skeleton::GetTransform(const std::string& jointName, const Animation& animation, TimePointd<TimeUnit::second> time) {

	// 対象のJointのAnimationがあれば, 値の適応
	if (auto it = animation.nodeAnimations.find(jointName); it != animation.nodeAnimations.end()) { //!< animationに対象のJointがある場

		// animationの参照取得
		const auto& nodeAnimation = (*it).second;

		QuaternionTransform transform = {};
		transform.scale     = CalculateAnimationCurve(nodeAnimation.scale, time);
		transform.rotate    = CalculateAnimationCurve(nodeAnimation.rotate, time);
		transform.translate = CalculateAnimationCurve(nodeAnimation.translate, time);

		return transform;
	}

	return std::nullopt;
}

void Skeleton::ApplyAnimation(const Animation& animation, TimePointd<TimeUnit::second> time) {

	for (auto& joint : joints) {

		std::optional<QuaternionTransform> transform = GetTransform(joint.name, animation, time);

		if (transform.has_value()) {
			joint.transform = transform.value();
		}
	}

}

void Skeleton::ApplyTransitionAnimation(
	const Animation& prevAnimation, TimePointd<TimeUnit::second> prevTime,
	const Animation& currAnimation, TimePointd<TimeUnit::second> currTime,
	float t) {

	for (auto& joint : joints) {

		std::optional<QuaternionTransform> prevTransform = GetTransform(joint.name, prevAnimation, prevTime);
		std::optional<QuaternionTransform> currTransform = GetTransform(joint.name, currAnimation, currTime);

		if (!(prevTransform.has_value() && currTransform.has_value())) {
			continue;
		}

		// 見つからなかった場合, defaultを入れておく
		if (!prevTransform.has_value()) {
			prevTransform = QuaternionTransform();
		}

		if (!currTransform.has_value()) {
			currTransform = QuaternionTransform();
		}

		joint.transform.scale     = Vector3f::Lerp(prevTransform.value().scale, currTransform.value().scale, t);
		joint.transform.rotate    = Quaternion::Slerp(prevTransform.value().rotate, currTransform.value().rotate, t);
		joint.transform.translate = Vector3f::Lerp(prevTransform.value().translate, currTransform.value().translate, t);
	}
}

void Skeleton::UpdateMatrix() {

	// すべてのJointの更新. 親が若いので(indexが子より親の方が小さいので)通常ループで処理可能
	for (auto& joint : joints) {
		// localMatrixの更新
		joint.localMatrix = Matrix4x4::MakeAffine(joint.transform.scale, joint.transform.rotate, joint.transform.translate);

		// skeletonSpaceMatrixの更新
		if (joint.parent.has_value()) { //!< 親がいる場合
			joint.skeletonSpaceMatrix = joint.localMatrix * joints[joint.parent.value()].skeletonSpaceMatrix;

		} else {
			//!< 親がいないのでlocalMatrixとskeletonSpaceMatrixは同じにする
			joint.skeletonSpaceMatrix = joint.localMatrix;
		}
	}

}
