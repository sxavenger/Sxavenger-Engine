#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* uasset
#include "UBaseAsset.h"

//* engine
#include <Engine/Content/Animation/Animation.h>

//* lib
#include <Lib/Adapter/Uuid/Uuid.h>

//* external
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

////////////////////////////////////////////////////////////////////////////////////////////
// UAssetAnimation class
////////////////////////////////////////////////////////////////////////////////////////////
class UAssetAnimation final
	: public UBaseAsset {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	UAssetAnimation(const Uuid& id) : UBaseAsset(id) {}
	~UAssetAnimation() override = default;

	void Setup(const aiAnimation* animation);

	//* getter *//

	const Animation& GetAnimation() const { return animation_; }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	Animation animation_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	static TimePointd<TimeUnit::second> GetTime(double time, double ticksPerSeconds);

	void LoadTransformAnimation(const aiNodeAnim* aiNodeAnimation, double tickPerSeconds);

};
