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

	//=========================================================================================
	// static private variables
	//=========================================================================================

	static const uint32_t kMaxLine = 1;

	////////////////////////////////////////////////////////////////////////////////////////////
	// PrimitiveMaterial structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct PrimitiveMaterial {
		Vector4f color;
	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// PrimitiveMatrix strtucture
	////////////////////////////////////////////////////////////////////////////////////////////
	struct PrimitiveMatrix {
		Matrix4x4 viewport;
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

	DrawData drawDatas_[DrawType::kCountOfDrawType];

	std::unique_ptr<DxObject::BufferResource<PrimitiveMatrix>> matrix_; //!< cameraのmatrixが入る

};