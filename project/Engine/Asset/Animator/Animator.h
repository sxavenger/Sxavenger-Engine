#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/Content/Animation/Animation.h>

//* external
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

//* c++
#include <filesystem>

////////////////////////////////////////////////////////////////////////////////////////////
// Animator class
////////////////////////////////////////////////////////////////////////////////////////////
class Animator {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	Animator()  = default;
	~Animator() { Term(); }

	void Load(const std::filesystem::path& filepath, uint32_t assimpOption = 0);

	void Term();

	//* animator option *//

	const uint32_t GetAnimationSize() const { return static_cast<uint32_t>(animations_.size()); }

	const Animation& GetAnimation(uint32_t index) const { return animations_[index]; }
	const Animation& GetAnimation(const std::string& name) const;

	const TimePointf<TimeUnit::second> GetDurationTime(uint32_t index) const { return animations_[index].duration; }
	const TimePointf<TimeUnit::second> GetDurationTime(const std::string& name) const;

	const std::vector<Animation>& GetAnimations() const { return animations_; }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* animation *//

	std::vector<Animation>                    animations_;
	std::unordered_map<std::string, uint32_t> animationMap_; //!< key: name, value: animation index

	//=========================================================================================
	// private methods
	//=========================================================================================

	static TimePointf<TimeUnit::second> GetTime(double time, double ticksPerSeconds);

	void LoadTransformAnimation(const aiNodeAnim* aiNodeAnimation, Animation& animation, double tickPerSeconds);

	const uint32_t GetAnimationIndex(const std::string& name) const;

};
