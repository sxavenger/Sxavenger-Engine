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
// UAssetMesh class
////////////////////////////////////////////////////////////////////////////////////////////
class UAssetMesh final
	: public UBaseAsset {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	UAssetMesh(const Uuid& id) : UBaseAsset(id) {}
	~UAssetMesh() override = default;

	void Setup(const aiMesh* mesh, const Uuid& material);

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	Uuid material_;

};
