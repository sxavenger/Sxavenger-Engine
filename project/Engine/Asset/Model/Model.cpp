#include "Model.h"
//* 右手座標から左手座標系への変換 -zを採用
//* Vector3
//*  position = x, y, -z;
//*  normal   = x, y, -z;
//*
//* Vector4
//*  Quarternion = -x, -y, z, w;

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/Content/SxavengerContent.h>

//=========================================================================================
// static const variables
//=========================================================================================

const uint32_t Model::kDefaultAssimpOption_
	= aiProcess_FlipWindingOrder
	| aiProcess_FlipUVs
	| aiProcess_Triangulate
	| aiProcess_CalcTangentSpace;

////////////////////////////////////////////////////////////////////////////////////////////
// Model class methods
////////////////////////////////////////////////////////////////////////////////////////////

void Model::Load(const DirectXThreadContext* context, const std::filesystem::path& filepath, uint32_t assimpOption = kDefaultAssimpOption_) {

	// sceneの取得
	Assimp::Importer importer; //!< scene保存するため保管
	const aiScene* aiScene = importer.ReadFile(filepath.generic_string().c_str(), assimpOption);

	Assert(aiScene != nullptr, "model load failed. filepath: " + filepath.generic_string(), importer.GetErrorString());
	Assert(aiScene->HasMeshes(), "model is not mesh.");

	LoadMesh(aiScene);
	LoadMaterial(aiScene, context, filepath.parent_path());
	root_ = ReadNode(aiScene->mRootNode);
}

void Model::Term() {
}

void Model::SetIABuffer(uint32_t meshIndex) const {
	CheckMeshIndex(meshIndex);
	meshes_.at(meshIndex).mesh.BindIABuffer();
}

void Model::DrawCall(uint32_t meshIndex, uint32_t instanceCount) const {
	meshes_[meshIndex].mesh.DrawCall(instanceCount);
}

const Model::MeshData& Model::GetMeshData(uint32_t meshIndex) const {
	CheckMeshIndex(meshIndex);
	return meshes_.at(meshIndex);
}

const InputMesh& Model::GetInputMesh(uint32_t meshIndex) const {
	return GetMeshData(meshIndex).mesh;
}

uint32_t Model::GetMaterialIndex(uint32_t meshIndex) const {
	uint32_t materialIndex = meshes_.at(meshIndex).materialIndex.value();
	Assert(materialIndex < materials_.size(), "material index out of range."); //!< materialサイズ以上のindex
	return materialIndex;
}

bool Model::CheckTextureWithMaterialIndex(uint32_t materialIndex, TextureType type) const {
	return materials_.at(materialIndex).textures_[static_cast<uint8_t>(type)] != nullptr;
}

bool Model::CheckTextureWithMeshIndex(uint32_t meshIndex, TextureType type) const {
	uint32_t materialIndex = GetMaterialIndex(meshIndex);
	return CheckTextureWithMaterialIndex(materialIndex, type);
}

const D3D12_GPU_DESCRIPTOR_HANDLE& Model::GetTextureHandle(uint32_t meshIndex, TextureType type) const {
	CheckMeshIndex(meshIndex);
	Assert(meshes_.at(meshIndex).materialIndex.has_value()); //!< materialが使われていない

	uint32_t materialIndex = GetMaterialIndex(meshIndex);

	if (!CheckTextureWithMaterialIndex(materialIndex, type)) {
		return SxavengerContent::GetGPUHandleSRV("white1x1");
	}

	return materials_.at(materialIndex).textures_[static_cast<uint8_t>(type)]->GetGPUHandleSRV();
}

