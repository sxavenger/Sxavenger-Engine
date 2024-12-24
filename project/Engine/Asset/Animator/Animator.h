#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* asset
#include "../BaseAsset.h"

//* engine
#include <Engine/Content/Animation/Animation.h>

//* external
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

//* c++
#include <vector>
#include <unordered_map>

////////////////////////////////////////////////////////////////////////////////////////////
// Animator class
////////////////////////////////////////////////////////////////////////////////////////////
class Animator
	: public BaseAsset {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	Animator()  = default;
	~Animator() { Term(); }

	void Load(_MAYBE_UNUSED const DirectXThreadContext* context) override;

	void Term();

	//* animator option *//

	const uint32_t GetAnimationSize() const { return static_cast<uint32_t>(animations_.size()); }

	const Animation& GetAnimation(uint32_t index) const { return animations_[index]; }
	const Animation& GetAnimation(const std::string& name) const;

	const DeltaTimePoint<TimeUnit::s> GetDurationTime(uint32_t index) const { return animations_[index].duration; }
	const DeltaTimePoint<TimeUnit::s> GetDurationTime(const std::string& name) const;

	const std::vector<Animation>& GetAnimations() const { return animations_; }

	//* setter *//

	void SetAssimpOption(uint32_t assimpOption) { assimpOption_ = assimpOption; }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* animation *//

	std::vector<Animation>                    animations_;
	std::unordered_map<std::string, uint32_t> animationMap_; //!< key: name, value: animation index

	//* input parameter *//

	uint32_t assimpOption_ = 0;

	//=========================================================================================
	// private methods
	//=========================================================================================

	static DeltaTimePoint<TimeUnit::s> GetTime(double time, double ticksPerSeconds);

	void LoadTransformAnimation(const aiNodeAnim* aiNodeAnimation, Animation& animation, double tickPerSeconds);

	const uint32_t GetAnimationIndex(const std::string& name) const;

};
