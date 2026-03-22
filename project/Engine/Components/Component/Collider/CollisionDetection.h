#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/Foundation.h>

//* lib
#include <Lib/CXXAttribute.h>
#include <Lib/Geometry/Vector3.h>
#include <Lib/Geometry/Matrix4x4.h>
#include <Lib/Geometry/VectorComparison.h>
#include <Lib/Geometry/GeometryMath.h>

//* c++
#include <variant>
#include <optional>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// CollisionPoundings namespace
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
		float distance     = 0.0f; //!< めり込み量

	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// Detection structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct Detection {
	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		Detection() = default;

		//* detection option *//

		Detection Inverse() const;

		bool HasPenetration() const { return penetration.has_value(); }

		const Penetration& GetPenetration() const;

		//* operator [assignment] *//

		Detection(const Detection&) noexcept            = default;
		Detection& operator=(const Detection&) noexcept = default;

		Detection(const Penetration& _penetration) noexcept : penetration(_penetration) {}
		Detection& operator=(const Penetration& _penetration) { penetration = _penetration; return *this; }

		Detection(std::nullopt_t) noexcept : penetration(std::nullopt) {}
		Detection& operator=(std::nullopt_t) noexcept { penetration = std::nullopt; return *this; }

		//=========================================================================================
		// public variables
		//=========================================================================================

		std::optional<Penetration> penetration; //!< 衝突検出結果
		// std::nullopt: 衝突なし
		// Penetration:  衝突あり

	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	static Detection CheckCollision(
		const Vector3f& positionA, const CollisionBoundings::Boundings& boundingA,
		const Vector3f& positionB, const CollisionBoundings::Boundings& boundingB
	);

private:

	////////////////////////////////////////////////////////////////////////////////////////////
	// DetectionFunction structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct DetectionFunction {
	public:

		//=========================================================================================
		// public variables
		//=========================================================================================

		Vector3f positionA;
		Vector3f positionB;

		//=========================================================================================
		// public methods
		//=========================================================================================

		//* [sphere to xxx]

		Detection operator()(const CollisionBoundings::Sphere& sphereA, const CollisionBoundings::Sphere& sphereB) const;
		Detection operator()(const CollisionBoundings::Sphere& sphereA, const CollisionBoundings::Capsule& capsuleB) const;
		Detection operator()(const CollisionBoundings::Sphere& sphereA, const CollisionBoundings::AABB& aabbB) const;
		Detection operator()(const CollisionBoundings::Sphere& sphereA, const CollisionBoundings::OBB& obbB) const;

		//* [capsule to xxx]

		Detection operator()(const CollisionBoundings::Capsule& capsuleA, const CollisionBoundings::Sphere& sphereB) const;
		Detection operator()(const CollisionBoundings::Capsule& capsuleA, const CollisionBoundings::Capsule& capsuleB) const;
		Detection operator()(const CollisionBoundings::Capsule& capsuleA, const CollisionBoundings::AABB& aabbB) const;
		Detection operator()(const CollisionBoundings::Capsule& capsuleA, const CollisionBoundings::OBB& obbB) const;


		//* [AABB to xxx]

		Detection operator()(const CollisionBoundings::AABB& aabbA, const CollisionBoundings::Sphere& sphereB) const;
		Detection operator()(const CollisionBoundings::AABB& aabbA, const CollisionBoundings::Capsule& capsuleB) const;
		Detection operator()(const CollisionBoundings::AABB& aabbA, const CollisionBoundings::AABB& aabbB) const;
		Detection operator()(const CollisionBoundings::AABB& aabbA, const CollisionBoundings::OBB& obbB) const;

		//* [OBB to xxx]

		Detection operator()(const CollisionBoundings::OBB& obbA, const CollisionBoundings::Sphere& sphereB) const;
		Detection operator()(const CollisionBoundings::OBB& obbA, const CollisionBoundings::Capsule& capsuleB) const;
		Detection operator()(const CollisionBoundings::OBB& obbA, const CollisionBoundings::AABB& aabbB) const;
		Detection operator()(const CollisionBoundings::OBB& obbA, const CollisionBoundings::OBB& obbB) const;

	};


private:

	//=========================================================================================
	// private methods
	//=========================================================================================

	//* [sphere to xxx]

	static Detection SphereToSphere(
		const Vector3f& positionA, const CollisionBoundings::Sphere& sphereA,
		const Vector3f& positionB, const CollisionBoundings::Sphere& sphereB
	);

	static Detection SphereToCapsule(
		const Vector3f& positionA, const CollisionBoundings::Sphere& sphereA,
		const Vector3f& positionB, const CollisionBoundings::Capsule& capsuleB
	);

	static Detection SphereToAABB(
		const Vector3f& positionA, const CollisionBoundings::Sphere& sphereA,
		const Vector3f& positionB, const CollisionBoundings::AABB& aabbB
	);

	static Detection SphereToOBB(
		const Vector3f& positionA, const CollisionBoundings::Sphere& sphereA,
		const Vector3f& positionB, const CollisionBoundings::OBB& obbB
	);

	//* [capsule to xxx]

	static Detection CapsuleToCapsule(
		const Vector3f& positionA, const CollisionBoundings::Capsule& capsuleA,
		const Vector3f& positionB, const CollisionBoundings::Capsule& capsuleB
	);

	//* [AABB to xxx]

	static Detection AABBToAABB(
		const Vector3f& positionA, const CollisionBoundings::AABB& aabbA,
		const Vector3f& positionB, const CollisionBoundings::AABB& aabbB
	);

	static Detection AABBToOBB(
		const Vector3f& positionA, const CollisionBoundings::AABB& aabbA,
		const Vector3f& positionB, const CollisionBoundings::OBB& obbB
	);

	//* [OBB to xxx]

	static Detection OBBToOBB(
		const Vector3f& positionA, const CollisionBoundings::OBB& obbA,
		const Vector3f& positionB, const CollisionBoundings::OBB& obbB
	);

	//* exception method *//

	NORETURN static void ExceptionUnimplemented();

};

SXAVENGER_ENGINE_NAMESPACE_END
