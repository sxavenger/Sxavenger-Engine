#include "Model.h"

/* 右手座標から左手座標系への変換 -zを採用
> Vector3
 position = x, y, -z;
 normal   = x, y, -z;

> Vector4
 Quarternion = -x, -y, z, w;
*/

////////////////////////////////////////////////////////////////////////////////////////////
// Model class methods
////////////////////////////////////////////////////////////////////////////////////////////

void Model::Load(const std::string& directoryPath, const std::string& filename, bool smooth) {

	// modelの取得
	LoadModelFile(directoryPath, filename, smooth);

	// modelで使うtextureの呼び出し
	for (const auto& material : materials_) {
		//!< textureの読み込み
		for (int i = 0; i < kCountOfTextureType; ++i) {
			//!< textureを使わないので
			if (material.textureFilepaths[i].empty()) { continue; }

			Sxavenger::LoadTexture(material.textureFilepaths[i]);

		}
	}

}

void Model::Term() {

	// modelで使ったtextureの解放
	for (auto& material : materials_) {
		//!< texture解放
		for (int i = 0; i < kCountOfTextureType; ++i) {
			//!< textureを使わないので
			if (material.textureFilepaths[i].empty()) { continue; }

			Sxavenger::ReleaseTexture(material.textureFilepaths[i]);
		}
	}
}

void Model::SetIABuffer(uint32_t meshIndex) {

	if (meshIndex >= meshes_.size()) {
		Assert(false); //!< 配列以上のmodelDataの呼び出し
	}

	meshes_.at(meshIndex).mesh.BindIABuffer();
}

const D3D12_GPU_DESCRIPTOR_HANDLE Model::GetTextureHandle(uint32_t meshIndex, TextureType type) {
	Assert(meshes_.at(meshIndex).materialIndex.has_value()); //!< materialが使われていない

	uint32_t materialIndex = meshes_.at(meshIndex).materialIndex.value();
	Assert(materialIndex < materials_.size()); //!< materialサイズ以上のindex

	if (materials_.at(materialIndex).textureFilepaths[type].empty()) {
		return Sxavenger::GetTextureHandleGPU("resources/white1x1.png"); //!< HACK
	}

	return Sxavenger::GetTextureHandleGPU(materials_.at(materialIndex).textureFilepaths[type]);
}

void Model::DrawCall(uint32_t meshIndex, uint32_t instanceCount) {
	meshes_[meshIndex].mesh.DrawCall(instanceCount);
}

void Model::ApplyMeshShader() {
	for (auto& mesh : meshes_) {
		mesh.mesh.CreateMeshlet();
	}
}

void Model::ApplyRaytracing() {
	for (auto& mesh : meshes_) {
		mesh.mesh.CreateBLAS();
	}
}

void Model::LoadModelFile(const std::string& directoryPath, const std::string& filename, bool smooth) {

	std::string filepath = directoryPath + "/" + filename;

	uint32_t option = 0;
	option |= aiProcess_FlipWindingOrder;
	option |= aiProcess_FlipUVs;
	option |= aiProcess_Triangulate;
	option |= aiProcess_JoinIdenticalVertices;
	option |= aiProcess_LimitBoneWeights;
	if (smooth) { option |= aiProcess_GenSmoothNormals; }

	// sceneの取得
	sceneAi_ = importer_.ReadFile(filepath.c_str(), option);

	Assert(sceneAi_ != nullptr, "model file not found. filepath: " + filepath);
	Assert(sceneAi_->HasMeshes()); //!< メッシュナシは未対応

	// 各objectの取得
	LoadMesh();
	LoadMaterial(directoryPath);

	// nodeの取得
	rootNode_ = ReadNode(sceneAi_->mRootNode);
}

