#include "AssetMesh.h"
SXAVENGER_ENGINE_USING

////////////////////////////////////////////////////////////////////////////////////////////
// Mesh class methods
////////////////////////////////////////////////////////////////////////////////////////////

void Asset::Mesh::Setup(const DirectXQueueContext* context, const aiMesh* mesh) {
	context->RequestQueue(DirectXQueueContext::RenderQueue::Compute); //!< ComputeQueue以上を使用

	// meshの解析
	// nameの設定
	name_ = mesh->mName.C_Str();

	// InputAssemblerの設定
	auto& ia = input_;
	ia.Create(mesh->mNumVertices, mesh->mNumFaces);

	auto vertex = ia.GetVertex();
	auto index  = ia.GetIndex();

	// verticesの解析
	for (uint32_t element = 0; element < mesh->mNumVertices; ++element) {

		//!< position
		const aiVector3D& position  = mesh->mVertices[element];
		(*vertex)[element].position = ConvertVector4(position);

		//!< normal
		if (mesh->HasNormals()) {
			const aiVector3D& normal  = mesh->mNormals[element];
			(*vertex)[element].normal = ConvertVector3(normal);
		}

		//!< texcoord
		if (mesh->HasTextureCoords(0)) {
			const aiVector3D& texcoord  = mesh->mTextureCoords[0][element];
			(*vertex)[element].texcoord = { texcoord.x, texcoord.y };
		}

		if (mesh->HasTangentsAndBitangents()) {
			//!< fixme: 左手座標系に変換
			
			const aiVector3D& tangent  = mesh->mTangents[element];
			(*vertex)[element].tangent = ConvertVector3(tangent); //!< 左手座標系に変換

			const aiVector3D& bitangent  = mesh->mBitangents[element];
			(*vertex)[element].bitangent = ConvertVector3(bitangent); //!< 左手座標系に変換
		}
	}

	// faceの解析
	for (uint32_t faceIndex = 0; faceIndex < mesh->mNumFaces; ++faceIndex) {

		// faceの取得
		const aiFace& aiFace = mesh->mFaces[faceIndex];

		StreamLogger::AssertA(aiFace.mNumIndices == 3, "mesh is not triangle."); //!< 三角形のみの対応

		// indexの解析
		(*index)[faceIndex] = { aiFace.mIndices[1], aiFace.mIndices[2], aiFace.mIndices[0] }; //!< 左手座標系に変換
	}

	// skinClusterの解析
	for (uint32_t boneIndex = 0; boneIndex < mesh->mNumBones; ++boneIndex) {

		// jointごとの格納領域を作る
		const aiBone* aiBone = mesh->mBones[boneIndex];

		// clusterの登録
		JointWeightData& jointWeightData = jointWeights_[aiBone->mName.C_Str()];

		// inverseBindPoseMatrixの抽出
		aiMatrix4x4 aiBindPoseMatrix = aiBone->mOffsetMatrix;
		aiBindPoseMatrix.Inverse();

		aiVector3D scale, translate;
		aiQuaternion rotate;
		aiBindPoseMatrix.Decompose(scale, rotate, translate); //!< 成分を抽出

		// 左手系のBindPoseMatrixを作る
		Matrix4x4 bindPoseMatrix = Matrix4x4::MakeAffine(
			{ scale.x, scale.y, scale.z },
			ConvertQuaternion(rotate),
			ConvertVector3(translate)
		);

		// inverseBindOiseMatrixにする
		jointWeightData.inverseBindPoseMatrix = bindPoseMatrix.Inverse();

		// weight情報を取り出し
		for (uint32_t weightIndex = 0; weightIndex < aiBone->mNumWeights; ++weightIndex) {
			jointWeightData.vertexWeights.emplace_back(aiBone->mWeights[weightIndex].mWeight, aiBone->mWeights[weightIndex].mVertexId);
		}
	}

	input_.CreateMeshlet(); //!< meshletの生成
	input_.CreateBottomLevelAS(context); //!< BLASの生成

	BaseAsset::SetComplete();
	StreamLogger::EngineThreadLog(std::format("[AssetMesh]: mesh setup complete. uuid: {}", BaseAsset::SerializeId()));

}

const InputMesh& Asset::Mesh::GetInputMesh() const {
	BaseAsset::WaitComplete(); //!< TODO: 仮meshを用意する.
	return input_;
}

Vector3f Asset::Mesh::ConvertVector3(const aiVector3D& aiVector) {
	return { aiVector.x, aiVector.y, -aiVector.z }; //!< 左手座標系に変換
}

Vector4f Asset::Mesh::ConvertVector4(const aiVector3D& aiVector) {
	return { aiVector.x, aiVector.y, -aiVector.z, 1.0f }; //!< 左手座標系に変換
}

Quaternion Asset::Mesh::ConvertQuaternion(const aiQuaternion& aiQuaternion) {
	return { -aiQuaternion.x, -aiQuaternion.y, aiQuaternion.z, aiQuaternion.w }; //!< 左手座標系に変換
}
