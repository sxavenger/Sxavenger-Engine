#include "PrimitiveDrawer.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include <MyEngine.h>

//-----------------------------------------------------------------------------------------
// using
//-----------------------------------------------------------------------------------------
using namespace DxObject;

//=========================================================================================
// static variables
//=========================================================================================

const std::wstring BasePrimitivePipeline::directoryPath_ = L"Primitive/";

////////////////////////////////////////////////////////////////////////////////////////////
// BasePrimitivePipeline base class methods
////////////////////////////////////////////////////////////////////////////////////////////

void BasePrimitivePipeline::Term() {
}

////////////////////////////////////////////////////////////////////////////////////////////
// PrimitiveTriangle class methods
////////////////////////////////////////////////////////////////////////////////////////////

void PrimitiveTriangle::Init() {

	/* graphics */
	grahicsBlob_ = std::make_unique<GraphicsBlob>();
	grahicsBlob_->Create(directoryPath_ + L"Primitive.VS.hlsl", GRAPHICS_VERTEX);
	grahicsBlob_->Create(directoryPath_ + L"Primitive.PS.hlsl", GRAPHICS_PIXEL);

	GraphicRootSignatureDesc desc;
	desc.Resize(1, 0);
	desc.SetCBV(0, VISIBILITY_VERTEX, 0);

	GraphicsInputLayoutDesc inputDesc;
	inputDesc.SetElement("POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT);
	inputDesc.SetElement("COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT);

	graphicsPipeline_ = std::make_unique<GraphicsPipeline>();
	graphicsPipeline_->CreateRootSignature(MyEngine::GetDevicesObj(), desc);
	graphicsPipeline_->CreatePipeline(MyEngine::GetDevicesObj(), grahicsBlob_.get(), inputDesc, kBlendModeNormal);

	/* buffer */
	objectBuffer_ = std::make_unique<DxObject::BufferResource<PrimitiveInput>>(MyEngine::GetDevicesObj(), kMaxObjectNum_ * 3);

}

void PrimitiveTriangle::DrawTriangle(const Vector3f& v0, const Vector3f& v1, const Vector3f& v2, const Vector4f& color) {

	assert(objectCount_ < kMaxObjectNum_); //!< 描画限界

	objectBuffer_->operator[](objectCount_ + 0).position = Vector4f(v0.x, v0.y, v0.z);
	objectBuffer_->operator[](objectCount_ + 0).color    = color;
	objectBuffer_->operator[](objectCount_ + 1).position = Vector4f(v1.x, v1.y, v1.z);
	objectBuffer_->operator[](objectCount_ + 1).color    = color;
	objectBuffer_->operator[](objectCount_ + 2).position = Vector4f(v2.x, v2.y, v2.z);
	objectBuffer_->operator[](objectCount_ + 2).color    = color;

	objectCount_++;

}

void PrimitiveTriangle::DrawAll(const D3D12_GPU_VIRTUAL_ADDRESS& camera) {

	// commandListの取得
	auto commandList = MyEngine::GetCommandList();

	graphicsPipeline_->SetPipeline(commandList);

	D3D12_VERTEX_BUFFER_VIEW bufferView = objectBuffer_->GetVertexBufferView();
	commandList->IASetVertexBuffers(0, 1, &bufferView);
	commandList->SetGraphicsRootConstantBufferView(0, camera);

	commandList->DrawInstanced(objectCount_ * 3, 1, 0, 0);

	objectCount_ = 0;

}

////////////////////////////////////////////////////////////////////////////////////////////
// PrimitiveLine class methods
////////////////////////////////////////////////////////////////////////////////////////////

void PrimitiveLine::Init() {

	/* graphics */
	grahicsBlob_ = std::make_unique<GraphicsBlob>();
	grahicsBlob_->Create(directoryPath_ + L"Primitive.VS.hlsl", GRAPHICS_VERTEX);
	grahicsBlob_->Create(directoryPath_ + L"Primitive.PS.hlsl", GRAPHICS_PIXEL);

	GraphicRootSignatureDesc desc;
	desc.Resize(1, 0);
	desc.SetCBV(0, VISIBILITY_VERTEX, 0);

	GraphicsInputLayoutDesc inputDesc;
	inputDesc.SetElement("POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT);
	inputDesc.SetElement("COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT);

	graphicsPipeline_ = std::make_unique<GraphicsPipeline>();
	graphicsPipeline_->CreateRootSignature(MyEngine::GetDevicesObj(), desc);
	graphicsPipeline_->CreatePipeline(MyEngine::GetDevicesObj(), grahicsBlob_.get(), inputDesc, kBlendModeNormal, D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE);

	/* buffer */
	objectBuffer_ = std::make_unique<DxObject::BufferResource<PrimitiveInput>>(MyEngine::GetDevicesObj(), kMaxObjectNum_ * 2);

}

void PrimitiveLine::DrawAll(const D3D12_GPU_VIRTUAL_ADDRESS& camera) {

	// commandListの取得
	auto commandList = MyEngine::GetCommandList();

	graphicsPipeline_->SetPipeline(commandList);

	D3D12_VERTEX_BUFFER_VIEW bufferView = objectBuffer_->GetVertexBufferView();
	commandList->IASetVertexBuffers(0, 1, &bufferView);
	commandList->SetGraphicsRootConstantBufferView(0, camera);

	commandList->DrawInstanced(objectCount_ * 2, 1, 0, 0);

	objectCount_ = 0;

}

void PrimitiveLine::DrawLine(const Vector3f& v0, const Vector3f& v1, const Vector4f& color) {

	assert(objectCount_ < kMaxObjectNum_); //!< 描画限界

	objectBuffer_->operator[](objectCount_ * 2 + 0).position = Vector4f(v0.x, v0.y, v0.z);
	objectBuffer_->operator[](objectCount_ * 2 + 0).color = color;
	objectBuffer_->operator[](objectCount_ * 2 + 1).position = Vector4f(v1.x, v1.y, v1.z);
	objectBuffer_->operator[](objectCount_ * 2 + 1).color = color;

	objectCount_++;

}


////////////////////////////////////////////////////////////////////////////////////////////
// PrimitiveDrawer class methods
////////////////////////////////////////////////////////////////////////////////////////////

void PrimitiveDrawer::Init() {
	triangle_ = std::make_unique<PrimitiveTriangle>();
	line_     = std::make_unique<PrimitiveLine>();
}

void PrimitiveDrawer::Term() {
}

PrimitiveDrawer* PrimitiveDrawer::GetInstance() {
	static PrimitiveDrawer instance;
	return &instance;
}

void PrimitiveDrawer::DrawTriangle(const Vector3f& v0, const Vector3f& v1, const Vector3f& v2, const Vector4f& color) {
	triangle_->DrawTriangle(v0, v1, v2, color);
}

void PrimitiveDrawer::DrawLine(const Vector3f& v0, const Vector3f& v1, const Vector4f& color) {
	line_->DrawLine(v0, v1, color);
}

void PrimitiveDrawer::DrawAll3D() {
	triangle_->DrawAll(MyEngine::camera3D_->GetGPUVirtualAddress());
	line_->DrawAll(MyEngine::camera3D_->GetGPUVirtualAddress());
}