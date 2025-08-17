#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* uasset
#include "UBaseAsset.h"

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

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

};
