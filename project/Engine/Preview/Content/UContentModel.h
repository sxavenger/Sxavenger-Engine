#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* ucontent
#include "UBaseContent.h"

//* engine
#include <Engine/Content/Animation/BornNode.h>

//* lib
#include <Lib/Geometry/Vector3.h>
#include <Lib/Geometry/Vector4.h>
#include <Lib/Geometry/Quaternion.h>

//* external
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

//* c++
#include <vector>

////////////////////////////////////////////////////////////////////////////////////////////
// UContentModel class
////////////////////////////////////////////////////////////////////////////////////////////
class UContentModel final
	: public UBaseContent {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// using
	////////////////////////////////////////////////////////////////////////////////////////////

	using Uuids = std::vector<Uuid>;

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	UContentModel()           = default;
	~UContentModel() override = default;

	void AsyncLoad(_MAYBE_UNUSED const DirectXQueueContext* context) override;

	AsyncExecution GetAsyncExecution() const { return AsyncExecution::None; }

	void AttachUuid() override;

	//* inspector option *//

	void ShowInspector() override;

	//* content option *//

	void Load(const std::filesystem::path& filepath, uint32_t assimpOption);

	size_t GetMeshCount() const { return meshes_.size(); }

	const Uuid& GetMeshId(size_t index) const { return meshes_[index]; }

	const Uuid& GetMaterialId(size_t index) const { return materials_[index]; }

	const Uuid& GetMeshToMaterialId(size_t meshIndex) const { return materials_[materialIndices_[meshIndex]]; }

	const BornNode& GetRoot() const { return root_; }

	const Uuid& GetSkeletonId() const { return skeleton_; }

	//* convert helper methods *//

	static Vector3f ConvertNormal(const aiVector3D& aiVector);
	static Vector3f ConvertPosition3(const aiVector3D& aiVector);
	static Vector4f ConvertPosition4(const aiVector3D& aiVector);
	static Quaternion ConvertQuaternion(const aiQuaternion& aiQuaternion);

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	Uuids meshes_;
	Uuids materials_;

	std::vector<uint32_t> materialIndices_; //!< mesh index to material index

	BornNode root_;
	Uuid skeleton_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	//* helper method *//

	void GetUuid();

	void LoadMeshes(const aiScene* aiScene);
	void LoadMaterials(const aiScene* aiScene, const std::filesystem::path& filepath);

	BornNode ReadNode(aiNode* node);
	void LoadSkeleton(const aiScene* aiScene, const std::filesystem::path& filepath);

};
