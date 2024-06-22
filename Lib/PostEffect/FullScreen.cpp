#include "FullScreen.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include <MyEngine.h>
#include <Environment.h>

//-----------------------------------------------------------------------------------------
// using
//-----------------------------------------------------------------------------------------
using namespace DxObject;

////////////////////////////////////////////////////////////////////////////////////////////
// FullScreen class methods
////////////////////////////////////////////////////////////////////////////////////////////

void FullScreen::Init() {
	// IA
	vertex_ = std::make_unique<BufferResource<VertexData>>(MyEngine::GetDevicesObj(), 4);
	vertex_->operator[](0).position = { 0.0f, kWindowHeight, 0.0f };
	vertex_->operator[](0).texcoord = { 0.0f, 1.0f }; // leftBottom
	vertex_->operator[](1).position = { 0.0f, 0.0f, 0.0f };
	vertex_->operator[](1).texcoord = { 0.0f, 0.0f }; // leftTop
	vertex_->operator[](2).position = { kWindowWidth, kWindowHeight, 0.0f };
	vertex_->operator[](2).texcoord = { 1.0f, 1.0f }; // rightBottom
	vertex_->operator[](3).position = { kWindowWidth, 0.0f, 0.0f };
	vertex_->operator[](3).texcoord = { 1.0f, 0.0f }; // rightTop

	index_ = std::make_unique<IndexBufferResource>(MyEngine::GetDevicesObj(), 3 * 2);
	index_->operator[](0) = 0;
	index_->operator[](1) = 1;
	index_->operator[](2) = 2;
	index_->operator[](3) = 1;
	index_->operator[](4) = 3;
	index_->operator[](5) = 2;

	matrix_ = std::make_unique<BufferResource<Matrix4x4>>(MyEngine::GetDevicesObj(), 1);
	matrix_->operator[](0) = Matrix::MakeOrthographic(0.0f, 0.0f, static_cast<float>(kWindowWidth), static_cast<float>(kWindowHeight), 0.0f, 100.0f);


	// create pipeline
	blob_ = std::make_unique<GraphicsBlob>();
	blob_->Create(L"FullScreen.VS.hlsl", GRAPHICS_VERTEX);
	blob_->Create(L"FullScreen.PS.hlsl", GRAPHICS_PIXEL);

	GraphicRootSignatureDesc desc = {};
	desc.Resize(2, 1);

	//!< camera2D
	desc.SetCBV(0, VISIBILITY_VERTEX, 0);

	//!< textureBuffer
	desc.SetSRV(1, VISIBILITY_PIXEL, 0);
	desc.SetSampler(0, MODE_CLAMP, VISIBILITY_PIXEL, 0);

	pipeline_ = std::make_unique<GraphicsPipeline>();
	pipeline_->CreateRootSiganture(MyEngine::GetDevicesObj(), desc);
	pipeline_->CreatePipeline(MyEngine::GetDevicesObj(), blob_.get(), kBlendModeNormal);
}

void FullScreen::Term() {
}

void FullScreen::DrawTexture(const D3D12_GPU_DESCRIPTOR_HANDLE& texture) {

	auto commandList = MyEngine::GetCommandList();

	pipeline_->SetPipeline(commandList);

	D3D12_VERTEX_BUFFER_VIEW vertexBufferView = vertex_->GetVertexBufferView();
	D3D12_INDEX_BUFFER_VIEW  indexBufferView  = index_->GetIndexBufferView();

	// IA
	commandList->IASetVertexBuffers(0, 1, &vertexBufferView);
	commandList->IASetIndexBuffer(&indexBufferView);

	// constantBuffer
	commandList->SetGraphicsRootConstantBufferView(0, matrix_->GetGPUVirtualAddress());
	commandList->SetGraphicsRootDescriptorTable(1, texture);

	// drawCall
	commandList->DrawIndexedInstanced(index_->GetIndexSize(), 1, 0, 0, 0);

}
