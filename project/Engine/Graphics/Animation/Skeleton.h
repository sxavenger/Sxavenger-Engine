#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/Graphics/Animation/Animation.h>
#include <Engine/Graphics/Animation/BornNode.h>

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
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// Joint structure
////////////////////////////////////////////////////////////////////////////////////////////
//! @brief Joint構造体
struct Joint {

	std::string name;
	uint32_t index; //!< 自身のindex

	std::optional<uint32_t> parent; //!< 親のJointのindex, いなければnull

	QuaternionTransform transform;

	Matrix4x4 localMatrix;
	Matrix4x4 skeletonSpaceMatrix; //!< skeleton空間でのmatrix
};

////////////////////////////////////////////////////////////////////////////////////////////
// Skeleton structure
////////////////////////////////////////////////////////////////////////////////////////////
//! @brief Skeleton構造体
struct Skeleton {

	//=========================================================================================
	// public methods
	//=========================================================================================

	void Create(const BornNode& node);

	void Update(const Animation& animation, TimePointd<TimeUnit::second> time, bool isLoop = true);
	//void Update(const AnimationGroup& animationGroup, DeltaTimePoint<TimeUnit::s> time, bool isLoop = true);

	void TransitionAnimation(
		const Animation& animationA, TimePointd<TimeUnit::second> timeA, bool isLoopA,
		const Animation& animationB, TimePointd<TimeUnit::second> timeB, bool isLoopB,
		float t
	);

	//* getter *//

	size_t GetJointSize() const { return joints.size(); }

	const bool ContainsJoint(const std::string& name) const { return jointMap.contains(name); }

	const Joint& GetJoint(const std::string& name) const;

	//=========================================================================================
	// public variables
	//=========================================================================================

	std::vector<Joint> joints; //!< 所属してるJoint

	uint32_t root;
	std::unordered_map<std::string, uint32_t> jointMap; //!< key: joint名, value: index

private:

	//=========================================================================================
	// private methods
	//=========================================================================================

	uint32_t CreateJoint(const BornNode& node, const std::optional<uint32_t>& parent);

	std::optional<QuaternionTransform> GetTransform(const std::string& jointName, const Animation& animation, TimePointd<TimeUnit::second> time);

	void ApplyAnimation(const Animation& animation, TimePointd<TimeUnit::second> time);

	void ApplyTransitionAnimation(
		const Animation& animationA, TimePointd<TimeUnit::second> timeA,
		const Animation& animationB, TimePointd<TimeUnit::second> timeB,
		float t
	);

	void UpdateMatrix();

};

SXAVENGER_ENGINE_NAMESPACE_END
