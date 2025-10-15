#include "InputMesh.h"
_DXOBJECT_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include <externals/meshoptimizer/meshoptimizer.h>

////////////////////////////////////////////////////////////////////////////////////////////
// MeshVertexData structure methods
////////////////////////////////////////////////////////////////////////////////////////////

void MeshVertexData::Init() {
	position = { 0.0f, 0.0f, 0.0f, 1.0f };
	texcoord = { 0.0f, 0.0f };
	normal   = { 0.0f, 0.0f, 0.0f };

	tangent   = { 0.0f, 0.0f, 0.0f };
	bitangent = { 0.0f, 0.0f, 0.0f };
}

////////////////////////////////////////////////////////////////////////////////////////////
// Meshlet structure methods
////////////////////////////////////////////////////////////////////////////////////////////

//void InputMesh::Meshlet::CreateMeshletOld(const TriangleInputAssembler<MeshVertexData>* ia) {
//	//* InputAssemblerから情報を取得
//	const auto vertex = ia->GetVertex();
//	const auto index  = ia->GetIndex();
//
//	//!< bufferが増えるので最適化は除外
//
//	//!< bufferで使う構造体
//	// meshlet情報
//	std::vector<DirectX::Meshlet>         bufferMeshlets;
//	std::vector<uint32_t>                 bufferUniqueVertexIndices;
//	std::vector<DirectX::MeshletTriangle> bufferPrimitiveIndices;
//
//	// cullData情報
//	std::vector<DirectX::CullData> bufferCullDatas;
//
//	// positionを別構造体へ
//	std::vector<DirectX::XMFLOAT3> positions; // todo: 別構造体に移さなくてもいいようにする
//	positions.resize(vertex->GetSize());
//
//	for (uint32_t i = 0; i < vertex->GetSize(); ++i) {
//		positions[i] = { vertex->At(i).position.x, vertex->At(i).position.y, vertex->At(i).position.z };
//	}
//
//	{ // meshletの情報の生成
//
//		std::vector<uint8_t> uniqueVertexIB;
//
//		auto hr = DirectX::ComputeMeshlets(
//			reinterpret_cast<const UINT*>(index->GetData()), index->GetSize(), //!< HACK: ここはuint32_tでないといけない
//			positions.data(), positions.size(),
//			nullptr,
//			bufferMeshlets,
//			uniqueVertexIB,
//			bufferPrimitiveIndices,
//			kMaxVertices_,
//			kMaxPrimitives_
//		);
//
//		DxObject::Assert(hr, L"meshlet create failed.");
//
//		// uint8_tから梱包してtriangles(uint32_t)に変換
//		bufferUniqueVertexIndices.reserve(uniqueVertexIB.size() / sizeof(uint32_t));
//
//		const uint32_t* datas  = reinterpret_cast<const uint32_t*>(uniqueVertexIB.data());
//		const size_t kDataSize = uniqueVertexIB.size() / sizeof(uint32_t);
//
//		for (size_t i = 0; i < kDataSize; ++i) {
//			bufferUniqueVertexIndices.emplace_back(datas[i]);
//		}
//	}
//
//	// cullData情報の生成
//	{
//		// メモリの確保
//		bufferCullDatas.resize(bufferMeshlets.size());
//
//		auto hr = DirectX::ComputeCullData(
//			positions.data(), positions.size(),
//			bufferMeshlets.data(), bufferMeshlets.size(),
//			bufferUniqueVertexIndices.data(), bufferUniqueVertexIndices.size(),
//			bufferPrimitiveIndices.data(), bufferPrimitiveIndices.size(),
//			bufferCullDatas.data()
//		);
//
//		DxObject::Assert(hr, L"cull data create failed.");
//
//	}
//
//	//!< meshletBufferの生成
//	{
//		// _DXOBJECT Deviceの取り出し
//		auto device = SxavengerSystem::GetDxDevice();
//
//		// mesh情報を初期化
//		info = std::make_unique<ConstantBuffer<MeshletInfo>>();
//		info->Create(device);
//		info->At().meshletCount = static_cast<uint32_t>(bufferMeshlets.size());
//
//		// uniqueVertexIndicesの初期化
//		uniqueVertexIndices = std::make_unique<DimensionBuffer<uint32_t>>();
//		uniqueVertexIndices->Create(device, static_cast<uint32_t>(bufferUniqueVertexIndices.size()));
//		uniqueVertexIndices->Memcpy(bufferUniqueVertexIndices.data());
//
//		// primitiveIndicesの初期化
//		primitiveIndices = std::make_unique<DimensionBuffer<DirectX::MeshletTriangle>>();
//		primitiveIndices->Create(device, static_cast<uint32_t>(bufferPrimitiveIndices.size()));
//		primitiveIndices->Memcpy(bufferPrimitiveIndices.data());
//
//		// meshletsの初期化
//		meshlets = std::make_unique<DimensionBuffer<DirectX::Meshlet>>();
//		meshlets->Create(device, static_cast<uint32_t>(bufferMeshlets.size()));
//		meshlets->Memcpy(bufferMeshlets.data());
//
//		// cullDataの初期化
//		cullDatas = std::make_unique<DimensionBuffer<DirectX::CullData>>();
//		cullDatas->Create(device, static_cast<uint32_t>(bufferCullDatas.size()));
//		cullDatas->Memcpy(bufferCullDatas.data());
//	}
//}

