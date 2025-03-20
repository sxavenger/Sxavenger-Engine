#include "InputMesh.h"
_DXOBJECT_USING

////////////////////////////////////////////////////////////////////////////////////////////
// Meshlet structure methods
////////////////////////////////////////////////////////////////////////////////////////////

void InputMesh::Meshlet::CreateMeshlet(TriangleInputAssembler<MeshVertexData>* ia) {
	//* InputAssemblerから情報を取得
	const auto vertex = ia->GetVertex();
	const auto index  = ia->GetIndex();

	//!< bufferが増えるので最適化は除外

	//!< bufferで使う構造体
	// meshlet情報
	std::vector<DirectX::Meshlet>         bufferMeshlets;
	std::vector<uint32_t>                 bufferUniqueVertexIndices;
	std::vector<DirectX::MeshletTriangle> bufferPrimitiveIndices;

	// cullData情報
	std::vector<DirectX::CullData> bufferCullDatas;

	// positionを別構造体へ
	std::vector<DirectX::XMFLOAT3> positions; // todo: 別構造体に移さなくてもいいようにする
	positions.resize(vertex->GetSize());

	for (uint32_t i = 0; i < vertex->GetSize(); ++i) {
		positions[i] = { vertex->At(i).position.x, vertex->At(i).position.y, vertex->At(i).position.z };
	}

	{ // meshletの情報の生成

		std::vector<uint8_t> uniqueVertexIB;

		auto hr = DirectX::ComputeMeshlets(
			reinterpret_cast<const UINT*>(ia->GetIndex()->GetData()), ia->GetIndex()->GetSize(), //!< HACK: ここはuint32_tでないといけない
			positions.data(), positions.size(),
			nullptr,
			bufferMeshlets,
			uniqueVertexIB,
			bufferPrimitiveIndices,
			kMaxVertices_,
			kMaxPrimitives_
		);

		Assert(SUCCEEDED(hr), "meshlet create failed.");

		// uint8_tから梱包してtriangles(uint32_t)に変換
		bufferUniqueVertexIndices.reserve(uniqueVertexIB.size() / sizeof(uint32_t));

		const uint32_t* datas  = reinterpret_cast<const uint32_t*>(uniqueVertexIB.data());
		const size_t kDataSize = uniqueVertexIB.size() / sizeof(uint32_t);

		for (size_t i = 0; i < kDataSize; ++i) {
			bufferUniqueVertexIndices.emplace_back(datas[i]);
		}
	}

	// cullData情報の生成
	{
		// メモリの確保
		bufferCullDatas.resize(bufferMeshlets.size());

		auto hr = DirectX::ComputeCullData(
			positions.data(), positions.size(),
			bufferMeshlets.data(), bufferMeshlets.size(),
			bufferUniqueVertexIndices.data(), bufferUniqueVertexIndices.size(),
			bufferPrimitiveIndices.data(), bufferPrimitiveIndices.size(),
			bufferCullDatas.data()
		);

		Assert(SUCCEEDED(hr), "cull data create failed.");
	}

	//!< meshletBufferの生成
	{
		// _DXOBJECT Deviceの取り出し
		auto device = SxavengerSystem::GetDxDevice();

		// uniqueVertexIndicesの初期化
		uniqueVertexIndices = std::make_unique<DimensionBuffer<uint32_t>>();
		uniqueVertexIndices->Create(device, static_cast<uint32_t>(bufferUniqueVertexIndices.size()));
		uniqueVertexIndices->Memcpy(bufferUniqueVertexIndices.data());

		// primitiveIndicesの初期化
		primitiveIndices = std::make_unique<DimensionBuffer<DirectX::MeshletTriangle>>();
		primitiveIndices->Create(device, static_cast<uint32_t>(bufferPrimitiveIndices.size()));
		primitiveIndices->Memcpy(bufferPrimitiveIndices.data());

		// meshletsの初期化
		meshlets = std::make_unique<DimensionBuffer<DirectX::Meshlet>>();
		meshlets->Create(device, static_cast<uint32_t>(bufferMeshlets.size()));
		meshlets->Memcpy(bufferMeshlets.data());

		// cullDataの初期化
		cullDatas = std::make_unique<DimensionBuffer<DirectX::CullData>>();
		cullDatas->Create(device, static_cast<uint32_t>(bufferCullDatas.size()));
		cullDatas->Memcpy(bufferCullDatas.data());

		// mesh情報を初期化
		info = std::make_unique<DimensionBuffer<MeshletInfo>>();
		info->Create(device, 1);
		info->At(0).meshletCount = static_cast<uint32_t>(bufferMeshlets.size());
	}
}

void InputMesh::Meshlet::Dispatch(const DirectXThreadContext* context, UINT instanceCount) const {
	context->GetCommandList()->DispatchMesh(
		RoundUp(static_cast<UINT>(meshlets->GetSize()), kAmplificationNumthread_),
		RoundUp(instanceCount, 1),
		1
	);
}

////////////////////////////////////////////////////////////////////////////////////////////
// MeshBottomLevelAS structure methods
////////////////////////////////////////////////////////////////////////////////////////////

void InputMesh::MeshBottomLevelAS::CreateBottomLevelAS(TriangleInputAssembler<MeshVertexData>* ia, const DirectXThreadContext* context) {
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
	if (meshlet_.has_value()) {
		return;
	}

	meshlet_ = Meshlet();
	meshlet_.value().CreateMeshlet(this);
}

void InputMesh::CreateBottomLevelAS(const DirectXThreadContext* context) {
	if (bottomLevelAS_.has_value()) {
		return;
	}

	bottomLevelAS_ = MeshBottomLevelAS();
	bottomLevelAS_.value().CreateBottomLevelAS(this, context);
}

const InputMesh::Meshlet& InputMesh::GetMeshlet() const {
	Assert(meshlet_.has_value(), "meshlet is not create.");
	return meshlet_.value();
}

const InputMesh::MeshBottomLevelAS& InputMesh::GetBottomLevelAS() const {
	Assert(bottomLevelAS_.has_value(), "bottom level as is not create.");
	return bottomLevelAS_.value();
}
