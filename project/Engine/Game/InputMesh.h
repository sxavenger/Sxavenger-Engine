#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* base
#include "InputAssembler.h"

//* DXROBJECT
#include <Engine/System/DxrObject/DxrAccelerationStructure.h>

//* DirectX
#include <DirectXMesh.h>

//* object
#include <Lib/ObjectStructure.h>

////////////////////////////////////////////////////////////////////////////////////////////
// InputMesh class
////////////////////////////////////////////////////////////////////////////////////////////
class InputMesh
	: public InputAssembler<VertexData> { //!< 新しくInputAssemblerをこれにするかどうか
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	InputMesh()  = default;
	~InputMesh() = default;

	//* mesh shader option *//

	void CreateMeshlet();

	void Dispatch(UINT verticesParam, UINT uniqueVertexIndicesParam, UINT meshletsParam, UINT primitiveIndices, UINT cullDataParam, UINT meshInfoParam) const;

	bool IsCreateMeshlet() const { return isCreateMeshlet_; }

	//* raytracing option *//

	void CreateBLAS();

	DxrObject::BottomLevelAS* GetBLAS() const;

	bool IsCreateBLAS() const { return isCreateBLAS_; }

	//* operator *//

	InputMesh(InputMesh&& other) noexcept = default;
	InputMesh& operator=(InputMesh&& other) noexcept = default;

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
	//* buffers
	std::unique_ptr<DxObject::BufferResource<uint32_t>>                 uniqueVertexIndices_;
	std::unique_ptr<DxObject::BufferResource<DirectX::MeshletTriangle>> primitiveIndices_;
	std::unique_ptr<DxObject::BufferResource<DirectX::Meshlet>>         meshlets_;
	std::unique_ptr<DxObject::BufferResource<DirectX::CullData>>        cullDatas_;
	std::unique_ptr<DxObject::BufferResource<MeshInfo>>                 meshInfo_;

	//* configs
	static const uint32_t kMaxVertices_        = 64;
	static const uint32_t kMaxPrimitives_      = 128;
	static const UINT kAmplificationNumthread_ = 32;

	//* raytracing *//

	std::unique_ptr<DxrObject::BottomLevelAS> blas_;
	
	//* info *//

	bool isCreateMeshlet_ = false;
	bool isCreateBLAS_    = false;
};