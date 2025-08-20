#include "Model.h"

////////////////////////////////////////////////////////////////////////////////////////////
// Model class methods
////////////////////////////////////////////////////////////////////////////////////////////

void Model::Load(const std::filesystem::path& filepath, uint32_t assimpOption) {

	// sceneの取得
	Assimp::Importer importer; //!< scene保存するため保管
	const aiScene* aiScene = importer.ReadFile(filepath.generic_string().c_str(), assimpOption);

	Exception::Assert(aiScene != nullptr, "model load failed. filepath: " + filepath.generic_string(), importer.GetErrorString());
	Exception::Assert(aiScene->HasMeshes(), "model is not mesh.");

	LoadMeshes(aiScene);
	LoadMaterials(aiScene, filepath.parent_path());

	root_ = ReadNode(aiScene->mRootNode);
	skeleton_.Create(root_);
}

const Model::AssimpMesh& Model::GetMesh(uint32_t index) const {
	Exception::Assert(index < meshes_.size(), "mesh index out of range.");
	return GetMeshes().at(index);
}

Model::AssimpMesh& Model::GetMesh(uint32_t index) {
	Exception::Assert(index < meshes_.size(), "mesh index out of range.");
	return GetMeshes().at(index);
}

Vector3f Model::ConvertNormal(const aiVector3D& aiVector) {
	return { aiVector.x, aiVector.y, -aiVector.z }; //!< 左手座標系に変換
}

Vector3f Model::ConvertPosition3(const aiVector3D& aiVector) {
	return { aiVector.x, aiVector.y, -aiVector.z }; //!< 左手座標系に変換
}

Vector4f Model::ConvertPosition4(const aiVector3D& aiVector) {
	return { aiVector.x, aiVector.y, -aiVector.z, 1.0f }; //!< 左手座標系に変換
}

Quaternion Model::ConvertQuaternion(const aiQuaternion& aiQuaternion) {
	return { -aiQuaternion.x, -aiQuaternion.y, aiQuaternion.z, aiQuaternion.w }; //!< 左手座標系に変換
}

std::optional<std::filesystem::path> Model::GetTextureFilepath(const aiMaterial* aiMaterial, aiTextureType type, const std::filesystem::path& directory) {
	if (aiMaterial->GetTextureCount(type) == 0) {
		return std::nullopt; //!< テクスチャが存在しない場合はnulloptを返す
	}

	aiString filepath;
	aiMaterial->GetTexture(type, 0, &filepath);

	return directory / filepath.C_Str();
}

void Model::LoadMesh(const aiMesh* aiMesh, AssimpMesh& mesh) {

	// nameの設定
	mesh.name = aiMesh->mName.C_Str();

	// InputAssemblerの設定
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
			//!< fixme: 左手座標系に変換
			
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

		Exception::Assert(aiFace.mNumIndices == 3, "mesh is not triangle."); //!< 三角形のみの対応

		// indexの解析
		(*index)[faceIndex] = { aiFace.mIndices[0], aiFace.mIndices[2], aiFace.mIndices[1] }; //!< 左手座標系に変換
	}

	// skinClusterの解析
	auto& jointWeights = mesh.jointWeights;

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
		Matrix4x4 bindPoseMatrix = Matrix4x4::MakeAffine(
			{ scale.x, scale.y, scale.z }, ConvertQuaternion(rotate), ConvertPosition3(translate)
		);

		// inverseBindOiseMatrixにする
		jointWeightData.inverseBindPoseMatrix = bindPoseMatrix.Inverse();

		// weight情報を取り出し
		for (uint32_t weightIndex = 0; weightIndex < aiBone->mNumWeights; ++weightIndex) {
			jointWeightData.vertexWeights.emplace_back(aiBone->mWeights[weightIndex].mWeight, aiBone->mWeights[weightIndex].mVertexId);
		}
	}

	// materialの設定
	mesh.materialIndex = aiMesh->mMaterialIndex;

}

