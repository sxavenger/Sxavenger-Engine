#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* content
#include "../InputAssembler/TriangleInputAssembler.h"

//* engine
#include <Engine/System/DirectX/DxObject/DxDimensionBuffer.h>
#include <Engine/System/DirectX/DxObject/DxBindBuffer.h>
#include <Engine/System/DirectX/DxrObject/DxrAccelerationStructure.h>
#include <Engine/System/DirectX/DirectXContext.h>

//* lib
#include <Lib/Traits.h>
#include <Lib/Geometry/Vector2.h>
#include <Lib/Geometry/Vector3.h>
#include <Lib/Geometry/Vector4.h>

//* DirectX
#include <DirectXMesh.h>

////////////////////////////////////////////////////////////////////////////////////////////
// MeshVertexData structure
////////////////////////////////////////////////////////////////////////////////////////////
struct MeshVertexData {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	void Init();

	//=========================================================================================
	// public variables
	//=========================================================================================

	Vector4f position;
	Vector2f texcoord;
	Vector3f normal;

	//!< for tangent space
	Vector3f tangent;
	Vector3f bitangent;

};

////////////////////////////////////////////////////////////////////////////////////////////
// InputMesh class
////////////////////////////////////////////////////////////////////////////////////////////
class InputMesh
	: public TriangleInputAssembler<MeshVertexData> {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// Meshlet structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct Meshlet {
	public:

		////////////////////////////////////////////////////////////////////////////////////////////
		// MeshletInfo structure
		////////////////////////////////////////////////////////////////////////////////////////////
		struct MeshletInfo {
		public:

			//=========================================================================================
			// public variables
			//=========================================================================================

			uint32_t meshletCount;

		};

	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		void CreateMeshlet(const TriangleInputAssembler<MeshVertexData>* ia);

		void Dispatch(const DirectXThreadContext* context, UINT instanceCount = 1) const;

		//=========================================================================================
		// public variables
		//=========================================================================================

		//* info
		std::unique_ptr<DxObject::DimensionBuffer<MeshletInfo>> info;

		//* meshlet
		std::unique_ptr<DxObject::DimensionBuffer<uint32_t>>                 uniqueVertexIndices;
		std::unique_ptr<DxObject::DimensionBuffer<DirectX::MeshletTriangle>> primitiveIndices;
		std::unique_ptr<DxObject::DimensionBuffer<DirectX::Meshlet>>         meshlets;

		//* cull data
		std::unique_ptr<DxObject::DimensionBuffer<DirectX::CullData>> cullDatas;

		//* parameter
		static constexpr uint32_t kMaxVertices_        = 64;
		static constexpr uint32_t kMaxPrimitives_      = 128;
		static constexpr UINT kAmplificationNumthread_ = 32;

	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// MeshBottomLevelAS structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct MeshBottomLevelAS {
	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		void CreateBottomLevelAS(const TriangleInputAssembler<MeshVertexData>* ia, const DirectXThreadContext* context);

		//=========================================================================================
		// public variables
		//=========================================================================================

		std::unique_ptr<DxrObject::BottomLevelAS> bottomLevelAS;

	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	InputMesh()  = default;
	~InputMesh() = default;

	void CreateMeshlet();

	void CreateBottomLevelAS(const DirectXThreadContext* context);

	//* getter *//

	const Meshlet& GetMeshlet() const;

	const MeshBottomLevelAS& GetBottomLevelAS() const;

	_DELETE_COPY(InputMesh)
	_DEFAULT_MOVE(InputMesh)

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	std::optional<Meshlet>           meshlet_;
	std::optional<MeshBottomLevelAS> bottomLevelAS_;

};


