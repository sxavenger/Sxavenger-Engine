#include "CollisionDetection.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/Utility/Logger.h>

////////////////////////////////////////////////////////////////////////////////////////////
// CollisionDetection class methods
////////////////////////////////////////////////////////////////////////////////////////////

bool CollisionDetection::CheckCollision(
	const Vector3f& positionA, const CollisionBoundings::Boundings& boundingA,
	const Vector3f& positionB, const CollisionBoundings::Boundings& boundingB) {

	Visitor visitor = {};
	visitor.positionA = positionA;
	visitor.positionB = positionB;

	return std::visit(visitor, boundingA, boundingB);
}

bool CollisionDetection::SphereTo(
	const Vector3f& positionA, const CollisionBoundings::Sphere& sphereA,
	const Vector3f& positionB, const CollisionBoundings::Sphere& sphereB) {

	float distance = Vector3f::Distance(positionA, positionB);

	if (distance <= sphereA.radius + sphereB.radius) {
		return true;
	}

	return false;
}

bool CollisionDetection::SphereToCapsule(
	const Vector3f& positionA, const CollisionBoundings::Sphere& sphereA,
	const Vector3f& positionB, const CollisionBoundings::Capsule& capsuleB) {

	Vector3f origin = positionB - capsuleB.direction * (capsuleB.length * 0.5f);
	Vector3f diff   = positionB + capsuleB.direction * (capsuleB.length * 0.5f) - origin;

	float lengthSq = Vector3f::Dot(diff, diff);
	float t = 0.0f;

	if (lengthSq != 0.0f) {
		t = Vector3f::Dot(diff, positionA - origin) / lengthSq;
	}

	t = std::clamp(t, 0.0f, 1.0f);

	Vector3f closest = origin + diff * t;

	float distance = Vector3f::Distance(closest, positionA);

	if (distance <= sphereA.radius + capsuleB.radius) {
		return true;
	}

	return false;
}

bool CollisionDetection::SphereToAABB(
	const Vector3f& positionA, const CollisionBoundings::Sphere& sphereA,
	const Vector3f& positionB, const CollisionBoundings::AABB& aabbB) {


	Vector3f closestPoint = Vector3f::Clamp(positionA, aabbB.min + positionB, aabbB.max + positionB);

	float distance = Vector3f::Distance(closestPoint, positionA);

	if (distance <= sphereA.radius) {
		return true;
	}

	return false;
}

bool CollisionDetection::SphereToOBB(
	const Vector3f& positionA, const CollisionBoundings::Sphere& sphereA,
	const Vector3f& positionB, const CollisionBoundings::OBB& obbB) {

	Vector3f centerAInOBBLocal = Matrix4x4::Transform(positionA, Matrix4x4::MakeAffine(kUnit3<float>, obbB.orientation.Inverse(), positionB));

	CollisionBoundings::Sphere sphereAInOBBLocal = {
		.radius = sphereA.radius
	};

	CollisionBoundings::AABB aabbBInOBBLocal = {
		.min = -obbB.size,
		.max = obbB.size,
	};

	return SphereToAABB(
		centerAInOBBLocal, sphereAInOBBLocal,
		{}, aabbBInOBBLocal
	);
}

bool CollisionDetection::CapsuleTo(
	const Vector3f& positionA, const CollisionBoundings::Capsule& capsuleA,
	const Vector3f& positionB, const CollisionBoundings::Capsule& capsuleB) {

	// 線分の生成
	Vector3f originA = positionA - capsuleA.direction * (capsuleA.length * 0.5f);
	Vector3f diffA   = positionA + capsuleA.direction * (capsuleA.length * 0.5f) - originA;
	Vector3f originB = positionB - capsuleB.direction * (capsuleB.length * 0.5f);
	Vector3f diffB   = positionB + capsuleB.direction * (capsuleB.length * 0.5f) - originB;

	Vector3f r = originA - originB;

	float A = Vector3f::Dot(diffA, diffA); // a.diff の長さの2乗
	float B = Vector3f::Dot(diffA, diffB); // a.diff と b.diff の内積
	float C = Vector3f::Dot(diffB, diffB); // b.diff の長さの2乗
	float D = Vector3f::Dot(diffA, r);     // a.diff と r の内積
	float E = Vector3f::Dot(diffB, r);     // b.diff と r の内積

	float denominator = A * C - B * B;

	float distance = {};

	if (denominator != 0.0f) {
		// s と t を計算
		float s, t = {};

		s = (B * E - C * D) / denominator;
		t = (A * E - B * D) / denominator;

		// 範囲制約を適用
		s = std::clamp(s, 0.0f, 1.0f);
		t = std::clamp(t, 0.0f, 1.0f);

		Vector3f closestA = originA + originA * s;
		Vector3f closestB = originB + originB * t;

		distance = Vector3f::Distance(closestA, closestB);

	} else {
		// HACK
		Vector3f endA = originA + diffA;
		Vector3f endB = originB + diffB;

		distance = Vector3f::Distance(originA, originB);
		distance = std::min(Vector3f::Distance(originA, endB), distance);
		distance = std::min(Vector3f::Distance(endA, originB), distance);
		distance = std::min(Vector3f::Distance(endA, endB), distance);
	}

	if (distance <= capsuleA.radius + capsuleB.radius) {
		return true;
	}

	return false;
}

bool CollisionDetection::AABBTo(
	const Vector3f& positionA, const CollisionBoundings::AABB& aabbA,
	const Vector3f& positionB, const CollisionBoundings::AABB& aabbB) {


	if (All(aabbA.min + positionA <= aabbB.max + positionB)
		&& All(aabbA.max + positionA >= aabbB.min + positionB)) {
		return true;
	}

	return false;
}

