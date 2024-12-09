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

void Skeleton::Update(const Animation& animation, DeltaTimePoint<TimeUnit::s> time) {
	ApplyAnimation(animation, time);
	UpdateMatrix();
}

void Skeleton::TransitionAnimation(
	const Animation& animationA, DeltaTimePoint<TimeUnit::s> timeA,
	const Animation& animationB, DeltaTimePoint<TimeUnit::s> timeB,
	float t) {

	ApplyTransitionAnimation(
		animationA, timeA,
		animationB, timeB,
		t
	);

	UpdateMatrix();
}

uint32_t Skeleton::CreateJoint(const BornNode& node, const std::optional<uint32_t>& parent) {

	Joint& joint = joints.emplace_back();

	joint.name  = node.name;
	joint.index = static_cast<uint32_t>(joints.size() - 1);

	joint.parent = parent;

	joint.transform = node.transform;

	joint.localMatrix         = node.localMatrix;
	joint.skeletonSpaceMatrix = Matrix4x4::Identity();

	for (const auto& child : node.children) {
		uint32_t& index = joint.children.emplace_back();
		index = CreateJoint(child, joint.index);
	}

	return joint.index;
}

std::optional<QuaternionTransform> Skeleton::GetTransform(const std::string& jointName, const Animation& animation, DeltaTimePoint<TimeUnit::s> time) {

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

void Skeleton::ApplyAnimation(const Animation& animation, DeltaTimePoint<TimeUnit::s> time) {

	for (auto& joint : joints) {

		std::optional<QuaternionTransform> transform = GetTransform(joint.name, animation, time);

		if (transform.has_value()) {
			joint.transform = transform.value();
		}
	}

}

void Skeleton::ApplyTransitionAnimation(
	const Animation& animationA, DeltaTimePoint<TimeUnit::s> timeA,
	const Animation& animationB, DeltaTimePoint<TimeUnit::s> timeB,
	float t) {

	for (auto& joint : joints) {

		std::optional<QuaternionTransform> transformA = GetTransform(joint.name, animationA, timeA);
		std::optional<QuaternionTransform> transformB = GetTransform(joint.name, animationB, timeB);

		// 見つからなかった場合, defaultを入れておく
		if (!transformA.has_value()) {
			transformA = QuaternionTransform();
		}

		if (!transformB.has_value()) {
			transformB = QuaternionTransform();
		}

		joint.transform.scale     = Lerp(transformA.value().scale, transformB.value().scale, t);
		joint.transform.rotate    = Slerp(transformA.value().rotate, transformB.value().rotate, t);
		joint.transform.translate = Lerp(transformA.value().translate, transformB.value().translate, t);
	}
}

void Skeleton::UpdateMatrix() {

	// すべてのJointの更新. 親が若いので(indexが子より親の方が小さいので)通常ループで処理可能
	for (auto& joint : joints) {
		// localMatrixの更新
		joint.localMatrix = Matrix::MakeAffine(joint.transform.scale, joint.transform.rotate, joint.transform.translate);

		// skeletonSpaceMatrixの更新
		if (joint.parent.has_value()) { //!< 親がいる場合
			joint.skeletonSpaceMatrix = joint.localMatrix * joints[joint.parent.value()].skeletonSpaceMatrix;

		} else {
			//!< 親がいないのでlocalMatrixとskeletonSpaceMatrixは同じにする
			joint.skeletonSpaceMatrix = joint.localMatrix;
		}
	}

}
