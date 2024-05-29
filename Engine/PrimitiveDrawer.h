#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
// DxObject
#include <DxBufferResource.h>

// camera

// Geometry
#include <Vector4.h>
#include <Matrix4x4.h>

// c++
#include <memory>
#include <array>

// Object
#include <ObjectStructure.h>

#include <MyEngine.h>


////////////////////////////////////////////////////////////////////////////////////////////
// PrimitiveDrawer class
////////////////////////////////////////////////////////////////////////////////////////////
class PrimitiveDrawer {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	void Init();

	void Term();

	void AllDraw();

	void DrawTriangle(const Vector3f& w1, const Vector3f& w2, const Vector3f& w3, uint32_t color);

	static PrimitiveDrawer* GetInstance();

private:

	////////////////////////////////////////////////////////////////////////////////////////////
	// PrimitiveMaterial structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct PrimitiveMaterial {
		Vector4f color;
	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// DrawData structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct DrawData {
		std::unique_ptr<DxObject::BufferResource<VertexData>>         vertices_;
		std::unique_ptr<DxObject::BufferResource<PrimitiveMaterial>>  materials_;

		void Reset() {
			vertices_.reset();
			materials_.reset();
		}

		void Create(uint32_t maxMeshNum, uint32_t vertexNum) {
			vertices_  = std::make_unique<DxObject::BufferResource<VertexData>>(MyEngine::GetDevicesObj(), maxMeshNum * vertexNum);
			materials_ = std::make_unique<DxObject::BufferResource<PrimitiveMaterial>>(MyEngine::GetDevicesObj(), maxMeshNum);
		}
	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// DrawType enum
	////////////////////////////////////////////////////////////////////////////////////////////
	enum DrawType {
		Line,
		Triangle,

		kCountOfDrawType
	};

	//=========================================================================================
	// private variables
	//=========================================================================================

	static const uint32_t kMaxLineNum_     = 1;
	static const uint32_t kMaxTriangleNum_ = 1;

	uint32_t triangleCount_ = 0;

	DrawData drawDatas_[DrawType::kCountOfDrawType];

	// MyEngineからカメラを持ってくるとする

};