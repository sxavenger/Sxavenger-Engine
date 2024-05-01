#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
// c++
#include <vector>
#include <unordered_map>
#include <string>
#include <fstream>
#include <sstream>
#include <cassert>
#include <memory>

#include "MyEngine.h" //!< devices, TextureManagerの取り出し
#include <TextureManager.h>

// DxObject
#include <DxBufferResource.h>

// Geometry
#include <Vector4.h>
#include <Vector3.h>
#include <Vector2.h>

#include <ObjectStructure.h>

////////////////////////////////////////////////////////////////////////////////////////////
// MeshData structure
////////////////////////////////////////////////////////////////////////////////////////////
struct MeshData {
	std::unique_ptr<DxObject::BufferResource<VertexData>> vertexResource;
	std::unique_ptr<DxObject::IndexBufferResource>        indexResource;
};

////////////////////////////////////////////////////////////////////////////////////////////
// MaterialData structure
////////////////////////////////////////////////////////////////////////////////////////////
struct MaterialData {
	std::string textureFilePath;
	bool        isUseTexture;
};

////////////////////////////////////////////////////////////////////////////////////////////
// ModelData structure
////////////////////////////////////////////////////////////////////////////////////////////
struct ModelData {
	std::vector<MeshData>     meshs;
	std::vector<MaterialData> materials;
	// meshsとmaterialsのsizeは同じ
};

////////////////////////////////////////////////////////////////////////////////////////////
// Model class
////////////////////////////////////////////////////////////////////////////////////////////
class Model {
public:

	//! @brief コンストラクタ
	Model(const std::string& directoryPath, const std::string& filename) {
		Init(directoryPath, filename);
	}

	//! @brief デストラクタ
	~Model() { Term(); }

	//! @brief 初期化処理
	void Init(const std::string& directoryPath, const std::string& filename);

	void Term();

	const uint32_t GetSize() const { return size_; }
	
	// Draw
	void SetBuffers(ID3D12GraphicsCommandList* commandList, uint32_t index) {
		if (index >= size_) {
			assert(false); //!< 配列以上のmodelDataの呼び出し
		}

		D3D12_VERTEX_BUFFER_VIEW vertexBufferView = modelData_.meshs[index].vertexResource->GetVertexBufferView();
		D3D12_INDEX_BUFFER_VIEW indexBufferView = modelData_.meshs[index].indexResource->GetIndexBufferView();

		commandList->IASetVertexBuffers(0, 1, &vertexBufferView);
		commandList->IASetIndexBuffer(&indexBufferView);
	}

	void SetTexture(UINT parameterNum, ID3D12GraphicsCommandList* commandList, uint32_t index) {
		if (modelData_.materials[index].isUseTexture) {
			commandList->SetGraphicsRootDescriptorTable(parameterNum, MyEngine::GetTextureHandleGPU(modelData_.materials[index].textureFilePath));
		}
	}

	void DrawCall(ID3D12GraphicsCommandList* commandList, uint32_t index, uint32_t instanceCount) {
		commandList->DrawIndexedInstanced(modelData_.meshs[index].indexResource->GetIndexSize(), instanceCount, 0, 0, 0);
	}

	const MeshData& GetMeshData(uint32_t index) const {
		return modelData_.meshs[index];
	}

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	ModelData modelData_;
	uint32_t  size_;
};

////////////////////////////////////////////////////////////////////////////////////////////
// ModelMethod namespace
////////////////////////////////////////////////////////////////////////////////////////////
namespace ModelMethods {

	ModelData LoadObjFile(const std::string& directoryPath, const std::string& filename);

	MaterialData LoadMaterailFile(const std::string& directoryPath, const std::string& filename, const std::string& usemtl);
}