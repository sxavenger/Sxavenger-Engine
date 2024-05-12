#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
// c++
#include <cstdint>
#include <vector>
#include <memory>

// object
#include <ObjectStructure.h>

// DxObject
#include <DxBufferResource.h>
#include <DxStructuredBuffer.h>


//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------


////////////////////////////////////////////////////////////////////////////////////////////
// Mesh structure
////////////////////////////////////////////////////////////////////////////////////////////
struct ResMeshlet {
	uint32_t vertexOffset;    //!< 頂点番号のオフセット
	uint32_t vertexCount;     //!< 頂点数
	uint32_t primitiveOffset; //!< プリミティブ番号のオフセット
	uint32_t primitiveCount;  //!< プリミティブオフセット
};

struct ResPrimitiveIndex {
	uint32_t index0 : 10;
	uint32_t index1 : 10;
	uint32_t index2 : 10;
	uint32_t reserved : 2;
};

struct ResMesh {
	std::vector<VertexData> vertices;
	std::vector<uint32_t>   indices;
	uint32_t                materialId;

	std::vector<ResMeshlet>        meshlets;
	std::vector<uint32_t>          uniqueVertexIndices;
	std::vector<ResPrimitiveIndex> primitiveIndices;

	void Term() {
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
	}
};

////////////////////////////////////////////////////////////////////////////////////////////
// DxObject namespace
////////////////////////////////////////////////////////////////////////////////////////////
namespace DxObject {



	////////////////////////////////////////////////////////////////////////////////////////////
	// MeshLoader class
	////////////////////////////////////////////////////////////////////////////////////////////
	class MeshLoader {
	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		static void PerseMesh( // test publics
			DxObject::BufferResource<VertexData>* verticesBuffer, DxObject::IndexBufferResource* indicesBuffer,
			ResMesh& dstMesh //!< param[out]
		);

	private:

		//=========================================================================================
		// private variables
		//=========================================================================================
		
		static const uint32_t kMaxVertices   = 64;
		static const uint32_t kMaxPrimitives = 126;

		//=========================================================================================
		// private methods
		//=========================================================================================

		static void PerseMesh(
			DxObject::BufferPtrResource<VertexData>* verticesBuffer, DxObject::IndexBufferResource* indicesBuffer,
			ResMesh& dstMesh //!< param[out]
		);

	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// Mesh class
	////////////////////////////////////////////////////////////////////////////////////////////
	class Mesh {
	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		Mesh(BufferResource<VertexData>* vertexResource, IndexBufferResource* indexResource) {
			Init(vertexResource, indexResource);
		}

		virtual ~Mesh() { Term(); }

		void Init(BufferResource<VertexData>* vertexResource, IndexBufferResource* indexResource);

		void Term();

		void Dispatch(UINT verticesParam, UINT uinqueVertexIndicesParam, UINT primitiveIndices, UINT meshletsParam);


	private:

		////////////////////////////////////////////////////////////////////////////////////////////
		// Buffer structure
		////////////////////////////////////////////////////////////////////////////////////////////
		template <typename T>
		struct Buffer {
			std::unique_ptr<BufferResource<T>> resource;
			std::unique_ptr<StructuredBuffer>  structuredBuffer;

			void Reset() {
				resource.reset();
				structuredBuffer.reset();
			}
		};

		//=========================================================================================
		// private variables
		//=========================================================================================

		// structuredBuffers
		Buffer<VertexData> vertices_;
		Buffer<uint32_t> uniqueVertexIndices_;
		Buffer<ResPrimitiveIndex> primitiveIndices_;
		Buffer<ResMeshlet> meshlets_;
	};

}
