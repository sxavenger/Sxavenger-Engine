#include "Sphere.h"

////////////////////////////////////////////////////////////////////////////////////////////
// Sphere class
////////////////////////////////////////////////////////////////////////////////////////////

void Sphere::Init() {
	DrawData data = DrawMethods::SphereData(1.0f, 16);

	vertexResource_ = data.vertex;
	indexResource_ = data.index;

	vertexBufferView_ = vertexResource_->GetVertexBufferView();
	indexBufferView_ = indexResource_->GetIndexBufferView();
}

void Sphere::EditorImGui(int identifier) {
	std::string title = "Sphere##" + std::to_string(identifier);

	SetImGui(title);
}

void Sphere::Draw(ID3D12GraphicsCommandList* commandList, Directional* directionalLight) {
	MyEngine::SetPipeLineState(TEXTURE);

	commandList->IASetVertexBuffers(0, 1, &vertexBufferView_);
	commandList->IASetIndexBuffer(&indexBufferView_);

	commandList->SetGraphicsRootConstantBufferView(0, materialResource_->GetGPUVirtualAddress());
	commandList->SetGraphicsRootConstantBufferView(1, matrixResource_->GetGPUVirtualAddress());
	commandList->SetGraphicsRootConstantBufferView(2, directionalLight->GetGPUVirtualAddress());
	commandList->SetGraphicsRootConstantBufferView(3, MyEngine::camera3D_->GetGPUVirtualAddress());

	commandList->SetGraphicsRootDescriptorTable(4, MyEngine::GetTextureHandleGPU("resources/uvChecker.png"));

	commandList->DrawIndexedInstanced(indexResource_->GetSize(), 1, 0, 0, 0);
}

void Sphere::Term() {
	vertexResource_.Release();
	indexResource_.Release();
}
