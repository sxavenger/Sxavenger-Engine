#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include <cstdint>
#include <vector>

#include <ObjectStructure.h>

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
};