void InputMesh::Meshlet::CreateMeshlet(const TriangleInputAssembler<MeshVertexData>* ia) {
	//* InputAssemblerから情報を取得
	const auto vertex = ia->GetVertex();
	const auto index  = ia->GetIndex();

	//!< bufferが増えるので最適化は除外

	// メッシュレットの最大個数を見積もり
	const size_t kMaxMeshletCount = meshopt_buildMeshletsBound(index->GetIndexCount(), kMaxVertices_, kMaxPrimitives_);

	std::vector<meshopt_Meshlet> meshlet(kMaxMeshletCount);
	std::vector<uint32_t>        meshletVertex(kMaxMeshletCount * kMaxVertices_);
	std::vector<uint8_t>         meshletPrimitives(kMaxMeshletCount * kMaxPrimitives_);

	meshletCount = static_cast<uint32_t>(meshopt_buildMeshlets(
		meshlet.data(),
		meshletVertex.data(),
		meshletPrimitives.data(),
		index->GetIndexData(),
		index->GetIndexCount(),
		&vertex->GetData()->position.x,
		vertex->GetSize(),
		vertex->GetStride(),
		kMaxVertices_,
		kMaxPrimitives_,
		NULL
	));

	std::vector<uint32_t> _uniqueVertexIndices;
	_uniqueVertexIndices.reserve(meshletCount * kMaxVertices_);

	std::vector<MeshletTriangle> _primitiveIndices;
	_primitiveIndices.reserve(meshletCount * kMaxPrimitives_);

	std::vector<MeshletData> _meshlet;
	_meshlet.reserve(meshletCount);

	for (size_t i = 0; i < meshletCount; ++i) {

		const auto& m = meshlet[i];

		uint32_t vertexOffset   = static_cast<uint32_t>(_uniqueVertexIndices.size());
		uint32_t triangleOffset = static_cast<uint32_t>(_primitiveIndices.size());

		for (uint32_t j = 0; j < m.vertex_count; ++j) {
			_uniqueVertexIndices.emplace_back(meshletVertex[m.vertex_offset + j]);
		}

		for (uint32_t j = 0; j < m.triangle_count; ++j) {
			uint32_t triangleIndex = m.triangle_offset + j * 3;

			MeshletTriangle triangle = {};
			triangle.i0 = meshletPrimitives[triangleIndex + 0];
			triangle.i1 = meshletPrimitives[triangleIndex + 1];
			triangle.i2 = meshletPrimitives[triangleIndex + 2];

			_primitiveIndices.emplace_back(triangle);
		}

		MeshletData data = {};
		data.vertexCount    = m.vertex_count;
		data.triangleCount  = m.triangle_count;
		data.vertexOffset   = vertexOffset;
		data.triangleOffset = triangleOffset;

		_meshlet.emplace_back(data);

	}

	{
		// _DXOBJECT Deviceの取り出し
		auto device = SxavengerSystem::GetDxDevice();

		meshlets = std::make_unique<DimensionBuffer<MeshletData>>();
		meshlets->Create(device, static_cast<uint32_t>(meshletCount));
		meshlets->Memcpy(_meshlet.data());

		uniqueVertexIndices = std::make_unique<DimensionBuffer<uint32_t>>();
		uniqueVertexIndices->Create(device, static_cast<uint32_t>(_uniqueVertexIndices.size()));
		uniqueVertexIndices->Memcpy(_uniqueVertexIndices.data());

		primitiveIndices = std::make_unique<DimensionBuffer<MeshletTriangle>>();
		primitiveIndices->Create(device, static_cast<uint32_t>(_primitiveIndices.size()));
		primitiveIndices->Memcpy(_primitiveIndices.data());

	}

}

