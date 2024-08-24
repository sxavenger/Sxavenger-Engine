#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
// assimp
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

// DxObject
#include <DxBufferResource.h>

// c++
#include <string>
#include <memory>

// Geometry
#include <Vector4.h>
#include <Vector3.h>
#include <Vector2.h>

// Animation
#include <Animation.h>

// Object
#include <ObjectStructure.h>

// InputAssembler
#include <InputAssembler.h>

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
	std::string textureFilePaths[kCountOfTextureType];
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
// ModelData structure
////////////////////////////////////////////////////////////////////////////////////////////
struct ModelData {
	std::vector<InputAssembler<VertexData>> meshes;
	std::vector<MaterialData>               materials;
	Node                                    rootNode;
	//!< meshsとmaterials, nodeのsizeは同じ
	
	std::unordered_map<std::string, JointWeightData> skinCkusterData;
	
	void Clear() {
		meshes.clear();
		materials.clear();
	}
};

////////////////////////////////////////////////////////////////////////////////////////////
// Model class
////////////////////////////////////////////////////////////////////////////////////////////
class Model {
public:
	
	//=========================================================================================
	// public methods
	//=========================================================================================

	Model() = delete;

	//! @brief コンストラクタ
	Model(const std::string& directoryPath, const std::string& filename) { Load(directoryPath, filename); }

	//! @brief デストラクタ
	~Model() { Term(); }

	//! @brief 初期化処理
	void Load(const std::string& directoryPath, const std::string& filename);

	//! @brief 終了処理
	void Term();

	//* draw methods *//

	void SetBuffers(uint32_t modelIndex);
	void SetBuffers(ID3D12GraphicsCommandList* commandList, uint32_t modelIndex);

	void SetGraphicsTextureHandle(ID3D12GraphicsCommandList* commandList, uint32_t modelIndex, UINT parameterNum, TextureType type = TEXTURE_DIFFUSE);

	const D3D12_GPU_DESCRIPTOR_HANDLE GetTextureHandle(uint32_t modelIndex, TextureType type = TEXTURE_DIFFUSE);

	void DrawCall(ID3D12GraphicsCommandList* commandList, uint32_t modelIndex, uint32_t instanceCount = 1);
	void DrawCall(uint32_t modelIndex, uint32_t instanceCount = 1);

	//* gertter *//

	const uint32_t GetModelIndexSize() const { return modelIndexSize_; }

	const InputAssembler<VertexData>& GetMesh(uint32_t modelIndexSize) const { return modelData_.meshes[modelIndexSize]; }

	const Node& GetRootNode() const { return modelData_.rootNode; }

	const ModelData& GetModelData() const { return modelData_; }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	ModelData modelData_;
	uint32_t  modelIndexSize_;

};

////////////////////////////////////////////////////////////////////////////////////////////
// ModelMethods namespace
////////////////////////////////////////////////////////////////////////////////////////////
namespace ModelMethods { //!< modelLoaderクラスを作ってもいいかも...

	ModelData LoadModelFile(const std::string& directoryPath, const std::string& filename);

	Node ReadNode(aiNode* node);

	Animation LoadAnimationFile(const std::string& directoryPath, const std::string& filename);

	int32_t CreateJoint(const Node& node, const std::optional<int32_t>& parent, std::vector<Joint>& joints);

	Skeleton CreateSkeleton(const Node& rootNode);

	SkinCluster CreateSkinCluster(const Skeleton& skeleton, const ModelData& modelData);

}