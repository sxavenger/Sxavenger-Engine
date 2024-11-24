#pragma once
//* colliderで使う当たり判定の構造体, 関数をまとめたsource

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* lib
#include <Lib/Geometry/Vector3.h>
#include <Lib/Geometry/Matrix4x4.h>
#include <Lib/Geometry/VectorComparison.h>

//* c++
#include <variant>

////////////////////////////////////////////////////////////////////////////////////////////
// CollisionBoundings namespace
////////////////////////////////////////////////////////////////////////////////////////////
namespace CollisionBoundings {
	//!< 外部のtransformがあるのでtransform系はない

	////////////////////////////////////////////////////////////////////////////////////////////
	// Sphere structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct Sphere {
		float radius;
	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// Capsule structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct Capsule {
		Vector3f direction;
		float radius;
		float length;
	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// AABB structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct AABB {
		Vector3f localMin;
		Vector3f localMax;
	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// OBB structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct OBB {
		Matrix4x4 orientation = Matrix4x4::Identity(); //!< 回転行列のみ入る
		Vector3f  size;
	};

	//=========================================================================================
	// using
	//=========================================================================================
	using Boundings = std::variant<Sphere, Capsule, AABB, OBB>;

	//=========================================================================================
	// BoundingsType enum
	//=========================================================================================
	enum BoundingsType : size_t { //!< boundingsのvariant indexに合わせる
		kSphere,
		kCapsule,
		kAABB,
		kOBB
	};

}

////////////////////////////////////////////////////////////////////////////////////////////
// CollisionDetection class
////////////////////////////////////////////////////////////////////////////////////////////
class CollisionDetection final {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	static bool CheckCollision(
		const Vector3f& positionA, const CollisionBoundings::Boundings& boundingA,
		const Vector3f& positionB, const CollisionBoundings::Boundings& boundingB
	);

private:

	//=========================================================================================
	// private handle methods
	//=========================================================================================

	static bool HandleSphereCollision(
		const Vector3f& posisionA, const CollisionBoundings::Sphere& sphereA,
		const Vector3f& posisionB, const CollisionBoundings::Boundings& boundingB
	);

	static bool HandleCapsuleCollision(
		const Vector3f& posisionA, const CollisionBoundings::Capsule& capsuleA,
		const Vector3f& posisionB, const CollisionBoundings::Boundings& boundingB
	);

	static bool HandleAABBCollision(
		const Vector3f& posisionA, const CollisionBoundings::AABB& aabbA,
		const Vector3f& posisionB, const CollisionBoundings::Boundings& boundingB
	);

	static bool HandleOBBCollision(
		const Vector3f& posisionA, const CollisionBoundings::OBB& obbA,
		const Vector3f& posisionB, const CollisionBoundings::Boundings& boundingB
	);


	//=========================================================================================
	// private collision methods
	//=========================================================================================

	//* Sphere to *//

	static bool SphereTo(
		const Vector3f& positionA, const CollisionBoundings::Sphere& sphereA,
		const Vector3f& positionB, const CollisionBoundings::Sphere& sphereB
	);

	static bool SphereToCapsule(
		const Vector3f& positionA, const CollisionBoundings::Sphere& sphereA,
		const Vector3f& positionB, const CollisionBoundings::Capsule& capsuleB
	);

	static bool SphereToAABB(
		const Vector3f& positionA, const CollisionBoundings::Sphere& sphereA,
		const Vector3f& positionB, const CollisionBoundings::AABB& aabbB
	);

	static bool SphereToOBB(
		const Vector3f& positionA, const CollisionBoundings::Sphere& sphereA,
		const Vector3f& positionB, const CollisionBoundings::OBB& obbB
	);

	//* Capsule to *//

	static bool CapsuleTo(
		const Vector3f& positionA, const CollisionBoundings::Capsule& capsuleA,
		const Vector3f& positionB, const CollisionBoundings::Capsule& capsuleB
	);

	//* AABB to *//

	static bool AABBTo(
		const Vector3f& positionA, const CollisionBoundings::AABB& aabbA,
		const Vector3f& positionB, const CollisionBoundings::AABB& aabbB
	);

	static bool AABBToOBB(
		const Vector3f& positionA, const CollisionBoundings::AABB& aabbA,
		const Vector3f& positionB, const CollisionBoundings::OBB& obbB
	);

	//* OBB to *//

	static bool OBBTo(
		const Vector3f& positionA, const CollisionBoundings::OBB& obbA,
		const Vector3f& positionB, const CollisionBoundings::OBB& obbB
	);

	//=========================================================================================
	// private exception methods
	//=========================================================================================

	static void ExceptionUnimplement();
	static void ExceptionValiant();
};