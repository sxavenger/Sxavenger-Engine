#include "Model.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/Component/Components/Transform/TransformComponent.h>
#include <Engine/Component/Components/MeshRenderer/MeshRendererComponent.h>

//* c++
#include <execution>

//* 右手座標から左手座標系への変換 -zを採用
//* Vector3
//*  position = x, y, -z;
//*  normal   = x, y, -z;
//*
//* Vector4
//*  Quarternion = -x, -y, z, w;

////////////////////////////////////////////////////////////////////////////////////////////
// AssimpMaterial structure methods
////////////////////////////////////////////////////////////////////////////////////////////

void Model::AssimpMaterial::CreateComponent() {
	Material::CreateBuffer();

	if (textures_[static_cast<uint8_t>(TextureType::Diffuse)] != nullptr) {
		Material::GetMaterial().albedo.SetTexture(textures_[static_cast<uint8_t>(TextureType::Diffuse)]->GetDescriptorSRV().GetIndex());
	}

	if (textures_[static_cast<uint8_t>(TextureType::Bump)] != nullptr) {
		Material::GetMaterial().normal.SetTexture(textures_[static_cast<uint8_t>(TextureType::Bump)]->GetDescriptorSRV().GetIndex());
	}
}

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

void Model::Load(const DirectXThreadContext* context, const std::filesystem::path& filepath, uint32_t assimpOption) {

	// sceneの取得
	Assimp::Importer importer; //!< scene保存するため保管
	const aiScene* aiScene = importer.ReadFile(filepath.generic_string().c_str(), assimpOption);

	Assert(aiScene != nullptr, "model load failed. filepath: " + filepath.generic_string(), importer.GetErrorString());
	Assert(aiScene->HasMeshes(), "model is not mesh.");

	LoadMesh(aiScene);
	LoadMaterial(aiScene, context, filepath.parent_path());
	root_ = ReadNode(aiScene->mRootNode);

	CreateSkeleton();
}

void Model::Term() {
}

const Model::AssimpMaterial& Model::GetMaterial(uint32_t index) const {
	CheckMaterialIndex(index);
	return materials_.at(index);
}

Model::AssimpMaterial& Model::GetMaterial(uint32_t index) {
	CheckMaterialIndex(index);
	return materials_.at(index);
}

const Model::AssimpMesh& Model::GetMesh(uint32_t index) const {
	return GetMeshes().at(index);
}

//void Model::CreateBottomLevelAS(const DirectXThreadContext* context) {
//	for (uint32_t i = 0; i < GetMeshSize(); ++i) {
//		meshes_.at(i).input.CreateBottomLevelAS(context);
//	}
//}
//
//const Model::AssimpMesh& Model::GetMesh(uint32_t index) const {
//	CheckMeshIndex(index);
//	return meshes_.at(index);
//}
//
//Model::AssimpMesh& Model::GetMesh(uint32_t index) {
//	CheckMeshIndex(index);
//	return meshes_.at(index);
//}
//
//void Model::SetIABuffer(const DirectXThreadContext* context, uint32_t meshIndex) const {
//	GetMesh(meshIndex).input.BindIABuffer(context);
//}
//
//void Model::DrawCall(const DirectXThreadContext* context, uint32_t meshIndex, uint32_t instanceCount) const {
//	GetMesh(meshIndex).input.DrawCall(context, instanceCount);
//}
//
//uint32_t Model::GetMaterialIndex(uint32_t meshIndex) const {
//	uint32_t materialIndex = meshes_.at(meshIndex).materialIndex.value();
//	Assert(materialIndex < materials_.size(), "material index out of range."); //!< materialサイズ以上のindex
//	return materialIndex;
//}

