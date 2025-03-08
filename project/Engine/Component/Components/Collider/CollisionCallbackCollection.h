#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* collider
#include "ColliderComponent.h"

//* lib
#include <Lib/CXXAttributeConfig.h>

//* c++
#include <functional>
#include <string>
#include <unordered_map>

////////////////////////////////////////////////////////////////////////////////////////////
// CollisionCallbackCollection class
////////////////////////////////////////////////////////////////////////////////////////////
class CollisionCallbackCollection {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// using
	////////////////////////////////////////////////////////////////////////////////////////////

	using OnCollisionFunction = std::function<void(_MAYBE_UNUSED ColliderComponent* const, _MAYBE_UNUSED ColliderComponent* const)>; //!< OnCollision関数
	//!< 制約: ColliderComponent*の順番はtagの昇順である.

	////////////////////////////////////////////////////////////////////////////////////////////
	// OnCollisionFunction structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct OnCollisionCallbacks {
		OnCollisionFunction enter; //!< OnCollisionEnter関数
		OnCollisionFunction exit;  //!< OnCollisionExit関数
		OnCollisionFunction stay;  //!< OnCollisionStay関数
	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* functions option *//

	void SetOnCollisionFunctions(const std::string& x, const std::string& y, const OnCollisionCallbacks& functions);

	void CallbackOnCollisionEnter(ColliderComponent* rhs, ColliderComponent* lhs) const;
	void CallbackOnCollisionExit(ColliderComponent* rhs, ColliderComponent* lhs) const;
	void CallbackOnCollisionStay(ColliderComponent* rhs, ColliderComponent* lhs) const;

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

	static std::pair<std::string, std::string> Sort(const std::string& x, const std::string& y);
	static std::pair<ColliderComponent*, ColliderComponent*> Sort(ColliderComponent* x, ColliderComponent* y);

};