void Model::LoadMesh() {

	// 配列数の確保
	meshes_.resize(sceneAi_->mNumMeshes);

	// meshesの解析
	for (uint32_t meshIndex = 0; meshIndex < sceneAi_->mNumMeshes; ++meshIndex) {

		// meshの取得
		const aiMesh* meshAi = sceneAi_->mMeshes[meshIndex];

		// IAの初期化
		meshes_.at(meshIndex).mesh.Create(meshAi->mNumVertices, meshAi->mNumFaces * 3/*triangle*/);
		//* mesh
		auto vertices = meshes_.at(meshIndex).mesh.GetVertexBuffer();
		auto indices  = meshes_.at(meshIndex).mesh.GetIndexBuffer();

		//* material
		auto& materialIndex = meshes_.at(meshIndex).materialIndex;

		//* skinCluster
		auto& skinCluster = meshes_.at(meshIndex).skinCluster;

		// verticesの解析
		for (uint32_t element = 0; element < meshAi->mNumVertices; ++element) {

			const aiVector3D& position = meshAi->mVertices[element];
			(*vertices)[element].position = { position.x, position.y, -position.z }; //!< 左手座標系に変換

			if (meshAi->HasNormals()) {
				aiVector3D& normal = meshAi->mNormals[element];
				(*vertices)[element].normal = { normal.x, normal.y, -normal.z }; //!< 左手座標系に変換
			}

			if (meshAi->HasTextureCoords(0)) {
				aiVector3D& texcoord = meshAi->mTextureCoords[0][element];
				(*vertices)[element].texcoord = { texcoord.x, texcoord.y };
			}
		}

		// faceの解析
		for (uint32_t faceIndex = 0; faceIndex < meshAi->mNumFaces; ++faceIndex) {

			// faceの取得
			const aiFace& faceAi = meshAi->mFaces[faceIndex];

			Assert(faceAi.mNumIndices == 3); //!< 三角形のみの対応

			// indexの解析
			for (uint32_t element = 0; element < faceAi.mNumIndices; ++element) {
				// データの保存
				(*indices)[faceIndex * 3 + element] = faceAi.mIndices[element];
			}
		}

		// skinClusterの解析
		for (uint32_t boneIndex = 0; boneIndex < meshAi->mNumBones; ++boneIndex) {

			// jointごとの格納領域を作る
			const aiBone* boneAi = meshAi->mBones[boneIndex];

			// clusterの登録
			std::string name = boneAi->mName.C_Str();
			JointWeightData& jointWeightData = skinCluster[name];

			// inverseBindPoseMatrixの抽出
			aiMatrix4x4 bindPoseMatrixAi = boneAi->mOffsetMatrix;
			bindPoseMatrixAi.Inverse();

			aiVector3D scale, translate;
			aiQuaternion rotate;
			bindPoseMatrixAi.Decompose(scale, rotate, translate); //!< 成分を抽出

			// 左手系のBindPoseMatrixを作る
			Matrix4x4 bindPoseMatrix = Matrix::MakeAffine(
				{ scale.x, scale.y, scale.z }, { -rotate.x, -rotate.y, rotate.z, rotate.w }, { translate.x, translate.y, -translate.z }
			);

			// inverseBindOiseMatrixにする
			jointWeightData.inverseBindPoseMatrix = bindPoseMatrix.Inverse();

			// weight情報を取り出し
			for (uint32_t weightIndex = 0; weightIndex < boneAi->mNumWeights; ++weightIndex) {
				jointWeightData.vertexWeights.emplace_back(boneAi->mWeights[weightIndex].mWeight, boneAi->mWeights[weightIndex].mVertexId);
			}
		}

		// meshに対応するmaterialの保存
		if (meshAi->mMaterialIndex < sceneAi_->mNumMaterials) { //!< meshに対応しているmaterialがあるか確認
			materialIndex = meshAi->mMaterialIndex; //!< 0番からmaterialを始めるので
		}
	}
}

void Model::LoadMaterial(const std::string& directoryPath) {

	// 配列数の確保
	materials_.resize(sceneAi_->mNumMaterials);

	// materialsの解析
	for (uint32_t materialIndex = 0; materialIndex < sceneAi_->mNumMaterials; ++materialIndex) {

		// materialの取得
		const aiMaterial* material = sceneAi_->mMaterials[materialIndex];

		// materialの保存
		auto& materialData = materials_.at(materialIndex);

		// diffuse textureの取得
		if (material->GetTextureCount(aiTextureType_DIFFUSE) != 0) {
			aiString textureFilePath;
			material->GetTexture(aiTextureType_DIFFUSE, 0, &textureFilePath);

			// データの保存
			materialData.textureFilepaths[TEXTURE_DIFFUSE] = directoryPath + "/" + textureFilePath.C_Str();
		}

		// normal textureの取得
		if (material->GetTextureCount(aiTextureType_NORMALS) != 0) {
			aiString textureFilePath;
			material->GetTexture(aiTextureType_NORMALS, 0, &textureFilePath);

			// データの保存
			materialData.textureFilepaths[TEXTURE_NORMAL] = directoryPath + "/" + textureFilePath.C_Str();
		}
	}
}

Node Model::ReadNode(aiNode* node) {
	
	Node result;

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

////////////////////////////////////////////////////////////////////////////////////////////
// ModelManager class methods
////////////////////////////////////////////////////////////////////////////////////////////

void ModelManager::Init() {
}

void ModelManager::Term() {
	models_.clear();
}

Model* ModelManager::Load(const std::string& directoryPath, const std::string& filename, bool smooth) {

	std::string lowerFilepath = ToLower(directoryPath + "/" + filename);

	auto it = models_.find(lowerFilepath);
	if (it == models_.end()) { //!< モデルが読み込まれていない場合
		// モデルの読み込みして登録
		std::unique_ptr<Model> newModel = std::make_unique<Model>(directoryPath, filename, smooth);
		models_.emplace(lowerFilepath, std::move(newModel));
	}

	return models_.at(lowerFilepath).get();
}

void ModelManager::Delete(const std::string& directoryPath, const std::string& filename) {

	std::string lowerFilepath = ToLower(directoryPath + "/" + filename);
	models_.erase(lowerFilepath);
}

std::string ModelManager::ToLower(const std::string& str) {
	std::string result;
	result.resize(str.size());

	std::transform(str.begin(), str.end(), result.begin(), [](unsigned char c) {
		return static_cast<char>(std::tolower(c));
	});

	return result;
}
