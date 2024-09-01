#include "DxMesh.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
// meshoptimizer
#include <meshoptimizer.h>

// Sxavenger
#include <Sxavenger.h>

////////////////////////////////////////////////////////////////////////////////////////////
// MeshLoader class methods
////////////////////////////////////////////////////////////////////////////////////////////

void DxObject::MeshLoader::PerseMesh(
	DxObject::BufferResource<VertexData>* verticesBuffer, DxObject::IndexBufferResource* indicesBuffer,
	ResMesh& dstMesh) {

	// マテリアルid
	dstMesh.materialId = 0; // assimpでのmaterialId

	// 頂点データのメモリを確保
	dstMesh.vertices.resize(verticesBuffer->GetIndexSize());

	// 頂点データのcopy
	memcpy(dstMesh.vertices.data(), verticesBuffer->GetData(), verticesBuffer->GetIndexSize() * verticesBuffer->GetStructureSize());

	// 頂点インデックスのメモリの確保
	dstMesh.indices.resize(indicesBuffer->GetIndexSize());

	// 頂点インデックスのメモリ確保
	memcpy(dstMesh.indices.data(), indicesBuffer->GetData(), indicesBuffer->GetIndexSize() * indicesBuffer->GetStructureSize());

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
				DirectX::MeshletTriangle tris = {};
				tris.i0 = meshlet.indices[i][0];
				tris.i1 = meshlet.indices[i][1];
				tris.i2 = meshlet.indices[i][2];
				dstMesh.primitiveIndices.push_back(tris);
			}

			// メッシュレットデータの設定
			DirectX::Meshlet m = {};
			m.VertCount  = meshlet.vertex_count;
			m.VertOffset = vertexOffset;
			m.PrimCount  = meshlet.triangle_count;
			m.PrimOffset = primitiveOffset;

			dstMesh.meshlets.push_back(m);
		}

		// サイズの最適化
		dstMesh.uniqueVertexIndices.shrink_to_fit();
		dstMesh.primitiveIndices.shrink_to_fit();
		dstMesh.meshlets.shrink_to_fit();
	}

	// CullDataの生成
	{
		// DirectX::XMFLOAT3に情報を移す
		std::vector<DirectX::XMFLOAT3> vertexPostions;
		
		for (auto& vertexData : dstMesh.vertices) {
			DirectX::XMFLOAT3 element = {};

			element.x = vertexData.position.x;
			element.y = vertexData.position.y;
			element.z = vertexData.position.z;

			vertexPostions.push_back(element);
		}

		// メモリの確保
		dstMesh.cullDatas.resize(dstMesh.meshlets.size());

		auto hr = DirectX::ComputeCullData(
			vertexPostions.data(), vertexPostions.size(),
			dstMesh.meshlets.data(), dstMesh.meshlets.size(),
			dstMesh.uniqueVertexIndices.data(), dstMesh.uniqueVertexIndices.size(),
			dstMesh.primitiveIndices.data(), dstMesh.primitiveIndices.size(),
			dstMesh.cullDatas.data()
		);

		Assert(SUCCEEDED(hr));
	}
}

////////////////////////////////////////////////////////////////////////////////////////////
// Mesh class methods
////////////////////////////////////////////////////////////////////////////////////////////

void DxObject::Mesh::Init(BufferResource<VertexData>* vertexResource, IndexBufferResource* indexResource) {

	ResMesh resource;
	MeshLoader::PerseMesh(vertexResource, indexResource, resource); // vertexResource, indexResourceからmeshの生成

	auto deviceObj = Sxavenger::GetDevicesObj();

	// verticesの初期化
	vertices_ = std::make_unique<BufferResource<VertexData>>(deviceObj, static_cast<uint32_t>(resource.vertices.size()));
	vertices_->Memcpy(resource.vertices.data());
	

	// uniqueVertexIndicesの初期化
	uniqueVertexIndices_ = std::make_unique<BufferResource<uint32_t>>(deviceObj, static_cast<uint32_t>(resource.uniqueVertexIndices.size()));
	uniqueVertexIndices_->Memcpy(resource.uniqueVertexIndices.data());
	

	// primitiveIndicesの初期化
	primitiveIndices_ = std::make_unique<BufferResource<DirectX::MeshletTriangle>>(deviceObj, static_cast<uint32_t>(resource.primitiveIndices.size()));
	primitiveIndices_->Memcpy(resource.primitiveIndices.data());
	

	// meshletsの初期化
	meshlets_ = std::make_unique<BufferResource<DirectX::Meshlet>>(deviceObj, static_cast<uint32_t>(resource.meshlets.size()));
	meshlets_->Memcpy(resource.meshlets.data());

	// cullDataの初期化
	cullDatas_ = std::make_unique<BufferResource<DirectX::CullData>>(deviceObj, static_cast<uint32_t>(resource.cullDatas.size()));
	cullDatas_->Memcpy(resource.cullDatas.data());

	info_ = std::make_unique<BufferResource<MeshInfo>>(Sxavenger::GetDevicesObj(), 1);
	info_->operator[](0).meshletCount = static_cast<uint32_t>(resource.meshlets.size());
}

void DxObject::Mesh::Term() {
}

void DxObject::Mesh::Dispatch(UINT verticesParam, UINT uniqueVertexIndicesParam, UINT meshletsParam, UINT primitiveIndices) {

	// commandListの取得
	auto commandList = Sxavenger::GetCommandList();

	// descriptorの設定
	commandList->SetGraphicsRootShaderResourceView(verticesParam, vertices_->GetGPUVirtualAddress());
	commandList->SetGraphicsRootShaderResourceView(uniqueVertexIndicesParam, uniqueVertexIndices_->GetGPUVirtualAddress());
	commandList->SetGraphicsRootShaderResourceView(meshletsParam, meshlets_->GetGPUVirtualAddress());
	commandList->SetGraphicsRootShaderResourceView(primitiveIndices, primitiveIndices_->GetGPUVirtualAddress());

	commandList->DispatchMesh(static_cast<UINT>(meshlets_->GetIndexSize()), 1, 1);

}

void DxObject::Mesh::Dispatch(UINT verticesParam, UINT uniqueVertexIndicesParam, UINT meshletsParam, UINT primitiveIndices, UINT cullDataParam, UINT infoParam) {

	// commandListの取得
	auto commandList = Sxavenger::GetCommandList();

	// descriptorの設定
	commandList->SetGraphicsRootShaderResourceView(verticesParam, vertices_->GetGPUVirtualAddress());
	commandList->SetGraphicsRootShaderResourceView(uniqueVertexIndicesParam, uniqueVertexIndices_->GetGPUVirtualAddress());
	commandList->SetGraphicsRootShaderResourceView(meshletsParam, meshlets_->GetGPUVirtualAddress());
	commandList->SetGraphicsRootShaderResourceView(primitiveIndices, primitiveIndices_->GetGPUVirtualAddress());
	commandList->SetGraphicsRootShaderResourceView(cullDataParam, cullDatas_->GetGPUVirtualAddress());

	commandList->SetGraphicsRootConstantBufferView(infoParam, info_->GetGPUVirtualAddress());

	commandList->DispatchMesh(DivRoundUp(static_cast<UINT>(meshlets_->GetIndexSize())), 1, 1);

}

UINT DxObject::Mesh::DivRoundUp(UINT meshletCount) {
	return (meshletCount + kAmplificationRound_ - 1) / kAmplificationRound_;
}