std::unique_ptr<MonoBehaviour> Model::CreateMonoBehavior(const std::string& name) {
	auto root = std::make_unique<MonoBehaviour>();
	root->SetName(name);

	// MonoBehaviorの登録
	std::for_each(/*std::execution::seq, */meshes_.begin(), meshes_.end(), [this, &root](AssimpMesh& mesh) {
		auto child = std::make_unique<MonoBehaviour>();
		child->SetName(mesh.name);

		// transform component
		auto transform = child->AddComponent<TransformComponent>();
		transform->CreateBuffer();

		// renderer component
		auto renderer = child->AddComponent<MeshRendererComponent>();
		renderer->SetMesh(&mesh.input);
		renderer->SetMaterial(&GetMaterial(mesh.materialIndex.value()));

		root->AddChild(std::move(child));
	});

	return std::move(root); 
}

void Model::CreateMonoBehavior(MonoBehaviour* root) {
	// MonoBehaviorの登録
	std::for_each(/*std::execution::seq, */meshes_.begin(), meshes_.end(), [this, root](AssimpMesh& mesh) {
		auto child = std::make_unique<MonoBehaviour>();
		child->SetName(mesh.name);

		// transform component
		auto transform = child->AddComponent<TransformComponent>();
		transform->CreateBuffer();

		// renderer component
		auto renderer = child->AddComponent<MeshRendererComponent>();
		renderer->SetMesh(&mesh.input);
		renderer->SetMaterial(&GetMaterial(mesh.materialIndex.value()));

		root->AddChild(std::move(child));
	});
}

Vector3f Model::ConvertNormal(const aiVector3D& aiVector) {
	return { aiVector.x, aiVector.y, -aiVector.z }; //!< 左手座標系に変換
}

Vector3f Model::ConvertPosition3(const aiVector3D& aiVector) {
	return { aiVector.x, aiVector.y, -aiVector.z }; //!< 左手座標系に変換
}

Vector4f Model::ConvertPosition4(const aiVector3D& aiVector) {
	return { aiVector.x, aiVector.y, -aiVector.z }; //!< 左手座標系に変換
}

Quaternion Model::ConvertQuaternion(const aiQuaternion& aiQuaternion) {
	return { -aiQuaternion.x, -aiQuaternion.y, aiQuaternion.z, aiQuaternion.w }; //!< 左手座標系に変換
} 

void Model::LoadMesh(const aiScene* aiScene) {
	// mesh数のメモリ確保
	meshes_.resize(aiScene->mNumMeshes);

	// meshesの解析
	std::for_each(std::execution::seq, meshes_.begin(), meshes_.end(), [this, aiScene](AssimpMesh& mesh) {
		size_t meshIndex = &mesh - &meshes_.front(); //!< indexの取得

		// meshの取得
		const aiMesh* aiMesh = aiScene->mMeshes[meshIndex];

		{ //!< name
			mesh.name = aiMesh->mName.C_Str();
		}

		{ //!< InputAssembler

			// inputの初期化
			auto& ia = mesh.input;
			ia.Create(aiMesh->mNumVertices, aiMesh->mNumFaces);

			auto vertex = ia.GetVertex();
			auto index  = ia.GetIndex();

			// verticesの解析
			for (uint32_t element = 0; element < aiMesh->mNumVertices; ++element) {

				//!< position
				const aiVector3D& position = aiMesh->mVertices[element];
				(*vertex)[element].position = ConvertPosition4(position);

				//!< normal
				if (aiMesh->HasNormals()) {
					const aiVector3D& normal = aiMesh->mNormals[element];
					(*vertex)[element].normal = ConvertNormal(normal);
				}

				//!< texcoord
				if (aiMesh->HasTextureCoords(0)) {
					const aiVector3D& texcoord = aiMesh->mTextureCoords[0][element];
					(*vertex)[element].texcoord = { texcoord.x, texcoord.y };
				}

				if (aiMesh->HasTangentsAndBitangents()) {
					//!< todo: 左手座標系に変換
					
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

				Assert(aiFace.mNumIndices == 3, "mesh is not triangle."); //!< 三角形のみの対応

				// indexの解析
				(*index)[faceIndex] = { aiFace.mIndices[0], aiFace.mIndices[2], aiFace.mIndices[1] }; //!< 左手座標系に変換
			}
		}

		{ //!< SkinCluster

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
					{ scale.x, scale.y, scale.z }, ConvertQuaternion(rotate), ConvertPosition3(translate)
				);

				// inverseBindOiseMatrixにする
				jointWeightData.inverseBindPoseMatrix = bindPoseMatrix.Inverse();

				// weight情報を取り出し
				for (uint32_t weightIndex = 0; weightIndex < aiBone->mNumWeights; ++weightIndex) {
					jointWeightData.vertexWeights.emplace_back(aiBone->mWeights[weightIndex].mWeight, aiBone->mWeights[weightIndex].mVertexId);
				}
			}
		}

		{ //!< material
			mesh.materialIndex = aiMesh->mMaterialIndex;
		}
	});
}

