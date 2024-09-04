#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* Graphics
#include <PrimitiveDrawer.h>
#include <DefaultGraphics.h>
#include <Light.h>

//* Camera
#include <Camera3D.h>
#include <Camera2D.h>

//* c++
#include <memory>

////////////////////////////////////////////////////////////////////////////////////////////
// SxavengerGraphics class
////////////////////////////////////////////////////////////////////////////////////////////
class SxavengerGraphics {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	static void Init();

	static void Term();

	//-----------------------------------------------------------------------------------------
	// PrimitiveDrawer option
	//-----------------------------------------------------------------------------------------

	static void DrawLine(const Vector3f& v0, const Vector3f& v1, const Color4f& color);
	static void DrawLine(const Vector2f& v0, const Vector2f& v1, const Color4f& color);

	static void DrawTriangle(const Vector3f& v0, const Vector3f& v1, const Vector3f& v2, const Color4f& color);
	static void DrawTriangle(const Vector2f& v0, const Vector2f& v1, const Vector2f& v2, const Color4f& color);

	static void DrawAll3D();
	static void DrawAll2D();

	static void ResetPrimitives();

	static PrimitiveDrawer* GetPrimitiveDrawer();

	//-----------------------------------------------------------------------------------------
	// Graphics option
	//-----------------------------------------------------------------------------------------

	static void DrawDefaultTexture(
		const InputAssembler<VertexData>& ia,
		const BaseTransformBuffer& transform, const ObjectMaterialBuffer& material,
		const D3D12_GPU_DESCRIPTOR_HANDLE& texture
	);

	static void DrawDefaultLightingTexture(
		const InputAssembler<VertexData>& ia,
		const BaseTransformBuffer& transform, const ObjectMaterialBuffer& material,
		const D3D12_GPU_DESCRIPTOR_HANDLE& texture
	);

	static DefaultGraphics* GetDefaultGraphics();

	//-----------------------------------------------------------------------------------------
	// light buffer option
	//-----------------------------------------------------------------------------------------

	static const D3D12_GPU_VIRTUAL_ADDRESS GetLightBufferAddress();

	//=========================================================================================
	// public variables
	//=========================================================================================

	static Camera3D* camera3D;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	static std::unique_ptr<LightBuffer> lightBuffer_;

};