#include "SxavengerGraphics.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------


////////////////////////////////////////////////////////////////////////////////////////////
// namespace
////////////////////////////////////////////////////////////////////////////////////////////
namespace {
	PrimitiveDrawer* sPrimitiveDrawer = nullptr;
	DefaultGraphics* sDefaultGraphics = nullptr;
}

//=========================================================================================
// static variables
//=========================================================================================

//* public

Camera3D* SxavengerGraphics::camera3D = nullptr;

//* private

std::unique_ptr<LightBuffer> SxavengerGraphics::lightBuffer_;

////////////////////////////////////////////////////////////////////////////////////////////
// SxavengerRendering class methods
////////////////////////////////////////////////////////////////////////////////////////////

void SxavengerGraphics::Init() {

	sPrimitiveDrawer = PrimitiveDrawer::GetInstance();
	sPrimitiveDrawer->Init();

	sDefaultGraphics = DefaultGraphics::GetInstance();
	sDefaultGraphics->Init();

	lightBuffer_ = std::make_unique<LightBuffer>();
	lightBuffer_->Init();
	lightBuffer_->SetThisAttribute();
}

void SxavengerGraphics::Term() {
	lightBuffer_.reset();

	sDefaultGraphics->Term();
	sDefaultGraphics = nullptr;

	sPrimitiveDrawer->Term();
	sPrimitiveDrawer = nullptr;
}

void SxavengerGraphics::DrawLine(const Vector3f& v0, const Vector3f& v1, const Color4f& color) {
	sPrimitiveDrawer->DrawLine(v0, v1, color);
}

void SxavengerGraphics::DrawLine(const Vector2f& v0, const Vector2f& v1, const Color4f& color) {
	sPrimitiveDrawer->DrawLine(v0, v1, color);
}

void SxavengerGraphics::DrawTriangle(const Vector3f& v0, const Vector3f& v1, const Vector3f& v2, const Color4f& color) {
	sPrimitiveDrawer->DrawTriangle(v0, v1, v2, color);
}

void SxavengerGraphics::DrawTriangle(const Vector2f& v0, const Vector2f& v1, const Vector2f& v2, const Color4f& color) {
	sPrimitiveDrawer->DrawTriangle(v0, v1, v2, color);
}

void SxavengerGraphics::DrawAll3D() {
	sPrimitiveDrawer->DrawAll3D();
}

void SxavengerGraphics::DrawAll2D() {
	sPrimitiveDrawer->DrawAll2D();
}

void SxavengerGraphics::ResetPrimitives() {
	sPrimitiveDrawer->Reset();
}

PrimitiveDrawer* SxavengerGraphics::GetPrimitiveDrawer() {
	return sPrimitiveDrawer;
}

void SxavengerGraphics::DrawDefaultTexture(
	const InputAssembler<VertexData>& ia,
	const BaseTransformBuffer& transform, const ObjectMaterialBuffer& material,
	const D3D12_GPU_DESCRIPTOR_HANDLE& texture) {

	sDefaultGraphics->DrawDefaultTexture(ia, transform, material, texture);
}

void SxavengerGraphics::DrawDefaultLightingTexture(
	const InputAssembler<VertexData>& ia,
	const BaseTransformBuffer& transform, const ObjectMaterialBuffer& material,
	const D3D12_GPU_DESCRIPTOR_HANDLE& texture) {

	sDefaultGraphics->DrawDefaultLightingTexture(ia, transform, material, texture);
}

DefaultGraphics* SxavengerGraphics::GetDefaultGraphics() {
	return sDefaultGraphics;
}

const D3D12_GPU_VIRTUAL_ADDRESS SxavengerGraphics::GetLightBufferAddress() {
	return lightBuffer_->GetGPUVirtualAddress();
}
