#include "Skeleton.h"

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

void Skeleton::Update(const Animation& animation, TimePointf<TimeUnit::second> time, bool isLoop) {
	if (isLoop) {
		time = Mod(time, animation.duration);
	}

	ApplyAnimation(animation, time);
	UpdateMatrix();
}

void Skeleton::TransitionAnimation(
	const Animation& animationA, TimePointf<TimeUnit::second> timeA, bool isLoopA,
	const Animation& animationB, TimePointf<TimeUnit::second> timeB, bool isLoopB,
	float t) {

	if (isLoopA) {
		timeA = Mod(timeA, animationA.duration);
	}

	if (isLoopB) {
		timeB = Mod(timeB, animationB.duration);
	}

	ApplyTransitionAnimation(
		animationA, timeA,
		animationB, timeB,
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

	joints.emplace_back(joint);

	for (const auto& child : node.children) {
		uint32_t index = CreateJoint(child, joint.index);
		joint.children.emplace_back(index);
	}

	return joint.index;
}

std::optional<QuaternionTransform> Skeleton::GetTransform(const std::string& jointName, const Animation& animation, TimePointf<TimeUnit::second> time) {

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

void Skeleton::ApplyAnimation(const Animation& animation, TimePointf<TimeUnit::second> time) {

	for (auto& joint : joints) {

		std::optional<QuaternionTransform> transform = GetTransform(joint.name, animation, time);

		if (transform.has_value()) {
			joint.transform = transform.value();
		}
	}

}

void Skeleton::ApplyTransitionAnimation(
	const Animation& animationA, TimePointf<TimeUnit::second> timeA,
	const Animation& animationB, TimePointf<TimeUnit::second> timeB,
	float t) {

	for (auto& joint : joints) {

		std::optional<QuaternionTransform> transformA = GetTransform(joint.name, animationA, timeA);
		std::optional<QuaternionTransform> transformB = GetTransform(joint.name, animationB, timeB);

		if (!(transformA.has_value() && transformB.has_value())) {
			continue;
		}

		// 見つからなかった場合, defaultを入れておく
		if (!transformA.has_value()) {
			transformA = QuaternionTransform();
		}

		if (!transformB.has_value()) {
			transformB = QuaternionTransform();
		}

		joint.transform.scale     = Vector3f::Lerp(transformA.value().scale, transformB.value().scale, t);
		joint.transform.rotate    = Quaternion::Slerp(transformA.value().rotate, transformB.value().rotate, t);
		joint.transform.translate = Vector3f::Lerp(transformA.value().translate, transformB.value().translate, t);
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