bool CollisionDetection::AABBToOBB(
	const Vector3f& positionA, const CollisionBoundings::AABB& aabbA,
	const Vector3f& positionB, const CollisionBoundings::OBB& obbB) {

	Vector3f axes[3 + 3 + 9] = {};

	Vector3f aabbAxesA[3] = {};
	Vector3f obbAxesB[3] = {};

	aabbAxesA[0] = { 1.0f, 0.0f, 0.0f };
	aabbAxesA[1] = { 0.0f, 1.0f, 0.0f };
	aabbAxesA[2] = { 0.0f, 0.0f, 1.0f };

	Matrix4x4 rotation = Matrix4x4::MakeRotate(obbB.orientation);

	obbAxesB[0] = Matrix4x4::Transform({ 1.0f, 0.0f, 0.0f }, rotation);
	obbAxesB[1] = Matrix4x4::Transform({ 0.0f, 1.0f, 0.0f }, rotation);
	obbAxesB[2] = Matrix4x4::Transform({ 0.0f, 0.0f, 1.0f }, rotation);

	int k = 0;

	axes[k++] = aabbAxesA[0];
	axes[k++] = aabbAxesA[1];
	axes[k++] = aabbAxesA[2];

	axes[k++] = obbAxesB[0];
	axes[k++] = obbAxesB[1];
	axes[k++] = obbAxesB[2];

	for (int i = 0; i < 3; ++i) {
		for (int j = 0; j < 3; ++j) {
			axes[k++] = Vector3f::Cross(aabbAxesA[i], obbAxesB[j]);
		}
	}

	for (int i = 0; i < 15; ++i) {
		Vector3f axisNormalize = axes[i].Normalize();

		float aabbAProjection
			= std::abs((aabbA.max.x - aabbA.min.x) * 0.5f * Vector3f::Dot(aabbAxesA[0], axisNormalize))
			+ std::abs((aabbA.max.y - aabbA.min.y) * 0.5f * Vector3f::Dot(aabbAxesA[1], axisNormalize))
			+ std::abs((aabbA.max.z - aabbA.min.z) * 0.5f * Vector3f::Dot(aabbAxesA[2], axisNormalize));


		float obbBProjection
			= std::abs(obbB.size.x * Vector3f::Dot(obbAxesB[0], axisNormalize))
			+ std::abs(obbB.size.y * Vector3f::Dot(obbAxesB[1], axisNormalize))
			+ std::abs(obbB.size.z * Vector3f::Dot(obbAxesB[2], axisNormalize));

		float disatnce = std::abs(Vector3f::Dot(positionA - positionB, axisNormalize));

		if (disatnce > aabbAProjection + obbBProjection) {
			return false;
		}
	}

	return true;
}

bool CollisionDetection::OBBTo(
	const Vector3f& positionA, const CollisionBoundings::OBB& obbA,
	const Vector3f& positionB, const CollisionBoundings::OBB& obbB) {

	Vector3f axes[3 + 3 + 9] = {};

	Vector3f obbAxesA[3] = {};
	Vector3f obbAxesB[3] = {};

	Matrix4x4 rotationA = Matrix4x4::MakeRotate(obbA.orientation);
	Matrix4x4 rotationB = Matrix4x4::MakeRotate(obbB.orientation);

	obbAxesA[0] = Matrix4x4::Transform({ 1.0f, 0.0f, 0.0f }, rotationA);
	obbAxesA[1] = Matrix4x4::Transform({ 0.0f, 1.0f, 0.0f }, rotationA);
	obbAxesA[2] = Matrix4x4::Transform({ 0.0f, 0.0f, 1.0f }, rotationA);

	obbAxesB[0] = Matrix4x4::Transform({ 1.0f, 0.0f, 0.0f }, rotationB);
	obbAxesB[1] = Matrix4x4::Transform({ 0.0f, 1.0f, 0.0f }, rotationB);
	obbAxesB[2] = Matrix4x4::Transform({ 0.0f, 0.0f, 1.0f }, rotationB);

	int k = 0;

	axes[k++] = obbAxesA[0];
	axes[k++] = obbAxesA[1];
	axes[k++] = obbAxesA[2];

	axes[k++] = obbAxesB[0];
	axes[k++] = obbAxesB[1];
	axes[k++] = obbAxesB[2];

	for (int i = 0; i < 3; ++i) {
		for (int j = 0; j < 3; ++j) {
			axes[k++] = Vector3f::Cross(obbAxesA[i], obbAxesB[j]);
		}
	}

	for (int i = 0; i < 15; ++i) {

		Vector3f axisNormalize = axes[i].Normalize();

		float obbAProjection
			= std::abs(obbA.size.x * Vector3f::Dot(obbAxesA[0], axisNormalize))
			+ std::abs(obbA.size.y * Vector3f::Dot(obbAxesA[1], axisNormalize))
			+ std::abs(obbA.size.z * Vector3f::Dot(obbAxesA[2], axisNormalize));

		float obbBProjection
			= std::abs(obbB.size.x * Vector3f::Dot(obbAxesB[0], axisNormalize))
			+ std::abs(obbB.size.y * Vector3f::Dot(obbAxesB[1], axisNormalize))
			+ std::abs(obbB.size.z * Vector3f::Dot(obbAxesB[2], axisNormalize));

		float disatnce = std::abs(Vector3f::Dot(positionA - positionB, axisNormalize));

		if (disatnce > obbAProjection + obbBProjection) {
			return false;
		}
	}

	return true;
}

bool CollisionDetection::ExceptionUnimplement() {
	Exception::Assert(false, "unimplemented."); //!< 未実装
	return false;
}
