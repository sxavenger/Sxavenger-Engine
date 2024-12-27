#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/Content/Animation/Animation.h>
#include <Engine/Content/Animation/BornNode.h>
//#include "AnimationGroup.h"

//* lib
#include <Lib/Geometry/Matrix4x4.h>
#include <Lib/Transform/Transform.h>

//* c++
#include <cstdint>
#include <string>
#include <vector>
#include <optional>
#include <unordered_map>

////////////////////////////////////////////////////////////////////////////////////////////
// Joint structure
////////////////////////////////////////////////////////////////////////////////////////////
struct Joint {

	std::string name;
	uint32_t index; //!< 自身のindex

	std::optional<uint32_t> parent; //!< 親のJointのindex, いなければnull
	std::vector<uint32_t> children; //!< 子のJointのindexリスト, いなければ空

	QuaternionTransform transform;

	Matrix4x4 localMatrix;
	Matrix4x4 skeletonSpaceMatrix; //!< skeleton空間でのmatrix
};

////////////////////////////////////////////////////////////////////////////////////////////
// Skeleton structure
////////////////////////////////////////////////////////////////////////////////////////////
struct Skeleton {

	//=========================================================================================
	// public methods
	//=========================================================================================

	void Create(const BornNode& node);

	void Update(const Animation& animation, DeltaTimePoint<TimeUnit::s> time, bool isLoop = true);
	//void Update(const AnimationGroup& animationGroup, DeltaTimePoint<TimeUnit::s> time, bool isLoop = true);

	void TransitionAnimation(
		const Animation& animationA, DeltaTimePoint<TimeUnit::s> timeA,
		const Animation& animationB, DeltaTimePoint<TimeUnit::s> timeB,
		float t
	);

	//=========================================================================================
	// public variables
	//=========================================================================================

	uint32_t root;

	std::unordered_map<std::string, uint32_t> jointMap; //!< key: joint名, value: index
	std::vector<Joint>                        joints;   //!< 所属してるJoint

private:

	//=========================================================================================
	// private methods
	//=========================================================================================

	uint32_t CreateJoint(const BornNode& node, const std::optional<uint32_t>& parent);

	std::optional<QuaternionTransform> GetTransform(const std::string& jointName, const Animation& animation, DeltaTimePoint<TimeUnit::s> time);

	void ApplyAnimation(const Animation& animation, DeltaTimePoint<TimeUnit::s> time);

	void ApplyTransitionAnimation(
		const Animation& animationA, DeltaTimePoint<TimeUnit::s> timeA,
		const Animation& animationB, DeltaTimePoint<TimeUnit::s> timeB,
		float t
	);

	void UpdateMatrix();

};
