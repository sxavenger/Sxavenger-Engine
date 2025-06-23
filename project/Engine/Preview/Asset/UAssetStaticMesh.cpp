#include "UAssetStaticMesh.h"

////////////////////////////////////////////////////////////////////////////////////////////
// UAssetStaticMesh class methods
////////////////////////////////////////////////////////////////////////////////////////////

void UAssetStaticMesh::Load(const aiMesh* aiMesh, const std::vector<Uuid>& materials) {

	uuid_ = Uuid::Generate();
	name_ = aiMesh->mName.C_Str();

	{ //!< InputAssembler

		// aiMeshの初期化
		mesh_.Create(aiMesh->mNumVertices, aiMesh->mNumFaces);

		auto vertex = mesh_.GetVertex();
		auto index  = mesh_.GetIndex();

		// verticesの解析
		for (uint32_t element = 0; element < aiMesh->mNumVertices; ++element) {

			//!< position
			const aiVector3D& position  = aiMesh->mVertices[element];
			(*vertex)[element].position = ConvertPosition4(position);

			//!< normal
			if (aiMesh->HasNormals()) {
				const aiVector3D& normal  = aiMesh->mNormals[element];
				(*vertex)[element].normal = ConvertNormal(normal);
			}

			//!< texcoord
			if (aiMesh->HasTextureCoords(0)) {
				const aiVector3D& texcoord  = aiMesh->mTextureCoords[0][element];
				(*vertex)[element].texcoord = { texcoord.x, texcoord.y };
			}

			if (aiMesh->HasTangentsAndBitangents()) {
				const aiVector3D& tangent  = aiMesh->mTangents[element];
				(*vertex)[element].tangent = { tangent.x, tangent.y, -tangent.z }; //!< 左手座標系に変換

				const aiVector3D& bitangent  = aiMesh->mBitangents[element];
				(*vertex)[element].bitangent = { bitangent.x, bitangent.y, -bitangent.z }; //!< 左手座標系に変換
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
	}

	{ //!< material
		if (aiMesh->mMaterialIndex < materials.size()) {
			material_ = materials.at(aiMesh->mMaterialIndex);
		}
	}

}

json UAssetStaticMesh::Serialize() const {
	json data = UBaseAsset::Serialize();

	{ //!< vertex
		auto& vertex = data["mesh"]["vertex"];
		vertex["size"] = mesh_.GetVertex()->GetSize();

		const auto& span = mesh_.GetVertex()->GetSpan();
		std::string binary(span.size() * mesh_.GetVertex()->GetStride(), '\0');
		std::memcpy(binary.data(), span.data(), span.size() * mesh_.GetVertex()->GetStride());

		vertex["binary"] = binary;
	}
	
	{ //!< index
		auto& index = data["mesh"]["index"];
		index["size"] = mesh_.GetIndex()->GetSize();

		const auto& span = mesh_.GetIndex()->GetSpan();
		std::string binary(span.size() * mesh_.GetIndex()->GetStride(), '\0');
		std::memcpy(binary.data(), span.data(), span.size() * mesh_.GetIndex()->GetStride());

		index["binary"] = binary;
	}

	//!< material
	data["material"] = material_.has_value() ? material_->Serialize() : "null";

	return data;
}

void UAssetStaticMesh::Deserialize(const json& data) {
	UBaseAsset::Deserialize(data);

	const auto& vertex = data.at("mesh").at("vertex");
	const auto& index  = data.at("mesh").at("index");

	mesh_.Create(vertex.at("size").get<uint32_t>(), index.at("size").get<uint32_t>());

	std::memcpy(
		mesh_.GetVertex()->GetSpan().data(),
		vertex.at("binary").get<std::string>().data(),
		mesh_.GetVertex()->GetSpan().size() * mesh_.GetVertex()->GetStride()
	);

	std::memcpy(
		mesh_.GetIndex()->GetSpan().data(),
		index.at("binary").get<std::string>().data(),
		mesh_.GetIndex()->GetSpan().size() * mesh_.GetIndex()->GetStride()
	);

	if (data.at("material") != "null") {
		material_ = Uuid::Deserialize(data.at("material").get<std::string>());
	}

}

Vector3f UAssetStaticMesh::ConvertNormal(const aiVector3D& aiVector) {
	return { aiVector.x, aiVector.y, -aiVector.z }; //!< 左手座標系に変換
}

Vector3f UAssetStaticMesh::ConvertPosition3(const aiVector3D& aiVector) {
	return { aiVector.x, aiVector.y, -aiVector.z }; //!< 左手座標系に変換
}

Vector4f UAssetStaticMesh::ConvertPosition4(const aiVector3D& aiVector) {
	return { aiVector.x, aiVector.y, -aiVector.z, 1.0f }; //!< 左手座標系に変換
}

Quaternion UAssetStaticMesh::ConvertQuaternion(const aiQuaternion& aiQuaternion) {
	return { -aiQuaternion.x, -aiQuaternion.y, aiQuaternion.z, aiQuaternion.w }; //!< 左手座標系に変換
}