void Model::LoadMesh(const aiScene* aiScene) {
	// mesh数のメモリ確保
	meshes_.resize(aiScene->mNumMeshes);

	// meshesの解析
	for (uint32_t meshIndex = 0; meshIndex < aiScene->mNumMeshes; ++meshIndex) {

		// meshの取得
		const aiMesh* aiMesh = aiScene->mMeshes[meshIndex];

		//* 
		//* InputAssembler
		//* 

		// InputAssemblerの初期化
		auto& ia = meshes_.at(meshIndex).mesh;
		ia.Create(aiMesh->mNumVertices, aiMesh->mNumFaces * 3 /*_triangle*/);

		auto vertex = ia.GetVertex();
		auto index  = ia.GetIndex();

		// verticesの解析
		for (uint32_t element = 0; element < aiMesh->mNumVertices; ++element) {
			//!< position
			const aiVector3D& position = aiMesh->mVertices[element];
			(*vertex)[element].position = { position.x, position.y, -position.z }; //!< 左手座標系に変換

			//!< normal
			if (aiMesh->HasNormals()) {
				const aiVector3D& normal = aiMesh->mNormals[element];
				(*vertex)[element].normal = { normal.x, normal.y, -normal.z }; //!< 左手座標系に変換
			}

			//!< texcoord
			if (aiMesh->HasTextureCoords(0)) {
				const aiVector3D& texcoord = aiMesh->mTextureCoords[0][element];
				(*vertex)[element].texcoord = { texcoord.x, texcoord.y };
			}

			if (aiMesh->HasTangentsAndBitangents()) {
				const aiVector3D& tangent = aiMesh->mTangents[element];
				(*vertex)[element].tangent = { tangent.x, tangent.y, tangent.z }; //!< 左手座標系に変換

				const aiVector3D& bitangent = aiMesh->mBitangents[element];
				(*vertex)[element].bitangent = { bitangent.x, bitangent.y, bitangent.z }; //!< 左手座標系に変換
			}
		}

		// faceの解析
		for (uint32_t faceIndex = 0; faceIndex < aiMesh->mNumFaces; ++faceIndex) {
			// faceの取得
			const aiFace& aiFace = aiMesh->mFaces[faceIndex];

			Assert(aiFace.mNumIndices == 3); //!< 三角形のみの対応

			// indexの解析
			for (uint32_t element = 0; element < aiFace.mNumIndices; ++element) {
				//!< index
				(*index)[faceIndex * 3 + element] = aiFace.mIndices[element];
			}
		}

		//* 
		//* SkinCluster
		//*

		auto& jointWeights = meshes_.at(meshIndex).jointWeights;

		// skinClusterの解析
		for (uint32_t boneIndex = 0; boneIndex < aiMesh->mNumBones; ++boneIndex) {

			// jointごとの格納領域を作る
			const aiBone* aiBone = aiMesh->mBones[boneIndex];

			// clusterの登録
			JointWeightData& jointWeightData = jointWeights[aiBone->mName.C_Str()];

			// inverseBindPoseMatrixの抽出
			aiMatrix4x4 aiBindPoseMatrix = aiBone->mOffsetMatrix;
			aiBindPoseMatrix.Inverse();

			aiVector3D scale, translate;
			aiQuaternion rotate;
			aiBindPoseMatrix.Decompose(scale, rotate, translate); //!< 成分を抽出

			// 左手系のBindPoseMatrixを作る
			Matrix4x4 bindPoseMatrix = Matrix::MakeAffine(
				{ scale.x, scale.y, scale.z }, { -rotate.x, -rotate.y, rotate.z, rotate.w }, { translate.x, translate.y, -translate.z }
			);

			// inverseBindOiseMatrixにする
			jointWeightData.inverseBindPoseMatrix = bindPoseMatrix.Inverse();

			// weight情報を取り出し
			for (uint32_t weightIndex = 0; weightIndex < aiBone->mNumWeights; ++weightIndex) {
				jointWeightData.vertexWeights.emplace_back(aiBone->mWeights[weightIndex].mWeight, aiBone->mWeights[weightIndex].mVertexId);
			}
		}

		//* 
		//* Material
		//*

		auto& materialIndex = meshes_.at(meshIndex).materialIndex;
		materialIndex = aiMesh->mMaterialIndex;
	}
}

