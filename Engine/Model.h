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

#include "MyEngine.h" //!< devices, TextureManagerの取り出し
#include <TextureManager.h>

// DxObject
#include <DxBufferResource.h>
#include <ObjectPtr.h>

// Geometry
#include <Vector4.h>
#include <Vector3.h>
#include <Vector2.h>

#include <ObjectStructure.h>

////////////////////////////////////////////////////////////////////////////////////////////
// MaterialData structure
////////////////////////////////////////////////////////////////////////////////////////////
struct MaterialData {
	std::string textureFilePath;
	bool isUseTexture;
};

////////////////////////////////////////////////////////////////////////////////////////////
// ModelData structure
////////////////////////////////////////////////////////////////////////////////////////////
struct ModelData {
	DxObject::BufferResource<VertexData>* vertexResource;
	DxObject::IndexBufferResource*        indexResource;
	MaterialData                          material;
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

	const uint32_t GetModelSize() const { return modelDataSize_; }
	
	// Draw
	void SetBuffers(ID3D12GraphicsCommandList* commandList, uint32_t index) {
		if (index >= modelDataSize_) {
			assert(false); //!< 配列以上のmodelDataの呼び出し
		}

		if (modelDatas_.at(index).material.isUseTexture) {
			MyEngine::SetPipeLineState(TEXTURE);

		} else {
			MyEngine::SetPipeLineState(POLYGON);
		}

		D3D12_VERTEX_BUFFER_VIEW vertexBufferView = modelDatas_.at(index).vertexResource->GetVertexBufferView();
		D3D12_INDEX_BUFFER_VIEW indexBufferView = modelDatas_.at(index).indexResource->GetIndexBufferView();

		commandList->IASetVertexBuffers(0, 1, &vertexBufferView);
		commandList->IASetIndexBuffer(&indexBufferView);
	}

	void SetTexture(UINT parameterNum, ID3D12GraphicsCommandList* commandList, uint32_t index) {
		if (modelDatas_.at(index).material.isUseTexture) {
			commandList->SetGraphicsRootDescriptorTable(parameterNum, MyEngine::GetTextureHandleGPU(modelDatas_.at(index).material.textureFilePath));
		}
	}

	void DrawCall(ID3D12GraphicsCommandList* commandList, uint32_t index) {
		commandList->DrawIndexedInstanced(modelDatas_.at(index).indexResource->GetSize(), 1, 0, 0, 0);
	}

	const ModelData& GetModelData(uint32_t index) const { // debug
		return modelDatas_.at(index);
	}

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	std::vector<ModelData> modelDatas_;
	uint32_t modelDataSize_;
};

////////////////////////////////////////////////////////////////////////////////////////////
// ModelMethod namespace
////////////////////////////////////////////////////////////////////////////////////////////
namespace ModelMethods {

	std::vector<ModelData> LoadObjFile(const std::string& directoryPath, const std::string& filename);

	MaterialData LoadMaterailFile(const std::string& directoryPath, const std::string& filename, const std::string& usemtl);
}