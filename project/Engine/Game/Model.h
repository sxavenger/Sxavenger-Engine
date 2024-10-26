#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* c++
#include <string>
#include <vector>
#include <optional>

//* assimp
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

//* ia
#include "InputMesh.h"

//* transform
#include "Transform.h"

//* object
#include <Lib/ObjectStructure.h>

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
// TextureType enum
////////////////////////////////////////////////////////////////////////////////////////////
enum TextureType {
	TEXTURE_DIFFUSE,
	TEXTURE_NORMAL,

	kCountOfTextureType
};

////////////////////////////////////////////////////////////////////////////////////////////
// MaterialData structure
////////////////////////////////////////////////////////////////////////////////////////////
struct MaterialData {
	std::string textureFilepaths[TextureType::kCountOfTextureType];
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
class Model {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	Model() = default;
	Model(const std::string& directoryPath, const std::string& filename, bool smooth = false) { Load(directoryPath, filename, smooth); }

	~Model() { Term(); }

	void Load(const std::string& directoryPath, const std::string& filename, bool smooth);

	void Term();

	//* opiton *//

	void SetIABuffer(uint32_t meshIndex);

	const D3D12_GPU_DESCRIPTOR_HANDLE GetTextureHandle(uint32_t meshIndex, TextureType type = TEXTURE_DIFFUSE);

	void DrawCall(uint32_t meshIndex, uint32_t instanceCount = 1);

	uint32_t GetMeshSize() const { return static_cast<uint32_t>(meshes_.size()); }

	uint32_t GetMaterialSize() const { return static_cast<uint32_t>(materials_.size()); }

	void ApplyMeshShader();

	void ApplyRaytracing();

	//* member getter *//

	const std::vector<MeshData>& GetMeshes() const { return meshes_; }

	const InputMesh& GetMesh(uint32_t meshIndex) const { return meshes_.at(meshIndex).mesh; }

	const std::vector<MaterialData> GetMaterials() const { return materials_; }

	const std::unordered_map<std::string, JointWeightData>& GetSkinCluster(uint32_t meshIndex) const { return meshes_.at(meshIndex).skinCluster; }

	const Node& GetRootNode() const { return rootNode_; }

	const aiScene* GetScene() const { return sceneAi_; }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* members *//

	std::vector<MeshData>     meshes_;
	std::vector<MaterialData> materials_;
	// FIXME: 余分な配列ができてる

	//* node
	Node rootNode_;

	//* externals *//

	Assimp::Importer importer_; //!< scene保存するため保管
	const aiScene*   sceneAi_ = nullptr;

	//=========================================================================================
	// private methods
	//=========================================================================================

	void LoadModelFile(const std::string& directoryPath, const std::string& filename, bool smooth);
	void LoadMesh();
	void LoadMaterial(const std::string& directoryPath);

	static Node ReadNode(aiNode* node);

};

////////////////////////////////////////////////////////////////////////////////////////////
// ModelManager class
////////////////////////////////////////////////////////////////////////////////////////////
class ModelManager {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	ModelManager()  = default;
	~ModelManager() = default;

	void Init();

	void Term();

	Model* Load(const std::string& directoryPath, const std::string& filename, bool smooth = false);

	void Delete(const std::string& directoryPath, const std::string& filename);

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//! [unordered_map<unordered_map>]
	//! key:   filepath
	//! value: モデル情報
	std::unordered_map<std::string, std::unique_ptr<Model>> models_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	static std::string ToLower(const std::string& str);

};

