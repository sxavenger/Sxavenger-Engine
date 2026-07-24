#include "CollisionDetection.h"
SXAVENGER_ENGINE_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/Utility/StreamLogger.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Penetration structure methods
////////////////////////////////////////////////////////////////////////////////////////////

CollisionDetection::Penetration CollisionDetection::Penetration::Inverse() const {
	Penetration penetration = {};
	penetration.direction = -direction;
	penetration.distance  = distance;

	return penetration;
}

////////////////////////////////////////////////////////////////////////////////////////////
// Detection structure methods
////////////////////////////////////////////////////////////////////////////////////////////

CollisionDetection::Detection CollisionDetection::Detection::Inverse() const {
	if (penetration.has_value()) {
		return (*penetration).Inverse();

	} else {
		return std::nullopt;
	}
}

const CollisionDetection::Penetration& CollisionDetection::Detection::GetPenetration() const {
	StreamLogger::AssertA(penetration.has_value(), "Detection does not have penetration.");
	return penetration.value();
}

////////////////////////////////////////////////////////////////////////////////////////////
// DetectionFunction structure methods
////////////////////////////////////////////////////////////////////////////////////////////

CollisionDetection::Detection CollisionDetection::DetectionFunction::operator()(const CollisionBoundings::Sphere& sphereA, const CollisionBoundings::Sphere& sphereB) const {
	return CollisionDetection::SphereToSphere(
		positionA, sphereA,
		positionB, sphereB
	);
}

CollisionDetection::Detection CollisionDetection::DetectionFunction::operator()(const CollisionBoundings::Sphere& sphereA, const CollisionBoundings::Capsule& capsuleB) const {
	return CollisionDetection::SphereToCapsule(
		positionA, sphereA,
		positionB, capsuleB
	);
}

CollisionDetection::Detection CollisionDetection::DetectionFunction::operator()(const CollisionBoundings::Sphere& sphereA, const CollisionBoundings::AABB& aabbB) const {
	return CollisionDetection::SphereToAABB(
		positionA, sphereA,
		positionB, aabbB
	);
}

CollisionDetection::Detection CollisionDetection::DetectionFunction::operator()(const CollisionBoundings::Sphere& sphereA, const CollisionBoundings::OBB& obbB) const {
	return CollisionDetection::SphereToOBB(
		positionA, sphereA,
		positionB, obbB
	);
}

CollisionDetection::Detection CollisionDetection::DetectionFunction::operator()(const CollisionBoundings::Capsule& capsuleA, const CollisionBoundings::Sphere& sphereB) const {
	return CollisionDetection::SphereToCapsule(
		positionB, sphereB,
		positionA, capsuleA
	).Inverse();
}

CollisionDetection::Detection CollisionDetection::DetectionFunction::operator()(const CollisionBoundings::Capsule& capsuleA, const CollisionBoundings::Capsule& capsuleB) const {
	return CollisionDetection::CapsuleToCapsule(
		positionA, capsuleA,
		positionB, capsuleB
	);
}

CollisionDetection::Detection CollisionDetection::DetectionFunction::operator()(MAYBE_UNUSED const CollisionBoundings::Capsule& capsuleA, MAYBE_UNUSED const CollisionBoundings::AABB& aabbB) const {
	CollisionDetection::ExceptionUnimplemented();
}

CollisionDetection::Detection CollisionDetection::DetectionFunction::operator()(MAYBE_UNUSED const CollisionBoundings::Capsule& capsuleA, MAYBE_UNUSED const CollisionBoundings::OBB& obbB) const {
	CollisionDetection::ExceptionUnimplemented();
}

CollisionDetection::Detection CollisionDetection::DetectionFunction::operator()(const CollisionBoundings::AABB& aabbA, const CollisionBoundings::Sphere& sphereB) const {
	return CollisionDetection::SphereToAABB(
		positionB, sphereB,
		positionA, aabbA
	).Inverse();
}