void Model::LoadMaterial(const aiScene* aiScene, const DirectXThreadContext* context, const std::filesystem::path& directory) {
	// materail数の要素数確保
	materials_.resize(aiScene->mNumMaterials);

	// materialsの解析
	std::for_each(std::execution::seq, materials_.begin(), materials_.end(), [this, aiScene, context, &directory](AssimpMaterial& material) {
		size_t materialIndex = &material - &materials_.front(); //!< indexの取得

		// materialの取得
		const aiMaterial* aiMaterial = aiScene->mMaterials[materialIndex];

		{ //!< Texture
			// diffuseの取得
			if (aiMaterial->GetTextureCount(aiTextureType_DIFFUSE) != 0) {
				aiString aiTextureFilepath;
				aiMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &aiTextureFilepath);

				// データの保存
				material.textures_[static_cast<uint8_t>(AssimpMaterial::TextureType::Diffuse)] = std::make_shared<Texture>();
				material.textures_[static_cast<uint8_t>(AssimpMaterial::TextureType::Diffuse)]->Load(context, directory / aiTextureFilepath.C_Str());
			}

			// normalの取得
			if (aiMaterial->GetTextureCount(aiTextureType_HEIGHT) != 0) { //!< .objの場合
				aiString aiTextureFilepath;
				aiMaterial->GetTexture(aiTextureType_HEIGHT, 0, &aiTextureFilepath);

				// データの保存
				material.textures_[static_cast<uint8_t>(AssimpMaterial::TextureType::Bump)] = std::make_shared<Texture>();
				material.textures_[static_cast<uint8_t>(AssimpMaterial::TextureType::Bump)]->Load(context, directory / aiTextureFilepath.C_Str());

			} else if (aiMaterial->GetTextureCount(aiTextureType_NORMALS) != 0) { //!< .gltfの場合
				aiString aiTextureFilepath;
				aiMaterial->GetTexture(aiTextureType_NORMALS, 0, &aiTextureFilepath);

				// データの保存
				material.textures_[static_cast<uint8_t>(AssimpMaterial::TextureType::Bump)] = std::make_shared<Texture>();
				material.textures_[static_cast<uint8_t>(AssimpMaterial::TextureType::Bump)]->Load(context, directory / aiTextureFilepath.C_Str());
			}
		}

		{ //!< Component
			material.CreateComponent();
		}
	});
}

BornNode Model::ReadNode(aiNode* node) {
	
	BornNode result = {};

	// transformの取得
	aiVector3D scale, translate;
	aiQuaternion rotate;

	node->mTransformation.Decompose(scale, rotate, translate);

	// resultに代入
	result.transform.scale     = { scale.x, scale.y, scale.z };
	result.transform.rotate    = ConvertQuaternion(rotate); //!< 右手 -> 左手座標系に変換
	result.transform.translate = ConvertPosition3(translate);

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

void Model::CreateSkeleton() {
	skeleton_.Create(root_);
}

bool Model::CheckMeshIndex(uint32_t meshIndex) const {
	bool expression = meshIndex < meshes_.size();
	Assert(expression, "mesh index out of range.");
	return !expression;
}

bool Model::CheckMaterialIndex(uint32_t materialIndex) const {
	bool expression = materialIndex < materials_.size();
	Assert(expression, "material index out of range.");
	return !expression;
}
