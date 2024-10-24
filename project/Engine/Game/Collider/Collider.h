#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* lib
#include <Lib/CXXAttributeConfig.h>
#include <Lib/Geometry/Vector3.h>

//* c++
#include <utility>
#include <string>
#include <unordered_map>
#include <optional>

// collisionDetection
#include "CollisionDetection.h"

////////////////////////////////////////////////////////////////////////////////////////////
// ColliderType enum
////////////////////////////////////////////////////////////////////////////////////////////
enum ColliderType {
	ColliderType_NONE  = 0,
	ColliderType_Enemy = 1 << 0,
};

////////////////////////////////////////////////////////////////////////////////////////////
// Collider base class
////////////////////////////////////////////////////////////////////////////////////////////
class Collider {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	Collider() { Init(); }
	virtual ~Collider() { Term(); }

	void Init();

	void Term();

	//* bounding setter *//

	void SetColliderBoundingSphere(const CollisionBoundings::Sphere& sphere = {.radius = 1.0f});

	void SetColliderBoundingAABB(const CollisionBoundings::AABB& aabb = {.localMin = {-0.5f, -0.5f, -0.5f}, .localMax = {0.5f, 0.5f, 0.5f}});

	//* collision states *//

	void CallOnCollisionMethods();

	void OnCollision(Collider* other);
	
	//* collision getter *//

	virtual const Vector3f& GetColliderPosition() const;

	const std::optional<CollisionBoundings::Boundings>& GetBounding() const { return bounding_; }

	//! @brief targetと相手のIdを比較して当たり判定が必要かどうか確認
	bool ShouldCheckForCollision(const Collider* const other) const;

	//* collision setter *//

	//! @brief CollisionStateの変更
	//! @param[in] collider 判定対象のptr
	//! @param[in] isHit    現フレームの当たり判定を引数の値に変更(std::nulloptの場合は現在の値を変更しない)
	//! @param[in] isPreHit 前フレームの当たり判定を引数の値に変更(std::nulloptの場合は現在の値を変更しない)
	void SetCollisionState(
		Collider* const collider,
		const std::optional<bool>& isHit = std::nullopt, const std::optional<bool>& isPreHit = std::nullopt
	);

	//* user collision methods *//

	virtual void OnCollisionEnter(_MAYBE_UNUSED Collider* const other) {}

	virtual void OnCollisionExit(_MAYBE_UNUSED Collider* const other) {}

	//* imgui *//

	void SetColliderImGuiCommand();

protected:

	//=========================================================================================
	// protected variables
	//=========================================================================================
	
	std::string colliderTag_ = "";

	//! 当たり判定の判定情報
	std::optional<CollisionBoundings::Boundings> bounding_ = std::nullopt;

	//! 座標情報
	std::optional<Vector3f> position_ = std::nullopt;

	//!< filter情報
	uint32_t typeId_       = 0; //!< 自分のid
	uint32_t targetTypeId_ = 0; //!< 判定対象とするtype

private:

	////////////////////////////////////////////////////////////////////////////////////////////
	// CollisionState structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct CollisionState {
		bool isHit, isPreHit;
	};

	//=========================================================================================
	// private variables
	//=========================================================================================

	//! [unordered_map]
	//! key:   対象のcollider
	//! value: 当たり判定結果
	std::unordered_map<Collider*, CollisionState> states_;

	//!< すり抜け等の管理flagが欲しい
	//!< 判定しないのflagが欲しい


};