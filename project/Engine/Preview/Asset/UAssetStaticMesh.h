#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* uasset
#include "UBaseAsset.h"

//* engine
#include <Engine/Content/InputGeometry/InputMesh.h>
#include <Engine/Content/Animation/JointWeight.h>

//* external
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

////////////////////////////////////////////////////////////////////////////////////////////
// UAssetStaticMesh class
////////////////////////////////////////////////////////////////////////////////////////////
class UAssetStaticMesh final
	: public UBaseAsset {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	UAssetStaticMesh()           = default;
	~UAssetStaticMesh() override = default;

	//* --- option *//

	void Load(const aiMesh* aiMesh, const std::vector<Uuid>& materials);

	//* sasset option *//

	json Serialize() const override;

	void Deserialize(const json& data) override;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	InputMesh mesh_;
	std::optional<Uuid> material_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	//* convert helper methods *//

	static Vector3f ConvertNormal(const aiVector3D& aiVector);
	static Vector3f ConvertPosition3(const aiVector3D& aiVector);
	static Vector4f ConvertPosition4(const aiVector3D& aiVector);
	static Quaternion ConvertQuaternion(const aiQuaternion& aiQuaternion);

};
