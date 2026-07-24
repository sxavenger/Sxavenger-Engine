#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* content
#include "BaseContent.h"

//* engine
#include <Engine/Foundation.h>
#include <Engine/Graphics/Animation/BornNode.h>

//* lib
#include <Lib/Math/Vector3.h>
#include <Lib/Math/Vector4.h>
#include <Lib/Math/Quaternion.h>

//* external
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

//* c++
#include <vector>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// ContentModel class
////////////////////////////////////////////////////////////////////////////////////////////
class ContentModel final
	: public BaseContent {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* constructor / destructor *//

	ContentModel() : BaseContent(Async::Execution::Cpu) {}

	~ContentModel() override = default;

	//* content option *//

	void Attach(const std::filesystem::path& filepath, const std::any& parameter) override;

	void Load(MAYBE_UNUSED const DirectXQueueContext* context) override;

	//* parameter option *//

	size_t GetMeshCount() const { return meshes_.size(); }

	const BornNode& GetRoot() const { return root_; }

	//* id option *//

	const Uuid& GetMeshId(size_t index) const { return meshes_[index]; }

	const Uuid& GetMaterialId(size_t index) const { return materials_[index]; }

	const Uuid& GetMeshToMaterialId(size_t meshIndex) const { return materials_[materialIndices_[meshIndex]]; }

	const Uuid& GetSkeletonId() const { return skeleton_; }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	std::vector<Uuid> meshes_;
	std::vector<Uuid> materials_;

	std::vector<uint32_t> materialIndices_; //!< mesh index to material index

	BornNode root_;
	Uuid skeleton_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	void AttachUuid(const std::filesystem::path& filepath);

	//* load helper methods *//

	uint32_t GetOption();

	static std::shared_ptr<Assimp::Importer> LoadImporter(const std::filesystem::path& filepath, uint32_t option);

	void SetupMaterialIndices(const aiScene* aiScene);

	static BornNode ReadNode(const aiNode* node);

	void SetupBornNode(const aiNode* aiNode);

};

SXAVENGER_ENGINE_NAMESPACE_END
