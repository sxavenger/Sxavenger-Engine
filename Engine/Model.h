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
#include <map>

// Geometry
#include <Vector4.h>
#include <Vector3.h>
#include <Vector2.h>

// Animation
#include <Animation.h>

// Object
#include <ObjectStructure.h>

////////////////////////////////////////////////////////////////////////////////////////////
// TextureType enum
////////////////////////////////////////////////////////////////////////////////////////////
enum TextureType {
	TEXTURE_DIFFUSE,
	TEXTURE_NORMAL,

	kCountOfTextureType
};

////////////////////////////////////////////////////////////////////////////////////////////
// MeshData structure
////////////////////////////////////////////////////////////////////////////////////////////
struct MeshData {
	std::unique_ptr<DxObject::BufferResource<VertexData>> vertexResource;
	std::unique_ptr<DxObject::IndexBufferResource>        indexResource;

	void Create(DxObject::Devices* devices, uint32_t vertexSize, uint32_t indexSize) {
		vertexResource = std::make_unique<DxObject::BufferResource<VertexData>>(devices, vertexSize);
		indexResource = std::make_unique<DxObject::IndexBufferResource>(devices, indexSize);
	}
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
	std::vector<MeshData>     meshes;
	std::vector<MaterialData> materials;
	Node                      rootNode;
	//!< meshsとmaterials, nodeのsizeは同じ
	
	std::map<std::string, JointWeightData> skinCkusterData;
	//!< hash mapにするかも
	
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
	Model(const std::string& directoryPath, const std::string& filename) { Init(directoryPath, filename); }

	//! @brief デストラクタ
	~Model() { Term(); }

	//! @brief 初期化処理
	void Init(const std::string& directoryPath, const std::string& filename);

	//! @brief 終了処理
	void Term();

	/* draw methods */

	void SetBuffers(ID3D12GraphicsCommandList* commandList, uint32_t modelIndex);

	void SetGraphicsTextureHandle(ID3D12GraphicsCommandList* commandList, uint32_t modelIndex, UINT parameterNum, TextureType type = TEXTURE_DIFFUSE);

	void DrawCall(ID3D12GraphicsCommandList* commandList, uint32_t modelIndex, uint32_t instanceCount = 1);

	/* gertter */

	const uint32_t GetModelIndexSize() const { return modelIndexSize_; }

	const MeshData& GetMesh(uint32_t modelIndexSize) const { return modelData_.meshes[modelIndexSize]; }

	const Node& GetRootNode() const { return modelData_.rootNode; }

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

}