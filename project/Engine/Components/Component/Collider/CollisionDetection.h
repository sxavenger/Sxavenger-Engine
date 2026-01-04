#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/Foundation.h>

//* lib
#include <Lib/Geometry/Vector3.h>
#include <Lib/Geometry/Matrix4x4.h>
#include <Lib/Geometry/VectorComparision.h>
#include <Lib/Geometry/GeometryMath.h>

//* c++
#include <variant>
#include <optional>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// CollisionBoundings namespace
////////////////////////////////////////////////////////////////////////////////////////////
//! @brief 衝突判定用境界構造体.
namespace CollisionBoundings {

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
		Vector3f min;
		Vector3f max;
	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// OBB structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct OBB {
		Quaternion orientation = Quaternion::Identity();
		Vector3f   size;
	};

	//=========================================================================================
	// variant using
	//=========================================================================================
	using Boundings = std::variant<Sphere, Capsule, AABB, OBB>;

	////////////////////////////////////////////////////////////////////////////////////////////
	// BoundingType enum class
	////////////////////////////////////////////////////////////////////////////////////////////
	enum class BoundingType : size_t {
		Sphere,
		Capsule,
		AABB,
		OBB,
	};

}

////////////////////////////////////////////////////////////////////////////////////////////
// CollisionDetection class
////////////////////////////////////////////////////////////////////////////////////////////
//! @brief 衝突判定クラス
class CollisionDetection {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// Penetration structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct Penetration {
	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		Penetration Inverse() const;

		//=========================================================================================
		// public variables
		//=========================================================================================

		Vector3f direction = {};   //!< 押し戻し方向(正規化)
		float    distance  = 0.0f; //!< めり込み量

	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// using
	////////////////////////////////////////////////////////////////////////////////////////////
	using Detection = std::optional<Penetration>; //!< 衝突検出結果
	// std::nullopt: 衝突なし
	// Penetration:  衝突あり

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	static bool CheckCollision(
		const Vector3f& positionA, const CollisionBoundings::Boundings& boundingA,
		const Vector3f& positionB, const CollisionBoundings::Boundings& boundingB
	);

	// TODO: 押し戻し量, 衝突点, 法線ベクトルの算出

private:

	////////////////////////////////////////////////////////////////////////////////////////////
	// Visitor structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct Visitor {

		//* member *//

		Vector3f positionA = {};
		Vector3f positionB = {};

		//* sphere To T *//

		bool operator()(
			const CollisionBoundings::Sphere& sphereA,
			const CollisionBoundings::Sphere& sphereB) {
			return SphereTo(
				positionA, sphereA, positionB, sphereB
			);
		}

		bool operator()(
			const CollisionBoundings::Sphere& sphereA,
			const CollisionBoundings::Capsule& capsuleB) {
			return SphereToCapsule(
				positionA, sphereA, positionB, capsuleB
			);
		}

		bool operator()(
			const CollisionBoundings::Sphere& sphereA,
			const CollisionBoundings::AABB& aabbB) {
			return SphereToAABB(
				positionA, sphereA, positionB, aabbB
			);
		}

		bool operator()(
			const CollisionBoundings::Sphere& sphereA,
			const CollisionBoundings::OBB& obbB) {
			return SphereToOBB(
				positionA, sphereA, positionB, obbB
			);
		}

		//* capsule To T *//

		bool operator()(
			const CollisionBoundings::Capsule& capsuleA,
			const CollisionBoundings::Sphere& sphereB) {
			return SphereToCapsule(
				positionB, sphereB, positionA, capsuleA
			);
		}

		bool operator()(
			const CollisionBoundings::Capsule& capsuleA,
			const CollisionBoundings::Capsule& capsuleB) {
			return CapsuleTo(
				positionA, capsuleA, positionB, capsuleB
			);
		}

		bool operator()(
			const CollisionBoundings::Capsule& capsuleA,
			const CollisionBoundings::AABB& aabbB) {
			capsuleA, aabbB;
			return ExceptionUnimplement();
		}

		bool operator()(
			const CollisionBoundings::Capsule& capsuleA,
			const CollisionBoundings::OBB& obbB) {
			capsuleA, obbB;
			return ExceptionUnimplement();
		}

		//* aabb To T *//

		bool operator()(
			const CollisionBoundings::AABB& aabbA,
			const CollisionBoundings::Sphere& sphereB) {
			return SphereToAABB(
				positionB, sphereB, positionA, aabbA
			);
		}

		bool operator()(
			const CollisionBoundings::AABB& aabbA,
			const CollisionBoundings::Capsule& capsuleB) {
			aabbA, capsuleB;
			return ExceptionUnimplement();
		}

		bool operator()(
			const CollisionBoundings::AABB& aabbA,
			const CollisionBoundings::AABB& aabbB) {
			return AABBTo(
				positionA, aabbA, positionB, aabbB
			);
		}

		bool operator()(
			const CollisionBoundings::AABB& aabbA,
			const CollisionBoundings::OBB& obbB) {
			return AABBToOBB(
				positionA, aabbA, positionB, obbB
			);
		}

		//* obb To T *//

		bool operator()(
			const CollisionBoundings::OBB& obbA,
			const CollisionBoundings::Sphere& sphereB) {
			return SphereToOBB(
				positionB, sphereB, positionA, obbA
			);
		}

		bool operator()(
			const CollisionBoundings::OBB& obbA,
			const CollisionBoundings::Capsule& capsuleB) {
			obbA, capsuleB;
			return ExceptionUnimplement();
		}

		bool operator()(
			const CollisionBoundings::OBB& obbA,
			const CollisionBoundings::AABB& aabbB) {
			return AABBToOBB(
				positionB, aabbB, positionA, obbA
			);
		}

		bool operator()(
			const CollisionBoundings::OBB& obbA,
			const CollisionBoundings::OBB& obbB) {
			return OBBTo(
				positionA, obbA, positionB, obbB
			);
		}
	};

	//=========================================================================================
	// private methods
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

	static bool ExceptionUnimplement();

};

SXAVENGER_ENGINE_NAMESPACE_END
