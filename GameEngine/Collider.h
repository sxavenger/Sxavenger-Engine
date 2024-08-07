#pragma once

//-----------------------------------------------------------------------------------------
// defines
//-----------------------------------------------------------------------------------------
#define MAYBE_UNUSED [[maybe_unused]]

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
// Geometry
#include <Vector3.h>

// c++
#include <utility>
#include <string>
#include <unordered_map>

// collisionDetection
#include <CollisionDetection.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Collider base class
////////////////////////////////////////////////////////////////////////////////////////////
class Collider {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	Collider();
	virtual ~Collider();

	void TEST_Init();

	//* collision states *//

	void CallOnCollisionMethods();

	void OnCollision(Collider* other);
	
	//* collision getter *//

	virtual const Vector3f& GetColliderPosition() const { return position_; }

	const CollisionBoundings::Boundings& GetBounding() const { return bounding_; }

	//* collision methods *//

	virtual void OnCollisionEnter(MAYBE_UNUSED Collider* const other) {}

	virtual void OnCollisionExit(MAYBE_UNUSED Collider* const other) {}

protected:

	//=========================================================================================
	// protected variables
	//=========================================================================================
	
	std::string collisionTag_ = "";

	Vector3f position_ = {}; //!< ユーザー定義のpositionでもok

	//! 当たり判定の判定情報
	CollisionBoundings::Boundings bounding_;

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