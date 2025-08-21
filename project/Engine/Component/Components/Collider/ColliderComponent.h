#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* component
#include "../BaseComponent.h"
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

	void ShowComponentInspector() override;

	//* bounding option *//

	void SetColliderBoundingSphere(const CollisionBoundings::Sphere& sphere = { .radius = 1.0f });

	void SetColliderBoundingCapsule(const CollisionBoundings::Capsule& capsule = { .direction = { 0.0f, 1.0f, 0.0f }, .radius = { 1.0f }, .length = { 2.0f } });

	void SetColliderBoundingAABB(const CollisionBoundings::AABB& aabb = { .min = { -0.5f, -0.5f, -0.5f }, .max = { 0.5f, 0.5f, 0.5f } });

	void SetColliderBoundingOBB(const CollisionBoundings::OBB& obb = { .orientation = Quaternion::Identity(), .size = { 1.0f, 1.0f, 1.0f } });

	const CollisionBoundings::Boundings& GetBoundings() const { return bounding_; }

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

	//* active option *//

	void SetEnable(bool isEnable) { isEnable_ = isEnable; }

	bool IsEnable() const { return isEnable_; }

	//* other component option *//

	TransformComponent* RequireTransform() const;

private:

	////////////////////////////////////////////////////////////////////////////////////////////
	// BoundingPrimitiveLine structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct BoundingPrimitiveLine {
	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		void operator()(const CollisionBoundings::Sphere& sphere);
		void operator()(const CollisionBoundings::Capsule& capsule);
		void operator()(const CollisionBoundings::AABB& aabb);
		void operator()(const CollisionBoundings::OBB& obb);

		//=========================================================================================
		// public variables
		//=========================================================================================

		Vector3f position = {};
		Color4f color     = {};

	};

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	bool isEnable_ = true;

	//* tag *//

	std::string tag_ = "new collider";

	//* boundings *//

	CollisionBoundings::Boundings bounding_ = CollisionBoundings::Sphere{ 1.0f };

	//* states *//

	//! [unordered_map]
	//! key:   対象のcollider
	//! value: state
	std::unordered_map<ColliderComponent*, CollisionStatesBit> states_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	//* helper methods *//

	void PushBoundingLine() const;

};
