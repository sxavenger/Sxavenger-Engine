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

	void Load(const std::filesystem::path& filepath);

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	std::vector<Animation>                    animations_;
	std::unordered_map<std::string, uint32_t> animationMap_; //!< key: name, value: animation index

};