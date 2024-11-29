#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/DirectX/DirectXContext.h>
#include <Engine/System/Runtime/Thread/Thread.h>
#include <Engine/Content/InputAssembler/InputMesh.h>
#include <Engine/Content/Texture/Texture.h>
#include <Engine/Module/Transform/Transform.h>

//* lib
#include <Lib/Geometry/Matrix4x4.h>

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
// Weight structure
////////////////////////////////////////////////////////////////////////////////////////////

struct VertexWeightData {
	float weight;
	uint32_t vertexIndex;
};

struct JointWeightData {
	Matrix4x4 inverseBindPoseMatrix;
	std::vector<VertexWeightData> vertexWeights;
};

////////////////////////////////////////////////////////////////////////////////////////////
// MeshData structure
////////////////////////////////////////////////////////////////////////////////////////////
struct MeshData {
	InputMesh                                        mesh;
	std::optional<uint32_t>                          materialIndex;
	std::unordered_map<std::string, JointWeightData> skinCluster;
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
// NodeData structure
////////////////////////////////////////////////////////////////////////////////////////////
struct Node {
	QuaternionTransform transform;
	Matrix4x4           localMatrix;

	std::string name;
	std::vector<Node> children;
};

////////////////////////////////////////////////////////////////////////////////////////////
// Model class
////////////////////////////////////////////////////////////////////////////////////////////
class Model
	: public TaskThreadExecution {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	Model()  = default;
	~Model() { Term(); }

	void Load(const std::filesystem::path& directory, const std::filesystem::path& filename, const DirectXThreadContext* context, uint32_t assimpOption = kDefaultAssimpOption_);

	void Term();

	//* task option *//

	//!< 非推奨
	void AsyncLoad(const std::filesystem::path& directory, const std::filesystem::path& filename, uint32_t assimpOption = kDefaultAssimpOption_);

	//* mesh getter *//

	void SetIABuffer(uint32_t meshIndex) const;

	const D3D12_GPU_DESCRIPTOR_HANDLE& GetTextureHandle(uint32_t meshIndex, MaterialTextureType type = MaterialTextureType::kDiffuse) const;

	void DrawCall(uint32_t meshIndex, uint32_t instanceCount = 1) const;

	uint32_t GetMeshSize() const { return static_cast<uint32_t>(meshes_.size()); }

	//* other getter *//

	static uint32_t GetDefaultAssimpOption() { return kDefaultAssimpOption_; }


private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	std::vector<MeshData>     meshes_;
	std::vector<MaterialData> materials_;

	Node root_;

	static const uint32_t kDefaultAssimpOption_;

	//* parameter *//

	std::filesystem::path directory_;
	std::filesystem::path filename_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	void LoadMesh(const aiScene* aiScene);
	void LoadMaterial(const aiScene* aiScene, const std::filesystem::path& directory, const DirectXThreadContext* context);
	Node ReadNode(aiNode* node);

	//* sub methods *//

	void CheckMeshIndex(uint32_t meshIndex) const;

};