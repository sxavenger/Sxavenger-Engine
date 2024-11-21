#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* CollisionDetection
#include "CollisionDetection.h"

//* lib
#include <Lib/CXXAttributeConfig.h>

//* c++
#include <string>
#include <functional>
#include <optional>
#include <bitset>

////////////////////////////////////////////////////////////////////////////////////////////
// ColliderType enum
////////////////////////////////////////////////////////////////////////////////////////////
enum ColliderType {
	ColliderType_NONE = 0,
};

////////////////////////////////////////////////////////////////////////////////////////////
// Collider class
////////////////////////////////////////////////////////////////////////////////////////////
class Collider {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// using
	////////////////////////////////////////////////////////////////////////////////////////////
	//!< OnCollision関数
	using OnCollisionFunction = std::function<void(_MAYBE_UNUSED Collider* const)>;

	//!< CollisionState記録用
	//! [bitset] (enum CollisionStateとindexを同期)
	//! index 0: 現在frameのhit情報
	//! index 1: 1frame前のhit情報
	using CollisionStatesBit = std::bitset<2>;

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	Collider() { Init(); }
	~Collider() { Term(); }

	void Init();

	void Term();

	//* bounding setter *//

	void SetColliderBoundingSphere(const CollisionBoundings::Sphere& sphere = { .radius = 1.0f });

	void SetColliderBoundingAABB(const CollisionBoundings::AABB& aabb = { .localMin = {-0.5f, -0.5f, -0.5f}, .localMax = {0.5f, 0.5f, 0.5f} });

	//* call back function setter *//

	void SetOnCollisionEnter(const OnCollisionFunction& func) { onCollisionEnterFunc_ = func; }

	void SetOnCollisionExit(const OnCollisionFunction& func) { onCollisionExitFunc_ = func; }

	//* collision states *//

	void SetupCallbackOnCollision();

	void CallbackOnCollision();

	void OnCollision(Collider* other);

	//* collision getter *//

	const Vector3f& GetColliderPosition() const;

	const CollisionBoundings::Boundings& GetBounding() const { return bounding_; }

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

	const CollisionStatesBit GetStates(Collider* other) const;

	void SetColliderPosition(const Vector3f& position) { position_ = position; }

	void SetTypeId(uint32_t typeId) { typeId_ = typeId; }

	void SetTargetTypeId(uint32_t targetTypeId) { targetTypeId_ = targetTypeId; }

	//* imgui option *//

	void SetColliderImGuiCommand();

private:

	////////////////////////////////////////////////////////////////////////////////////////////
	// CollisionState enum
	////////////////////////////////////////////////////////////////////////////////////////////
	enum CollisionState : uint32_t {
		kCollisionState_Current = 0,
		kCollisionState_Prev    = 1,
	};

	//=========================================================================================
	// private variables
	//=========================================================================================

	std::string colliderTag_ = "";

	//! 当たり判定の判定情報
	CollisionBoundings::Boundings bounding_;

	//!< filter情報
	uint32_t typeId_       = 0; //!< 自分のid
	uint32_t targetTypeId_ = 0; //!< 判定対象とするtype

	//* functions *//

	OnCollisionFunction onCollisionEnterFunc_;
	OnCollisionFunction onCollisionExitFunc_;

	// TODO: positionを持ってくる
	// worldInstanceから持ってくる...?

	Vector3f position_;

	//! [unordered_map]
	//! key:   対象のcollider
	//! value: 当たり判定結果
	std::unordered_map<Collider*, CollisionStatesBit> states_;

	//!< すり抜け等の管理flagが欲しい
	//!< 判定しないのflagが欲しい



};