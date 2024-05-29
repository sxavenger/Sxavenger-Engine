#include "GaussianBlur.h"

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
// GaussianBlur class methods
////////////////////////////////////////////////////////////////////////////////////////////

void GaussianBlur::Init() {

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

	// constantBuffer
	param_ = std::make_unique<BufferResource<BlurParameter>>(MyEngine::GetDevicesObj(), 1);
	param_->operator[](0).isHolizontalEnable = true;
	param_->operator[](0).isVerticlaEnable = true;

	matrix_ = std::make_unique<BufferResource<Matrix4x4>>(MyEngine::GetDevicesObj(), 1);
	matrix_->operator[](0) = Matrix::MakeOrthographic(0.0f, 0.0f, static_cast<float>(kWindowWidth), static_cast<float>(kWindowHeight), 0.0f, 100.0f);

	outputTexture_ = std::make_unique<RenderTexture>();
	outputTexture_->Create(kWindowWidth, kWindowHeight, MyEngine::GetDxCommon());

	SetThisAttribute("GaussianBlur");

}

void GaussianBlur::Term() {
	outputTexture_.reset();
}

void GaussianBlur::CreateBlurTexture(int32_t width, int32_t height, const D3D12_GPU_DESCRIPTOR_HANDLE& texture) {

	param_->operator[](0).renderSize = { static_cast<float>(width), static_cast<float>(height) };

	{
		MyEngine::BeginOffScreen(outputTexture_.get());

		MyEngine::SetBlendMode(kBlendModeNormal);
		MyEngine::SetPipelineType(GAUSSIANBLUR);
		MyEngine::SetPipelineState();

		auto commandList = MyEngine::GetCommandList();

		D3D12_VERTEX_BUFFER_VIEW vertexBufferView = vertex_->GetVertexBufferView();
		D3D12_INDEX_BUFFER_VIEW  indexBufferView  = index_->GetIndexBufferView();

		// IA
		commandList->IASetVertexBuffers(0, 1, &vertexBufferView);
		commandList->IASetIndexBuffer(&indexBufferView);

		// constantBuffer
		commandList->SetGraphicsRootConstantBufferView(0, matrix_->GetGPUVirtualAddress());
		commandList->SetGraphicsRootConstantBufferView(1, param_->GetGPUVirtualAddress());

		commandList->SetGraphicsRootDescriptorTable(2, texture);

		// drawCall
		commandList->DrawIndexedInstanced(index_->GetIndexSize(), 1, 0, 0, 0);

		MyEngine::EndOffScreen();
	}

}

void GaussianBlur::CreateBlurTexture(
	Texture* outputTexture,
	int32_t width, int32_t height, const D3D12_GPU_DESCRIPTOR_HANDLE& texture) {

	param_->operator[](0).renderSize = { static_cast<float>(width), static_cast<float>(height) };

	{
		MyEngine::BeginOffScreen(outputTexture);

		MyEngine::SetBlendMode(kBlendModeNormal);
		MyEngine::SetPipelineType(GAUSSIANBLUR);
		MyEngine::SetPipelineState();

		auto commandList = MyEngine::GetCommandList();

		D3D12_VERTEX_BUFFER_VIEW vertexBufferView = vertex_->GetVertexBufferView();
		D3D12_INDEX_BUFFER_VIEW  indexBufferView  = index_->GetIndexBufferView();

		// IA
		commandList->IASetVertexBuffers(0, 1, &vertexBufferView);
		commandList->IASetIndexBuffer(&indexBufferView);

		// constantBuffer
		commandList->SetGraphicsRootConstantBufferView(0, matrix_->GetGPUVirtualAddress());
		commandList->SetGraphicsRootConstantBufferView(1, param_->GetGPUVirtualAddress());

		commandList->SetGraphicsRootDescriptorTable(2, texture);

		// drawCall
		commandList->DrawIndexedInstanced(index_->GetIndexSize(), 1, 0, 0, 0);

		MyEngine::EndOffScreen();
	}
}

void GaussianBlur::SetAttributeImGui() {

	static bool isHolizontal = param_->operator[](0).isHolizontalEnable;
	static bool isVertical   = param_->operator[](0).isVerticlaEnable;

	ImGui::Checkbox("isHolizontalEnable", &isHolizontal);
	ImGui::Checkbox("isVerticlaEnable", &isVertical);

	param_->operator[](0).isHolizontalEnable = isHolizontal;
	param_->operator[](0).isVerticlaEnable = isVertical;

}
