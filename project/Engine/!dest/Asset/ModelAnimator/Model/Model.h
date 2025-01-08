#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* asset
#include "../../BaseAsset.h"
#include "../../Texture/Texture.h"

//* engine
#include <Engine/Content/InputAssembler/InputMesh.h>
#include <Engine/Content/Animation/JointWeight.h>
#include <Engine/Content/Animation/BornNode.h>

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
// constexpr variables
////////////////////////////////////////////////////////////////////////////////////////////

constexpr const uint32_t kChildThreadCount = 4;

////////////////////////////////////////////////////////////////////////////////////////////
// Model class
////////////////////////////////////////////////////////////////////////////////////////////
class Model
	: public BaseAsset {
public:

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
	enum class TextureType : uint8_t {
		Diffuse,
		Bump,
	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// MaterialData structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct MaterialData {
		std::array<std::shared_ptr<Texture>, static_cast<uint8_t>(TextureType::Bump) + 1> textures_;
	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	Model()  = default;
	~Model() { Term(); }

	void Load(_MAYBE_UNUSED const DirectXThreadContext* context) override;

	void Term();

	//* meshes option *//

	void SetIABuffer(uint32_t meshIndex) const;

	void DrawCall(uint32_t meshIndex, uint32_t instanceCount = 1) const;

	uint32_t GetMeshSize() const { return static_cast<uint32_t>(meshes_.size()); }

	const MeshData& GetMeshData(uint32_t meshIndex) const;

	const InputMesh& GetInputMesh(uint32_t meshIndex) const;

	//* material option *//

	uint32_t GetMaterialIndex(uint32_t meshIndex) const;

	bool CheckTextureWithMaterialIndex(uint32_t materialIndex, TextureType type) const;
	bool CheckTextureWithMeshIndex(uint32_t meshIndex, TextureType type) const;

	const D3D12_GPU_DESCRIPTOR_HANDLE& GetTextureHandle(uint32_t meshIndex, TextureType type = TextureType::Diffuse) const;

	//* node option *//

	const BornNode& GetRootNode() const { return root_; }

	//* getter *//

	static const uint32_t GetDefaultAssimpOption() { return kDefaultAssimpOption_; }

	//* setter *//

	void SetAssimpOption(uint32_t option) { assimpOption_ = option; }

	void SetUseMeshShader(bool useMeshShader) { useMeshShader_ = useMeshShader; }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* mesh data

	std::vector<MeshData>     meshes_;
	std::vector<MaterialData> materials_;

	BornNode root_;

	//* input parameter *//

	static const uint32_t kDefaultAssimpOption_;
	uint32_t assimpOption_ = kDefaultAssimpOption_;

	bool useMeshShader_ = false;

	//=========================================================================================
	// private methods
	//=========================================================================================

	void LoadMesh(const aiScene* aiScene);

	void LoadMaterial(const aiScene* aiScene, const std::filesystem::path& directory, const DirectXThreadContext* context);

	BornNode ReadNode(aiNode* node);

	void CreateMeshlet();

	//* sub methods *//

	void CheckMeshIndex(uint32_t meshIndex) const;

};
