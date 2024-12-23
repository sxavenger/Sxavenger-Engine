#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* animation
#include "Animation.h"

//* external
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

//* c++
#include <filesystem>
#include <vector>
#include <unordered_map>

////////////////////////////////////////////////////////////////////////////////////////////
// AnimationGroup class
////////////////////////////////////////////////////////////////////////////////////////////
class AnimationGroup {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	void Load(const std::filesystem::path& filepath, uint32_t assimpOption = 0);

	//* getter *//

	const uint32_t GetAnimationSize() const { return static_cast<uint32_t>(animations_.size()); }

	const Animation& GetAnimation(uint32_t index) const { return animations_[index]; }
	const Animation& GetAnimation(const std::string& name) const;

	const DeltaTimePoint<TimeUnit::s> GetDurationTime(uint32_t index) const { return animations_[index].duration; }
	const DeltaTimePoint<TimeUnit::s> GetDurationTime(const std::string& name) const;

	const std::vector<Animation>& GetAnimations() const { return animations_; }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	std::vector<Animation>                    animations_;
	std::unordered_map<std::string, uint32_t> animationMap_; //!< key: name, value: animation index

	//=========================================================================================
	// private methods
	//=========================================================================================

	static DeltaTimePoint<TimeUnit::s> GetTime(double time, double ticksPerSeconds);

	void LoadTransformAnimation(const aiNodeAnim* aiNodeAnimation, Animation& animation, double tickPerSeconds);

};
