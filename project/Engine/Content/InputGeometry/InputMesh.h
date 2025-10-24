#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* content
#include "../InputAssembler/TriangleInputAssembler.h"

//* engine
#include <Engine/System/DirectX/DxObject/DxDimensionBuffer.h>
#include <Engine/System/DirectX/DxObject/DxConstantBuffer.h>
#include <Engine/System/DirectX/DxObject/DxBindBuffer.h>
#include <Engine/System/DirectX/DxrObject/DxrAccelerationStructure.h>
#include <Engine/System/DirectX/Context/DirectXQueueContext.h>

//* lib
#include <Lib/Traits.h>
#include <Lib/Geometry/Vector2.h>
#include <Lib/Geometry/Vector3.h>
#include <Lib/Geometry/Vector4.h>

//* externals
#include <externals/DirectXMesh/DirectXMesh.h>

////////////////////////////////////////////////////////////////////////////////////////////
// MeshVertexData structure
////////////////////////////////////////////////////////////////////////////////////////////
//! @brief メッシュ頂点データ構造体
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
//! @brief メッシュ入力ジオメトリクラス
class InputMesh
	: public TriangleInputAssembler<MeshVertexData> {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// Meshlet structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct Meshlet {
	public:

		////////////////////////////////////////////////////////////////////////////////////////////
		// MeshletTriangle structure
		////////////////////////////////////////////////////////////////////////////////////////////
		struct MeshletTriangle {
			uint32_t i0      : 10;
			uint32_t i1      : 10;
			uint32_t i2      : 10;
			//!< 1頂点当たり10bit(uint8_t)で表現
		};

		////////////////////////////////////////////////////////////////////////////////////////////
		// MeshletData structure
		////////////////////////////////////////////////////////////////////////////////////////////
		struct MeshletData {
			uint32_t vertexOffset;
			uint32_t triangleOffset;
			uint32_t vertexCount;
			uint32_t triangleCount;
			//!< meshopt_Meshlet と同じレイアウトで作成する
		};

	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		void CreateMeshlet(const TriangleInputAssembler<MeshVertexData>* ia);

		void Dispatch(const DirectXQueueContext* context, UINT instanceCount = 1) const;

		//=========================================================================================
		// public variables
		//=========================================================================================

		uint32_t meshletCount;

		//* meshlet
		std::unique_ptr<DxObject::DimensionBuffer<uint32_t>>        uniqueVertexIndices;
		std::unique_ptr<DxObject::DimensionBuffer<MeshletTriangle>> primitiveIndices;
		std::unique_ptr<DxObject::DimensionBuffer<MeshletData>>     meshlets;

		// TODO: culldataを生成.

		//* parameter
		static constexpr size_t kMaxVertices_          = 64;
		static constexpr size_t kMaxPrimitives_        = 128;
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

		void CreateBottomLevelAS(const TriangleInputAssembler<MeshVertexData>* ia, const DirectXQueueContext* context);

		const DxrObject::BottomLevelAS* Get() const { return bottomLevelAS.get(); }

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

	bool IsCreateMeshlet() const { return meshlet_.has_value(); }

	void CreateBottomLevelAS(const DirectXQueueContext* context);

	bool IsCreateBottomLevelAS() const { return bottomLevelAS_.has_value(); }

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


