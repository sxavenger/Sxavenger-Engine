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
		std::unique_ptr<DxObject::BufferResource<Vector4f>>           vertices_;
		std::unique_ptr<DxObject::IndexBufferResource>                indices_;
		std::unique_ptr<DxObject::BufferResource<PrimitiveMaterial>>  materials_;

		void Reset() {
			vertices_.reset();
			indices_.reset();
			materials_.reset();
		}
	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// DrawType enum
	////////////////////////////////////////////////////////////////////////////////////////////
	enum DrawType {
		Line,

		kCountOfDrawType
	};

	//=========================================================================================
	// private variables
	//=========================================================================================

	static const uint32_t kMaxLine = 1;

	DrawData drawDatas_[DrawType::kCountOfDrawType];

	// MyEngineからカメラを持ってくるとする

};