#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* ucontent
#include "UBaseContent.h"

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

	//* content option *//

	void Load(const std::filesystem::path& filepath, uint32_t assimpOption);

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	Uuids meshes_;
	Uuids materials_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	//* helper method *//

	void GetUuid();

	void LoadMeshes(const aiScene* aiScene, const std::filesystem::path& filepath);
	void LoadMaterials(const aiScene* aiScene, const std::filesystem::path& filepath);

};
