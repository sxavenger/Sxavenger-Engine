#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* component
#include "../../BaseComponent.h"
#include "../Transform/TransformComponent.h"

//* collider
#include "CollisionDetection.h"

//* lib
#include <Lib/CXXAttributeConfig.h>

//* c++
#include <functional>
#include <bitset>
#include <optional>

////////////////////////////////////////////////////////////////////////////////////////////
// forward
////////////////////////////////////////////////////////////////////////////////////////////
class CollisionCallbackCollection;

////////////////////////////////////////////////////////////////////////////////////////////
// ColliderComponent class
////////////////////////////////////////////////////////////////////////////////////////////
class ColliderComponent final
	: public BaseComponent {
public:
	// hack: Boundingごとにクラスを分ける

	////////////////////////////////////////////////////////////////////////////////////////////
	// CollisionState enum
	////////////////////////////////////////////////////////////////////////////////////////////
	enum CollisionState : bool {
		kCurrent = 0, //!< 現在frameのhit情報
		kPrev    = 1, //!< 1frame前のhit情報
	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// using
	////////////////////////////////////////////////////////////////////////////////////////////
	
	using CollisionStatesBit  = std::bitset<static_cast<bool>(CollisionState::kPrev) + 1>; //!< CollisionState記録用

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	ColliderComponent(MonoBehaviour* behaviour) : BaseComponent(behaviour) {}
	virtual ~ColliderComponent() = default;

	//* bounding option *//

	void SetColliderBoundingSphere(const CollisionBoundings::Sphere& sphere = { .radius = 1.0f });

	void SetColliderBoundingCapsule(const CollisionBoundings::Capsule& capsule = { .direction = { 0.0f, 1.0f, 0.0f }, .radius = { 1.0f }, .length = { 2.0f } });

	void SetColliderBoundingAABB(const CollisionBoundings::AABB& aabb = { .min = { -0.5f, -0.5f, -0.5f }, .max = { 0.5f, 0.5f, 0.5f } });

	void SetColliderBoundingOBB(const CollisionBoundings::OBB& obb = { .orientation = Quaternion::Identity(), .size = { 1.0f, 1.0f, 1.0f } });

	void ResetColliderBounding();

	const std::optional<CollisionBoundings::Boundings>& GetBoundings() const { return bounding_; }

	//* collision state option *//

	void UpdateColliderState();

	void CallbackOnCollision(const CollisionCallbackCollection* collection);

	void OnCollision(ColliderComponent* other);

	//! @brief CollisionStateの変更
	//! @param[in] collider 判定対象のptr
	//! @param[in] isHit    現フレームの当たり判定を引数の値に変更(std::nulloptの場合は現在の値を変更しない)
	//! @param[in] isPreHit 前フレームの当たり判定を引数の値に変更(std::nulloptの場合は現在の値を変更しない)
	void SetCollisionState(
		ColliderComponent* const other,
		const std::optional<bool>& isHitCurrent = std::nullopt, const std::optional<bool>& isHitPrev = std::nullopt
	);

	//* tag option *//

	void SetTag(const std::string& tag) { tag_ = tag; }

	const std::string& GetTag() const { return tag_; }

	//* other component option *//

	TransformComponent* GetTransform() const;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* tag *//

	std::string tag_ = "new collider";

	//* boundings *//

	std::optional<CollisionBoundings::Boundings> bounding_;

	//* states *//

	//! [unordered_map]
	//! key:   対象のcollider
	//! value: state
	std::unordered_map<ColliderComponent*, CollisionStatesBit> states_;


};
