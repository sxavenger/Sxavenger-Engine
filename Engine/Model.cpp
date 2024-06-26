#include "Model.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
// MyEngine
#include <MyEngine.h>

// logger
#include <Logger.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Model class methods
////////////////////////////////////////////////////////////////////////////////////////////

void Model::Init(const std::string& directoryPath, const std::string& filename) {

	// modelDataの取得
	modelData_ = ModelMethods::LoadModelFile(directoryPath, filename);

	// modelIndexSizeの保存
	modelIndexSize_ = static_cast<uint32_t>(modelData_.meshes.size());

	// modelで使うtextureの呼び出し
	for (auto& material : modelData_.materials) {
		//!< textureの読み込み
		for (int i = 0; i < kCountOfTextureType; ++i) {
			//!< textureを使わないので
			if (material.textureFilePaths[i].empty()) { continue; }

			MyEngine::LoadTexture(material.textureFilePaths[i]);
		}
	}

}

void Model::Term() {

	// modelで使ったtextureの解放
	for (auto& material : modelData_.materials) {
		//!< texture解放
		for (int i = 0; i < kCountOfTextureType; ++i) {
			//!< textureを使わないので
			if (material.textureFilePaths[i].empty()) { continue; }

			MyEngine::ReleaseTexture(material.textureFilePaths[i]);
		}
	}

	// modelの解放
	modelData_.Clear();
	modelIndexSize_ = 0;

}

void Model::SetBuffers(ID3D12GraphicsCommandList* commandList, uint32_t modelIndex) {

	if (modelIndex >= modelIndexSize_) {
		assert(false); //!< 配列以上のmodelDataの呼び出し
	}

	D3D12_VERTEX_BUFFER_VIEW vertexBufferView = modelData_.meshes[modelIndex].vertexResource->GetVertexBufferView();
	D3D12_INDEX_BUFFER_VIEW indexBufferView = modelData_.meshes[modelIndex].indexResource->GetIndexBufferView();

	commandList->IASetVertexBuffers(0, 1, &vertexBufferView);
	commandList->IASetIndexBuffer(&indexBufferView);
}

void Model::SetGraphicsTextureHandle(ID3D12GraphicsCommandList* commandList, uint32_t modelIndex, UINT parameterNum, TextureType type) {

	if (modelData_.materials[modelIndex].textureFilePaths[type].empty()) { return; } //!< textureを使ってないので

	commandList->SetGraphicsRootDescriptorTable(parameterNum, MyEngine::GetTextureHandleGPU(modelData_.materials[modelIndex].textureFilePaths[type]));
}

void Model::DrawCall(ID3D12GraphicsCommandList* commandList, uint32_t modelIndex, uint32_t instanceCount) {
	commandList->DrawIndexedInstanced(modelData_.meshes[modelIndex].indexResource->GetIndexSize(), instanceCount, 0, 0, 0);
}

////////////////////////////////////////////////////////////////////////////////////////////
// ModelMethods namespace methods
////////////////////////////////////////////////////////////////////////////////////////////

ModelData ModelMethods::LoadModelFile(const std::string& directoryPath, const std::string& filename) {

	ModelData result;

	Assimp::Importer importer;
	std::string filePath = directoryPath + "/" + filename;

	uint32_t option = 0;
	option |= aiProcess_FlipWindingOrder;
	option |= aiProcess_FlipUVs;
	option |= aiProcess_JoinIdenticalVertices;
	option |= aiProcess_Triangulate;

	const aiScene* scene
		= importer.ReadFile(filePath.c_str(), option);

	assert(scene->HasMeshes()); //!< メッシュナシは未対応

	// ModelDataの要素数の指定
	result.meshes.resize(scene->mNumMeshes);
	result.materials.resize(scene->mNumMaterials);

	// meshesの解析
	for (uint32_t meshIndex = 0; meshIndex < scene->mNumMeshes; ++meshIndex) {

		// meshの取得
		aiMesh* mesh = scene->mMeshes[meshIndex];

		//assert(mesh->HasNormals());        //!< 法線がない場合は未対応
		//assert(mesh->HasTextureCoords(0)); //!< Texcoordがない場合は未対応 todo...

		// 保存の確保
		result.meshes[meshIndex].Create(MyEngine::GetDevicesObj(), mesh->mNumVertices, mesh->mNumFaces * 3);
		auto& meshData = result.meshes[meshIndex];

		// verticesの解析
		for (uint32_t element = 0; element < mesh->mNumVertices; ++element) {

			// 自前の構造体に保存
			VertexData vtx;
			vtx.Init();

			aiVector3D& position = mesh->mVertices[element];
			vtx.position = { position.x, position.y, position.z };

			if (mesh->HasNormals()) {
				aiVector3D& normal = mesh->mNormals[element];
				vtx.normal = { normal.x, normal.y, normal.z };
			}

			if (mesh->HasTextureCoords(0)) {
				aiVector3D& texcoord = mesh->mTextureCoords[0][element];
				vtx.texcoord = { texcoord.x, texcoord.y };
			}

			// 左手座標系に変換
			vtx.position.z *= -1.0f;
			vtx.normal.z   *= -1.0f;

			// データの保存
			meshData.vertexResource->operator[](element) = vtx;

		}

		// faceの解析
		for (uint32_t faceIndex = 0; faceIndex < mesh->mNumFaces; ++faceIndex) {

			// faceの取得
			aiFace& face = mesh->mFaces[faceIndex];

			assert(face.mNumIndices == 3); //!< 三角形のみの対応

			for (uint32_t element = 0; element < face.mNumIndices; ++element) {
				// データの保存
				meshData.indexResource->operator[](faceIndex * 3 + element) = face.mIndices[element];
			}
		}
	}

	// materialsの解析
	for (uint32_t materialIndex = 0; materialIndex < scene->mNumMaterials; ++materialIndex) {

		// materialの取得
		aiMaterial* material = scene->mMaterials[materialIndex];

		// materialデータの保存場所の指定
		auto& materialData = result.materials[materialIndex];

		// diffuse textureの取得
		if (material->GetTextureCount(aiTextureType_DIFFUSE) != 0) {
			aiString textureFilePath;
			material->GetTexture(aiTextureType_DIFFUSE, 0, &textureFilePath);
			
			// データの保存
			materialData.textureFilePaths[TEXTURE_DIFFUSE] = directoryPath + "/" + textureFilePath.C_Str();
		}

		// normal textureの取得
		if (material->GetTextureCount(aiTextureType_NORMALS) != 0) {
			aiString textureFilePath;
			material->GetTexture(aiTextureType_NORMALS, 0, &textureFilePath);

			// データの保存
			materialData.textureFilePaths[TEXTURE_NORMAL] = directoryPath + "/" + textureFilePath.C_Str();
		}
	}

	// nodeの取得
	result.rootNode = ReadNode(scene->mRootNode);
	
	return result;
}

Node ModelMethods::ReadNode(aiNode* node) {
	Node result;

	// nodeのlocalmatの取得
	aiMatrix4x4 aiLocalMatrix = node->mTransformation;
	aiLocalMatrix.Transpose();

	// resultにcopy
	std::memcpy(&result.localMatrix, &aiLocalMatrix, sizeof(Matrix4x4));

	// ノード名を格納
	result.name = node->mName.C_Str();

	// 子の数だけの要素の格納
	result.children.resize(node->mNumChildren);

	// 子のノード情報の取得
	for (uint32_t childIndex = 0; childIndex < node->mNumChildren; ++childIndex) {
		result.children[childIndex] = ReadNode(node->mChildren[childIndex]);
	}

	return result;

}
