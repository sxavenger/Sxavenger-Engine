#include "InputMesh.h"
SXAVENGER_ENGINE_USING
DXOBJECT_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/Utility/StreamLogger.h>

//* externals
#include <meshoptimizer.h>

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

void InputMesh::Meshlet::CreateMeshlet(const TriangleInputAssembler<MeshVertexData>* ia) {
	//* InputAssemblerから情報を取得
	const auto vertex = ia->GetVertex();
	const auto index  = ia->GetIndex();

	//!< bufferが増えるので最適化は除外

	// メッシュレットの最大個数を見積もり
	const size_t kMaxMeshletCount = meshopt_buildMeshletsBound(index->GetIndexCount(), kMaxVertices, kMaxPrimitives);

	std::vector<meshopt_Meshlet> meshlet(kMaxMeshletCount);
	std::vector<uint32_t>        meshletVertex(kMaxMeshletCount * kMaxVertices);
	std::vector<uint8_t>         meshletPrimitives(kMaxMeshletCount * kMaxPrimitives);

	meshletCount = static_cast<uint32_t>(meshopt_buildMeshlets(
		meshlet.data(),
		meshletVertex.data(),
		meshletPrimitives.data(),
		index->GetIndexData(),
		index->GetIndexCount(),
		&vertex->GetData()->position.x,
		vertex->GetSize(),
		vertex->GetStride(),
		kMaxVertices,
		kMaxPrimitives,
		NULL
	));

	std::vector<uint32_t> _uniqueVertexIndices;
	_uniqueVertexIndices.reserve(meshletCount * kMaxVertices);

	std::vector<MeshletTriangle> _primitiveIndices;
	_primitiveIndices.reserve(meshletCount * kMaxPrimitives);

	std::vector<MeshletData> _meshlet;
	_meshlet.reserve(meshletCount);

	std::vector<MeshletBounds> _meshletBounds;
	_meshletBounds.reserve(meshletCount);

	//!< meshlet情報の格納
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

	//!< boundingの生成
	for (size_t i = 0; i < meshletCount; ++i) {

		const auto& m = meshlet[i];

		meshopt_Bounds data = meshopt_computeMeshletBounds(
			&meshletVertex[m.vertex_offset],        //!< メッシュレット内のユニーク頂点インデックス
			&meshletPrimitives[m.triangle_offset],  //!< メッシュレット内の三角形 (i0,i1,i2) の並び
			m.triangle_count,                       //!< 三角形数
			&vertex->GetData()->position.x,        //!< 全頂点の位置バッファ
			vertex->GetSize(),                     //!< 全頂点数
			vertex->GetStride()                    //!< 頂点のストライド
		);

		MeshletBounds bounds = {};
		bounds.center    = { data.center[0], data.center[1], data.center[2] };
		bounds.radius    = data.radius;
		bounds.coneApex  = { data.cone_apex[0], data.cone_apex[1], data.cone_apex[2] };
		bounds.coneAxis  = { data.cone_axis[0], data.cone_axis[1], data.cone_axis[2] };
		bounds.coneCutoff = data.cone_cutoff;

		_meshletBounds.emplace_back(bounds);
	}

	{
		// DXOBJECT Deviceの取り出し
		auto device = System::GetDxDevice();

		meshlets = std::make_unique<DimensionBuffer<MeshletData>>();
		meshlets->Create(device, static_cast<uint32_t>(meshletCount));
		meshlets->Memcpy(_meshlet.data());

		uniqueVertexIndices = std::make_unique<DimensionBuffer<uint32_t>>();
		uniqueVertexIndices->Create(device, static_cast<uint32_t>(_uniqueVertexIndices.size()));
		uniqueVertexIndices->Memcpy(_uniqueVertexIndices.data());

		primitiveIndices = std::make_unique<DimensionBuffer<MeshletTriangle>>();
		primitiveIndices->Create(device, static_cast<uint32_t>(_primitiveIndices.size()));
		primitiveIndices->Memcpy(_primitiveIndices.data());

		meshletBounds = std::make_unique<DimensionBuffer<MeshletBounds>>();
		meshletBounds->Create(device, static_cast<uint32_t>(_meshletBounds.size()));
		meshletBounds->Memcpy(_meshletBounds.data());

	}

}

void InputMesh::Meshlet::Dispatch(const DirectXQueueContext* context, UINT instanceCount) const {
	context->GetCommandList()->DispatchMesh(
		RoundUp(static_cast<UINT>(meshlets->GetSize()), kAmplificationNumthread),
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
	bottomLevelAS->Build(System::GetDxDevice(), context->GetDxCommand(), desc);
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
	StreamLogger::AssertA(meshlet_.has_value(), "meshlet is not create.");
	return meshlet_.value();
}

const InputMesh::MeshBottomLevelAS& InputMesh::GetBottomLevelAS() const {
	StreamLogger::AssertA(bottomLevelAS_.has_value(), "bottom level as is not create.");
	return bottomLevelAS_.value();
}
