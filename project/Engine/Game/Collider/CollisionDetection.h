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

	//!< カプセルも入れたい...

	////////////////////////////////////////////////////////////////////////////////////////////
	// AABB structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct AABB {
		Vector3f localMin;
		Vector3f localMax;
		//!< 外部のtranslationを使ってworldに変換する
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
	using Boundings = std::variant<Sphere, AABB, OBB>;

	//=========================================================================================
	// BoundingsType enum
	//=========================================================================================
	enum BoundingsType : size_t { //!< boundingsのvariant indexに合わせる
		kSphere,
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
		const Vector3f& posisionA, const CollisionBoundings::Boundings& boundingA,
		const Vector3f& posisionB, const CollisionBoundings::Boundings& boundingB
	);

private:

	//=========================================================================================
	// private handle methods
	//=========================================================================================

	static bool HandleSphereCollision(
		const Vector3f& posisionA, const CollisionBoundings::Sphere&    sphereA,
		const Vector3f& posisionB, const CollisionBoundings::Boundings& boundingB
	);

	static bool HandleAABBCollision(
		const Vector3f& posisionA, const CollisionBoundings::AABB&      aabbA,
		const Vector3f& posisionB, const CollisionBoundings::Boundings& boundingB
	);

	static bool HandleOBBCollision(
		const Vector3f& posisionA, const CollisionBoundings::OBB&       obbA,
		const Vector3f& posisionB, const CollisionBoundings::Boundings& boundingB
	);

	//=========================================================================================
	// private collision detection methods
	//=========================================================================================

	//* sphere to *//

	static bool SphereTo(
		const Vector3f& posisionA, const CollisionBoundings::Sphere& sphereA,
		const Vector3f& posisionB, const CollisionBoundings::Sphere& sphereB
	);

	static bool SphereToAABB(
		const Vector3f& posisionA, const CollisionBoundings::Sphere& sphereA,
		const Vector3f& posisionB, const CollisionBoundings::AABB& aabbB
	);

	//* AABB to *//

	static bool AABBTo(
		const Vector3f& posisionA, const CollisionBoundings::AABB& aabbA,
		const Vector3f& posisionB, const CollisionBoundings::AABB& aabbB
	);

};