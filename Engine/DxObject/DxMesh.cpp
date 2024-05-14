#include "DxMesh.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
// meshoptimizer
#include <meshoptimizer.h>

// MyEngine
#include <MyEngine.h>

////////////////////////////////////////////////////////////////////////////////////////////
// MeshLoader class methods
////////////////////////////////////////////////////////////////////////////////////////////

void DxObject::MeshLoader::PerseMesh(
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

			auto vertexOffset = static_cast<uint32_t>(dstMesh.uniqueVertexIndices.size());
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
			m.vertexCount = meshlet.vertex_count;
			m.vertexOffset = vertexOffset;
			m.primitiveCount = meshlet.triangle_count;
			m.primitiveOffset = primitiveOffset;

			dstMesh.meshlets.push_back(m);
		}

		// サイズの最適化
		dstMesh.uniqueVertexIndices.shrink_to_fit();
		dstMesh.primitiveIndices.shrink_to_fit();
		dstMesh.meshlets.shrink_to_fit();
	}

}

void DxObject::MeshLoader::PerseMesh(
	DxObject::BufferResource<VertexData>* verticesBuffer, DxObject::IndexBufferResource* indicesBuffer,
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

			auto vertexOffset = static_cast<uint32_t>(dstMesh.uniqueVertexIndices.size());
			auto primitiveOffset = static_cast<uint32_t>(dstMesh.primitiveIndices.size());

			for (auto i = 0u; i < meshlet.vertex_count; ++i) {
				dstMesh.uniqueVertexIndices.push_back(meshlet.vertices[i]);
			}

			for (auto i = 0; i < meshlet.triangle_count; ++i) {
				ResPrimitiveIndex tris = {};
				tris.index0 = meshlet.indices[i][0];
				tris.index1 = meshlet.indices[i][1];
				tris.index2 = meshlet.indices[i][2];
				dstMesh.primitiveIndices.push_back(tris);
			}

			// メッシュレットデータの設定
			ResMeshlet m = {};
			m.vertexCount = meshlet.vertex_count;
			m.vertexOffset = vertexOffset;
			m.primitiveCount = meshlet.triangle_count;
			m.primitiveOffset = primitiveOffset;

			dstMesh.meshlets.push_back(m);
		}

		// サイズの最適化
		dstMesh.uniqueVertexIndices.shrink_to_fit();
		dstMesh.primitiveIndices.shrink_to_fit();
		dstMesh.meshlets.shrink_to_fit();
	}

}

////////////////////////////////////////////////////////////////////////////////////////////
// Mesh class
////////////////////////////////////////////////////////////////////////////////////////////

void DxObject::Mesh::Init(BufferResource<VertexData>* vertexResource, IndexBufferResource* indexResource) {
	
	ResMesh resource;
	MeshLoader::PerseMesh(vertexResource, indexResource, resource); // vertexResource, indexResourceからmeshの生成

	auto deviceObj = MyEngine::GetDevicesObj();

	// verticesの初期化
	{
		vertices_.resource = std::make_unique<BufferResource<VertexData>>(deviceObj, static_cast<uint32_t>(resource.vertices.size()));
		vertices_.resource->Memcpy(resource.vertices.data());

		vertices_.structuredBuffer = std::make_unique<StructuredBuffer>(vertices_.resource.get());
	}

	// uniqueVertexIndices
	{
		uniqueVertexIndices_.resource = std::make_unique<BufferResource<uint32_t>>(deviceObj, static_cast<uint32_t>(resource.uniqueVertexIndices.size()));
		uniqueVertexIndices_.resource->Memcpy(resource.uniqueVertexIndices.data());

		uniqueVertexIndices_.structuredBuffer = std::make_unique<StructuredBuffer>(uniqueVertexIndices_.resource.get());
	}

	// primitiveIndices
	{
		primitiveIndices_.resource = std::make_unique<BufferResource<ResPrimitiveIndex>>(deviceObj, static_cast<uint32_t>(resource.primitiveIndices.size()));
		primitiveIndices_.resource->Memcpy(resource.primitiveIndices.data());

		primitiveIndices_.structuredBuffer = std::make_unique<StructuredBuffer>(primitiveIndices_.resource.get());
	}

	// meshlets
	{
		meshlets_.resource = std::make_unique<BufferResource<ResMeshlet>>(deviceObj, static_cast<uint32_t>(resource.meshlets.size()));
		meshlets_.resource->Memcpy(resource.meshlets.data());

		meshlets_.structuredBuffer = std::make_unique<StructuredBuffer>(meshlets_.resource.get());
	}

	// 値をBuffer側に保存したので解放
	resource.Term();
}

void DxObject::Mesh::Term() {
	vertices_.Reset();
	uniqueVertexIndices_.Reset();
	primitiveIndices_.Reset();
	meshlets_.Reset();
}

void DxObject::Mesh::Dispatch(UINT verticesParam, UINT uinqueVertexIndicesParam, UINT meshletsParam, UINT primitiveIndices) {
	// commandListの取得
	auto commandList = MyEngine::GetCommandList();

	// descriptorの設定
	commandList->SetGraphicsRootDescriptorTable(verticesParam, vertices_.structuredBuffer->GetGPUHandle());
	commandList->SetGraphicsRootDescriptorTable(uinqueVertexIndicesParam, uniqueVertexIndices_.structuredBuffer->GetGPUHandle());
	commandList->SetGraphicsRootDescriptorTable(meshletsParam, meshlets_.structuredBuffer->GetGPUHandle());
	commandList->SetGraphicsRootDescriptorTable(primitiveIndices, primitiveIndices_.structuredBuffer->GetGPUHandle());

	commandList->DispatchMesh(static_cast<UINT>(meshlets_.resource->GetIndexSize()), 1, 1);
}
