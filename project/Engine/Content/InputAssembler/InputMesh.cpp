#include "InputMesh.h"
_DXOBJECT_USING

////////////////////////////////////////////////////////////////////////////////////////////
// InputMesh class methods
////////////////////////////////////////////////////////////////////////////////////////////

void InputMesh::CreateMeshlet() {
	if (isCreateMeshlet_) {
		return;
	}

	//!< 最適化は除外
	//!< bufferが増えるので
	
	//!< bufferで使う構造体
	// meshlet情報
	std::vector<DirectX::Meshlet>         bufferMeshlets;
	std::vector<uint32_t>                 bufferUniqueVertexIndices;
	std::vector<DirectX::MeshletTriangle> bufferPrimitiveIndices;

	// cullData情報
	std::vector<DirectX::CullData>        bufferCullDatas;

	// positionを別構造体へ
	std::vector<DirectX::XMFLOAT3> positions;
	positions.reserve(vertex_->GetSize());

	for (uint32_t i = 0; i < vertex_->GetSize(); ++i) {
		DirectX::XMFLOAT3 element = {};

		element.x = (*vertex_)[i].position.x;
		element.y = (*vertex_)[i].position.y;
		element.z = (*vertex_)[i].position.z;

		positions.emplace_back(element);
	}

	// meshletの情報の生成
	{
		std::vector<uint8_t> uniqueVertexIB;

		auto hr = DirectX::ComputeMeshlets(
			reinterpret_cast<const UINT*>(index_->GetData()), index_->GetSize(), //!< HACK: ここはuint32_tでないといけない
			positions.data(), positions.size(),
			nullptr,
			bufferMeshlets,
			uniqueVertexIB,
			bufferPrimitiveIndices,
			kMaxVertices_,
			kMaxPrimitives_
		);

		Assert(SUCCEEDED(hr));

		// uint8_tから梱包してtriangles(uint32_t)に変換
		bufferUniqueVertexIndices.reserve(uniqueVertexIB.size() / sizeof(uint32_t));

		const uint32_t* datas = reinterpret_cast<const uint32_t*>(uniqueVertexIB.data());
		size_t count = uniqueVertexIB.size() / sizeof(uint32_t);

		for (size_t i = 0; i < count; ++i) {
			bufferUniqueVertexIndices.push_back(datas[i]);
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

		Assert(SUCCEEDED(hr));
	}

	//!< meshletBufferの生成
	{
		// _DXOBJECT Deviceの取り出し
		auto device = SxavengerSystem::GetDxDevice();

		// uniqueVertexIndicesの初期化
		uniqueVertexIndices_ = std::make_unique<DimensionBuffer<uint32_t>>();
		uniqueVertexIndices_->Create(device, static_cast<uint32_t>(bufferUniqueVertexIndices.size()));
		uniqueVertexIndices_->Memcpy(bufferUniqueVertexIndices.data());

		// primitiveIndicesの初期化
		primitiveIndices_ = std::make_unique<DimensionBuffer<DirectX::MeshletTriangle>>();
		primitiveIndices_->Create(device, static_cast<uint32_t>(bufferPrimitiveIndices.size()));
		primitiveIndices_->Memcpy(bufferPrimitiveIndices.data());

		// meshletsの初期化
		meshlets_ = std::make_unique<DimensionBuffer<DirectX::Meshlet>>();
		meshlets_->Create(device, static_cast<uint32_t>(bufferMeshlets.size()));
		meshlets_->Memcpy(bufferMeshlets.data());

		// cullDataの初期化
		cullDatas_ = std::make_unique<DimensionBuffer<DirectX::CullData>>();
		cullDatas_->Create(device, static_cast<uint32_t>(bufferCullDatas.size()));
		cullDatas_->Memcpy(bufferCullDatas.data());

		// mesh情報を初期化
		meshInfo_ = std::make_unique<DimensionBuffer<MeshInfo>>();
		meshInfo_->Create(device, 1);
		(*meshInfo_)[0].meshletCount = static_cast<uint32_t>(bufferMeshlets.size());
	}

	isCreateMeshlet_ = true;
}

DxObject::BindBufferDesc InputMesh::GetMeshletBindBufferDesc() const {
	DxObject::BindBufferDesc desc = {};
	desc.SetAddress("gVertices",   vertex_->GetGPUVirtualAddress());
	desc.SetAddress("gIndices",    uniqueVertexIndices_->GetGPUVirtualAddress());
	desc.SetAddress("gMeshlets",   meshlets_->GetGPUVirtualAddress());
	desc.SetAddress("gPrimitives", primitiveIndices_->GetGPUVirtualAddress());
	desc.SetAddress("gCullData",   cullDatas_->GetGPUVirtualAddress());
	desc.SetAddress("gMeshInfo",   meshInfo_->GetGPUVirtualAddress());
	return desc;
}

void InputMesh::Dispatch(const DirectXThreadContext* context, UINT instanceCount) const {
	Assert(isCreateMeshlet_, "meshlet is not create.");
	context->GetCommandList()->DispatchMesh(
		RoundUp(static_cast<UINT>(meshlets_->GetSize()), kAmplificationNumthread_),
		RoundUp(instanceCount, 1),
		1
	);
}