void Model::LoadMaterial(const aiScene* aiScene, const DirectXThreadContext* context, const std::filesystem::path& directory) {

	// materail数のメモリ確保
	materials_.resize(aiScene->mNumMaterials);

	// materialsの解析
	for (uint32_t materialIndex = 0; materialIndex < aiScene->mNumMaterials; ++materialIndex) {

		// materialの取得
		const aiMaterial* aiMaterial = aiScene->mMaterials[materialIndex];

		//*
		//* Texture
		//*

		auto& material = materials_.at(materialIndex);

		// diffuseの取得
		if (aiMaterial->GetTextureCount(aiTextureType_DIFFUSE) != 0) {
			aiString aiTextureFilepath;
			aiMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &aiTextureFilepath);

			// データの保存
			material.textures_[static_cast<uint8_t>(TextureType::Diffuse)] = std::make_unique<Texture>();
			material.textures_[static_cast<uint8_t>(TextureType::Diffuse)]->Load(context, directory / aiTextureFilepath.C_Str());
		}

		// normalの取得
		if (aiMaterial->GetTextureCount(aiTextureType_HEIGHT) != 0) { //!< objの場合.
			aiString aiTextureFilepath;
			aiMaterial->GetTexture(aiTextureType_HEIGHT, 0, &aiTextureFilepath);

			// データの保存
			material.textures_[static_cast<uint8_t>(TextureType::Bump)] = std::make_unique<Texture>();
			material.textures_[static_cast<uint8_t>(TextureType::Bump)]->Load(context, directory / aiTextureFilepath.C_Str());

		} else if (aiMaterial->GetTextureCount(aiTextureType_NORMALS) != 0) { //!< gltfの場合.
			aiString aiTextureFilepath;
			aiMaterial->GetTexture(aiTextureType_NORMALS, 0, &aiTextureFilepath);

			// データの保存
			material.textures_[static_cast<uint8_t>(TextureType::Bump)] = std::make_unique<Texture>();
			material.textures_[static_cast<uint8_t>(TextureType::Bump)]->Load(context, directory / aiTextureFilepath.C_Str());
		}
	}
}

BornNode Model::ReadNode(aiNode* node) {
	
	BornNode result = {};

	// transformの取得
	aiVector3D scale, translate;
	aiQuaternion rotate;

	node->mTransformation.Decompose(scale, rotate, translate);

	// resultに代入
	result.transform.scale     = { scale.x, scale.y, scale.z };
	result.transform.rotate    = { -rotate.x, -rotate.y, rotate.z, rotate.w }; //!< 右手 -> 左手座標系に変換
	result.transform.translate = { translate.x, translate.y, -translate.z };   //!< 右手 -> 左手座標系に変換

	// nodeのlocalMatの取得
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

//void Model::CreateMeshlet() {
//
//	//!< thread pools
//	std::array<std::thread, kChildThreadCount> threads;
//	std::queue<std::function<void()>>          tasks;
//	std::mutex                                 mutex;
//
//	for (uint32_t i = 0; i < GetMeshSize(); ++i) {
//		tasks.emplace([this, i]() { meshes_[i].mesh.CreateMeshlet(); });
//	}
//
//	for (uint32_t i = 0; i < threads.size(); ++i) {
//		threads[i] = std::thread([&]() {
//			while (true) {
//				std::function<void()> task;
//				{
//					std::lock_guard<std::mutex> lock(mutex);
//					if (tasks.empty()) {
//						break;
//					}
//					task = tasks.front();
//					tasks.pop();
//				}
//				task();
//			}
//		});
//	}
//
//	std::for_each(threads.begin(), threads.end(), [](std::thread& thread) { thread.join(); });
//}

void Model::CheckMeshIndex(uint32_t meshIndex) const {
	Assert(meshIndex < meshes_.size(), "mesh index out of range.");
}