void Model::LoadMeshes(const aiScene* aiScene) {
	// mesh数のメモリ確保
	meshes_.resize(aiScene->mNumMeshes);

	for (uint32_t meshIndex = 0; meshIndex < aiScene->mNumMeshes; ++meshIndex) {
		// meshの取得
		const aiMesh* aiMesh = aiScene->mMeshes[meshIndex];
		AssimpMesh& mesh     = meshes_[meshIndex];

		// meshの読み込み
		LoadMesh(aiMesh, mesh);
	}
}

void Model::LoadMaterial(const aiMaterial* aiMaterial, const std::filesystem::path& directory, AssimpMaterial& material) {
	//!< Texture pathの取得

	// diffuseの取得
	material.textures[static_cast<uint8_t>(AssimpMaterial::TextureType::Diffuse)] = GetTextureFilepath(aiMaterial, aiTextureType_DIFFUSE, directory);

	// normalの取得
	if (aiMaterial->GetTextureCount(aiTextureType_HEIGHT) != 0) { //!< .objの場合
		material.textures[static_cast<uint8_t>(AssimpMaterial::TextureType::Bump)] = GetTextureFilepath(aiMaterial, aiTextureType_HEIGHT, directory);

	} else if (aiMaterial->GetTextureCount(aiTextureType_NORMALS) != 0) { //!< .gltfの場合
		material.textures[static_cast<uint8_t>(AssimpMaterial::TextureType::Bump)] = GetTextureFilepath(aiMaterial, aiTextureType_NORMALS, directory);
	}

	// roughnessの取得
	material.textures[static_cast<uint8_t>(AssimpMaterial::TextureType::Roughness)] = GetTextureFilepath(aiMaterial, aiTextureType_DIFFUSE_ROUGHNESS, directory);

	// metallicの取得
	material.textures[static_cast<uint8_t>(AssimpMaterial::TextureType::Metallic)] = GetTextureFilepath(aiMaterial, aiTextureType_METALNESS, directory);

	// ambient occlusionの取得
	material.textures[static_cast<uint8_t>(AssimpMaterial::TextureType::AmbientOcclusion)] = GetTextureFilepath(aiMaterial, aiTextureType_AMBIENT_OCCLUSION, directory);

	//!< Parameterの取得

	// colorの取得
	aiColor3D color;
	if (aiMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, color) == AI_SUCCESS) {
		material.color = { color.r, color.g, color.b };
	}

	// roughnessの取得
	float roughness;
	if (aiMaterial->Get(AI_MATKEY_ROUGHNESS_FACTOR, roughness) == AI_SUCCESS) {
		material.roughness = roughness;
	}

	// metallicの取得
	float metallic;
	if (aiMaterial->Get(AI_MATKEY_METALLIC_FACTOR, metallic) == AI_SUCCESS) {
		material.metallic = metallic;
	}

	// todo: specularFactorの設定

}

void Model::LoadMaterials(const aiScene* aiScene, const std::filesystem::path& directory) {
	// materail数の要素数確保
	materials_.resize(aiScene->mNumMaterials);

	for (uint32_t materialIndex = 0; materialIndex < aiScene->mNumMaterials; ++materialIndex) {
		// materialの取得
		const aiMaterial* aiMaterial = aiScene->mMaterials[materialIndex];
		AssimpMaterial& material     = materials_[materialIndex];
		LoadMaterial(aiMaterial, directory, material);
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
	result.transform.rotate    = ConvertQuaternion(rotate);
	result.transform.translate = ConvertPosition3(translate);

	// nodeのlocalMatの取得
	aiMatrix4x4 aiLocalMatrix = node->mTransformation;
	aiLocalMatrix.Transpose();

	// resultにcopy
	std::memcpy(&result.localMatrix, &aiLocalMatrix, sizeof(Matrix4x4));

	// ノード名を格納
	result.name = node->mName.C_Str();

	// meshのindexの取得
	for (uint32_t meshIndex = 0; meshIndex < node->mNumMeshes; ++meshIndex) {
		result.meshIndices.emplace_back(node->mMeshes[meshIndex]);
	}

	// 子の数だけの要素の格納
	result.children.resize(node->mNumChildren);

	// 子のノード情報の取得
	for (uint32_t childIndex = 0; childIndex < node->mNumChildren; ++childIndex) {
		result.children[childIndex] = ReadNode(node->mChildren[childIndex]);
	}

	return result;
}
