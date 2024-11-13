#pragma once
/*
* todo: PerseMesh, Modelとの相性が悪いので修正
*/

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* DxObjectCommon
#include "DxObjectCommon.h"

//* DirectX
#include <DirectXMesh.h>

//* c++
#include <cstdint>
#include <vector>
#include <memory>

// object
#include <Lib/ObjectStructure.h>

//* DxObject
#include "DxBufferResource.h"

////////////////////////////////////////////////////////////////////////////////////////////
// ResMesh structure
////////////////////////////////////////////////////////////////////////////////////////////
struct ResMesh {
	std::vector<VertexData> vertices;
	std::vector<uint32_t>   indices;
	uint32_t                materialId;

	std::vector<DirectX::Meshlet>         meshlets;
	std::vector<uint32_t>                 uniqueVertexIndices;
	std::vector<DirectX::MeshletTriangle> primitiveIndices;

	std::vector<DirectX::CullData> cullDatas;

	ResMesh() = default;

	~ResMesh() { Clear(); }

	void Clear() {
		vertices.clear();
		vertices.shrink_to_fit();
		indices.clear();
		indices.shrink_to_fit();
		materialId = NULL;

		meshlets.clear();
		meshlets.shrink_to_fit();
		uniqueVertexIndices.clear();
		uniqueVertexIndices.shrink_to_fit();
		primitiveIndices.clear();
		primitiveIndices.shrink_to_fit();

		cullDatas.clear();
		cullDatas.shrink_to_fit();
	}

};

////////////////////////////////////////////////////////////////////////////////////////////
// DxObject
////////////////////////////////////////////////////////////////////////////////////////////
_DXOBJECT_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// MeshLoader class
////////////////////////////////////////////////////////////////////////////////////////////
class MeshLoader {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	static void PerseMesh(
		DxObject::BufferResource<VertexData>* verticesBuffer, DxObject::IndexBufferResource* indicesBuffer,
		ResMesh& dstMesh //!< param[out]
	);
	
	//!< InputAssemblerから継承

private:

	//=========================================================================================
	// private variables
	//=========================================================================================
	
	static const uint32_t kMaxVertices   = 64;
	static const uint32_t kMaxPrimitives = 126;

	//=========================================================================================
	// private methods
	//=========================================================================================

};

////////////////////////////////////////////////////////////////////////////////////////////
// Mesh class
////////////////////////////////////////////////////////////////////////////////////////////
class Mesh {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	Mesh() = delete;

	Mesh(BufferResource<VertexData>* vertexResource, IndexBufferResource* indexResource) {
		Init(vertexResource, indexResource);
	}

	~Mesh() { Term(); }

	void Init(BufferResource<VertexData>* vertexResource, IndexBufferResource* indexResource);

	void Term();

	void Dispatch(UINT verticesParam, UINT uniqueVertexIndicesParam, UINT meshletsParam, UINT primitiveIndices);

	void Dispatch(UINT verticesParam, UINT uniqueVertexIndicesParam, UINT meshletsParam, UINT primitiveIndices, UINT cullDataParam, UINT infoParam);


private:

	////////////////////////////////////////////////////////////////////////////////////////////
	// MeshInfo structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct MeshInfo {
		uint32_t meshletCount;
	};

	//=========================================================================================
	// private variables
	//=========================================================================================

	// buffers
	std::unique_ptr<DxObject::BufferResource<VertexData>>               vertices_;
	std::unique_ptr<DxObject::BufferResource<uint32_t>>                 uniqueVertexIndices_;
	std::unique_ptr<DxObject::BufferResource<DirectX::MeshletTriangle>> primitiveIndices_;
	std::unique_ptr<DxObject::BufferResource<DirectX::Meshlet>>         meshlets_;
	std::unique_ptr<DxObject::BufferResource<DirectX::CullData>>        cullDatas_;
	//!< SRV節約のためVirtualAddressでの設定
	
	std::unique_ptr<DxObject::BufferResource<MeshInfo>> info_;

	// config
	static const UINT kAmplificationRound_ = 32; //<! numthreadが32で作られてるため

	//=========================================================================================
	// private methods
	//=========================================================================================

	static UINT DivRoundUp(UINT meshletCount);
	
};

_DXOBJECT_NAMESPACE_END
