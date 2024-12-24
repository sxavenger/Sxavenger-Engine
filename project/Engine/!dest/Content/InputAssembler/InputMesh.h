#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* base
#include "InputAssembler.h"

//* DirectX
#include <DirectXMesh.h>

//* engine
#include <Engine/System/DirectX/DxObject/DxDimensionBuffer.h>
//#include <Engine/System/DxrObject/DxrAccelerationStructure.h>

//* lib
#include <Lib/VertexStructure.h>

////////////////////////////////////////////////////////////////////////////////////////////
// InputMesh class
////////////////////////////////////////////////////////////////////////////////////////////
class InputMesh
	: public InputAssembler<MeshVertexData> {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	InputMesh() = default;
	~InputMesh() = default;

	//* meshlet option *//

	void CreateMeshlet();

	void Dispatch( //!< TODO:
		UINT verticesParam, UINT uniqueVertexIndicesParam, UINT meshletsParam, UINT primitiveIndices, UINT cullDataParam, UINT meshInfoParam,
		UINT instanceCount = 1
	) const;

	void Dispatch(

	) const;

	bool IsCreateMeshlet() const { return isCreateMeshlet_; }

	//* operator *//

	InputMesh(InputMesh&)            = delete;
	InputMesh& operator=(InputMesh&) = delete;

	InputMesh(InputMesh&&) noexcept            = default;
	InputMesh& operator=(InputMesh&&) noexcept = default;

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

	//* mesh shader *//
	bool isCreateMeshlet_ = false;

	//* buffers
	std::unique_ptr<DxObject::DimensionBuffer<uint32_t>>                 uniqueVertexIndices_;
	std::unique_ptr<DxObject::DimensionBuffer<DirectX::MeshletTriangle>> primitiveIndices_;
	std::unique_ptr<DxObject::DimensionBuffer<DirectX::Meshlet>>         meshlets_;
	std::unique_ptr<DxObject::DimensionBuffer<DirectX::CullData>>        cullDatas_;
	std::unique_ptr<DxObject::DimensionBuffer<MeshInfo>>                 meshInfo_;

	//* configs
	static const uint32_t kMaxVertices_        = 64;
	static const uint32_t kMaxPrimitives_      = 128;
	static const UINT kAmplificationNumthread_ = 32;

	//* raytracing *//
	// TODO:

};