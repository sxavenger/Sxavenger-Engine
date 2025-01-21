#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* base
#include <Engine/Module/Component/TransformComponent.h>

//* collider
#include "CollisionDetection.h"

//* lib
#include <Lib/CXXAttributeConfig.h>

//* c++
#include <functional>
#include <bitset>
#include <optional>
#include <unordered_map>

////////////////////////////////////////////////////////////////////////////////////////////
// ColliderType enum
////////////////////////////////////////////////////////////////////////////////////////////
enum ColliderType {


	kCountOfColliderType
};

////////////////////////////////////////////////////////////////////////////////////////////
// Collider class
////////////////////////////////////////////////////////////////////////////////////////////
class Collider
	: public TransformComponent {
public:

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
	//!< OnCollision関数
	using OnCollisionFunction = std::function<void(_MAYBE_UNUSED Collider* const)>;

	//!< CollisionState記録用
	using CollisionStatesBit = std::bitset<static_cast<bool>(CollisionState::kPrev) + 1>;

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	Collider()  = default;
	~Collider() { Term(); }

	void Term();

	void SetToCollection();

	//* bounding option *//

	void SetColliderBoundingSphere(const CollisionBoundings::Sphere& sphere = { .radius = 1.0f });

	void SetColliderBoundingCapsule(const CollisionBoundings::Capsule& capsule = { .direction = { 0.0f, 1.0f, 0.0f }, .radius = { 1.0f }, .length = { 2.0f } });

	void SetColliderBoundingAABB(const CollisionBoundings::AABB& aabb = { .min = {-0.5f, -0.5f, -0.5f}, .max = {0.5f, 0.5f, 0.5f} });

	void SetColliderBoundingOBB(const CollisionBoundings::OBB& obb = { .orientation = Quaternion::Identity(), .size = { 1.0f, 1.0f, 1.0f } });

	const std::optional<CollisionBoundings::Boundings>& GetBoundings() const { return bounding_; }

	//* collision function setter *//

	void SetOnCollisionEnter(const OnCollisionFunction& function) { collisionEnterFunction_ = function; }

	void SetOnCollisionExit(const OnCollisionFunction& function) { collisionExitFunction_ = function; }

	//* collision state option *//

	void SetupColliderState();

	void CallbackOnCollision();

	void OnCollision(Collider* other);

	//! @brief CollisionStateの変更
	//! @param[in] collider 判定対象のptr
	//! @param[in] isHit    現フレームの当たり判定を引数の値に変更(std::nulloptの場合は現在の値を変更しない)
	//! @param[in] isPreHit 前フレームの当たり判定を引数の値に変更(std::nulloptの場合は現在の値を変更しない)
	void SetCollisionState(
		Collider* const collider,
		const std::optional<bool>& isHitCurrent = std::nullopt, const std::optional<bool>& isHitPrev = std::nullopt
	);

	const CollisionStatesBit GetState(Collider* other) const;

	//* type and target option *//

	void SetTypeId(ColliderType type, bool isEnabled = true) { typeId_.set(static_cast<uint32_t>(type), isEnabled); }

	void SetTargetId(ColliderType type, bool isEnabled = true) { targetId_.set(static_cast<uint32_t>(type), isEnabled); }

	bool CheckCollisionTarget(const Collider* const other) const;

	//* active method *//

	void SetIsActive(bool isActive) { isActive_ = isActive; }

	bool IsActive() const { return isActive_; }

	//* imgui command *//

	void SetImGuiCommand();

protected:

	//=========================================================================================
	// protected variables
	//=========================================================================================

	std::string tag_ = "new collider";

	bool isActive_ = true;

	//* boundings *//

	std::optional<CollisionBoundings::Boundings> bounding_;

	//* filter *//

	std::bitset<static_cast<uint32_t>(kCountOfColliderType)> typeId_;   //!< 自分のid
	std::bitset<static_cast<uint32_t>(kCountOfColliderType)> targetId_; //!< 判定対象とするtype

	//* functions *//

	OnCollisionFunction collisionEnterFunction_;
	OnCollisionFunction collisionExitFunction_;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* states *//

	//! [unordered_map]
	//! key:   対象のcollider
	//! value: state
	std::unordered_map<Collider*, CollisionStatesBit> states_;

};
