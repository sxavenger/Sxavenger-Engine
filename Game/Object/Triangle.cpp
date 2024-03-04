#include "Triangle.h"

////////////////////////////////////////////////////////////////////////////////////////////
// Triangle methods
////////////////////////////////////////////////////////////////////////////////////////////

void Triangle::Init() {
	// vertexResource
	vertexResource_ = std::make_unique<DxObject::BufferResource<VertexData>>(MyEngine::GetDevice(), kVertexNum_);

	vertexResource_->operator[](0).position = { -0.5f, -0.5f, 0.0f, 1.0f };
	vertexResource_->operator[](0).texcoord = { 0.0f, 1.0f };
	vertexResource_->operator[](1).position = { 0.0f, 0.5f, 0.0f, 1.0f };
	vertexResource_->operator[](1).texcoord = { 0.5f, 0.0f };
	vertexResource_->operator[](2).position = { 0.5f, -0.5f, 0.0f, 1.0f };
	vertexResource_->operator[](2).texcoord = { 1.0f, 1.0f };

	for (uint32_t index = 0; index < kVertexNum_; ++index) {
		vertexResource_->operator[](index).normal = { 0.0f, 0.0f, -1.0f };
	}

	vertexBufferView_ = vertexResource_->GetVertexBufferView();
}

void Triangle::EditorImGui(int identifier) {
	
	std::string title = "Triangle##" + std::to_string(identifier);

	SetImGui(title);
}

void Triangle::Draw(ID3D12GraphicsCommandList* commandList, Directional* directionalLight) {
	MyEngine::SetPipeLineState(TEXTURE);

	commandList->IASetVertexBuffers(0, 1, &vertexBufferView_);

	commandList->SetGraphicsRootConstantBufferView(0, materialResource_->GetGPUVirtualAddress());
	commandList->SetGraphicsRootConstantBufferView(1, matrixResource_->GetGPUVirtualAddress());
	commandList->SetGraphicsRootConstantBufferView(2, directionalLight->GetGPUVirtualAddress());
	commandList->SetGraphicsRootConstantBufferView(3, MyEngine::camera3D_->GetGPUVirtualAddress());

	commandList->SetGraphicsRootDescriptorTable(4, MyEngine::GetTextureHandleGPU("resources/uvChecker.png"));

	commandList->DrawInstanced(vertexResource_->GetSize(), 1, 0, 0);
}

void Triangle::Term() {
	vertexResource_.reset();
}