CollisionDetection::Detection CollisionDetection::DetectionFunction::operator()(MAYBE_UNUSED const CollisionBoundings::AABB& aabbA, MAYBE_UNUSED const CollisionBoundings::Capsule& capsuleB) const {
	CollisionDetection::ExceptionUnimplemented();
}

CollisionDetection::Detection CollisionDetection::DetectionFunction::operator()(const CollisionBoundings::AABB& aabbA, const CollisionBoundings::AABB& aabbB) const {
	return CollisionDetection::AABBToAABB(
		positionA, aabbA,
		positionB, aabbB
	);
}

CollisionDetection::Detection CollisionDetection::DetectionFunction::operator()(const CollisionBoundings::AABB& aabbA, const CollisionBoundings::OBB& obbB) const {
	return CollisionDetection::AABBToOBB(
		positionA, aabbA,
		positionB, obbB
	);
}

CollisionDetection::Detection CollisionDetection::DetectionFunction::operator()(const CollisionBoundings::OBB& obbA, const CollisionBoundings::Sphere& sphereB) const {
	return CollisionDetection::SphereToOBB(
		positionB, sphereB,
		positionA, obbA
	).Inverse();
}

CollisionDetection::Detection CollisionDetection::DetectionFunction::operator()(MAYBE_UNUSED const CollisionBoundings::OBB& obbA, MAYBE_UNUSED const CollisionBoundings::Capsule& capsuleB) const {
	CollisionDetection::ExceptionUnimplemented();
}

CollisionDetection::Detection CollisionDetection::DetectionFunction::operator()(const CollisionBoundings::OBB& obbA, const CollisionBoundings::AABB& aabbB) const {
	return CollisionDetection::AABBToOBB(
		positionB, aabbB,
		positionA, obbA
	).Inverse();
}

CollisionDetection::Detection CollisionDetection::DetectionFunction::operator()(const CollisionBoundings::OBB& obbA, const CollisionBoundings::OBB& obbB) const {
	return CollisionDetection::OBBToOBB(
		positionA, obbA,
		positionB, obbB
	);
}

////////////////////////////////////////////////////////////////////////////////////////////
// CollisionDetection class methods
////////////////////////////////////////////////////////////////////////////////////////////

CollisionDetection::Detection CollisionDetection::CheckCollision(
	const Vector3f& positionA, const CollisionBoundings::Boundings& boundingA,
	const Vector3f& positionB, const CollisionBoundings::Boundings& boundingB) {

	DetectionFunction function = {};
	function.positionA = positionA;
	function.positionB = positionB;

	return std::visit(function, boundingA, boundingB);
}

CollisionDetection::Detection CollisionDetection::SphereToSphere(
	const Vector3f& positionA, const CollisionBoundings::Sphere& sphereA,
	const Vector3f& positionB, const CollisionBoundings::Sphere& sphereB) {

	float distanceSquared = Vector3f::DistanceSquare(positionA, positionB);
	float radiusSquared   = Square(sphereA.radius + sphereB.radius);

	if (distanceSquared >= radiusSquared) {
		return std::nullopt; //!< 衝突なし
	}

	Penetration penetration = {};
	penetration.direction = (positionA - positionB).Normalize();
	penetration.distance  = (sphereA.radius + sphereB.radius) - std::sqrt(distanceSquared);

	return penetration; //!< 衝突あり
}

CollisionDetection::Detection CollisionDetection::SphereToCapsule(
	const Vector3f& positionA, const CollisionBoundings::Sphere& sphereA,
	const Vector3f& positionB, const CollisionBoundings::Capsule& capsuleB) {

	Vector3f origin = positionB - capsuleB.direction * (capsuleB.length * 0.5f);
	Vector3f diff   = positionB + capsuleB.direction * (capsuleB.length * 0.5f) - origin;

	float lengthSquare = Vector3f::Dot(diff, diff);
	float t = 0.0f;

	if (lengthSquare != 0.0f) {
		t = Vector3f::Dot(diff, positionA - origin) / lengthSquare;
	}

	t = Clamp(t, 0.0f, 1.0f);

	Vector3f closest = origin + diff * t; //!< カプセルの線分上の点で、球の中心に最も近い点

	float distanceSquared = Vector3f::DistanceSquare(closest, positionA);
	float radiusSquared   = Square(sphereA.radius + capsuleB.radius);

	if (distanceSquared >= radiusSquared) {
		return std::nullopt; //!< 衝突なし
	}

	Penetration penetration = {};
	penetration.direction = (positionA - closest).Normalize();
	penetration.distance  = (sphereA.radius + capsuleB.radius) - std::sqrt(distanceSquared);

	return penetration; //!< 衝突あり
}

