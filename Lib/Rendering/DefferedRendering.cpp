#include "DefferedRendering.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include <MyEngine.h>
#include <Environment.h>
#include "DxShaderReflection.h"

//-----------------------------------------------------------------------------------------
// using
//-----------------------------------------------------------------------------------------
using namespace DxObject;

//=========================================================================================
// static variables
//=========================================================================================

const std::array<DXGI_FORMAT, DefferedRenderingType::kCountOfDefferedRenderingType> DefferedRendering::formats_ = {
	DXGI_FORMAT_R8G8B8A8_UNORM,     //!< DEFFERERD_ALBED
	DXGI_FORMAT_R8G8B8A8_UNORM,     //!< DEFFERERD_DEPTH
	DXGI_FORMAT_R8G8B8A8_UNORM,     //!< DEFFERERD_NORMAL
	DXGI_FORMAT_R32G32B32A32_FLOAT, //!< DEFFERERD_WORLD
};

////////////////////////////////////////////////////////////////////////////////////////////
// DefferedRendering class methods
////////////////////////////////////////////////////////////////////////////////////////////

void DefferedRendering::Init() {

	/* deffered */
	for (int i = 0; i < DefferedRenderingType::kCountOfDefferedRenderingType; ++i) {
		// deffered用 renderTextureの生成
		defferedTextures_[i] = std::make_unique<RenderTexture>();
		defferedTextures_[i]->Create(
			MyEngine::GetDxCommon(),
			kWindowWidth, kWindowHeight, { 0.0f, 0.0f, 0.0f, 0.0f }, formats_[i]
		);

		// 外部参照用のptr配列の設定
		texturePtrs_[i] = defferedTextures_[i].get();
		textureHandles_[i] = defferedTextures_[i]->GetGPUHandleSRV();
	}

	/* rendering */

	// IA
	vertexBuffer_ = std::make_unique<DxObject::BufferResource<VertexData>>(MyEngine::GetDevicesObj(), 4);
	vertexBuffer_->operator[](0).position = { -1.0f, -1.0f, 0.0f };
	vertexBuffer_->operator[](0).texcoord = { 0.0f, 1.0f };

	vertexBuffer_->operator[](1).position = { -1.0f, 1.0f, 0.0f };
	vertexBuffer_->operator[](1).texcoord = { 0.0f, 0.0f };

	vertexBuffer_->operator[](2).position = { 1.0f, -1.0f, 0.0f };
	vertexBuffer_->operator[](2).texcoord = { 1.0f, 1.0f };

	vertexBuffer_->operator[](3).position = { 1.0f, 1.0f, 0.0f };
	vertexBuffer_->operator[](3).texcoord = { 1.0f, 0.0f };

	indexBuffer_  = std::make_unique<DxObject::IndexBufferResource>(MyEngine::GetDevicesObj(), 6);
	indexBuffer_->operator[](0) = 0;
	indexBuffer_->operator[](1) = 1;
	indexBuffer_->operator[](2) = 2;
	indexBuffer_->operator[](3) = 1;
	indexBuffer_->operator[](4) = 3;
	indexBuffer_->operator[](5) = 2;

	// Graphics
	blob_ = std::make_unique<GraphicsBlob>();
	blob_->Create(L"defferedRendering.VS.hlsl", GRAPHICS_VERTEX);
	blob_->Create(L"defferedRendering.PS.hlsl", GRAPHICS_PIXEL);

	GraphicsRootSignatureDesc desc;
	desc.Resize(6, 1);
	desc.SetSRV(0, VISIBILITY_PIXEL, 0);
	desc.SetSRV(1, VISIBILITY_PIXEL, 1);
	desc.SetSRV(2, VISIBILITY_PIXEL, 2);
	desc.SetSRV(3, VISIBILITY_PIXEL, 3);
	desc.SetSampler(0, MODE_CLAMP, VISIBILITY_PIXEL, 0);
	// other buffers
	desc.SetCBV(4, VISIBILITY_PIXEL, 0); //!< camera
	desc.SetCBV(5, VISIBILITY_PIXEL, 1); //!< light

	GraphicsPipelineDesc pipelineDesc;
	pipelineDesc.CreateDefaultDesc();
	pipelineDesc.rtvFormats.clear();
	pipelineDesc.SetRTVFormat(DXGI_FORMAT_R32G32B32A32_FLOAT);

	pipeline_ = std::make_unique<GraphicsPipeline>();
	pipeline_->CreateRootSignature(MyEngine::GetDevicesObj(), desc);
	pipeline_->CreatePipeline(MyEngine::GetDevicesObj(), blob_.get(), pipelineDesc);

}

void DefferedRendering::Draw(const D3D12_GPU_VIRTUAL_ADDRESS& cameraAddress, const D3D12_GPU_VIRTUAL_ADDRESS& lightAddress) {

	auto commandList = MyEngine::GetCommandList();
	
	pipeline_->SetPipeline(commandList);

	D3D12_VERTEX_BUFFER_VIEW vertexBufferView = vertexBuffer_->GetVertexBufferView();
	D3D12_INDEX_BUFFER_VIEW indexBufferView   = indexBuffer_->GetIndexBufferView();

	commandList->IASetVertexBuffers(0, 1, &vertexBufferView);
	commandList->IASetIndexBuffer(&indexBufferView);

	for (int i = 0; i < DefferedRenderingType::kCountOfDefferedRenderingType; ++i) {
		commandList->SetGraphicsRootDescriptorTable(i, textureHandles_[i]);
	}

	commandList->SetGraphicsRootConstantBufferView(4, cameraAddress);
	commandList->SetGraphicsRootConstantBufferView(5, lightAddress);

	commandList->DrawIndexedInstanced(indexBuffer_->GetIndexSize(), 1, 0, 0, 0);
}

void DefferedRendering::Term() {
}