void InputMesh::Meshlet::Dispatch(const DirectXQueueContext* context, UINT instanceCount) const {
	context->GetCommandList()->DispatchMesh(
		RoundUp(static_cast<UINT>(meshlets->GetSize()), kAmplificationNumthread_),
		RoundUp(instanceCount, 1),
		1
	);
}

////////////////////////////////////////////////////////////////////////////////////////////
// MeshBottomLevelAS structure methods
////////////////////////////////////////////////////////////////////////////////////////////

void InputMesh::MeshBottomLevelAS::CreateBottomLevelAS(const TriangleInputAssembler<MeshVertexData>* ia, const DirectXQueueContext* context) {
	D3D12_RAYTRACING_GEOMETRY_DESC desc = {};
	desc.Type                                 = D3D12_RAYTRACING_GEOMETRY_TYPE_TRIANGLES;
	desc.Flags                                = D3D12_RAYTRACING_GEOMETRY_FLAG_NONE;
	desc.Triangles.VertexBuffer.StartAddress  = ia->GetVertex()->GetGPUVirtualAddress();
	desc.Triangles.VertexBuffer.StrideInBytes = ia->GetVertex()->GetStride();
	desc.Triangles.VertexCount                = ia->GetVertex()->GetSize();
	desc.Triangles.VertexFormat               = DXGI_FORMAT_R32G32B32_FLOAT;
	desc.Triangles.IndexBuffer                = ia->GetIndex()->GetGPUVirtualAddress();
	desc.Triangles.IndexCount                 = ia->GetIndex()->GetIndexCount();
	desc.Triangles.IndexFormat                = DXGI_FORMAT_R32_UINT;

	// BottomLevelASの生成
	bottomLevelAS = std::make_unique<DxrObject::BottomLevelAS>();
	bottomLevelAS->Build(SxavengerSystem::GetDxDevice(), context->GetDxCommand(), desc);
}

////////////////////////////////////////////////////////////////////////////////////////////
// InputMesh class methods
////////////////////////////////////////////////////////////////////////////////////////////

void InputMesh::CreateMeshlet() {
	meshlet_ = Meshlet();
	(*meshlet_).CreateMeshlet(this);
}

void InputMesh::CreateBottomLevelAS(const DirectXQueueContext* context) {
	bottomLevelAS_ = MeshBottomLevelAS();
	bottomLevelAS_.value().CreateBottomLevelAS(this, context);
}

const InputMesh::Meshlet& InputMesh::GetMeshlet() const {
	Exception::Assert(meshlet_.has_value(), "meshlet is not create.");
	return meshlet_.value();
}

const InputMesh::MeshBottomLevelAS& InputMesh::GetBottomLevelAS() const {
	Exception::Assert(bottomLevelAS_.has_value(), "bottom level as is not create.");
	return bottomLevelAS_.value();
}
