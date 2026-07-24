#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* collider
#include "ColliderComponent.h"

//* engine
#include <Engine/Foundation.h>

//* lib
#include <Lib/CXXAttribute.h>

//* c++
#include <functional>
#include <string>
#include <unordered_map>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// CollisionCallbackCollection class
////////////////////////////////////////////////////////////////////////////////////////////
//! @brief 衝突コールバック関数管理クラス
class CollisionCallbackCollection {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// using
	////////////////////////////////////////////////////////////////////////////////////////////

	using OnCollisionFunctionPenetration
		= std::function<void(MAYBE_UNUSED ColliderComponent* const, MAYBE_UNUSED ColliderComponent* const, const CollisionDetection::Penetration&)>; //!< OnCollision関数
	//!< [制約]
	//!< Penetration結果はlhsから見たrhsのPenetrationである必要がある.

	using OnCollisionFunction
		= std::function<void(MAYBE_UNUSED ColliderComponent* const, MAYBE_UNUSED ColliderComponent* const)>; //!< OnCollision関数

	////////////////////////////////////////////////////////////////////////////////////////////
	// OnCollisionFunction structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct OnCollisionCallbacks {
	public:

		//=========================================================================================
		// public variables
		//=========================================================================================

		OnCollisionFunctionPenetration enter; //!< OnCollisionEnter関数
		OnCollisionFunctionPenetration exit;  //!< OnCollisionExit関数
		OnCollisionFunctionPenetration stay;  //!< OnCollisionStay関数

	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* functions option *//

	void SetOnCollisionFunctions(const std::string& tagA, const std::string& tagB, const OnCollisionCallbacks& functions);

	void SetOnCollisionFunctionEnter(const std::string& tagA, const std::string& tagB, const OnCollisionFunctionPenetration& function);
	void SetOnCollisionFunctionEnter(const std::string& tagA, const std::string& tagB, const OnCollisionFunction& function);

	void SetOnCollisionFunctionExit(const std::string& tagA, const std::string& tagB, const OnCollisionFunctionPenetration& function);
	void SetOnCollisionFunctionExit(const std::string& tagA, const std::string& tagB, const OnCollisionFunction& function);

	void SetOnCollisionFunctionStay(const std::string& tagA, const std::string& tagB, const OnCollisionFunctionPenetration& function);
	void SetOnCollisionFunctionStay(const std::string& tagA, const std::string& tagB, const OnCollisionFunction& function);

	//* callback option *//

	void CallbackOnCollisionEnter(ColliderComponent* lhs, ColliderComponent* rhs, const CollisionDetection::Penetration& penetration) const;

	void CallbackOnCollisionExit(ColliderComponent* lhs, ColliderComponent* rhs, const CollisionDetection::Penetration& penetration) const;

	void CallbackOnCollisionStay(ColliderComponent* lhs, ColliderComponent* rhs, const CollisionDetection::Penetration& penetration) const;

	//* check option *//

	bool CheckRegistered(const ColliderComponent* lhs, const ColliderComponent* rhs) const;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* functions *//

	std::unordered_map<std::string, std::unordered_map<std::string, OnCollisionCallbacks>> callbacks_; //!< OnCollision関数
	//!< 制約: std::string x, std::string yはsortされている必要がある
	// todo: sorted pairを作る.

	//=========================================================================================
	// private methods
	//=========================================================================================

	bool Contains(const std::string& tagA, const std::string& tagB) const;

};

SXAVENGER_ENGINE_NAMESPACE_END
