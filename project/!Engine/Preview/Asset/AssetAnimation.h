#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* uasset
#include "BaseAsset.h"

//* engine
#include <Engine/Content/Animation/Animation.h>

//* lib
#include <Lib/Adapter/Uuid/Uuid.h>

//* external
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

////////////////////////////////////////////////////////////////////////////////////////////
// AssetAnimation class
////////////////////////////////////////////////////////////////////////////////////////////
class AssetAnimation final
	: public BaseAsset {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	AssetAnimation(const Uuid& id) : BaseAsset(id) {}
	~AssetAnimation() override = default;

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
