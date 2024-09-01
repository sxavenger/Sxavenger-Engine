#include "PrimitiveDrawer.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include <Sxavenger.h>

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

void BasePrimitivePipeline::ResetObjectCount() {
	objectCount_ = 0;
}

////////////////////////////////////////////////////////////////////////////////////////////
// PrimitiveTriangle class methods
////////////////////////////////////////////////////////////////////////////////////////////

void PrimitiveTriangle::Init() {

	/* graphics */
	grahicsBlob_ = std::make_unique<GraphicsBlob>();
	grahicsBlob_->Create(directoryPath_ + L"Primitive.VS.hlsl", GRAPHICS_VERTEX);
	grahicsBlob_->Create(directoryPath_ + L"Primitive.PS.hlsl", GRAPHICS_PIXEL);

	GraphicsRootSignatureDesc desc;
	desc.SetCBV(0, VISIBILITY_VERTEX, 0);

	GraphicsPipelineDesc pipelineDesc;
	pipelineDesc.CreateDefaultDesc();

	pipelineDesc.elements.clear(); //!< elementの代入し直し
	pipelineDesc.SetElement("POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT);
	pipelineDesc.SetElement("COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT);

	graphicsPipeline_ = std::make_unique<GraphicsPipeline>();
	graphicsPipeline_->CreateRootSignature(Sxavenger::GetDevicesObj(), desc);
	graphicsPipeline_->CreatePipeline(Sxavenger::GetDevicesObj(), grahicsBlob_.get(), pipelineDesc);

	/* buffer */
	objectBuffer_ = std::make_unique<DxObject::BufferResource<PrimitiveInput>>(Sxavenger::GetDevicesObj(), kMaxObjectNum_ * 3);

}

void PrimitiveTriangle::DrawTriangle(const Vector3f& v0, const Vector3f& v1, const Vector3f& v2, const Color4f& color) {

	Assert(objectCount_ < kMaxObjectNum_); //!< 描画限界

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
	auto commandList = Sxavenger::GetCommandList();

	graphicsPipeline_->SetPipeline(commandList);

	D3D12_VERTEX_BUFFER_VIEW bufferView = objectBuffer_->GetVertexBufferView();
	commandList->IASetVertexBuffers(0, 1, &bufferView);
	commandList->SetGraphicsRootConstantBufferView(0, camera);

	commandList->DrawInstanced(objectCount_ * 3, 1, 0, 0);

}

////////////////////////////////////////////////////////////////////////////////////////////
// PrimitiveLine class methods
////////////////////////////////////////////////////////////////////////////////////////////

void PrimitiveLine::Init() {

	/* graphics */
	grahicsBlob_ = std::make_unique<GraphicsBlob>();
	grahicsBlob_->Create(directoryPath_ + L"Primitive.VS.hlsl", GRAPHICS_VERTEX);
	grahicsBlob_->Create(directoryPath_ + L"Primitive.PS.hlsl", GRAPHICS_PIXEL);

	GraphicsRootSignatureDesc desc;
	desc.SetCBV(0, VISIBILITY_VERTEX, 0);

	GraphicsPipelineDesc pipelineDesc;
	pipelineDesc.CreateDefaultDesc();

	pipelineDesc.elements.clear(); //!< elementの代入し直し
	pipelineDesc.SetElement("POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT);
	pipelineDesc.SetElement("COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT);

	pipelineDesc.SetRasterizer(D3D12_CULL_MODE_NONE, D3D12_FILL_MODE_SOLID);

	pipelineDesc.SetPrimitive(PRIMITIVE_LINE);

	graphicsPipeline_ = std::make_unique<GraphicsPipeline>();
	graphicsPipeline_->CreateRootSignature(Sxavenger::GetDevicesObj(), desc);
	graphicsPipeline_->CreatePipeline(Sxavenger::GetDevicesObj(), grahicsBlob_.get(), pipelineDesc);

	/* buffer */
	objectBuffer_ = std::make_unique<DxObject::BufferResource<PrimitiveInput>>(Sxavenger::GetDevicesObj(), kMaxObjectNum_ * 2);

}

void PrimitiveLine::DrawAll(const D3D12_GPU_VIRTUAL_ADDRESS& camera) {

	// commandListの取得
	auto commandList = Sxavenger::GetCommandList();

	graphicsPipeline_->SetPipeline(commandList);

	D3D12_VERTEX_BUFFER_VIEW bufferView = objectBuffer_->GetVertexBufferView();
	commandList->IASetVertexBuffers(0, 1, &bufferView);
	commandList->SetGraphicsRootConstantBufferView(0, camera);

	commandList->DrawInstanced(objectCount_ * 2, 1, 0, 0);

}

void PrimitiveLine::DrawLine(const Vector3f& v0, const Vector3f& v1, const Color4f& color) {

	Assert(objectCount_ < kMaxObjectNum_); //!< 描画限界

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

void PrimitiveDrawer::DrawTriangle(const Vector3f& v0, const Vector3f& v1, const Vector3f& v2, const Color4f& color) {
	triangle_->DrawTriangle(v0, v1, v2, color);
}

void PrimitiveDrawer::DrawLine(const Vector3f& v0, const Vector3f& v1, const Color4f& color) {
	line_->DrawLine(v0, v1, color);
}

void PrimitiveDrawer::DrawSphere(const Vector3f& center, float radius, const uint32_t kSubdivision, const Color4f& color) {
	const float kLatEvery = (std::numbers::pi_v<float> * 2.0f) / static_cast<float>(kSubdivision); // horizontal
	const float kLonEvery = std::numbers::pi_v<float> / static_cast<float>(kSubdivision); // vertical

	enum PointName {
		A, B, C,
	};

	for (uint32_t latIndex = 0; latIndex < kSubdivision; ++latIndex) {
		float lat = -std::numbers::pi_v<float> / 2.0f + kLatEvery * latIndex;

		for (uint32_t lonIndex = 0; lonIndex < kSubdivision; ++lonIndex) {
			float lon = lonIndex * kLonEvery;

			Vector3f point[3];

			point[A] = { std::cos(lat) * std::cos(lon), std::sin(lat), std::cos(lat) * std::sin(lon) };
			point[A] *= radius;

			point[B] = {
				std::cos(lat + kLatEvery) * std::cos(lon),
				std::sin(lat + kLatEvery),
				std::cos(lat + kLatEvery) * std::sin(lon)
			};
			point[B] *= radius;

			point[C] = {
				std::cos(lat) * std::cos(lon + kLonEvery),
				std::sin(lat),
				std::cos(lat) * std::sin(lon + kLonEvery),
			};
			point[C] *= radius;

			DrawLine(point[A] + center, point[B] + center, color);
			DrawLine(point[A] + center, point[C] + center, color);

		}
	}
}

void PrimitiveDrawer::DrawAll3D() {
	triangle_->DrawAll(Sxavenger::camera3D->GetGPUVirtualAddress());
	line_->DrawAll(Sxavenger::camera3D->GetGPUVirtualAddress());
}

void PrimitiveDrawer::ResetObjectCount() {
	triangle_->ResetObjectCount();
	line_->ResetObjectCount();
}
