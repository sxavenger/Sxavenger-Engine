#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* _DXOBJECT
#include <DxShaderBlob.h>
#include <DxGraphicsPipeline.h>
#include <DxBufferResource.h>

//* c++
#include <memory>

//* Geomtery
#include <Vector4.h>
#include <Vector3.h>
#include <Vector2.h>

//* Texture
#include <Texture.h>

////////////////////////////////////////////////////////////////////////////////////////////
// PrimitiveDimensionType enum
////////////////////////////////////////////////////////////////////////////////////////////
enum PrimitiveDimensionType {
	kPrimitive_3D,
	kPrimitive_2D,

	kCountOfPrimitiveDimensionType

};

////////////////////////////////////////////////////////////////////////////////////////////
// PrimitiveObjectType enum
////////////////////////////////////////////////////////////////////////////////////////////
enum PrimitiveObjectType {
	kPrimitive_Triangle,
	kPrimitive_Line,

	kCountOfPrimitiveObjectType
};

////////////////////////////////////////////////////////////////////////////////////////////
// BasePrimitive3D base class
////////////////////////////////////////////////////////////////////////////////////////////
class BasePrimitive {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	virtual ~BasePrimitive() = default;

	void ResetObjectCount();

protected:

	////////////////////////////////////////////////////////////////////////////////////////////
	// PrimitiveInput structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct PrimitiveInput {
		Vector4f position;
		Vector2f texcoord;
		Color4f  color;
	};

	//=========================================================================================
	// protected variables
	//=========================================================================================

	//* buffer *//

	std::unique_ptr<DxObject::BufferResource<PrimitiveInput>> vertex_;
	//!< todo: InputAssemblurの対応
	
	//* data *//

	uint32_t objectCount_;
	
	//* parameter *//

	PrimitiveDimensionType dimension_ = kCountOfPrimitiveDimensionType;

	//=========================================================================================
	// protected methods
	//=========================================================================================

	void CreateVertexBuffer(uint32_t vertexCount);

	static Vector4f ToProjection(const Vector2f& v);

};

////////////////////////////////////////////////////////////////////////////////////////////
// PrimitiveLine class
////////////////////////////////////////////////////////////////////////////////////////////
class PrimitiveLine
	: public BasePrimitive {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	PrimitiveLine(PrimitiveDimensionType dimension) { Init(dimension); }

	void Init(PrimitiveDimensionType dimension);

	void Term();

	//* primitive option *//

	void DrawLine(const Vector3f& v0, const Vector3f& v1, const Color4f& color);
	void DrawLine(const Vector2f& v0, const Vector2f& v1, const Color4f& color);

	//* methods *//

	void DrawAll(Texture* sampleTexture);

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* config *//

	static const uint32_t kMaxLineNum_      = 1024;
	static const uint32_t kObjectVertexNum_ = 2;

};

////////////////////////////////////////////////////////////////////////////////////////////
// PrimitiveTriangle class
////////////////////////////////////////////////////////////////////////////////////////////
class PrimitiveTriangle
	: public BasePrimitive {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	PrimitiveTriangle(PrimitiveDimensionType dimension) { Init(dimension); }

	void Init(PrimitiveDimensionType dimension);

	void Term();

	//* primitive option *//

	void DrawTriangle(const Vector3f& v0, const Vector3f& v1, const Vector3f& v2, const Color4f& color);
	void DrawTriangle(const Vector2f& v0, const Vector2f& v1, const Vector2f& v2, const Color4f& color);

	//* methods *//

	void DrawAll(Texture* sampleTexture);

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* config *//

	static const uint32_t kMaxLineNum_      = 1024;
	static const uint32_t kObjectVertexNum_ = 3;

};

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

	//* primitive option *//

	void DrawLine(const Vector3f& v0, const Vector3f& v1, const Color4f& color);
	void DrawLine(const Vector2f& v0, const Vector2f& v1, const Color4f& color);

	void DrawTriangle(const Vector3f& v0, const Vector3f& v1, const Vector3f& v2, const Color4f& color);
	void DrawTriangle(const Vector2f& v0, const Vector2f& v1, const Vector2f& v2, const Color4f& color);

	//* draw methods *//

	void DrawAll3D();

	void DrawAll2D();

	void Reset();

	//* singleton *//

	static PrimitiveDrawer* GetInstance();
	PrimitiveDrawer()  = default;
	~PrimitiveDrawer() = default;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* parameter *//

	static const std::wstring directory_;

	//* pipeline *//

	std::unique_ptr<DxObject::GraphicsBlob>     blobs_[kCountOfPrimitiveDimensionType];
	std::unique_ptr<DxObject::GraphicsPipeline> pipelines_[kCountOfPrimitiveDimensionType][kCountOfPrimitiveObjectType];

	//* texture *//

	std::unique_ptr<Texture> texture_;

	//* primitives *//

	std::unique_ptr<PrimitiveLine>     lines_[kCountOfPrimitiveDimensionType];
	std::unique_ptr<PrimitiveTriangle> triangles_[kCountOfPrimitiveDimensionType];

	//=========================================================================================
	// private methods
	//=========================================================================================

	void CreatePipeline();
	void CreateSampleTexture();


};