CollisionDetection::Detection CollisionDetection::SphereToAABB(
	const Vector3f& positionA, const CollisionBoundings::Sphere& sphereA,
	const Vector3f& positionB, const CollisionBoundings::AABB& aabbB) {

	Vector3f closest      = Vector3f::Clamp(positionA, aabbB.min + positionB, aabbB.max + positionB);
	float distanceSquared = Vector3f::DistanceSquare(closest, positionA);

	if (distanceSquared >= Square(sphereA.radius)) {
		return std::nullopt; //!< 衝突なし
	}

	Penetration penetration = {};
	penetration.direction = (positionA - closest).Normalize();
	penetration.distance  = sphereA.radius - std::sqrt(distanceSquared);

	return penetration; //!< 衝突あり
}

CollisionDetection::Detection CollisionDetection::SphereToOBB(
	const Vector3f& positionA, const CollisionBoundings::Sphere& sphereA,
	const Vector3f& positionB, const CollisionBoundings::OBB& obbB) {

	Vector3f localPositionA = Matrix4x4::Transform(positionA, Matrix4x4::MakeAffine(kUnit3<float>, obbB.orientation.Inverse(), positionB));
	//!< obbのローカル空間に球の中心を変換

	CollisionBoundings::Sphere localSphereA = { //!< ローカル空間での球の当たり判定
		.radius = sphereA.radius
	};

	CollisionBoundings::AABB localAABBB = { //!< ローカル空間でのobbの当たり判定
		.min = -obbB.size,
		.max = obbB.size,
	};

	return SphereToAABB(
		localPositionA, localSphereA,
		{}, localAABBB
	);
}

CollisionDetection::Detection CollisionDetection::CapsuleToCapsule(
	const Vector3f& positionA, const CollisionBoundings::Capsule& capsuleA,
	const Vector3f& positionB, const CollisionBoundings::Capsule& capsuleB) {

	// 線分の生成
	Vector3f originA = positionA - capsuleA.direction * (capsuleA.length * 0.5f);
	Vector3f diffA   = positionA + capsuleA.direction * (capsuleA.length * 0.5f) - originA;
	Vector3f originB = positionB - capsuleB.direction * (capsuleB.length * 0.5f);
	Vector3f diffB   = positionB + capsuleB.direction * (capsuleB.length * 0.5f) - originB;

	Vector3f r = originA - originB;

	float lengthSquareA = Vector3f::Dot(diffA, diffA);
	float lengthSquareB = Vector3f::Dot(diffB, diffB);

	float dotAB = Vector3f::Dot(diffA, diffB);
	float dotAR = Vector3f::Dot(diffA, r);
	float dotBR = Vector3f::Dot(diffB, r);

	float denominator = lengthSquareA * lengthSquareB - Square(dotAB);

	float distanceSquare = {};

	if (denominator != 0.0f) {

		float s = (dotAB * dotBR - lengthSquareB * dotAR) / denominator;
		float t = (lengthSquareA * dotBR - dotAB * dotAR) / denominator;

		//!< 範囲制約を適用
		s = Clamp(s, 0.0f, 1.0f);
		t = Clamp(t, 0.0f, 1.0f);

		Vector3f closestA = originA + diffA * s;
		Vector3f closestB = originB + diffB * t;

		distanceSquare = Vector3f::DistanceSquare(closestA, closestB);

		if (distanceSquare >= Square(capsuleA.radius + capsuleB.radius)) {
			return std::nullopt; //!< 衝突なし
		}

		Penetration penetration = {};
		penetration.direction = (closestA - closestB).Normalize();
		penetration.distance  = (capsuleA.radius + capsuleB.radius) - std::sqrt(distanceSquare);

		return penetration; //!< 衝突あり

	} else {
		//!< 平行な場合の処理
		
		// (HACK) 線分の両端点同士の距離を計算して、最小の距離を採用する

		Vector3f endA = originA + diffA;
		Vector3f endB = originB + diffB;

		distanceSquare = Vector3f::DistanceSquare(originA, originB);
		distanceSquare = std::min(Vector3f::DistanceSquare(originA, endB), distanceSquare);
		distanceSquare = std::min(Vector3f::DistanceSquare(endA, originB), distanceSquare);
		distanceSquare = std::min(Vector3f::DistanceSquare(endA, endB), distanceSquare);

		if (distanceSquare >= Square(capsuleA.radius + capsuleB.radius)) {
			return std::nullopt; //!< 衝突なし
		}

		Penetration penetration = {};
		penetration.direction = (positionA - positionB).Normalize();
		penetration.distance  = (capsuleA.radius + capsuleB.radius) - std::sqrt(distanceSquare);

		return penetration; //!< 衝突あり
	}
}

