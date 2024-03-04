#include "Bunny.h"

////////////////////////////////////////////////////////////////////////////////////////////
// Bunny methods
////////////////////////////////////////////////////////////////////////////////////////////

void Bunny::Init() {
	model_ = std::make_unique<Model>("resources/model", "bunny.obj");
}

void Bunny::EditorImGui(int identifier) {
	std::string title = "Bunny##" + std::to_string(identifier);

	SetImGui(title);

	ImGui::Text("Bunny Debacker");
	ImGui::Text("vertexSize: %d, indexSize: %d, face: %d", model_->GetMeshData(0).vertexResource->GetSize(), model_->GetMeshData(0).indexResource->GetSize(), model_->GetMeshData(0).indexResource->GetSize() / 3);
}

void Bunny::Draw(ID3D12GraphicsCommandList* commandList, Directional* directionalLight) {
	model_->SetBuffers(commandList, 0);

	commandList->SetGraphicsRootConstantBufferView(0, materialResource_->GetGPUVirtualAddress());
	commandList->SetGraphicsRootConstantBufferView(1, matrixResource_->GetGPUVirtualAddress());
	commandList->SetGraphicsRootConstantBufferView(2, directionalLight->GetGPUVirtualAddress());
	commandList->SetGraphicsRootConstantBufferView(3, MyEngine::camera3D_->GetGPUVirtualAddress());

	model_->SetTexture(4, commandList, 0);

	model_->DrawCall(commandList, 0);
}

void Bunny::Term() {
	model_.reset();
}
