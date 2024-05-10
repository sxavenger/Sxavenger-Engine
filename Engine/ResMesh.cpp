#include <ResMesh.h>

// test include
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/cimport.h>

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
// DxObject
#include <DxBufferResource.h>

// meshoptimizer
#include <meshoptimizer.h>
// srcはd3d sampleから旧バージョンの取得
// hack: 最新バージョンの導入

////////////////////////////////////////////////////////////////////////////////////////////
// MeshLoader class
////////////////////////////////////////////////////////////////////////////////////////////
class MeshLoader {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//=========================================================================================
	// private methods
	//=========================================================================================


	void PerseMesh(
		DxObject::BufferPtrResource<VertexData>* verticesBuffer, DxObject::IndexBufferResource* indicesBuffer,
		ResMesh& dstMesh //!< param[out]
	);

	void PerseMesh(
		DxObject::BufferResource<VertexData>* verticesBuffer, DxObject::IndexBufferResource* indicesBuffer,
		ResMesh& dstMesh //!< param[out]
	);

};

////////////////////////////////////////////////////////////////////////////////////////////
// MeshLoader class methods
////////////////////////////////////////////////////////////////////////////////////////////

void MeshLoader::PerseMesh(
	DxObject::BufferPtrResource<VertexData>* verticesBuffer, DxObject::IndexBufferResource* indicesBuffer,
	ResMesh& dstMesh) {

	// マテリアルid
	dstMesh.materialId = 0; // assimpでのmaterialId

	// 頂点データのメモリを確保
	dstMesh.vertices.resize(verticesBuffer->GetIndexSize());

	// 頂点データのcopy
	memcpy(dstMesh.vertices.data(), verticesBuffer->GetDataArray(), verticesBuffer->GetIndexSize() * verticesBuffer->GetStructureSize());

	// 頂点インデックスのメモリの確保
	dstMesh.indices.resize(indicesBuffer->GetIndexSize());

	// 頂点インデックスのメモリ確保
	memcpy(dstMesh.indices.data(), indicesBuffer->GetDataArray(), indicesBuffer->GetIndexSize() * indicesBuffer->GetStructureSize());

	// 最適化
	{
		std::vector<uint32_t> remap(dstMesh.indices.size());

		// 重複データ削除のための再マッピング用インデックスを生成
		auto vertexCount = meshopt_generateVertexRemap(
			remap.data(),
			dstMesh.indices.data(),
			dstMesh.indices.size(),
			dstMesh.vertices.data(),
			dstMesh.vertices.size(),
			verticesBuffer->GetStructureSize()
		);

		std::vector<VertexData> vertices(vertexCount);
		std::vector<uint32_t>   indices(dstMesh.indices.size());

		// 頂点インデックスを再マッピング
		meshopt_remapIndexBuffer(
			indices.data(),
			dstMesh.indices.data(),
			dstMesh.indices.size(),
			remap.data()
		);

		// 頂点データを再マッピング
		meshopt_remapVertexBuffer(
			vertices.data(),
			dstMesh.vertices.data(),
			dstMesh.vertices.size(),
			verticesBuffer->GetStructureSize(),
			remap.data()
		);

		// 不要になったメモリの削除
		remap.clear();
		remap.shrink_to_fit();

		// 最適化したサイズにメモリ量を減らす
		dstMesh.vertices.resize(vertices.size());
		dstMesh.indices.resize(indices.size());

		// 頂点キャッシュの最適化
		meshopt_optimizeVertexCache(
			dstMesh.indices.data(),
			indices.data(),
			indices.size(),
			vertexCount
		);

		// 不要になったメモリの削除
		indices.clear();
		indices.shrink_to_fit();

		// 頂点フェッチの最適化
		meshopt_optimizeVertexFetch(
			dstMesh.vertices.data(),
			dstMesh.indices.data(),
			dstMesh.indices.size(),
			vertices.data(),
			vertices.size(),
			verticesBuffer->GetStructureSize()
		);

		// 不要になったメモリの削除
		vertices.clear();
		vertices.shrink_to_fit();

	}

	// メッシュレット生成
	{
		const uint32_t kMaxVertices   = 64;
		const uint32_t kMaxPrimitives = 126;

		std::vector<meshopt_Meshlet> meshlets(
			meshopt_buildMeshletsBound(
				dstMesh.indices.size(),
				kMaxVertices,
				kMaxPrimitives
			)
		);

		meshlets.resize(
			meshopt_buildMeshlets(
				meshlets.data(),
				dstMesh.indices.data(),
				dstMesh.indices.size(),
				dstMesh.vertices.size(),
				kMaxVertices,
				kMaxPrimitives
			)
		);

		// 最大値でのメモリ予約
		dstMesh.uniqueVertexIndices.reserve(meshlets.size() * kMaxVertices);
		dstMesh.primitiveIndices.reserve(meshlets.size() * kMaxPrimitives);

		for (auto& meshlet : meshlets) {

			auto vertexOffset    = static_cast<uint32_t>(dstMesh.uniqueVertexIndices.size());
			auto primitiveOffset = static_cast<uint32_t>(dstMesh.primitiveIndices.size());

			for (auto i = 0u; i < meshlet.vertex_count; ++i) {
				dstMesh.uniqueVertexIndices.push_back(meshlet.vertices[i]);
			}

			for (auto i = 0; i < meshlet.triangle_count; ++i) {
				ResPrimitiveIndex tris = {};
				tris.index1 = meshlet.indices[i][0];
				tris.index0 = meshlet.indices[i][1];
				tris.index2 = meshlet.indices[i][2];
				dstMesh.primitiveIndices.push_back(tris);
			}

			// メッシュレットデータの設定
			ResMeshlet m = {};
			m.vertexCount     = meshlet.vertex_count;
			m.vertexOffset    = vertexOffset;
			m.primitiveCount  = meshlet.triangle_count;
			m.primitiveOffset = primitiveOffset;

			dstMesh.meshlets.push_back(m);
		}

		// サイズの最適化
		dstMesh.uniqueVertexIndices.shrink_to_fit();
		dstMesh.primitiveIndices.shrink_to_fit();
		dstMesh.meshlets.shrink_to_fit();
	}

}
