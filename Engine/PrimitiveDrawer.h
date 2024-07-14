#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
// DxObject
#include <DxGraphicsBlob.h>
#include <DxGraphicsPipeline.h>
#include <DxBufferResource.h>

// Geomtoery
#include <Vector4.h>
#include <Vector3.h>

////////////////////////////////////////////////////////////////////////////////////////////
// BasePrimitivePipeline base class
////////////////////////////////////////////////////////////////////////////////////////////
class BasePrimitivePipeline {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	BasePrimitivePipeline() = default;

	~BasePrimitivePipeline() { Term(); }

	virtual void Init() = 0;

	virtual void Term();

	virtual void DrawAll(const D3D12_GPU_VIRTUAL_ADDRESS& camera) = 0;

protected:

	////////////////////////////////////////////////////////////////////////////////////////////
	// PrimitiveInput structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct PrimitiveInput {
		Vector4f position;
		Color4f color;
	};

	//=========================================================================================
	// protected variables
	//=========================================================================================

	static const std::wstring directoryPath_;

	/* graphics */
	std::unique_ptr<DxObject::GraphicsBlob>     grahicsBlob_;
	std::unique_ptr<DxObject::GraphicsPipeline> graphicsPipeline_;

	/* config */
	static const uint32_t kMaxObjectNum_ = 10000;
	uint32_t              objectCount_   = 0;

	/* buffer */
	std::unique_ptr<DxObject::BufferResource<PrimitiveInput>> objectBuffer_;

};

////////////////////////////////////////////////////////////////////////////////////////////
// PrimitivePolygon class
////////////////////////////////////////////////////////////////////////////////////////////
class PrimitiveTriangle
	: public BasePrimitivePipeline {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	PrimitiveTriangle() { Init(); }

	void Init() override;

	void DrawAll(const D3D12_GPU_VIRTUAL_ADDRESS& camera) override;

	void DrawTriangle(const Vector3f& v0, const Vector3f& v1, const Vector3f& v2, const Color4f& color);

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

};

////////////////////////////////////////////////////////////////////////////////////////////
// PrimitiveLine class
////////////////////////////////////////////////////////////////////////////////////////////
class PrimitiveLine
	: public BasePrimitivePipeline {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	PrimitiveLine() { Init(); }

	void Init() override;

	void DrawAll(const D3D12_GPU_VIRTUAL_ADDRESS& camera) override;

	void DrawLine(const Vector3f& v0, const Vector3f& v1, const Color4f& color);

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

};

////////////////////////////////////////////////////////////////////////////////////////////
// PrimitiveDrawer class
////////////////////////////////////////////////////////////////////////////////////////////
class PrimitiveDrawer {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	PrimitiveDrawer() = default;

	void Init();

	void Term();

	static PrimitiveDrawer* GetInstance();

	/* draw methods */

	void DrawTriangle(const Vector3f& v0, const Vector3f& v1, const Vector3f& v2, const Color4f& color);

	void DrawLine(const Vector3f& v0, const Vector3f& v1, const Color4f& color);

	void DrawSphere(const Vector3f& center, float radius, const uint32_t kSubdivision, const Color4f& color);

	void DrawAll3D();


private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	std::unique_ptr<PrimitiveTriangle> triangle_;
	std::unique_ptr<PrimitiveLine>     line_;

};