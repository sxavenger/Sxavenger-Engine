#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
// Geometry
#include <Vector3.h>

////////////////////////////////////////////////////////////////////////////////////////////
// AABB structure
////////////////////////////////////////////////////////////////////////////////////////////
struct AABB {
	Vector3f min;
	Vector3f max;
};

////////////////////////////////////////////////////////////////////////////////////////////
// Collider namespace
////////////////////////////////////////////////////////////////////////////////////////////
namespace Collider {

	//! @brief AABBとAABBの衝突判定
	//! 
	//! @param[in] a AABB
	//! @param[in] b AABB
	//! 
	//! @retval true  衝突してる
	//! @retval false 衝突してない
	bool AABBToAABB(const AABB& a, const AABB& b);

	//! @brief AABBとPointの衝突判定
	//! 
	//! @param[in] aabb AABB
	//! @param[in] point Point 
	//! 
	//! @retval true  衝突してる
	//! @retval false 衝突してない
	bool AABBToPoint(const AABB& aabb, const Vector3f& point);

}