#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/DirectX/DirectXContext.h>
#include <Engine/Content/InputAssembler/InputMesh.h>
#include <Engine/Content/Texture/Texture.h>
#include <Engine/Content/Animation/BornNode.h>
#include <Engine/Content/Animation/JointWeight.h>
#include <Engine/Module/Transform/Transform.h>

//* external
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

//* c++
#include <cstdint>
#include <vector>
#include <optional>
#include <unordered_map>
#include <string>
#include <memory>
#include <array>
#include <filesystem>

////////////////////////////////////////////////////////////////////////////////////////////
// MeshData structure
////////////////////////////////////////////////////////////////////////////////////////////
struct MeshData {
	InputMesh                                        mesh;
	std::optional<uint32_t>                          materialIndex;
	std::unordered_map<std::string, JointWeightData> jointWeights;
};

////////////////////////////////////////////////////////////////////////////////////////////
// MaterailTextureType enum class
////////////////////////////////////////////////////////////////////////////////////////////
enum class MaterialTextureType : uint8_t {
	kDiffuse,
	kNormal,
};

////////////////////////////////////////////////////////////////////////////////////////////
// MaterialData structure
////////////////////////////////////////////////////////////////////////////////////////////
struct MaterialData {
	std::array<std::shared_ptr<Texture>, static_cast<uint8_t>(MaterialTextureType::kNormal) + 1> textures_;
};

////////////////////////////////////////////////////////////////////////////////////////////
// Model class
////////////////////////////////////////////////////////////////////////////////////////////
class Model {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	Model()  = default;
	~Model() { Term(); }

	void Load(const std::filesystem::path& filepath, const DirectXThreadContext* context, uint32_t assimpOption = kDefaultAssimpOption_);

	void Term();

	//* meshes option *//

	void SetIABuffer(uint32_t meshIndex) const;

	void DrawCall(uint32_t meshIndex, uint32_t instanceCount = 1) const;

	//* materials option *//

	const D3D12_GPU_DESCRIPTOR_HANDLE& GetTextureHandle(uint32_t meshIndex, MaterialTextureType type = MaterialTextureType::kDiffuse) const;

	//* mesh getter *//

	uint32_t GetMeshSize() const { return static_cast<uint32_t>(meshes_.size()); }

	const MeshData& GetMeshData(uint32_t meshIndex) const;

	const InputMesh& GetInputMesh(uint32_t meshIndex) const;

	//* other getter *//

	static uint32_t GetDefaultAssimpOption() { return kDefaultAssimpOption_; }

	const BornNode& GetRootNode() const { return root_; }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	std::vector<MeshData>     meshes_;
	std::vector<MaterialData> materials_;

	BornNode root_;

	static const uint32_t kDefaultAssimpOption_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	void LoadMesh(const aiScene* aiScene);
	void LoadMaterial(const aiScene* aiScene, const std::filesystem::path& directory, const DirectXThreadContext* context);
	BornNode ReadNode(aiNode* node);

	//* sub methods *//

	void CheckMeshIndex(uint32_t meshIndex) const;

};