CollisionDetection::Detection CollisionDetection::AABBToAABB(
	const Vector3f& positionA, const CollisionBoundings::AABB& aabbA,
	const Vector3f& positionB, const CollisionBoundings::AABB& aabbB) {

	bool isCollision
		= Comparison::All(aabbA.min + positionA <= aabbB.max + positionB)
		&& Comparison::All(aabbA.max + positionA >= aabbB.min + positionB);

	if (!isCollision) {
		return std::nullopt; //!< 衝突なし
	}

	Vector3f overlap = Vector3f::Min(aabbA.max + positionA, aabbB.max + positionB) - Vector3f::Max(aabbA.min + positionA, aabbB.min + positionB);

	Vector3f pushback = {};

	if (overlap.x < overlap.y && overlap.x < overlap.z) {
		//!< x軸方向の押し戻し
		pushback.x = aabbA.max.x + positionA.x > aabbB.max.x + positionB.x
			? overlap.x   //!< 右面
			: -overlap.x; //!< 左面

	} else if (overlap.y < overlap.x && overlap.y < overlap.z) {
		//!< y軸方向の押し戻し
		pushback.y = aabbA.max.y + positionA.y > aabbB.max.y + positionB.y
			? overlap.y   //!< 上面
			: -overlap.y; //!< 下面

	} else {
		//!< z軸方向の押し戻し
		pushback.z = aabbA.max.z + positionA.z > aabbB.max.z + positionB.z
			? overlap.z   //!< 前面
			: -overlap.z; //!< 背面
	}

	Penetration penetration = {};
	penetration.direction = pushback.Normalize();
	penetration.distance  = pushback.Length();

	return penetration; //!< 衝突あり
}

CollisionDetection::Detection CollisionDetection::AABBToOBB(
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

		float distance = std::abs(Vector3f::Dot(positionA - positionB, axisNormalize));

		if (distance > aabbAProjection + obbBProjection) {
			return std::nullopt; //!< 衝突なし
		}
	}

	Penetration penetration = {}; //!< TODO: 押し戻しベクトルの計算

	return penetration;
}

CollisionDetection::Detection CollisionDetection::OBBToOBB(
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

		float distance = std::abs(Vector3f::Dot(positionA - positionB, axisNormalize));

		if (distance > obbAProjection + obbBProjection) {
			return std::nullopt;
		}
	}

	Penetration penetration = {}; //!< TODO: 押し戻しベクトルの計算

	return penetration;
}

NORETURN void CollisionDetection::ExceptionUnimplemented() {
	StreamLogger::Exception("collision detection unimplemented."); //!< 未実装
}
